/*
-----------------------------------------------------------------------------

// blowfish.cpp   C++ class implementation of the BLOWFISH encryption algorithm
// _THE BLOWFISH ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger
// Converted to Symbian May/2007, Jukka Silvennoinen
-----------------------------------------------------------------------------
*/ 
#include "blowfish.h"
#include "blowfish.h2"	// holds the random digit tables
#include "md5.h"

#define S(x,i) (SBoxes[i][x.w.byte##i])
#define bf_F(x) (((S(x,0) + S(x,1)) ^ S(x,2)) + S(x,3))
#define ROUND(a,b,n) (a.dword ^= bf_F(b) ^ PArray[n])

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CBlowFish* CBlowFish::NewL(void)
    {
    CBlowFish* self = CBlowFish::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CBlowFish* CBlowFish::NewLC(void)
    {
    CBlowFish* self = new (ELeave) CBlowFish();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CBlowFish::CBlowFish ()
{
 	PArray = new(ELeave)TUint32 [18];
 	SBoxes = new(ELeave)TUint32[4][256];
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/ 
CBlowFish::~CBlowFish ()
{
	delete PArray ;
	delete [] SBoxes ;
	delete iMD5_Handler;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBlowFish::ConstructL(void)
{
	iMD5_Handler = new(ELeave)CMD5_Handler();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBlowFish :: Encrypt(TDes8& aKey, TDes& aData)
{	
	TPtr8 HelpaData((TUint8*)aData.Ptr(),aData.Size());	
	Encrypt(aKey,HelpaData);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBlowFish :: Encrypt(TDes8& aKey, TDes8& aData)
{	
	HBufC8* NeBuffer = HBufC8::NewLC(aData.Size());
	
	TUint8 *data =(TUint8 *) aData.Ptr();
	TUint8 *output =(TUint8 *) NeBuffer->Des().Ptr();
	
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

	Initialize(keybuf,16);
	Encode(data,output,aData.Size());
	TPtr8 Zeroingpointer((TUint8*)keybuf,sizeof(16));
    Zeroingpointer.FillZ(sizeof(16));
	
	aData.Copy(*NeBuffer);
	CleanupStack::PopAndDestroy(NeBuffer);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBlowFish :: Decrypt(TDes8& aKey, TDes& aData)
{	
	TPtr8 HelpaData((TUint8*)aData.Ptr(),aData.Size());	
	Decrypt(aKey,HelpaData);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CBlowFish :: Decrypt(TDes8& aKey, TDes8& aData)
{
	HBufC8* NeBuffer = HBufC8::NewLC(aData.Size());
    
    TUint8 *data =(TUint8 *) aData.Ptr();
	TUint8 *output =(TUint8 *) NeBuffer->Des().Ptr();
	
	struct MD5Context md5c;
	TUint8 keybuf[16];
	
	TUint8 *KeyPtr = (TUint8 *) aKey.Ptr();
	  
    iMD5_Handler->MD5Init(&md5c);
	iMD5_Handler->MD5Update(&md5c,KeyPtr, aKey.Size());
	iMD5_Handler->MD5Final(keybuf, &md5c);		
	
	Initialize(keybuf,16);
    Decode (data,output,aData.Size());
    TPtr8 Zeroingpointer((TUint8*)keybuf,sizeof(16));
    Zeroingpointer.FillZ(sizeof(16));
    
    aData.Copy(*NeBuffer);
    CleanupStack::PopAndDestroy(NeBuffer);
}
/*
-----------------------------------------------------------------------------
***********************  original public functions **************************
Encode pIntput into pOutput.  Input length in lSize.  Returned value
is length of output which will be even MOD 8 bytes.  Input buffer and
output buffer can be the same, but be sure buffer length is even MOD 8.
-----------------------------------------------------------------------------
*/
TUint32 CBlowFish::Encode (TUint8 * pInput, TUint8 * pOutput, TUint32 lSize)
{
	TUint32 lCount, lOutSize, lGoodBytes ;
	TUint8	*pi, *po ;
	int		i, j ;
	int		SameDest = (pInput == pOutput ? 1 : 0) ;

	lOutSize = GetOutputLength (lSize) ;
	for (lCount = 0 ; lCount < lOutSize ; lCount += 8)
	{
		if (SameDest) // if encoded data is being written into input buffer
		{
		 	if (lCount < lSize - 7)	// if not dealing with uneven bytes at end
		 	{
		 	 	Blowfish_encipher ((TUint32 *) pInput,
		 	 		(TUint32 *) (pInput + 4)) ;
		 	}
		 	else // pad end of data with null bytes to complete encryption
		 	{
				po = pInput + lSize ;	// point at byte past the end of actual data
				j = (int) (lOutSize - lSize) ;	// number of bytes to set to null
				for (i = 0 ; i < j ; i++)
					*po++ = 0 ;
		 	 	Blowfish_encipher ((TUint32 *) pInput,
		 	 		(TUint32 *) (pInput + 4)) ;
		 	}
		 	pInput += 8 ;
		}
		else // output buffer not equal to input buffer, so must copy
		{    // input to output buffer prior to encrypting
		 	if (lCount < lSize - 7)	// if not dealing with uneven bytes at end
		 	{
		 		pi = pInput ;
		 		po = pOutput ;
		 		for (i = 0 ; i < 8 ; i++)// copy bytes to output
		 			*po++ = *pi++ ;
		 	 	Blowfish_encipher ((TUint32 *) pOutput,	// now encrypt them
		 	 		(TUint32 *) (pOutput + 4)) ;
		 	}
		 	else // pad end of data with null bytes to complete encryption
		 	{
		 		lGoodBytes = lSize - lCount ;	// number of remaining data bytes
		 		po = pOutput ;
		 		for (i = 0 ; i < (int) lGoodBytes ; i++)
		 			*po++ = *pInput++ ;
		 		for (j = i ; j < 8 ; j++)
		 			*po++ = 0 ;
		 	 	Blowfish_encipher ((TUint32 *) pOutput,
		 	 		(TUint32 *) (pOutput + 4)) ;
		 	}
		 	pInput += 8 ;
		 	pOutput += 8 ;
		}
	}
	return lOutSize ;
 }
/*
-----------------------------------------------------------------------------
Decode pIntput into pOutput.  Input length in lSize.  Input buffer and
output buffer can be the same, but be sure buffer length is even MOD 8.
-----------------------------------------------------------------------------
*/

void CBlowFish::Decode (TUint8 * pInput, TUint8 * pOutput, TUint32 lSize)
{
	TUint32 lCount ;
	TUint8	*pi, *po ;
	TInt	i;
	TInt	SameDest = (pInput == pOutput ? 1 : 0) ;

	for (lCount = 0 ; lCount < lSize ; lCount += 8)
	{
		// if encoded data is being written into input buffer
		if (SameDest)	
		{
	 	 	Blowfish_decipher ((TUint32 *) pInput,
	 	 		(TUint32 *) (pInput + 4)) ;
		 	pInput += 8 ;
		}
		else // output buffer not equal to input buffer
		{    // so copy input to output before decoding
	 		pi = pInput ;
	 		po = pOutput ;
	 		for (i = 0 ; i < 8 ; i++)
	 			*po++ = *pi++ ;
	 	 	Blowfish_decipher ((TUint32 *) pOutput,
	 	 		(TUint32 *) (pOutput + 4)) ;
		 	pInput += 8 ;
		 	pOutput += 8 ;
		}
	}
}

/*
-----------------------------------------------------------------------------
constructs the enctryption sieve
-----------------------------------------------------------------------------
*/
void CBlowFish::Initialize (TUint8 key[], int keybytes)
{
	TInt  		i, j ;
	TUint32  		data, datal, datar ;
	union aword temp ;

	// first fill arrays from data tables
	for (i = 0 ; i < 18 ; i++)
		PArray [i] = bf_P [i] ;

	for (i = 0 ; i < 4 ; i++)
	{
	 	for (j = 0 ; j < 256 ; j++)
	 		SBoxes [i][j] = bf_S [i][j] ;
	}


	j = 0 ;
	for (i = 0 ; i < NPASS + 2 ; ++i)
	{
		temp.dword = 0 ;
		temp.w.byte0 = key[j];
		temp.w.byte1 = key[(j+1) % keybytes] ;
		temp.w.byte2 = key[(j+2) % keybytes] ;
		temp.w.byte3 = key[(j+3) % keybytes] ;
		data = temp.dword ;
		PArray [i] ^= data ;
		j = (j + 4) % keybytes ;
	}

	datal = 0 ;
	datar = 0 ;

	for (i = 0 ; i < NPASS + 2 ; i += 2)
	{
		Blowfish_encipher (&datal, &datar) ;
		PArray [i] = datal ;
		PArray [i + 1] = datar ;
	}

	for (i = 0 ; i < 4 ; ++i)
	{
		for (j = 0 ; j < 256 ; j += 2)
		{
		  Blowfish_encipher (&datal, &datar) ;
		  SBoxes [i][j] = datal ;
		  SBoxes [i][j + 1] = datar ;
		}
	}
}
/*
-----------------------------------------------------------------------------
get output length, which must be even MOD 8
-----------------------------------------------------------------------------
*/
TUint32 CBlowFish::GetOutputLength (TUint32 lInputLong)
{
	TUint32 	lVal ;

	lVal = lInputLong % 8 ;	// find out if uneven number of bytes at the end
	if (lVal != 0)
		return lInputLong + 8 - lVal ;
	else
		return lInputLong ;
}
/*
-----------------------------------------------------------------------------
*********************** original private functions **************************
the low level (private) encryption function
-----------------------------------------------------------------------------
*/
void CBlowFish::Blowfish_encipher (TUint32 *xl, TUint32 *xr)
{
	union aword  Xl, Xr ;

	Xl.dword = *xl ;
	Xr.dword = *xr ;

	Xl.dword ^= PArray [0];
	ROUND (Xr, Xl, 1) ;  ROUND (Xl, Xr, 2) ;
	ROUND (Xr, Xl, 3) ;  ROUND (Xl, Xr, 4) ;
	ROUND (Xr, Xl, 5) ;  ROUND (Xl, Xr, 6) ;
	ROUND (Xr, Xl, 7) ;  ROUND (Xl, Xr, 8) ;
	ROUND (Xr, Xl, 9) ;  ROUND (Xl, Xr, 10) ;
	ROUND (Xr, Xl, 11) ; ROUND (Xl, Xr, 12) ;
	ROUND (Xr, Xl, 13) ; ROUND (Xl, Xr, 14) ;
	ROUND (Xr, Xl, 15) ; ROUND (Xl, Xr, 16) ;
	Xr.dword ^= PArray [17] ;

	*xr = Xl.dword ;
	*xl = Xr.dword ;
}
/*
-----------------------------------------------------------------------------
the low level (private) decryption function
-----------------------------------------------------------------------------
*/
void CBlowFish::Blowfish_decipher (TUint32 *xl, TUint32 *xr)
{
   union aword  Xl ;
   union aword  Xr ;

   Xl.dword = *xl ;
   Xr.dword = *xr ;

   Xl.dword ^= PArray [17] ;
   ROUND (Xr, Xl, 16) ;  ROUND (Xl, Xr, 15) ;
   ROUND (Xr, Xl, 14) ;  ROUND (Xl, Xr, 13) ;
   ROUND (Xr, Xl, 12) ;  ROUND (Xl, Xr, 11) ;
   ROUND (Xr, Xl, 10) ;  ROUND (Xl, Xr, 9) ;
   ROUND (Xr, Xl, 8) ;   ROUND (Xl, Xr, 7) ;
   ROUND (Xr, Xl, 6) ;   ROUND (Xl, Xr, 5) ;
   ROUND (Xr, Xl, 4) ;   ROUND (Xl, Xr, 3) ;
   ROUND (Xr, Xl, 2) ;   ROUND (Xl, Xr, 1) ;
   Xr.dword ^= PArray[0];

   *xl = Xr.dword;
   *xr = Xl.dword;
}


