#include <stdio.h>
#include "../firmware/game.h"
int main(void) {
 unsigned int n,t,j,k,ct[10],cg[10],ex,ey,cases=0;
 unsigned int targets[]={0,1,11,1122,1212,1234,9090,9999};
 u8 a[4],b[4],x,y;
 for(k=0;k<sizeof(targets)/sizeof(targets[0]);k++) for(n=0;n<10000;n++) {
  t=targets[k];j=n;
  for(int i=3;i>=0;i--){a[i]=t%10;t/=10;b[i]=j%10;j/=10;}
  for(int i=0;i<10;i++)ct[i]=cg[i]=0;
  ey=0;ex=0;
  for(int i=0;i<4;i++){ct[a[i]]++;cg[b[i]]++;if(a[i]==b[i])ey++;}
  for(int i=0;i<10;i++)ex+=ct[i]<cg[i]?ct[i]:cg[i];
  score_guess(a,b,&x,&y);
  if(x!=ex||y!=ey){printf("FAIL %u %u\n",targets[k],n);return 1;}cases++;
 }
 printf("PASS: %u cases, independent digit-frequency oracle\n",cases);return 0;
}
