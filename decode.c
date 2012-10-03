/* decode.c   */

/*

   This is part of source code for FEC codes described in
   ICSI Tech Report TR-95-048
   "An XOR-Based Erasure-Resilient Coding Scheme"
   Johannes Bloemer, Malik Kalfane, Marek Karpinski,
   Richard Karp, Michael Luby, David Zuckerman

*/

/*******************************************************************

Copyright (c) 1992-1997 Michael Luby
International Computer Science Institute (ICSI)
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
#include "decode.h"
#include <sys/time.h>
#include <stdio.h>

int Decode(UNSIGNED COLBIT, UNSIGNED *BIT, UNSIGNED *ExptoFE,  
           UNSIGNED *FEtoExp, UNSIGNED *rec_packets, int *pNrec, 
           UNSIGNED *rec_message)
{
int i,j,k,l,m, index, seg_ind ;
int col_ind, row_ind, col_eqn, row_eqn ;
int Nfirstrec, Nextra ;
int *Rec_index ;
int *Col_Ind, *Row_Ind, ExpFE ;
UNSIGNED  *M ;
UNSIGNED *C, *D, *E, *F ;
struct timeval start_time, end_time;

Rec_index = (int *) calloc(Mpackets, sizeof(int));
if (!(Rec_index)) {printf("\ndecode: Rec_index malloc failed\n"); exit(434); }

Col_Ind = (int *) calloc(Mpackets, sizeof(int));
if (!(Col_Ind)) {printf("\ndecode: Col_Ind malloc failed\n"); exit(434); }

Row_Ind = (int *) calloc(Rpackets, sizeof(int));
if (!(Row_Ind)) {printf("\ndecode: Row_Ind malloc failed\n"); exit(434); }

C = (UNSIGNED *) calloc(Rpackets, sizeof(UNSIGNED));
if (!(C)) {printf("\ndecode: C malloc failed\n"); exit(434); }

D = (UNSIGNED *) calloc(Mpackets, sizeof(UNSIGNED));
if (!(D)) {printf("\ndecode: D malloc failed\n"); exit(434); }

E = (UNSIGNED *) calloc(Mpackets, sizeof(UNSIGNED));
if (!(E)) {printf("\ndecode: E malloc failed\n"); exit(434); }

F = (UNSIGNED *) calloc(Rpackets, sizeof(UNSIGNED));
if (!(F)) {printf("\ndecode: F malloc failed\n"); exit(434); }

M = (UNSIGNED *) calloc(Nsegs*Rpackets*Lfield, sizeof(UNSIGNED));
if (!(M)) {printf("\ndecode: M malloc failed\n"); exit(434); }

if (*pNrec < Mpackets) 
  {
  #ifdef PRINT
  printf("*** Need %d packets to recover message",Mpackets) ;
  printf(" but only %d packets received *** \n",*pNrec) ;
  #endif
  return 1 ;
  } ;

/* Initialize the received message */

for (i=0; i < Mlen ; i++) rec_message[i] = 0 ;

/* Move information from packets into received message.
   Fill in parts of received message that 
   requires no processing and figure out how 
   many of the redundant packets are needed.
   Nfirstrec is the number of packets received
   from among the first Mpackets that carry portions
   of the unprocessed original message.
   Rec_index is an array that indicates which parts
   of the message are received.  The pattern is
   the same within all Nsegs segments,  */

gettimeofday(&start_time,0) ; 

Nfirstrec = 0 ;
for (i=0; i < Mpackets; i++) Rec_index[i] = 0 ;
m = 0 ;
for (i=0; i < *pNrec ; i++)
  {
  index = rec_packets[m] ;
  if (index < Mpackets)
    {
    j = index * Plen ;
    Rec_index[index] = 1 ;
    for (row_eqn=0; row_eqn < Lfield ; row_eqn++)
      {
      k = row_eqn * Nsegs ;
      l = j + k ;
      for (seg_ind=0; seg_ind < Nsegs ; seg_ind++)
        rec_message[seg_ind+l] = rec_packets[m+1+seg_ind+k] ;
       } ;
    Nfirstrec++ ;
    } ;
  m += Plentot ;
  } ;

/* Nextra is the number of redundant packets that need to be processed */

Nextra = Mpackets - Nfirstrec ;
#ifdef PRINT
printf("Nfirstrec= %d, Nextra= %d \n",Nfirstrec,Nextra) ;
#endif

/* Compute the indices of the missing words in the message */

col_ind = 0 ;
for (i=0; i < Mpackets ; i++)
  {
  if (Rec_index[i] == 0)
    Col_Ind[col_ind++] = i ;
  } ;

/* Keep track of indices of extra packets in Row_Ind array
   and initialize M array from the received extra packets */

row_ind = 0 ;
m = 0 ;
for (i=0; i < *pNrec ; i++)
  {
  if (rec_packets[m] >= Mpackets)
    {
     k = Nsegs*row_ind*Lfield ;
     Row_Ind[row_ind] = rec_packets[m] - Mpackets ;
     for (row_eqn=0 ; row_eqn < Lfield ; row_eqn++) 
      {
       j = row_eqn*Nsegs ;
       for (seg_ind=0 ; seg_ind < Nsegs ; seg_ind++)
         {
          M[k] = rec_packets[m+1+seg_ind+j] ;
          k++ ;
         } ;
      } ;
     row_ind ++ ;
     if (row_ind >= Nextra) break ; 
    } ;
  m += Plentot ;
  } ;

/* Adjust M array according to the equations and the contents of rec_message */

for (row_ind = 0 ; row_ind < Nextra ; row_ind++)
  {
  for (col_ind=0 ; col_ind < Mpackets ; col_ind++)
    {
    if (Rec_index[col_ind] == 1)
      {   
      ExpFE = (SMultField - FEtoExp[Row_Ind[row_ind]^col_ind^COLBIT]) % SMultField ;
      for (row_eqn=0 ; row_eqn < Lfield ; row_eqn++)
        {
        j = Nsegs*(row_eqn + row_ind*Lfield) ;
        for (col_eqn=0 ; col_eqn < Lfield ; col_eqn++)
          {
          k = Nsegs*(col_eqn + col_ind*Lfield) ;
          if (ExptoFE[ExpFE+row_eqn] & BIT[col_eqn])
            {
            for (seg_ind=0 ; seg_ind < Nsegs ; seg_ind++)
              {
              M[j+seg_ind] ^= rec_message[k+seg_ind] ;
              } ;
            } ;
          } ;
        } ;
      } ;
    } ;
  } ;

/* Compute the determinant of the matrix in the finite field
   and then compute the inverse matrix */

for (row_ind = 0 ; row_ind < Nextra ; row_ind++)
  {
  for (col_ind = 0 ; col_ind < Nextra ; col_ind++)
    {
    if (col_ind != row_ind)
      {
      C[row_ind] += FEtoExp[Row_Ind[row_ind]^Row_Ind[col_ind]] ;
      D[col_ind] += FEtoExp[Col_Ind[row_ind]^Col_Ind[col_ind]] ;
      } ;
    E[row_ind] += FEtoExp[Row_Ind[row_ind]^Col_Ind[col_ind]^COLBIT] ;
    F[col_ind] += FEtoExp[Row_Ind[row_ind]^Col_Ind[col_ind]^COLBIT] ;
    } ;
  } ;

/* Fill in the recovered information in the message from
   the inverted matrix and from M */

for (row_ind = 0 ; row_ind < Nextra ; row_ind++)
  {
  for (col_ind = 0 ; col_ind < Nextra ; col_ind++)
    {
    ExpFE = E[col_ind] + F[row_ind] - C[col_ind] - D[row_ind] 
      - FEtoExp[Row_Ind[col_ind]^Col_Ind[row_ind]^COLBIT] ;
    if (ExpFE < 0) ExpFE = SMultField-((-ExpFE) % SMultField) ;
    ExpFE = ExpFE % SMultField ;
    j = Col_Ind[row_ind] * Lfield * Nsegs ;
    for (row_eqn=0 ; row_eqn < Lfield ; row_eqn++)
      {
      k = row_eqn*Nsegs + j ;
      for (col_eqn=0 ; col_eqn < Lfield ; col_eqn++)
        {
        l = Nsegs *(col_eqn + col_ind*Lfield) ;
        if (ExptoFE[ExpFE+row_eqn] & BIT[col_eqn])
          {
          for (seg_ind=0 ; seg_ind < Nsegs ; seg_ind++)
            {
             rec_message[seg_ind+k] ^= M[l] ;
             l++ ;
            } ;
          } ;
        } ;
      } ;
    } ;
  } ;

gettimeofday(&end_time,0) ;
#ifdef PRINT
printf ("\n------------------------------------------------") ;
printf ("\n decode: number of seconds is %f",
     (float)(end_time.tv_sec-start_time.tv_sec)+
      (float)(end_time.tv_usec-start_time.tv_usec)/1000000.0) ; 
printf ("\n------------------------------------------------\n\n") ;
fflush(stdout) ;
#endif

return 0 ;
}
