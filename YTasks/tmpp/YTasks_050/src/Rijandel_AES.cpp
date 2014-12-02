/* Copyright (c) 2001 - 2005,Jukka Silvennoinen , Solution One Telecom LTd. All rights reserved */

#include "Rijandel_AES.h"

#include "IMCVCODC.H"

#include <COEMAIN.H>
#include <BAUTILS.H>
#include <apmrec.h> 
#include <apgcli.h>
#include <smut.h> 

/*
-------------------------------------------------------------------------------
****************************  CMyAESCrypter   *************************************
-------------------------------------------------------------------------------
*/  

CMyAESCrypter* CMyAESCrypter::NewL(const TDesC8& aPassword)
    {
    CMyAESCrypter* self = NewLC(aPassword);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CMyAESCrypter* CMyAESCrypter::NewLC(const TDesC8& aPassword)
    {
    CMyAESCrypter* self = new (ELeave) CMyAESCrypter();
    CleanupStack::PushL(self);
    self->ConstructL(aPassword);
    return self;
    }
    
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/   
CMyAESCrypter::CMyAESCrypter(void)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyAESCrypter::~CMyAESCrypter()
{
#ifdef __SERIES60_3X__
	delete iModeCBCEncryptor;
	iModeCBCEncryptor =NULL;
	delete iModeCBCDecryptor;
	iModeCBCDecryptor = NULL;
#else
#endif
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyAESCrypter::ConstructL(const TDesC8& aPassword)
{
#ifdef __SERIES60_3X__	
	CMD5* Md55 = CMD5::NewL();
	CleanupStack::PushL(Md55);
	
	TBuf8<16> Keyyy;
	Keyyy.Copy(Md55->Hash(aPassword));
	
	iAESEncryptor = CAESEncryptor::NewL(Keyyy);	
	iModeCBCEncryptor = CModeCBCEncryptor::NewL(iAESEncryptor,Keyyy);
	iAESDecryptor = CAESDecryptor::NewL(Keyyy);
	iModeCBCDecryptor = CModeCBCDecryptor::NewL(iAESDecryptor,Keyyy);
	
	Keyyy.FillZ();

	CleanupStack::PopAndDestroy(Md55);
#else
#endif
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyAESCrypter::Encrypt(const TDesC8& aInput, TDes8& aOutput)
{
#ifdef __SERIES60_3X__
	aOutput.Zero();
	
	if(iModeCBCEncryptor)
	{
		TBuf8<16> hlll;
	
		for(TInt i=0; i < aInput.Length(); i = (i + 16))
		{
			if(aInput.Length() > (i + 16))
			{
				hlll.Copy(aInput.Mid(i,16));
			}
			else
			{
				hlll.Copy(aInput.Right(aInput.Length() - i));
				hlll.Append(MyOwnPadding().Left(16 - hlll.Length()));
			}
		
			iModeCBCEncryptor->Transform(hlll);
			aOutput.Append(hlll);
		}
	}
#else
	aOutput.Copy(aInput);
#endif
}


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyAESCrypter::Decrypt(const TDesC8& aInput, TDes8& aOutput)
{
#ifdef __SERIES60_3X__
	aOutput.Zero();
	
	if(iModeCBCDecryptor)
	{
		TBuf8<16> hlll;
	
		for(TInt i=0; i < aInput.Length(); i = (i + 16))
		{
			if(aInput.Length() > (i + 16))
			{
				hlll.Copy(aInput.Mid(i,16));
			}
			else
			{
				hlll.Copy(aInput.Right(aInput.Length() - i));
				hlll.Append(MyOwnPadding().Left(16 - hlll.Length()));
			}
		
			iModeCBCDecryptor->Transform(hlll);
			aOutput.Append(hlll);
		}
	}
#else
	aOutput.Copy(aInput);
#endif
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyAESCrypter::Base64Decode(const TDesC8& aSource, TDes8& aDestination)
{
	TImCodecB64 MyEnc;
	MyEnc.Initialise();
	MyEnc.Decode(aSource,aDestination);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyAESCrypter::Base64Encode(const TDesC8& aSource, TDes8& aDestination)
{
	TImCodecB64 MyEnc;
	MyEnc.Initialise();
	MyEnc.Encode(aSource,aDestination);
}
