/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#include "SDES_interface.h" 
#include "md5.h"


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDESHandler* CDESHandler::NewL(void)
    {
    CDESHandler* self = CDESHandler::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDESHandler* CDESHandler::NewLC(void)
    {
    CDESHandler* self = new (ELeave) CDESHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDESHandler::CDESHandler()
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDESHandler::~CDESHandler()
{
	delete iMD5_Handler;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler::ConstructL(void)
{
	iMD5_Handler = new(ELeave)CMD5_Handler();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_encryptLC(TDes& aKey, TDes& aData)
{	
	TInt AddLen = ((8 - ((aData.Length() * 2) % 8)) / 2);  
	
	for(TInt i=0 ; i < AddLen ; i++)
	{
		aData.Append(TChar(' '));
	}
	
	struct MD5Context md5c;
	TUint8 keybuf[16];
	  
	TUint8 *KeyPtr =(TUint8 *) aKey.Ptr();
	 
    iMD5_Handler->MD5Init(&md5c);
	iMD5_Handler->MD5Update(&md5c, KeyPtr, aKey.Size());
	iMD5_Handler->MD5Final(keybuf, &md5c);
	
	TUint8 *data =(TUint8 *) aData.Ptr();
	DES_encryptLC(keybuf,data,(aData.Length() * 2));
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_encryptLC(TDes8& aKey, TDes8& aData)
{	
	TInt AddLen = (8 - (aData.Length() % 8));  
	
	for(TInt i=0 ; i < AddLen ; i++)
	{
		aData.Append(TChar(' '));
	}
	
	struct MD5Context md5c;
	TUint8 keybuf[16];
	
	TUint8 *KeyPtr = (TUint8 *) aKey.Ptr();
	  
    iMD5_Handler->MD5Init(&md5c);
	iMD5_Handler->MD5Update(&md5c,KeyPtr, aKey.Size());
	iMD5_Handler->MD5Final(keybuf, &md5c);


	TUint8 *data =(TUint8 *) aData.Ptr();
	DES_encryptLC(keybuf,data,aData.Length());
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_encryptLC(TDes& aKey, TDes8& aData)
{	
	TInt AddLen = (8 - (aData.Length() % 8));  
	
	for(TInt i=0 ; i < AddLen ; i++)
	{
		aData.Append(TChar(' '));
	}
	
	struct MD5Context md5c;
	TUint8 keybuf[16];
	
	TUint8 *KeyPtr = (TUint8 *) aKey.Ptr();
	  
    iMD5_Handler->MD5Init(&md5c);
	iMD5_Handler->MD5Update(&md5c,KeyPtr, aKey.Size());
	iMD5_Handler->MD5Final(keybuf, &md5c);


	TUint8 *data =(TUint8 *) aData.Ptr();
	DES_encryptLC(keybuf,data,aData.Length());
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_encryptLC(TUint8 *aKey,TUint8 *aData,TInt aLength)
{	
    DESContext ourkeys[3];
    
    des_key_setup(GET_32BIT_MSB_FIRST(aKey),
		  GET_32BIT_MSB_FIRST(aKey + 4), &ourkeys[0]);
		  
    des_key_setup(GET_32BIT_MSB_FIRST(aKey + 8),
		  GET_32BIT_MSB_FIRST(aKey + 12), &ourkeys[1]);
		  
    des_key_setup(GET_32BIT_MSB_FIRST(aKey),
		  GET_32BIT_MSB_FIRST(aKey + 4), &ourkeys[2]);
	
    des_3cbc_encrypt(aData,aData,aLength, ourkeys);
    
    TPtr8 Zeroingpointer((TUint8*)ourkeys,sizeof(ourkeys));
    Zeroingpointer.FillZ(sizeof(ourkeys));
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_decryptLC(TDes& aKey, TDes& aData)
{ 
    TUint8 *data =(TUint8 *) aData.Ptr();

	struct MD5Context md5c;
	TUint8 keybuf[16];
	
	TUint8 *KeyPtr =(TUint8 *) aKey.Ptr();	
	  
    iMD5_Handler->MD5Init(&md5c);
	iMD5_Handler->MD5Update(&md5c, KeyPtr, aKey.Size());
	iMD5_Handler->MD5Final(keybuf, &md5c);	

    DES_decryptLC(keybuf,data,(aData.Length() *2));
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_decryptLC(TDes8& aKey, TDes8& aData)
{
    TUint8 *data =(TUint8 *) aData.Ptr();

	struct MD5Context md5c;
	TUint8 keybuf[16];
	
	TUint8 *KeyPtr = (TUint8 *) aKey.Ptr();
	  
    iMD5_Handler->MD5Init(&md5c);
	iMD5_Handler->MD5Update(&md5c,KeyPtr, aKey.Size());
	iMD5_Handler->MD5Final(keybuf, &md5c);		

    DES_decryptLC(keybuf,data,aData.Length());
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_decryptLC(TDes& aKey, TDes8& aData)
{
    TUint8 *data =(TUint8 *) aData.Ptr();

	struct MD5Context md5c;
	TUint8 keybuf[16];
	
	TUint8 *KeyPtr = (TUint8 *) aKey.Ptr();
	  
    iMD5_Handler->MD5Init(&md5c);
	iMD5_Handler->MD5Update(&md5c,KeyPtr, aKey.Size());
	iMD5_Handler->MD5Final(keybuf, &md5c);		

    DES_decryptLC(keybuf,data,aData.Length());
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: DES_decryptLC(TUint8 *aKey,TUint8 *aData,TInt aLength)
{
  	DESContext ourkeys[3];

    des_key_setup(GET_32BIT_MSB_FIRST(aKey),
		  GET_32BIT_MSB_FIRST(aKey + 4), &ourkeys[0]);
    des_key_setup(GET_32BIT_MSB_FIRST(aKey + 8),
		  GET_32BIT_MSB_FIRST(aKey + 12), &ourkeys[1]);
    des_key_setup(GET_32BIT_MSB_FIRST(aKey),
		  GET_32BIT_MSB_FIRST(aKey + 4), &ourkeys[2]);
    
    des_3cbc_decrypt(aData, aData,aLength, ourkeys);
    
    TPtr8 Zeroingpointer((TUint8*)ourkeys,sizeof(ourkeys));
    Zeroingpointer.FillZ(sizeof(ourkeys));
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: des_3cbc_encrypt(TUint8 *dest, const TUint8 *src,
			     TUint len, DESContext * scheds)
{
    des_cbc_encrypt(dest, src, len, &scheds[0]);
    des_cbc_decrypt(dest, src, len, &scheds[1]);
    des_cbc_encrypt(dest, src, len, &scheds[2]);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: des_3cbc_decrypt(TUint8 *dest, const TUint8 *src,
			     TUint len, DESContext * scheds)
{
    des_cbc_decrypt(dest, src, len, &scheds[2]);
    des_cbc_encrypt(dest, src, len, &scheds[1]);
    des_cbc_decrypt(dest, src, len, &scheds[0]);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: des_key_setup(TUint32 key_msw, TUint32 key_lsw, DESContext * sched)
{

    static const TInt PC1_Cbits[] = 
    {
		7, 15, 23, 31, 39, 47, 55, 63, 6, 14, 22, 30, 38, 46,
		54, 62, 5, 13, 21, 29, 37, 45, 53, 61, 4, 12, 20, 28
    };
    static const TInt PC1_Dbits[] = 
    {
		1, 9, 17, 25, 33, 41, 49, 57, 2, 10, 18, 26, 34, 42,
		50, 58, 3, 11, 19, 27, 35, 43, 51, 59, 36, 44, 52, 60
    };
    /*
     * The bit numbers in the two lists below don't correspond to
     * the ones in the above description of PC2, because in the
     * above description C and D are concatenated so `bit 28' means
     * bit 0 of C. In this implementation we're using the standard
     * `bitsel' function above and C is in the second word, so bit
     * 0 of C is addressed by writing `32' here.
     */
    static const TInt PC2_0246[] = 
    {
		49, 36, 59, 55, -1, -1, 37, 41, 48, 56, 34, 52, -1, -1, 15, 4,
		25, 19, 9, 1, -1, -1, 12, 7, 17, 0, 22, 3, -1, -1, 46, 43
    };
    static const TInt PC2_1357[] = 
    {
		-1, -1, 57, 32, 45, 54, 39, 50, -1, -1, 44, 53, 33, 40, 47, 58,
		-1, -1, 26, 16, 5, 11, 23, 8, -1, -1, 10, 14, 6, 20, 27, 24
    };
    static const TInt leftshifts[] =
	{ 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

    TUint32 C, D;
    TUint32 buf[2];
    TInt i;

    buf[0] = key_lsw;
    buf[1] = key_msw;

    C = bitsel(buf, PC1_Cbits, 28);
    D = bitsel(buf, PC1_Dbits, 28);

    for (i = 0; i < 16; i++) 
    {
		C = rotl28(C, leftshifts[i]);
		D = rotl28(D, leftshifts[i]);
		buf[0] = D;
		buf[1] = C;
		sched->k0246[i] = bitsel(buf, PC2_0246, 32);
		sched->k1357[i] = bitsel(buf, PC2_1357, 32);
    }

    sched->eiv0 = sched->eiv1 = 0;
    sched->div0 = sched->div1 = 0;     /* for good measure */
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TUint32 CDESHandler :: bitsel(TUint32 * input, const TInt *bitnums, TInt size)
{
    TUint32 ret = 0;
    while (size--) 
    {
		TInt bitpos = *bitnums++;
		ret <<= 1;
		if (bitpos >= 0)
	    	ret |= 1 & (input[bitpos / 32] >> (bitpos % 32));
    }
    return ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler ::des_cbc_decrypt(TUint8 *dest, const TUint8 *src,
			    TUint len, DESContext * sched)
{
    TUint32 out[2], iv0, iv1, xL, xR;
    TUint i;

    ASSERT((len & 7) == 0);

    iv0 = sched->div0;
    iv1 = sched->div1;
    
    for (i = 0; i < len; i += 8) 
    {
		xL = GET_32BIT_MSB_FIRST(src);
		src += 4;
		xR = GET_32BIT_MSB_FIRST(src);
		src += 4;
	
		des_decipher(out, xL, xR, sched);
	
		iv0 ^= out[0];
		iv1 ^= out[1];
		PUT_32BIT_MSB_FIRST(dest, iv0);
		dest += 4;
		PUT_32BIT_MSB_FIRST(dest, iv1);
		dest += 4;
		iv0 = xL;
		iv1 = xR;
    }
    
    sched->div0 = iv0;
    sched->div1 = iv1;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CDESHandler :: des_cbc_encrypt(TUint8 *dest, const TUint8 *src,
			    TUint len, DESContext * sched)
{
    TUint32 out[2], iv0, iv1;
    TUint i;

    ASSERT((len & 7) == 0);

    iv0 = sched->eiv0;
    iv1 = sched->eiv1;
    for (i = 0; i < len; i += 8) 
    {
		iv0 ^= GET_32BIT_MSB_FIRST(src);
		src += 4;
		iv1 ^= GET_32BIT_MSB_FIRST(src);
		src += 4;
	
		des_encipher(out, iv0, iv1, sched);
	
		iv0 = out[0];
		iv1 = out[1];
		PUT_32BIT_MSB_FIRST(dest, iv0);
		dest += 4;
		PUT_32BIT_MSB_FIRST(dest, iv1);
		dest += 4;
    }
    
    sched->eiv0 = iv0;
    sched->eiv1 = iv1;
}
 /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/   
void CDESHandler ::des_decipher(TUint32 * output, TUint32 L, TUint32 R, DESContext * sched)
{
    TUint32 swap, s0246, s1357;

    IP(L, R);

    L = rotl(L, 1);
    R = rotl(R, 1);

    L ^= f(R, sched->k0246[15], sched->k1357[15]);
    R ^= f(L, sched->k0246[14], sched->k1357[14]);
    L ^= f(R, sched->k0246[13], sched->k1357[13]);
    R ^= f(L, sched->k0246[12], sched->k1357[12]);
    L ^= f(R, sched->k0246[11], sched->k1357[11]);
    R ^= f(L, sched->k0246[10], sched->k1357[10]);
    L ^= f(R, sched->k0246[9], sched->k1357[9]);
    R ^= f(L, sched->k0246[8], sched->k1357[8]);
    L ^= f(R, sched->k0246[7], sched->k1357[7]);
    R ^= f(L, sched->k0246[6], sched->k1357[6]);
    L ^= f(R, sched->k0246[5], sched->k1357[5]);
    R ^= f(L, sched->k0246[4], sched->k1357[4]);
    L ^= f(R, sched->k0246[3], sched->k1357[3]);
    R ^= f(L, sched->k0246[2], sched->k1357[2]);
    L ^= f(R, sched->k0246[1], sched->k1357[1]);
    R ^= f(L, sched->k0246[0], sched->k1357[0]);

    L = rotl(L, 31);
    R = rotl(R, 31);

    swap = L;
    L = R;
    R = swap;

    FP(L, R);

    output[0] = L;
    output[1] = R;
}
 /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  
void CDESHandler :: des_encipher(TUint32 * output, TUint32 L, TUint32 R, DESContext * sched)
{
    TUint32 swap, s0246, s1357;

    IP(L, R);

    L = rotl(L, 1);
    R = rotl(R, 1);

    L ^= f(R, sched->k0246[0], sched->k1357[0]);
    R ^= f(L, sched->k0246[1], sched->k1357[1]);
    L ^= f(R, sched->k0246[2], sched->k1357[2]);
    R ^= f(L, sched->k0246[3], sched->k1357[3]);
    L ^= f(R, sched->k0246[4], sched->k1357[4]);
    R ^= f(L, sched->k0246[5], sched->k1357[5]);
    L ^= f(R, sched->k0246[6], sched->k1357[6]);
    R ^= f(L, sched->k0246[7], sched->k1357[7]);
    L ^= f(R, sched->k0246[8], sched->k1357[8]);
    R ^= f(L, sched->k0246[9], sched->k1357[9]);
    L ^= f(R, sched->k0246[10], sched->k1357[10]);
    R ^= f(L, sched->k0246[11], sched->k1357[11]);
    L ^= f(R, sched->k0246[12], sched->k1357[12]);
    R ^= f(L, sched->k0246[13], sched->k1357[13]);
    L ^= f(R, sched->k0246[14], sched->k1357[14]);
    R ^= f(L, sched->k0246[15], sched->k1357[15]);

    L = rotl(L, 31);
    R = rotl(R, 31);

    swap = L;
    L = R;
    R = swap;

    FP(L, R);

    output[0] = L;
    output[1] = R;
}


