/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#include "Gif_Reader.h"

#include <cntfield.h>
#include <cntdef.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <COEMAIN.H>
#include "CommonStuff.h"

#include <akniconutils.h> 
/*
-------------------------------------------------------------------------------
****************************  CGif_Reader   *************************************
-------------------------------------------------------------------------------
*/  
CGif_Reader::CGif_Reader(MGifReadUpdate* aUpdate,const TDesC& aFileName)
:CActive(0),iFileName(aFileName),iUpdate(aUpdate)
    {
    }
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGif_Reader::~CGif_Reader()
{
	Cancel();
	
	delete iImageDecoder;
	iImageDecoder = NULL;
	
	delete iFrame;
	iFrame = NULL;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CGif_Reader::ConstructL(void)
{
	CActiveScheduler::Add(this);
	iError = KErrNone;
	iCurrCount = 0;
	iCurrImg = -1;
	
	TBuf8<100> Datatype;
	Datatype.Copy(KtxTypeImageGif);
	
	delete iImageDecoder;
	iImageDecoder = NULL;
	iImageDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(),iFileName,Datatype);
	
	iCurrCount = iImageDecoder->FrameCount();
	
	return iCurrCount;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CGif_Reader::LoadImageL(TInt aIndex)
{
	if(!IsActive())
	{
		delete iFrame;
		iFrame = NULL;
		
		iCurrImg = aIndex;
		
		if(iImageDecoder)
		{
			TFrameInfo FrameInf = iImageDecoder->FrameInfo(iCurrImg);
			
			iFrame = new(ELeave)CFbsBitmap();
			iFrame->Create(FrameInf.iOverallSizeInPixels,FrameInf.iFrameDisplayMode);
		
			iImageDecoder->Convert(&iStatus,*iFrame,iCurrImg);
//			Convert(TRequestStatus* aRequestStatus, CFbsBitmap& aDestination, CFbsBitmap& aDestinationMask, TInt aFrameNumber = 0);
			SetActive();
		}
		else
		{
			iUpdate->ImageLoadedL(iCurrImg,iFrame);
		}
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CGif_Reader::DoCancel()
{
	iCancel = ETrue;
	
	if(iImageDecoder)
	{
		iImageDecoder->Cancel();
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
void CGif_Reader::RunL()
{
	iError = iStatus.Int();

	iUpdate->ImageLoadedL(iCurrImg,iFrame);
	iFrame = NULL;
	iCurrImg = -1;
}
