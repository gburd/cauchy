/* driver.c   */

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "parameters.h"
#include "init_field.h"
#include "get_msg.h"
#include "encode.h"
#include "lose_packets.h"
#include "decode.h"
#include "compare_msg.h"

int main()
{
static UNSIGNED COLBIT, BIT[15] ;

UNSIGNED *ExptoFE, *FEtoExp ;
UNSIGNED *message, *rec_message ;
UNSIGNED *packets, *rec_packets ;
int i, j, iter, Nrec, TNrec, seed, return_code ;
struct timeval stime, etime ;

ExptoFE = (UNSIGNED *) calloc(TableLength+Lfield, sizeof(UNSIGNED));
if (!(ExptoFE)) {printf("\ndriver: ExptoFE malloc failed\n"); exit(434); }

FEtoExp = (UNSIGNED *) calloc(TableLength, sizeof(UNSIGNED));
if (!(FEtoExp)) {printf("\ndriver: FEtoExp malloc failed\n"); exit(434); }

rec_message = (UNSIGNED *) calloc(Mlen, sizeof(UNSIGNED));
if (!(rec_message)) {printf("\ndriver: rec_message malloc failed\n"); exit(434); }

message = (UNSIGNED *) calloc(Mlen, sizeof(UNSIGNED));
if (!(message)) {printf("\ndriver: message malloc failed\n"); exit(434); }

packets = (UNSIGNED *) calloc(Npackets*Plentot, sizeof(UNSIGNED));
if (!(packets)) {printf("\ndriver: packets malloc failed\n"); exit(434); }

rec_packets = (UNSIGNED *) calloc(Npackets*Plentot, sizeof(UNSIGNED));
if (!(rec_packets)) {printf("\ndriver: rec_packets malloc failed\n"); exit(434); }

/* Initialize the arrays for finite field operations */

Init_field(&COLBIT, BIT, ExptoFE, FEtoExp);

/* Do several iterations of encoding and decoding message */

TNrec = 0 ;

gettimeofday(&stime,0) ;

for (iter=0; iter < Niter; iter++)
  {

  Get_msg(message);

  #ifdef PRINT
  printf("--------------------------------------\nIteration %d.\n",iter) ;
  printf("\nDone getting message of %d bytes \n",Mlen*4) ;
  #endif

  Encode(COLBIT, BIT, ExptoFE, FEtoExp, packets, message);
  #ifdef PRINT
  printf("Done encoding %d packets\n", Npackets) ;
  #endif
  seed = Lose_Packets(packets, rec_packets, &Nrec) ;
  #ifdef PRINT
  printf("Received %d packets with seed %d \n",Nrec,seed);
  #endif
  return_code = Decode(COLBIT, BIT, ExptoFE, FEtoExp,
                       rec_packets, &Nrec, rec_message) ;
  #ifdef PRINT
  printf("Done with decoding\n");
  #endif
  if (return_code == 0)
    {
    Compare_msg(message, rec_message);
    #ifdef PRINT
    printf("Done with comparison\n");
    #endif
    } ;
  TNrec += Nrec ;
  } ;

gettimeofday(&etime,0) ;

printf("--------- Cauchy Code ---------------\n") ;
printf("Number of Mbytes processed is %f \n",((float) Elen*Niter*4)/1000000.0) ;
printf("Time in seconds is %f \n", (float) (etime.tv_sec-stime.tv_sec)
       + (float)(etime.tv_usec-stime.tv_usec)/1000000.0 );
printf("MBytes per second is %f \n",((float) Elen*Niter*4)/
((float) (etime.tv_sec-stime.tv_sec)* 1000000.0+(float)(etime.tv_usec-stime.tv_usec)));
printf("Overhead is %d\n", Mpackets) ;
printf("Encoding redundancy is %f \n",1.0 + (float) Rpackets/ (float) Mpackets) ;
printf("Fraction of packets received is %f \n\n",(float) TNrec/ ((float) Niter * Npackets)) ;
}
