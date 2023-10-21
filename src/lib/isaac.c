/*
------------------------------------------------------------------------------
readable.c: My random number generator, ISAAC.
(c) Bob Jenkins, March 1996, Public Domain
You may use this code in any way you wish, and it is free.  No warrantee.
* May 2008 -- made it not depend on standard.h
------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "isaac.h"

static void doisaac(struct isaac *);

static void doisaac(struct isaac *isaac)
{
   uint32_t i,x,y;

   uint32_t *mm = isaac->mm;
   uint32_t *randrsl = isaac->randrsl;
   uint32_t aa = isaac->aa;
   uint32_t bb = isaac->bb;
   uint32_t cc = isaac->cc;

   cc = cc + 1;    /* cc just gets incremented once per 256 results */
   bb = bb + cc;   /* then combined with bb */

   for (i=0; i<256; ++i)
   {
     x = mm[i];
     switch (i%4)
     {
     case 0: aa = aa^(aa<<13); break;
     case 1: aa = aa^(aa>>6); break;
     case 2: aa = aa^(aa<<2); break;
     case 3: aa = aa^(aa>>16); break;
     }
     aa              = mm[(i+128)%256] + aa;
     mm[i]      = y  = mm[(x>>2)%256] + aa + bb;
     randrsl[i] = bb = mm[(y>>10)%256] + x;

     /* Note that bits 2..9 are chosen from x but 10..17 are chosen
        from y.  The only important thing here is that 2..9 and 10..17
        don't overlap.  2..9 and 10..17 were then chosen for speed in
        the optimized version (rand.c) */
     /* See http://burtleburtle.net/bob/rand/isaac.html
        for further explanations and analysis. */
   }

   isaac->aa = aa;
   isaac->bb = bb;
   isaac->cc = cc;
}


/* if (flag!=0), then use the contents of randrsl[] to initialize mm[]. */
#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

uint32_t isaac_next(struct isaac *isaac)
{
   if (isaac->randcnt-- == 0) {
      doisaac(isaac);
      isaac->randcnt = 255;
   }
   return isaac->randrsl[isaac->randcnt];
}

void isaac_init(struct isaac *isaac, int flag)
{
   int i;
   uint32_t a,b,c,d,e,f,g,h;
   uint32_t *mm = isaac->mm;
   uint32_t *randrsl = isaac->randrsl;
   isaac->aa = 0;
   isaac->bb = 0;
   isaac->cc = 0;
   a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

   memset(mm, 0, sizeof(uint32_t) * 256);

   for (i=0; i<4; ++i)          /* scramble it */
   {
     mix(a,b,c,d,e,f,g,h);
   }

   for (i=0; i<256; i+=8)   /* fill in mm[] with messy stuff */
   {
     if (flag)                  /* use all the information in the seed */
     {
       a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
       e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
     }
     mix(a,b,c,d,e,f,g,h);
     mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
     mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
   }

   if (flag)
   {        /* do a second pass to make all of the seed affect all of mm */
     for (i=0; i<256; i+=8)
     {
       a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
       e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
       mix(a,b,c,d,e,f,g,h);
       mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
       mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
     }
   }

   doisaac(isaac);     /* fill in the first set of results */
   isaac->randcnt=256; /* prepare to use the first set of results */
}

#if 0
int main()
{
  uint32_t i,j;
  aa=bb=cc=(uint32_t)0;
  for (i=0; i<256; ++i) mm[i]=randrsl[i]=(uint32_t)0;
  randinit(1);
  for (i=0; i<2; ++i)
  {
    doisaac();
    for (j=0; j<256; ++j)
    {
      printf("%.8lx",randrsl[j]);
      if ((j&7)==7) printf("\n");
    }
  }
}
#endif
