#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

char* itoa(uint32_t val, char * buf, int base) {
    
    int i = 32;
    buf[i+1] = 0;
    for(; i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    
    return &buf[i+1];
}

float itof(uint32_t reg, char sign_bit) {
  uint32_t rval = 0;
   char i = 0;

   if(sign_bit) {
     if ( reg & (uint32_t)0x00800000L ) 
       rval = 0x80000000L;
     reg <<= 1;
     reg &= 0xFFFFFFL;
   }
   if ( reg == 0 ) return 0;


   while ( !(reg & (uint32_t)0x00800000L) ) {
     reg <<= 1;
     i++;
   }
   i++;
   rval = rval | ((uint32_t)(127-i)<<(uint32_t)23) | (reg & 0x7FFFFFL);
   return *(float *)&rval;
}

int main(int argc, char * argv[]) {
   uint32_t cnt;
   char buf1[128],buf2[128], buf3[128];
   uint32_t chunk;
   float reta,calc;
   for(cnt = (1<<24L) - 1; cnt != 0xFFFFFFFF;) {
       chunk = cnt;
       for(;cnt >= (int64_t)chunk-(1<<16L) && cnt != 0xFFFFFFFF;cnt--) {
            reta = itof(cnt, 0);
            calc = (float)(((float)cnt)/(1<<24L));
            if ( reta != calc ) {
                printf("%10u : got [%24s] expected [%32s] got [%32s]\n",cnt,itoa(cnt,buf3,2),itoa(*(uint32_t *)&calc,buf1,2),itoa(*(uint32_t *)&reta,buf2,2));
            } else {
                //printf("%10d\n",cnt);
            }
       }    
       printf("Checked %08X\n",cnt);
   }
   return 0; 
}