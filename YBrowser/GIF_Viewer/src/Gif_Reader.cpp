/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#include "Gif_Reader.h"

#include <cntfield.h>
#include <cntdef.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <COEMAIN.H>
#include <akniconutils.h> 
/*
-------------------------------------------------------------------------------
****************************  CGif_Reader   *************************************
-------------------------------------------------------------------------------
*/  
CGif_Reader::CGif_Reader(CCoeControl& aParent)
:CActive(0),iParent(aParent),iCurrImg(-1)
    {
    }
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGif_Reader::~CGif_Reader()
{
	Cancel();
	delete iTimeOutTimer;
	delete iImageDecoder;
	delete iFrame;
	delete iFrameImg;
	delete iFrameMsk;
	
	delete iBitmapDevice;
	delete iGraphicsContext;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CGif_Reader::ConstructL(const TDesC& aFileName)
{
	CActiveScheduler::Add(this);
	iImageDecoder = CImageDecoder::FileNewL(CCoeEnv::Static()->FsSession(),aFileName,KtxTypeImageGif_8);
	iCurrCount = iImageDecoder->FrameCount();
	
	if(iCurrCount > 0)
	{
		iFrame = new(ELeave)CFbsBitmap();
		iFrame->Create(iImageDecoder->FrameInfo(0).iOverallSizeInPixels,iImageDecoder->FrameInfo(0).iFrameDisplayMode);
				
		iBitmapDevice = CFbsBitmapDevice::NewL(iFrame);
		User::LeaveIfError(iBitmapDevice->CreateContext(iGraphicsContext));
		
		TimerExpired();
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CGif_Reader::TimerExpired()
{
	Cancel();

	iCurrImg++;
	if(iCurrImg >= iCurrCount || iCurrImg < 0)
	{
		iCurrImg = 0;
	}
	
	delete iFrameImg;
	iFrameImg = NULL;
	iFrameImg = new(ELeave)CFbsBitmap();
	iFrameImg->Create(iImageDecoder->FrameInfo(iCurrImg).iOverallSizeInPixels,iImageDecoder->FrameInfo(iCurrImg).iFrameDisplayMode);
		
	delete iFrameMsk;
	iFrameMsk = NULL;
	iFrameMsk = new(ELeave)CFbsBitmap();
	iFrameMsk->Create(iImageDecoder->FrameInfo(iCurrImg).iOverallSizeInPixels,EGray2);

	iImageDecoder->Convert(&iStatus,*iFrameImg,*iFrameMsk,iCurrImg);
	SetActive();	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CGif_Reader::DoCancel()
{
	iImageDecoder->Cancel();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CFbsBitmap* CGif_Reader::Bitmap()
{
	return iFrame;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
void CGif_Reader::RunL()
{
	if(iFrameMsk && iFrameImg && iFrame && iGraphicsContext)
	{
		if(iFrameMsk->Handle() && iFrameImg->Handle() && iFrame->Handle())
		{
			if(TFrameInfo::ERestoreToBackground & iImageDecoder->FrameInfo(iCurrImg).iFlags)
			{
				iGraphicsContext->DrawBitmap(TRect(0,0,iFrame->SizeInPixels().iWidth,iFrame->SizeInPixels().iHeight),iFrameImg);
			}
			else
			{
				iGraphicsContext->DrawBitmapMasked(iImageDecoder->FrameInfo(iCurrImg).iFrameCoordsInPixels,iFrameImg,TRect(0,0,iFrameImg->SizeInPixels().iWidth,iFrameImg->SizeInPixels().iHeight),iFrameMsk,EFalse);
			}
		}
	}
	
	iParent.DrawNow();	
	
	if(iStatus.Int() != KErrCancel)
	{
		if(!iTimeOutTimer)
		{
			iTimeOutTimer = new(ELeave)CTimeOutTimer(CActive::EPriorityStandard,*this);
			iTimeOutTimer->ConstructL();
		}	
		
		iTimeOutTimer->After(KFrameTimeOut);
	}
}

