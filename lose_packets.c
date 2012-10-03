/* lose_packets.c */

/* 

   This is part of source code for FEC codes described in
   ICSI Tech Report TR-95-048
   "An XOR-Based Erasure-Resilient Coding Scheme"
   Johannes Bloemer, Malik Kalfane, Marek Karpinski,
   Richard Karp, Michael Luby, David Zuckerman

*/

/*******************************************************************

Copyright (c) 1992-1997 Michael Luby
International Computer Science Institute
All Rights Reserved.

Permission to use, copy and modify this software and its
documentation for internal evaluation purposes only and
without fee is hereby granted to any party.

The International Computer Science Institute will receive any
enhancements and modifications made to this software.

Permission to incorporate this software into commercial applications
must be obtained from the International Computer Science Institute,
1947 Center Street, Suite 600, Berkeley, CA 94704, USA.

IN NO EVENT SHALL THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE BE
LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE INTERNATIONAL
COMPUTER SCIENCE INSTITUTE  HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE INTERNATIONAL COMPUTER SCIENCE INSTITUTE SPECIFICALLY DISCLAIMS
ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE INTERNATIONAL
COMPUTER SCIENCE INSTITUTE  HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE,
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*********************************************************************/


#include "parameters.h"
#include "lose_packets.h"
#include <sys/time.h>
#include <stdio.h>

int Lose_Packets(UNSIGNED *packets, UNSIGNED *rec_packets, int *pNrec)

{
int i,j,k,m;
int ranval ;

/* Set up seed to random generator for losing packets */

struct timeval tp ;
struct timezone tzp;
gettimeofday(&tp, &tzp);
srand48(tp.tv_usec);

/* Decide which packets to keep randomly

*pNrec = 0 ;
k = 0 ;
m = 0 ;
for (i=0; i < Npackets ; i++) 
  {
   ranval = (int) mrand48();
   if (ranval <= 0xC0000000) 
     {
      for (j=0; j < Plentot ; j++) 
        {
         rec_packets[m] = packets[k+j] ;
         m++ ;
        } ;
      (*pNrec)++ ;
     } ;
  k += Plentot ;
  } ;
*/

/* Keep last packets and only */

*pNrec = 0 ;
k = 0 ;
m = 0 ;
for (i=(Npackets-Mpackets)/2 ; i < (Npackets+Mpackets)/2; i++) 
  {
   k = Plentot * i ;
   m = Plentot * (*pNrec) ; 
   for (j=0; j < Plentot; j++) rec_packets[m+j] = packets[k+j] ;
   (*pNrec)++ ;
  } ;

return tp.tv_usec ;
}
