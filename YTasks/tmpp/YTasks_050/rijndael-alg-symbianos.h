/* 
 * Rijndael port to Symbian OS
 *
 * bit-side GmbH 2005
 * www.bit-side.com
 *
 * Author: Philipp Henkel <p.henkel@bit-side.com>
 * 
 * US National Institute of Standards and Technology 
 * chose the Rijndael block cipher as Advanced Encryption
 * Standard (AES).
 * http://csrc.nist.gov/CryptoToolkit/aes/rijndael
 * 
 * This code is derived from the original C reference code
 * and it is placed in the public domain.
 *
 * All changes are marked which with the tag "*** p.henkel".
 *
 * The code is distributed on an 'AS IS' basis, WITHOUT 
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 *
 * The original code comes with the following notice:
 */

/* rijndael-alg-ref.h   v2.0   August '99
 * Reference ANSI C code
 * authors: Paulo Barreto
 *          Vincent Rijmen, K.U.Leuven
 */
#ifndef __RIJNDAEL_ALG_H
#define __RIJNDAEL_ALG_H



//  *** p.henkel 
#include <e32std.h>

//  *** p.henkel
#pragma warning( disable : 4244 )	// conversion from 'int' to 'unsigned char', possible loss of data


#define MAXBC				(256/32)
#define MAXKC				(256/32)
#define MAXROUNDS			14

// *** p.henkel
namespace aes {

// *** p.henkel: typedefs to SymbianOS types
/*
typedef unsigned char		word8;	
typedef unsigned short		word16;	
typedef unsigned long		word32;
*/
typedef TUint8  word8;	
typedef TUint16 word16;	
typedef TUint32	word32;


int rijndaelKeySched (word8 k[4][MAXKC], int keyBits, int blockBits, 
		word8 rk[MAXROUNDS+1][4][MAXBC]);
int rijndaelEncrypt (word8 a[4][MAXBC], int keyBits, int blockBits, 
		word8 rk[MAXROUNDS+1][4][MAXBC]);
int rijndaelEncryptRound (word8 a[4][MAXBC], int keyBits, int blockBits, 
		word8 rk[MAXROUNDS+1][4][MAXBC], int rounds);
int rijndaelDecrypt (word8 a[4][MAXBC], int keyBits, int blockBits, 
		word8 rk[MAXROUNDS+1][4][MAXBC]);
int rijndaelDecryptRound (word8 a[4][MAXBC], int keyBits, int blockBits, 
		word8 rk[MAXROUNDS+1][4][MAXBC], int rounds);


} // namespace

#endif /* __RIJNDAEL_ALG_H */
