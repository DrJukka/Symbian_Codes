/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#include "MBM_Reader.h"

#include <cntfield.h>
#include <cntdef.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <COEMAIN.H>


#include <akniconutils.h> 
/*
-------------------------------------------------------------------------------
****************************  CMBM_Reader   *************************************
-------------------------------------------------------------------------------
*/  

CMBM_Reader* CMBM_Reader::NewL(MMBMReadUpdate* aUpdate,const TDesC& aFileName)
    {
    CMBM_Reader* self = NewLC(aUpdate,aFileName);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/    
CMBM_Reader* CMBM_Reader::NewLC(MMBMReadUpdate* aUpdate,const TDesC& aFileName)
    {
    CMBM_Reader* self = new (ELeave) CMBM_Reader(aUpdate,aFileName);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CMBM_Reader::CMBM_Reader(MMBMReadUpdate* aUpdate,const TDesC& aFileName)
:CActive(0),iFileName(aFileName),iUpdate(aUpdate)
    {
    }
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CMBM_Reader::~CMBM_Reader()
{
	Cancel();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMBM_Reader::ConstructL(void)
{
	CActiveScheduler::Add(this);
	iError = KErrNone;
	
	iCurrCount = 0;
	
	CFbsBitmap* TmpImage(NULL);
	
	if(AknIconUtils::IsMifFile(iFileName))
	{
		iIsMif = ETrue;
		
		TInt MaskId(-1);
		TInt UseIndex(iCurrCount);
		AknIconUtils::ValidateLogicalAppIconId (iFileName, UseIndex, MaskId);
		
	 	TRAP(iError,TmpImage = AknIconUtils::CreateIconL(iFileName,UseIndex)); 
	
		if(iError == KErrNone)
		{
			iError = AknIconUtils::SetSize(TmpImage,TSize(50,50));
	 	}
	}
	else
	{	
		iIsMif = EFalse;
		TmpImage = new(ELeave)CFbsBitmap();
		iError = TmpImage->Load(iFileName,iCurrCount);
	}
	
	delete TmpImage;
	TmpImage = NULL;
		
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CFbsBitmap* CMBM_Reader::LoadImageL(TInt aIndex,const TSize aSize)
{
	CFbsBitmap* TmpImage(NULL);
	
	if(iIsMif)
	{
		TInt MaskId(-1);
		TInt UseIndex(aIndex);
		AknIconUtils::ValidateLogicalAppIconId (iFileName, UseIndex, MaskId); 

		TRAP(iError,TmpImage = AknIconUtils::CreateIconL(iFileName,UseIndex)); 
		AknIconUtils::SetSize(TmpImage,aSize);
	}
	else
	{
		TmpImage = new(ELeave)CFbsBitmap();
		TmpImage->Load(iFileName,aIndex);
	}
	
	return TmpImage;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMBM_Reader::DoCancel()
{
	iCancel = ETrue;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
void CMBM_Reader::RunL()
{
	if(iCancel)
	{
		FinnishReadL();
	}
	else if((iError != KErrNone))
	{
		FinnishReadL();
	}
	else 
	{
		iCurrCount++;
		
		CFbsBitmap* TmpImage(NULL);
	
		if(iIsMif)
		{
			TInt MaskId(-1);
			TInt UseIndex(iCurrCount);
			AknIconUtils::ValidateLogicalAppIconId (iFileName, UseIndex, MaskId);
				
		 	TRAP(iError,TmpImage = AknIconUtils::CreateIconL(iFileName,UseIndex)); 
		 	
		 	if(iError == KErrNone)
		 	{
		 		iError = AknIconUtils::SetSize(TmpImage,TSize(50,50));
		 	}
		}
		else
		{
			TmpImage = new(ELeave)CFbsBitmap();
			iError = TmpImage->Load(iFileName,iCurrCount);
		}
		
		delete TmpImage;
		TmpImage = NULL;
	}
	
	if(!iDone)
	{
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMBM_Reader::FinnishReadL(void)
{
	iDone = ETrue;
	iUpdate->ReadDoneL(iCurrCount);
}
