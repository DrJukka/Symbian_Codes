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

/* rijndael-api-ref.h   v2.0   August '99
 * Reference ANSI C code
 */

/*  AES Cipher header file for ANSI C Submissions
      Lawrence E. Bassham III
      Computer Security Division
      National Institute of Standards and Technology

      April 15, 1998

    This sample is to assist implementers developing to the Cryptographic 
API Profile for AES Candidate Algorithm Submissions.  Please consult this 
document as a cross-reference.

    ANY CHANGES, WHERE APPROPRIATE, TO INFORMATION PROVIDED IN THIS FILE
MUST BE DOCUMENTED.  CHANGES ARE ONLY APPROPRIATE WHERE SPECIFIED WITH
THE STRING "CHANGE POSSIBLE".  FUNCTION CALLS AND THEIR PARAMETERS CANNOT 
BE CHANGED.  STRUCTURES CAN BE ALTERED TO ALLOW IMPLEMENTERS TO INCLUDE 
IMPLEMENTATION SPECIFIC INFORMATION.
*/

// *** p.henkel
#ifndef __RIJNDAEL_API_H
#define __RIJNDAEL_API_H

/*  Includes:
	Standard include files
*/

// *** p.henkel: include not needed
//#include <stdio.h>

// *** p.henkel: file names changed
#include "rijndael-alg-symbianos.h"

//  *** p.henkel 
#include <e32std.h>

// *** p.henkel
namespace aes {

#pragma warning( disable : 4244 )	// conversion from 'int' to 'unsigned char', possible loss of data



/*  Defines:
	Add any additional defines you need
*/

#define     DIR_ENCRYPT     0    /*  Are we encrpyting?  */
#define     DIR_DECRYPT     1    /*  Are we decrpyting?  */
#define     MODE_ECB        1    /*  Are we ciphering in ECB mode?   */
#define     MODE_CBC        2    /*  Are we ciphering in CBC mode?   */
#define     MODE_CFB1       3    /*  Are we ciphering in 1-bit CFB mode? */

// *** p.henkel: already defined in E32DEF.H
/*
#define     TRUE            1
#define     FALSE           0
*/

#define	BITSPERBLOCK		128		/* Default number of bits in a cipher block */

/*  Error Codes - CHANGE POSSIBLE: inclusion of additional error codes  */
#define     BAD_KEY_DIR        -1  /*  Key direction is invalid, e.g.,
					unknown value */
#define     BAD_KEY_MAT        -2  /*  Key material not of correct 
					length */
#define     BAD_KEY_INSTANCE   -3  /*  Key passed is not valid  */
#define     BAD_CIPHER_MODE    -4  /*  Params struct passed to 
					cipherInit invalid */
#define     BAD_CIPHER_STATE   -5  /*  Cipher in wrong state (e.g., not 
					initialized) */
#define     BAD_CIPHER_INSTANCE   -7 


/*  CHANGE POSSIBLE:  inclusion of algorithm specific defines  */
#define     MAX_KEY_SIZE	64  /* # of ASCII char's needed to
					represent a key */
#define     MAX_IV_SIZE		BITSPERBLOCK/8  /* # bytes needed to
					represent an IV  */

/*  Typedefs:

	Typedef'ed data storage elements.  Add any algorithm specific 
parameters at the bottom of the structs as appropriate.
*/

// *** p.henkel: replaced typedef
// typedef    unsigned char    BYTE;
typedef TUint8 BYTE;

// *** p.henkel: replaced BYTE with TUint8 in the following structures
//               replaced char with TText8, int with TInt
/*  The structure for key information */
typedef struct {
      TUint8  direction;	/*  Key used for encrypting or decrypting? */
      TInt   keyLen;	/*  Length of the key  */
      TText8  keyMaterial[MAX_KEY_SIZE+1];  /*  Raw key data in ASCII,
                                    e.g., user input or KAT values */
      /*  The following parameters are algorithm dependent, replace or
      		add as necessary  */
      TInt   blockLen;   /* block length */
      word8 keySched[MAXROUNDS+1][4][MAXBC];	/* key schedule		*/
      } keyInstance;

/*  The structure for cipher information */
typedef struct {
      TUint8  mode;            /* MODE_ECB, MODE_CBC, or MODE_CFB1 */
      TUint8  IV[MAX_IV_SIZE]; /* A possible Initialization Vector for 
      					ciphering */
      /*  Add any algorithm specific parameters needed here  */
      TInt   blockLen;    	/* Sample: Handles non-128 bit block sizes
      					(if available) */
      } cipherInstance;


/*  Function protoypes  */
/*  CHANGED: makeKey(): parameter blockLen added
                        this parameter is absolutely necessary if you want to
			setup the round keys in a variable block length setting 
	     cipherInit(): parameter blockLen added (for obvious reasons)		
 */
 
// *** p.henkel: replaced BYTE with TUint8 in the following function declarations
//               replaced char with TText8, int with TInt
//               added const if possible
TInt makeKey(keyInstance *key, TUint8 direction, TInt keyLen,const TDesC8& aKeyMaterial);

TInt cipherInit(cipherInstance *cipher, TUint8 mode, const TText8 *IV);

TInt blockEncrypt(const cipherInstance *cipher, keyInstance *key, TUint8 *input, 
			TInt inputLen, TUint8 *outBuffer);

TInt blockDecrypt(const cipherInstance *cipher, keyInstance *key, TUint8 *input,
			TInt inputLen, TUint8 *outBuffer);
			
TInt cipherUpdateRounds(const cipherInstance *cipher, keyInstance *key, TUint8 *input, 
                        TInt inputLen, TUint8 *outBuffer, TInt Rounds);


} // namespace

// *** p.henkel
#endif // __RIJNDAEL_API_H

