#include <REG52.H>
#include <intrins.h>
#include "display.h"

/* Puzhong LCD1602: P0 data, P2.6 RS, P2.5 RW, P2.7 E.
 * Seven-segment decoder: P2.4/P2.3/P2.2. Both use P0.
 * E stays LOW during multiplexing. LCD writes mask interrupts only
 * for the short E pulse, then restore the selected segment pattern.
 */
sbit LCD_RS=P2^6;
sbit LCD_RW=P2^5;
sbit LCD_E=P2^7;
static volatile u8 segments[8];
static volatile u8 last_segment;
static u8 code number[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

static void wait_ms(u16 ms)
{
    u16 i;
    while(ms--) for(i=0;i<110;i++) _nop_();
}

static void lcd_write(u8 value,u8 is_data)
{
    bit enabled;
    enabled=EA; EA=0;
    LCD_E=0; LCD_RW=0; LCD_RS=is_data;
    P0=value;
    _nop_(); _nop_();
    LCD_E=1;
    _nop_(); _nop_(); _nop_(); _nop_();
    LCD_E=0;
    P0=last_segment;
    EA=enabled;
    wait_ms(1);
}

static void line(u8 row,char code *text)
{
    u8 i;
    lcd_write(row?0xC0:0x80,0);
    for(i=0;i<16;i++) {
        if(*text) lcd_write(*text++,1);
        else lcd_write(' ',1);
    }
}

static void lcd_digits(u8 column,u8 *digits,u8 count)
{
    u8 i;
    lcd_write(0x80+column,0);
    for(i=0;i<4;i++) lcd_write(i<count?('0'+digits[i]):'_',1);
}

static void set_input(u8 *digits,u8 count)
{
    u8 i;
    bit enabled;
    enabled=EA;EA=0;
    for(i=0;i<4;i++) segments[i]=i<count?number[digits[i]]:0x40;
    EA=enabled;
}

void display_begin(void)
{
    u8 i;
    for(i=0;i<8;i++) segments[i]=0x40;
    line(0,"IN:____ K4:OK");
    line(1,"X:- Y:- K3:NEW");
}

void display_input(u8 *digits,u8 count)
{
    set_input(digits,count);
    line(0,"IN:____ K4:OK");
    lcd_digits(3,digits,count);
}

void display_short(void)
{
    line(1,"NEED 4 DIGITS");
}

void display_result(u8 *digits,u8 x,u8 y)
{
    set_input(digits,4);
    if(y==4) {
        segments[4]=0x73;segments[5]=0x77;
        segments[6]=0x6d;segments[7]=0x6d; /* PASS */
        line(0,"ALL CORRECT!");
        line(1,"K3:NEW GAME");
    } else {
        segments[4]=0x40;segments[5]=number[x];
        segments[6]=0x40;segments[7]=number[y];
        line(0,"LAST:____");lcd_digits(5,digits,4);
        line(1,"X:0 Y:0 NEXT:KEY");
        lcd_write(0xC2,0);lcd_write('0'+x,1);
        lcd_write(0xC6,0);lcd_write('0'+y,1);
    }
}

void display_init(void)
{
    u8 i;
    EA=0;ET2=0;TR2=0;
    LCD_E=0;LCD_RW=0;LCD_RS=0;P0=0;last_segment=0;
    for(i=0;i<8;i++) segments[i]=0x40;
    wait_ms(40);
    lcd_write(0x38,0);wait_ms(5);
    lcd_write(0x38,0);lcd_write(0x38,0);
    lcd_write(0x0C,0);lcd_write(0x06,0);
    lcd_write(0x01,0);wait_ms(5);
    line(0,"GUESS 0000-9999");
    line(1,"K3:START K4:OK");
    /* Timer2 auto-reload, 922 machine cycles ~=1ms at 11.0592MHz/12. */
    T2CON=0;RCAP2H=0xFC;RCAP2L=0x66;
    TH2=0xFC;TL2=0x66;TF2=0;ET2=1;TR2=1;EA=1;
}

void display_tick(void) interrupt 5
{
    static u8 position=0;
    TF2=0;
    P0=0;
    /* Decoder 111 selects the leftmost digit. Preserve LCD control bits. */
    P2=(P2&0xE3)|((7-position)<<2);
    last_segment=segments[position];P0=last_segment;
    position=(position+1)&7;
}
