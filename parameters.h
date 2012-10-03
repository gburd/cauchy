/*  pet.h  */

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

/* verbose mode */

#define PRINT

/* Niter is the number of messages sent. */

#define Niter 1

/* Lfield is the log of the length of the field. */

#define	Lfield 10

/* Nsegs is the number of segments in packet. */
/* Length of packet in bytes is 4*Nsegs*Lfield */

#define	Nsegs 25

/* Mpackets is the number of message packets */

#define	Mpackets 200

/* Rpackets is the number of redundant packets */

#define Rpackets 200

/* IMPORTANT: The max of Mpackets and Rpackets is at most 2^{Lfield-1} */
/* Lfield must be set large enough to make this true else the encoding
   and decoding won't work */

/* Npackets is the total number of packets sent.
   Mseglen is the message segment length.
   Plen is the packet length in words excluding the overhead 
        for storing the index.
   Plentot is the packet length in words including the overhead
        for storing the index.
   Mlen is the length of the message in words
   Elen is the length of the encoding in words 
   TableLength is 2^{Lfield}
   SMultField is the size of the multiplicative field (2^{Lfield}-1) 
*/
 
/* THIS INFORMATION DEPENDS ON THE LENGTH OF THE FIELD (set above) */

enum {Npackets = Mpackets + Rpackets,
      Mseglen = Mpackets * Lfield,
      Plen = Nsegs * Lfield,
      Plentot = Plen + 1,
      Mlen = Plen * Mpackets,
      Elen = Plen * (Mpackets + Rpackets),
      TableLength = 1 << Lfield,
      SMultField = TableLength - 1};

typedef unsigned int UNSIGNED ;
