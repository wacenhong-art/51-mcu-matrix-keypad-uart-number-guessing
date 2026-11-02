#include "game.h"
/* Count the multiset intersection, including exact-position matches. */
void score_guess(u8 *target,u8 *guess,u8 *digits,u8 *positions)
{
    u8 used[4]={0,0,0,0};
    u8 i,j;
    *digits=0; *positions=0;
    for(i=0;i<4;i++) {
        if(target[i]==guess[i]) ++*positions;
        for(j=0;j<4;j++) if(!used[j] && guess[i]==target[j]) {
            used[j]=1; ++*digits; break;
        }
    }
}
