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

/* rijndael-api-ref.c   v2.1   April 2000
 * Reference ANSI C code
 * authors: v2.0 Paulo Barreto
 *               Vincent Rijmen, K.U.Leuven
 *          v2.1 Vincent Rijmen, K.U.Leuven
 *
 * This code is placed in the public domain.
 */
 
// *** p.henkel: includes not needed
/*
#include <stdlib.h>
#include <string.h>
*/



// *** p.henkel: file names changed
#include "rijndael-alg-symbianos.h"
#include "rijndael-api-symbianos.h"

using namespace aes;

// *** p.henkel: parameter types adapted
int aes::makeKey(keyInstance *key, TUint8 direction, int keyLen, const TDesC8& aKeyMaterial)
{
	word8 k[4][MAXKC];
	int i, j, t;
	
	if (key == NULL) {
		return BAD_KEY_INSTANCE;
	}

	if ((direction == DIR_ENCRYPT) || (direction == DIR_DECRYPT)) {
		key->direction = direction;
	} else {
		return BAD_KEY_DIR;
	}

	if ((keyLen == 128) || (keyLen == 192) || (keyLen == 256)) { 
		key->keyLen = keyLen;
	} else {
		return BAD_KEY_MAT;
	}

	if (aKeyMaterial.Length()) 
	{
    
    // *** p.henkel: replaced strncpy	  
		//strncpy(key->keyMaterial, keyMaterial, keyLen/4);
	
	TPtr8 key_keyMaterial(key->keyMaterial, keyLen/4);
    key_keyMaterial.Zero();
    
    	if(aKeyMaterial.Length() > (keyLen/4))
    	{
    		key_keyMaterial.Append(aKeyMaterial.Left(keyLen/4));
    	}
    	else
    	{
    		key_keyMaterial.Append(aKeyMaterial);
    	}
	}

	/* initialize key schedule: */ 
 	for(i = 0; i < key->keyLen/8; i++) {
		t = key->keyMaterial[2*i];
		if ((t >= '0') && (t <= '9')) j = (t - '0') << 4;
		else if ((t >= 'a') && (t <= 'f')) j = (t - 'a' + 10) << 4; 
		else if ((t >= 'A') && (t <= 'F')) j = (t - 'A' + 10) << 4; 
		else return BAD_KEY_MAT;
		
		t = key->keyMaterial[2*i+1];
		if ((t >= '0') && (t <= '9')) j ^= (t - '0');
		else if ((t >= 'a') && (t <= 'f')) j ^= (t - 'a' + 10); 
		else if ((t >= 'A') && (t <= 'F')) j ^= (t - 'A' + 10); 
		else return BAD_KEY_MAT;
		
		k[i % 4][i / 4] = (word8) j; 
	}	
	
	rijndaelKeySched (k, key->keyLen, key->blockLen, key->keySched);
	
	return TRUE;
}
   
// *** p.henkel: parameter types adapted               
int aes::cipherInit(cipherInstance *cipher, TUint8 mode, const TText8 *IV)
{
	int i, j, t;
	
	if ((mode == MODE_ECB) || (mode == MODE_CBC) || (mode == MODE_CFB1)) {
		cipher->mode = mode;
	} else {
		return BAD_CIPHER_MODE;
	}
	
	if (IV != NULL) {
 		for(i = 0; i < cipher->blockLen/8; i++) {		
			t = IV[2*i];
			if ((t >= '0') && (t <= '9')) j = (t - '0') << 4;
			else if ((t >= 'a') && (t <= 'f')) j = (t - 'a' + 10) << 4; 
			else if ((t >= 'A') && (t <= 'F')) j = (t - 'A' + 10) << 4; 
			else return BAD_CIPHER_INSTANCE;
		
			t = IV[2*i+1];
			if ((t >= '0') && (t <= '9')) j ^= (t - '0');
			else if ((t >= 'a') && (t <= 'f')) j ^= (t - 'a' + 10); 
			else if ((t >= 'A') && (t <= 'F')) j ^= (t - 'A' + 10); 
			else return BAD_CIPHER_INSTANCE;
			
			cipher->IV[i] = (BYTE) j;
		} 
	}

	return TRUE;
}


// *** p.henkel: parameter types adapted
int aes::blockEncrypt(const cipherInstance *cipher,
	keyInstance *key, TUint8 *input, TInt inputLen, TUint8 *outBuffer)
{
	int i, j, t, numBlocks;
	word8 block[4][MAXBC];

	
        /* check parameter consistency: */
        if (key == NULL ||
                key->direction != DIR_ENCRYPT ||
                (key->keyLen != 128 && key->keyLen != 192 && key->keyLen != 256)) {
                return BAD_KEY_MAT;
        }
        if (cipher == NULL ||
                (cipher->mode != MODE_ECB && cipher->mode != MODE_CBC && cipher->mode != MODE_CFB1) ||
                (cipher->blockLen != 128 && cipher->blockLen != 192 && cipher->blockLen != 256)) {
                return BAD_CIPHER_STATE;
        }


	numBlocks = inputLen/cipher->blockLen;
	
	switch (cipher->mode) {
	case MODE_ECB: 
		for (i = 0; i < numBlocks; i++) {
			for (j = 0; j < cipher->blockLen/32; j++) {
				for(t = 0; t < 4; t++)
				/* parse input stream into rectangular array */
					block[t][j] = input[cipher->blockLen/8*i+4*j+t] & 0xFF;
			}
			rijndaelEncrypt (block, key->keyLen, cipher->blockLen, key->keySched);
			for (j = 0; j < cipher->blockLen/32; j++) {
				/* parse rectangular array into output ciphertext bytes */
				for(t = 0; t < 4; t++)
					outBuffer[cipher->blockLen/8*i+4*j+t] = (BYTE) block[t][j];
			}
		}
		break;
		
	case MODE_CBC:
		for (j = 0; j < cipher->blockLen/32; j++) {
			for(t = 0; t < 4; t++)
			/* parse initial value into rectangular array */
					block[t][j] = cipher->IV[t+4*j] & 0xFF;
			}
		for (i = 0; i < numBlocks; i++) {
			for (j = 0; j < cipher->blockLen/32; j++) {
				for(t = 0; t < 4; t++)
				/* parse input stream into rectangular array and exor with 
				   IV or the previous ciphertext */
					block[t][j] ^= input[cipher->blockLen/8*i+4*j+t] & 0xFF;
			}
			rijndaelEncrypt (block, key->keyLen, cipher->blockLen, key->keySched);
			for (j = 0; j < cipher->blockLen/32; j++) {
				/* parse rectangular array into output ciphertext bytes */
				for(t = 0; t < 4; t++)
					outBuffer[cipher->blockLen/8*i+4*j+t] = (BYTE) block[t][j];
			}
		}
		break;
	
	default: return BAD_CIPHER_STATE;
	}
	
	return numBlocks*cipher->blockLen;
}


// *** p.henkel: parameter types adapted
int aes::blockDecrypt(const cipherInstance *cipher,
	keyInstance *key, TUint8 *input, TInt inputLen, TUint8 *outBuffer)
{
	int i, j, t, numBlocks;
	word8 block[4][MAXBC];

	if (cipher == NULL ||
		key == NULL ||
		key->direction == DIR_ENCRYPT ||
		cipher->blockLen != key->blockLen) {
		return BAD_CIPHER_STATE;
	}

        /* check parameter consistency: */
        if (key == NULL ||
                key->direction != DIR_DECRYPT ||
                (key->keyLen != 128 && key->keyLen != 192 && key->keyLen != 256)) {
                return BAD_KEY_MAT;
        }
        if (cipher == NULL ||
                (cipher->mode != MODE_ECB && cipher->mode != MODE_CBC && cipher->mode != MODE_CFB1) ||
                (cipher->blockLen != 128 && cipher->blockLen != 192 && cipher->blockLen != 256)) {
                return BAD_CIPHER_STATE;
        }
	

	numBlocks = inputLen/cipher->blockLen;
	
	switch (cipher->mode) {
	case MODE_ECB: 
		for (i = 0; i < numBlocks; i++) {
			for (j = 0; j < cipher->blockLen/32; j++) {
				for(t = 0; t < 4; t++)
				/* parse input stream into rectangular array */
					block[t][j] = input[cipher->blockLen/8*i+4*j+t] & 0xFF;
			}
			rijndaelDecrypt (block, key->keyLen, cipher->blockLen, key->keySched);
			for (j = 0; j < cipher->blockLen/32; j++) {
				/* parse rectangular array into output ciphertext bytes */
				for(t = 0; t < 4; t++)
					outBuffer[cipher->blockLen/8*i+4*j+t] = (BYTE) block[t][j];
			}
		}
		break;
		
	case MODE_CBC:
		/* first block */
		for (j = 0; j < cipher->blockLen/32; j++) {
			for(t = 0; t < 4; t++)
			/* parse input stream into rectangular array */
				block[t][j] = input[4*j+t] & 0xFF;
		}
		rijndaelDecrypt (block, key->keyLen, cipher->blockLen, key->keySched);
		
		for (j = 0; j < cipher->blockLen/32; j++) {
			/* exor the IV and parse rectangular array into output ciphertext bytes */
			for(t = 0; t < 4; t++)
				outBuffer[4*j+t] = (BYTE) (block[t][j] ^ cipher->IV[t+4*j]);
		}
		
		/* next blocks */
		for (i = 1; i < numBlocks; i++) {
			for (j = 0; j < cipher->blockLen/32; j++) {
				for(t = 0; t < 4; t++)
				/* parse input stream into rectangular array */
					block[t][j] = input[cipher->blockLen/8*i+4*j+t] & 0xFF;
			}
			rijndaelDecrypt (block, key->keyLen, cipher->blockLen, key->keySched);
			
			for (j = 0; j < cipher->blockLen/32; j++) {
				/* exor previous ciphertext block and parse rectangular array 
				       into output ciphertext bytes */
				for(t = 0; t < 4; t++)
					outBuffer[cipher->blockLen/8*i+4*j+t] = (BYTE) (block[t][j] ^ 
						input[cipher->blockLen/8*i+4*j+t-4*cipher->blockLen/32]);
			}
		}
		break;
	
	default: return BAD_CIPHER_STATE;
	}
	
	return numBlocks*cipher->blockLen;
}


/**
 *	cipherUpdateRounds:
 *
 *	Encrypts/Decrypts exactly one full block a specified number of rounds.
 *	Only used in the Intermediate Value Known Answer Test.	
 *
 *	Returns:
 *		TRUE - on success
 *		BAD_CIPHER_STATE - cipher in bad state (e.g., not initialized)
 */
 // *** p.henkel: parameter types adapted
int aes::cipherUpdateRounds(const cipherInstance *cipher,
	keyInstance *key, TUint8 *input, int /*inputLen*/, TUint8 *outBuffer, TInt rounds)
{
	int j, t;
	word8 block[4][MAXBC];

	if (cipher == NULL ||
		key == NULL ||
		cipher->blockLen != key->blockLen) {
		return BAD_CIPHER_STATE;
	}

	for (j = 0; j < cipher->blockLen/32; j++) {
		for(t = 0; t < 4; t++)
			/* parse input stream into rectangular array */
			block[t][j] = input[4*j+t] & 0xFF;
	}
	switch (key->direction) {
	case DIR_ENCRYPT:
		rijndaelEncryptRound (block, key->keyLen, cipher->blockLen, 
				key->keySched, rounds);
	break;
		
	case DIR_DECRYPT:
		rijndaelDecryptRound (block, key->keyLen, cipher->blockLen, 
				key->keySched, rounds);
	break;
		
	default: return BAD_KEY_DIR;
	} 
	for (j = 0; j < cipher->blockLen/32; j++) {
		/* parse rectangular array into output ciphertext bytes */
		for(t = 0; t < 4; t++)
			outBuffer[4*j+t] = (BYTE) block[t][j];
	}
	
	return TRUE;
}

