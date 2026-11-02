#include <REG52.H>
#include <intrins.h>
#include "game.h"

sbit START_KEY = P3^2; /* K3: leave K1/TXD and K2/RXD untouched */
sbit CONFIRM_KEY = P3^3;
static u8 target[4], guess[4];
static u8 count, playing;
static unsigned long seed = 1;

static void delay_ms(u16 ms)
{
    u16 i;
    while (ms--) for (i=0; i<110; ++i) _nop_();
}

static void uart_init(void)
{
    ES=0; /* polling TX; no empty receive interrupt handler */
    TMOD=(TMOD & 0x0F) | 0x20;
    SCON=0x50;
    PCON |= 0x80;
    TH1=0xFA; TL1=0xFA;
    TI=0; RI=0; TR1=1;
}

static void send_char(u8 c)
{
    SBUF=c;
    while (!TI);
    TI=0;
}

static void send_text(char code *s)
{
    while (*s) send_char(*s++);
}

/* UTF-8 output, independent of the editor's source-file encoding. */
static void prompt(void)
{
    send_text("\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\r\n");
}

/* Matches supplied Puzhong example: columns P1.3..0, rows P1.7..4. */
static u8 matrix_raw(void)
{
    u8 c,r,v,found=0xFF;
    for(c=0;c<4;c++) {
        P1=(u8)~(0x08 >> c);
        _nop_(); _nop_();
        v=P1 & 0xF0;
        for(r=0;r<4;r++) if (!(v & (0x80 >> r))) {
            if(found!=0xFF) { P1=0xFF; return 0xFE; }
            found=r*4+c;
        }
    }
    P1=0xFF;
    return found;
}

/* Debounced press and release; holding a key produces one event. */
static u8 key_event(void)
{
    static u8 previous=0xFF, stable=0xFF, samples=0, armed=1;
    u8 k;
    if(!START_KEY && CONFIRM_KEY) k=16;
    else if(START_KEY && !CONFIRM_KEY) k=17;
    else if(!START_KEY && !CONFIRM_KEY) k=0xFE;
    else k=matrix_raw();
    if(k!=previous) { previous=k; samples=0; }
    else if(samples<4) ++samples;
    if(samples==4 && k!=stable) {
        stable=k;
        if(k==0xFF) armed=1;
        else if(armed) { armed=0; if(k!=0xFE) return k; }
    }
    return 0xFF;
}

static void start_game(void)
{
    u16 n,t;
    u8 hi,lo,h2,i;
    do { hi=TH0; lo=TL0; h2=TH0; } while(hi!=h2);
    t=((u16)hi<<8)|lo;
    seed=((seed ^ t)*1103515245UL+12345UL)&0x7FFFUL;
    n=(u16)(seed%10000UL);
    for(i=4;i>0;i--) { target[i-1]=n%10; n/=10; }
    count=0; playing=1;
    send_text("\r\n"); prompt();
}

static void confirm_guess(void)
{
    u8 x,y;
    if(!playing) return;
    if(count!=4) { send_text("\r\nNeed 4 digits.\r\n"); return; }
    score_guess(target,guess,&x,&y);
    send_text("\r\n");
    if(y==4) {
        send_text("\xE6\x81\xAD\xE5\x96\x9C\xE4\xBD\xA0\xEF\xBC\x8C\xE5\x85\xA8\xE5\xAF\xB9\xE4\xBA\x86\r\n");
        send_text("K3: new game.\r\n"); playing=0;
    } else {
        send_char('0'+x);
        send_text("\xE4\xB8\xAA\xE6\x95\xB0\xE5\xAD\x97\xE6\xAD\xA3\xE7\xA1\xAE\xEF\xBC\x8C");
        send_char('0'+y);
        send_text("\xE4\xB8\xAA\xE4\xBD\x8D\xE7\xBD\xAE\xE6\xAD\xA3\xE7\xA1\xAE\r\n");
        count=0; prompt();
    }
}

void main(void)
{
    u8 k;
    P1=0xFF; START_KEY=1; CONFIRM_KEY=1;
    P0=0; /* blank seven-segment display */
    TMOD=0x01; TH0=0; TL0=0; ET0=0; TR0=1;
    uart_init();
    send_text("\r\nGuess game: K3=start, K4=confirm; S1..S10=0..9.\r\n");
    while(1) {
        k=key_event();
        if(k==16) start_game();
        else if(k==17) confirm_guess();
        else if(k<10 && playing && count<4) {
            guess[count++]=k; send_char('0'+k);
        }
        delay_ms(5);
    }
}
