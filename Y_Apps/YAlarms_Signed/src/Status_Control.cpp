/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/

#include "Status_Control.h"
#include <EIKENV.H>
#include <EIKAPPUI.H>
#include <aknutils.h>
#include <eikapp.h>
#include <AknGlobalNote.h>
#include <APGCLI.H>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>

#include "YApp_E8876001.h"

/*
-----------------------------------------------------------------------------
***************************** CMyStatusControl ******************************
-----------------------------------------------------------------------------
*/
CMyStatusControl::~CMyStatusControl()
{
	delete iNameImg;
	iNameImg = NULL;
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyStatusControl::CMyStatusControl(MMyBgProvided& aBgProvided)
:iBgProvided(aBgProvided)
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyStatusControl* CMyStatusControl::NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided)
    {
    CMyStatusControl* self = new(ELeave)CMyStatusControl(aBgProvided);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aRect);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMyStatusControl::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    SetRect(aRect);
    
	ActivateL();	
	DrawNow();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMyStatusControl::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, aImg, aMsk);

	AknIconUtils::SetSize(FrameImg,aSize,EAspectRatioPreservedAndUnusedSpaceRemoved);  
	AknIconUtils::SetSize(FrameMsk,aSize,EAspectRatioPreservedAndUnusedSpaceRemoved); 

	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyStatusControl::SizeChanged()
{
	TFileName ImgFileName;
	iBgProvided.GetFileName(ImgFileName);
	
	if(ImgFileName.Length())
	{	
		delete iNameImg;
		iNameImg = NULL;
		iNameImg = LoadImageL(ImgFileName,EMbmYapp_a000312dName_img,EMbmYapp_a000312dName_img_mask,Rect().Size());
	}
} 


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyStatusControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	TRect SourceRect(0,0,0,0);
	CFbsBitmap* BgBitMap = iBgProvided.BagImageL(this,SourceRect);
	if(BgBitMap)
	{
		if(BgBitMap->Handle())
		{
			gc.DrawBitmap(Rect(),BgBitMap,SourceRect);
		}
	}	
	
	if(iNameImg)
	{
		if(iNameImg->Bitmap() && iNameImg->Mask())
		{
			if(iNameImg->Bitmap()->Handle() && iNameImg->Mask()->Handle())
			{
				TSize ImgSiz(iNameImg->Bitmap()->SizeInPixels());
				
				TInt RghMargin = Rect().iTl.iY + ((Rect().Height() - ImgSiz.iHeight) / 2);
				
				TRect DrwRect(Rect().iTl.iX,RghMargin,(Rect().iTl.iX + ImgSiz.iWidth),(ImgSiz.iHeight + RghMargin));
				
				gc.DrawBitmapMasked(DrwRect,iNameImg->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iNameImg->Mask(),EFalse);
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
***************************** CMyButtonControl ******************************
-----------------------------------------------------------------------------
*/
CMyButtonControl::~CMyButtonControl()
{

}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyButtonControl::CMyButtonControl(MMyBgProvided& aBgProvided,const CFont* aFont)
:iBgProvided(aBgProvided),iUseFont(aFont)
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyButtonControl* CMyButtonControl::NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided,const CFont* aFont)
    {
    CMyButtonControl* self = new(ELeave)CMyButtonControl(aBgProvided,aFont);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aRect);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMyButtonControl::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    SetRect(aRect);
    
	ActivateL();	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyButtonControl::SetAlligment(CGraphicsContext::TTextAlign aAlligment)
{
	iAlligment = aAlligment;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyButtonControl::SetText(const TDesC& aText)
{
	if(aText.Length() > 100)
	{
		iText.Copy(aText.Left(100));
	}
	else
	{
		iText.Copy(aText);
	}
	
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyButtonControl::SetTextColot(const TRgb& aColor,const CFont* aFont)
{
	iUseFont = aFont;
	iFontColor = aColor;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyButtonControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	TRect SourceRect(0,0,0,0);
	CFbsBitmap* BgBitMap = iBgProvided.BagImageL(this,SourceRect);
	if(BgBitMap)
	{
		if(BgBitMap->Handle())
		{
			gc.DrawBitmap(Rect(),BgBitMap,SourceRect);
		}
	}

	if(iUseFont)
	{
		gc.SetPenColor(iFontColor);
		gc.UseFont(iUseFont);
	
		TRect DrwRect(Rect());
		
		TInt DropMe = ((DrwRect.Height() - iUseFont->AscentInPixels()) / 2);
		DrwRect.iTl.iY = (DrwRect.iTl.iY + DropMe);
		
		gc.DrawText(iText,DrwRect,iUseFont->AscentInPixels(), iAlligment, 0);
	}
}

/*
-----------------------------------------------------------------------------
***************************** CMyBatteryControl ******************************
-----------------------------------------------------------------------------
*/
CMyBatteryControl::~CMyBatteryControl()
{
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	delete iBatteryCheck;
	iBatteryCheck = NULL;
	
	delete iLevelImg;
	iLevelImg = NULL;
	
	delete iStatusImg;
	iStatusImg = NULL;
	
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyBatteryControl::CMyBatteryControl(TBool aNormal,MMyBgProvided& aBgProvided)
:iBgProvided(aBgProvided),iNormal(aNormal)
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyBatteryControl* CMyBatteryControl::NewL(const CCoeControl& aContainer,const TRect& aRect,TBool aNormal,MMyBgProvided& aBgProvided)
    {
    CMyBatteryControl* self = new(ELeave)CMyBatteryControl(aNormal,aBgProvided);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aRect);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMyBatteryControl::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    SetRect(aRect);
    
    iBatteryCheck =  new(ELeave)CBatteryCheck(*this);
    iBatteryCheck->ConstructL();
    
	ActivateL();	
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyBatteryControl::SetRectangle(const TRect& aRect,TBool aNormal)
{
	iNormal = aNormal;
	SetRect(aRect);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMyBatteryControl::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, aImg, aMsk);

	AknIconUtils::SetSize(FrameImg,aSize,aScaleMode);  
	AknIconUtils::SetSize(FrameMsk,aSize,aScaleMode); 

	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyBatteryControl::SizeChanged()
{
	TRect MyRect(Rect());
	
	#ifdef __WINS__
		iChangeStatus = 75;
	#endif
	
	if(iNormal)
	{
		TInt Hght = (Rect().Height() / 5);
		
		iStatusRect.iBr    = MyRect.iBr;

		iStatusRect.iTl.iX = MyRect.iTl.iX ;
		iStatusRect.iTl.iY = (iStatusRect.iBr.iY - (Hght));
		
		iLevelRect.iTl	  = MyRect.iTl;
		iLevelRect.iBr.iX = MyRect.iBr.iX;
		iLevelRect.iBr.iY = iStatusRect.iTl.iY;
	}
	else
	{
		iLevelRect.iTl	  = MyRect.iTl;
		iLevelRect.iBr.iX = (iLevelRect.iTl.iX + (MyRect.Width() / 2));
		iLevelRect.iBr.iY = MyRect.iBr.iY;
		
		iStatusRect.iBr    = MyRect.iBr;
		iStatusRect.iTl.iX = iLevelRect.iBr.iX;
		iStatusRect.iTl.iY = (iStatusRect.iBr.iY - (MyRect.Height() / 2));
		
	}
	
	TFileName ImgFileName;
	iBgProvided.GetFileName(ImgFileName);
	
	if(ImgFileName.Length())
	{	
		delete iLevelImg;
		iLevelImg = NULL;
		iLevelImg = LoadImageL(ImgFileName,EMbmYapp_a000312dBat_status,EMbmYapp_a000312dBat_status_mask,iLevelRect.Size(),EAspectRatioNotPreserved);
	
		delete iStatusImg;
		iStatusImg = NULL;
		iStatusImg = LoadImageL(ImgFileName,EMbmYapp_a000312dBattery,EMbmYapp_a000312dBattery_mask,iStatusRect.Size());
	}
} 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyBatteryControl::BatteryLevelL(TUint aChangeStatus,CTelephony::TBatteryStatus aStatus)
{
	iChangeStatus = aChangeStatus;
	iStatus = aStatus;
	
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}

	delete iTimeOutTimer;
	iTimeOutTimer = NULL;

	if(iStatus == CTelephony::EBatteryConnectedButExternallyPowered)
	{
		iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal,*this);
		iTimeOutTimer->After(KBatteryTimeOut);
		
		iChangeStatus = 0;
	}

	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyBatteryControl::TimerExpired()
{
	if(iChangeStatus >= 100)
	{
		iChangeStatus = 0;
	}
	else
	{
		iChangeStatus = iChangeStatus + 17;	
		
		if(iChangeStatus > 100)
		{
			iChangeStatus = 100;
		}
	}
	
	if(iTimeOutTimer)
	{
		iTimeOutTimer->After(KBatteryTimeOut);
	}
	
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyBatteryControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	TRect SourceRect(0,0,0,0);
	CFbsBitmap* BgBitMap = iBgProvided.BagImageL(this,SourceRect);
	if(BgBitMap)
	{
		if(BgBitMap->Handle())
		{
			gc.DrawBitmap(Rect(),BgBitMap,SourceRect);
		}
	}
	
	if(iLevelImg && iChangeStatus > 0)
	{
		if(iLevelImg->Bitmap() && iLevelImg->Mask())
		{
			if(iLevelImg->Bitmap()->Handle() && iLevelImg->Mask()->Handle())
			{
				TSize ImgSiz(iLevelImg->Bitmap()->SizeInPixels());
				
				TInt LftMargin = iLevelRect.iTl.iX + ((iLevelRect.Width() - ImgSiz.iWidth) / 2);
				TInt RghMargin = iLevelRect.iTl.iY + ((iLevelRect.Height() - ImgSiz.iHeight) / 2);
				
				TRect DrwRect(LftMargin,RghMargin,(LftMargin + ImgSiz.iWidth),(ImgSiz.iHeight + RghMargin));
				
				TInt StartY = ((ImgSiz.iHeight * (100 - iChangeStatus )) / 100);
				DrwRect.iTl.iY = (DrwRect.iTl.iY + StartY);
				
				gc.DrawBitmapMasked(DrwRect,iLevelImg->Bitmap(),TRect(0,StartY,ImgSiz.iWidth,ImgSiz.iHeight),iLevelImg->Mask(),EFalse);
			}
		}
	}

	if(iStatusImg)
	{
		if(iStatusImg->Bitmap() && iStatusImg->Mask())
		{
			if(iStatusImg->Bitmap()->Handle() && iStatusImg->Mask()->Handle())
			{
				TSize ImgSiz(iStatusImg->Bitmap()->SizeInPixels());
				
				TInt LftMargin = iStatusRect.iTl.iX + ((iStatusRect.Width() - ImgSiz.iWidth) / 2);
				TInt RghMargin = iStatusRect.iTl.iY + ((iStatusRect.Height() - ImgSiz.iHeight) / 2);
				
				TRect DrwRect(LftMargin,RghMargin,(LftMargin + ImgSiz.iWidth),(ImgSiz.iHeight + RghMargin));
				
				gc.DrawBitmapMasked(DrwRect,iStatusImg->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iStatusImg->Mask(),EFalse);
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
***************************** CMySignalControl ******************************
-----------------------------------------------------------------------------
*/
CMySignalControl::~CMySignalControl()
{
	delete iNwSignalCheck;
	iNwSignalCheck = NULL;
	
	delete iLevelImg;
	iLevelImg = NULL;
	
	delete iStatusImg;
	iStatusImg = NULL;
	
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMySignalControl::CMySignalControl(TBool aNormal,MMyBgProvided& aBgProvided)
:iBgProvided(aBgProvided),iNormal(aNormal)
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMySignalControl* CMySignalControl::NewL(const CCoeControl& aContainer,const TRect& aRect,TBool aNormal,MMyBgProvided& aBgProvided)
    {
    CMySignalControl* self = new(ELeave)CMySignalControl(aNormal,aBgProvided);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aRect);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMySignalControl::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    SetRect(aRect);
	
    iNwSignalCheck =  new(ELeave)CNwSignalCheck(*this);
    iNwSignalCheck->ConstructL();
    
	ActivateL();	
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySignalControl::SetRectangle(const TRect& aRect,TBool aNormal)
{
	iNormal = aNormal;
	SetRect(aRect);
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMySignalControl::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, aImg, aMsk);

	AknIconUtils::SetSize(FrameImg,aSize,aScaleMode);  
	AknIconUtils::SetSize(FrameMsk,aSize,aScaleMode); 

	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySignalControl::SizeChanged()
{
	TRect MyRect(Rect());
	
	#ifdef __WINS__
		iBars = 5;
	#endif
	
	if(iNormal)
	{
		TInt Hght = (Rect().Height() / 5);
		
		iStatusRect.iBr    = MyRect.iBr;

		iStatusRect.iTl.iX = MyRect.iTl.iX ;
		iStatusRect.iTl.iY = (iStatusRect.iBr.iY - (Hght));
		
		iLevelRect.iTl	  = MyRect.iTl;
		iLevelRect.iBr.iX = MyRect.iBr.iX;
		iLevelRect.iBr.iY = iStatusRect.iTl.iY;
	}
	else
	{
		iLevelRect.iTl	  = MyRect.iTl;
		iLevelRect.iBr.iX = (iLevelRect.iTl.iX + (MyRect.Width() / 2));
		iLevelRect.iBr.iY = MyRect.iBr.iY;
		
		iStatusRect.iBr    = MyRect.iBr;
		iStatusRect.iTl.iX = iLevelRect.iBr.iX;
		iStatusRect.iTl.iY = (iStatusRect.iBr.iY - (MyRect.Height() / 2));	
	}
	
	TFileName ImgFileName;
	iBgProvided.GetFileName(ImgFileName);
	
	if(ImgFileName.Length())
	{	
		delete iLevelImg;
		iLevelImg = NULL;
		iLevelImg = LoadImageL(ImgFileName,EMbmYapp_a000312dSignal_status,EMbmYapp_a000312dSignal_status_mask,iLevelRect.Size(),EAspectRatioNotPreserved);
	
		delete iStatusImg;
		iStatusImg = NULL;
		iStatusImg = LoadImageL(ImgFileName,EMbmYapp_a000312dSignal,EMbmYapp_a000312dSignal_mask,iStatusRect.Size());
	}
} 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySignalControl::SignalStatus(TInt32 aStrength,TInt8 aBars)
{
	iStrength = aStrength;
	iBars = aBars;
	
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySignalControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	
	TRect SourceRect(0,0,0,0);
	CFbsBitmap* BgBitMap = iBgProvided.BagImageL(this,SourceRect);
	if(BgBitMap)
	{
		if(BgBitMap->Handle())
		{
			gc.DrawBitmap(Rect(),BgBitMap,SourceRect);
		}
	}
	
	if(iLevelImg && iBars > 0)
	{
		if(iLevelImg->Bitmap() && iLevelImg->Mask())
		{
			if(iLevelImg->Bitmap()->Handle() && iLevelImg->Mask()->Handle())
			{
				TSize ImgSiz(iLevelImg->Bitmap()->SizeInPixels());
				
				TInt LftMargin = iLevelRect.iTl.iX + ((iLevelRect.Width() - ImgSiz.iWidth) / 2);
				TInt RghMargin = iLevelRect.iTl.iY + ((iLevelRect.Height() - ImgSiz.iHeight) / 2);
				
				TRect DrwRect(LftMargin,RghMargin,(LftMargin + ImgSiz.iWidth),(ImgSiz.iHeight + RghMargin));
				
				TInt StartY = ((ImgSiz.iHeight * (7 - iBars)) / 7);
				
				DrwRect.iTl.iY = (DrwRect.iTl.iY + StartY);			
				
				gc.DrawBitmapMasked(DrwRect,iLevelImg->Bitmap(),TRect(0,StartY,ImgSiz.iWidth,ImgSiz.iHeight),iLevelImg->Mask(),EFalse);
			}
		}
	}

	if(iStatusImg)
	{
		if(iStatusImg->Bitmap() && iStatusImg->Mask())
		{
			if(iStatusImg->Bitmap()->Handle() && iStatusImg->Mask()->Handle())
			{
				TSize ImgSiz(iStatusImg->Bitmap()->SizeInPixels());
				
				TInt LftMargin = iStatusRect.iTl.iX + ((iStatusRect.Width() - ImgSiz.iWidth) / 2);
				TInt RghMargin = iStatusRect.iTl.iY + ((iStatusRect.Height() - ImgSiz.iHeight) / 2);
				
				TRect DrwRect(LftMargin,RghMargin,(LftMargin + ImgSiz.iWidth),(ImgSiz.iHeight + RghMargin));
				
				gc.DrawBitmapMasked(DrwRect,iStatusImg->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iStatusImg->Mask(),EFalse);
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
***************************** CMyScrollControl ******************************
-----------------------------------------------------------------------------
*/
CMyScrollControl::~CMyScrollControl()
{
	delete iScroller;
	iScroller = NULL;
	delete iScrollBack;
	iScrollBack = NULL;
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyScrollControl::CMyScrollControl(MMyBgProvided& aBgProvided)
:iBgProvided(aBgProvided)
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyScrollControl* CMyScrollControl::NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided)
    {
    CMyScrollControl* self = new(ELeave)CMyScrollControl(aBgProvided);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aRect);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMyScrollControl::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    SetRect(aRect);
    
	ActivateL();	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyScrollControl::SizeChanged()
{
	TFileName ImgFileName;
	iBgProvided.GetFileName(ImgFileName);
	
	if(ImgFileName.Length())
	{	
		TSize ImgSiz(((Rect().Width() * 8) / 10),(Rect().Height() /2));
			
		if(iMax > 0)
		{
			// calclate the size
		}
		
		delete iScroller;
		iScroller = NULL;
		iScroller = LoadImageL(ImgFileName,EMbmYapp_a000312dScroller,EMbmYapp_a000312dScroller_mask,ImgSiz);

		delete iScrollBack;
		iScrollBack = NULL;
		iScrollBack = LoadImageL(ImgFileName,EMbmYapp_a000312dScroll,EMbmYapp_a000312dScroll_mask,Rect().Size());
	}
} 

	 
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMyScrollControl::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, aImg, aMsk);

	if(aSize.iWidth > 0 && aSize.iHeight > 0)
	{
		AknIconUtils::SetSize(FrameImg,aSize,EAspectRatioNotPreserved);  
		AknIconUtils::SetSize(FrameMsk,aSize,EAspectRatioNotPreserved); 
	}
	
	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyScrollControl::SetValues(TInt aCurrent, TInt aMax,TInt aCount)
{
	iCurrent = aCurrent;
	iMax = aMax;
	iCount = aCount;
	
	SizeChanged();// misusing this function a bit....
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyScrollControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	if(iMax > 1 && iMax < iCount)
	{
		TRect SourceRect(0,0,0,0);
		CFbsBitmap* BgBitMap = iBgProvided.BagImageL(this,SourceRect);
		if(BgBitMap)
		{
			if(BgBitMap->Handle())
			{
				gc.DrawBitmap(Rect(),BgBitMap,SourceRect);
			}
		}

		if(iScrollBack)
		{
			if(iScrollBack->Bitmap() && iScrollBack->Mask())
			{
				if(iScrollBack->Bitmap()->Handle() && iScrollBack->Mask()->Handle())
				{
					TSize ImgSiz(iScrollBack->Bitmap()->SizeInPixels());
					gc.DrawBitmapMasked(Rect(),iScrollBack->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iScrollBack->Mask(),EFalse);
				}
			}
		}
		
		if(iScroller)
		{
			if(iScroller->Bitmap() && iScroller->Mask())
			{
				if(iScroller->Bitmap()->Handle() && iScroller->Mask()->Handle())
				{
					TSize ImgSiz(iScroller->Bitmap()->SizeInPixels());
					
					TSize SizeImg(Rect().Width(), ((Rect().Height() * iMax) / iCount));
									
					TInt ScroollHeight = (Rect().Height() - SizeImg.iHeight);
					
					TInt TmpCunt((iCount - 1));
					
					TInt StartSrollY = ((ScroollHeight * iCurrent) / TmpCunt);
							
					StartSrollY = StartSrollY + Rect().iTl.iY;
					
					TRect ScrollerRect(Rect().iTl.iX,StartSrollY,Rect().iBr.iX,(StartSrollY + SizeImg.iHeight));

					// have the match in here..
					gc.DrawBitmapMasked(ScrollerRect,iScroller->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iScroller->Mask(),EFalse);
				}
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
***************************** CMyIconControl ******************************
-----------------------------------------------------------------------------
*/
CMyIconControl::~CMyIconControl()
{
    if (iTimeTimer)
        iTimeTimer->Cancel();
    
    delete iTimeTimer;
    
	iNumbers.ResetAndDestroy();
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyIconControl::CMyIconControl(MMyBgProvided& aBgProvided)
:iBgProvided(aBgProvided)
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyIconControl* CMyIconControl::NewL(const CCoeControl& aContainer,const TRect& aRect,MMyBgProvided& aBgProvided)
    {
    CMyIconControl* self = new(ELeave)CMyIconControl(aBgProvided);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aRect);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMyIconControl::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    SetRect(aRect);
    
	ActivateL();	
	DrawNow();
	
	iTimeTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iTimeTimer->Start(800000,800000, TCallBack(DoCheckTimeL, this));
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
TInt CMyIconControl::DoCheckTimeL(TAny* aPtr)
{
	CMyIconControl* self = static_cast<CMyIconControl*>(aPtr);
	self->DrawNow();
    
	return KErrNone;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMyIconControl::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, aImg, aMsk);

	AknIconUtils::SetSize(FrameImg,aSize,EAspectRatioPreservedAndUnusedSpaceRemoved);  
	AknIconUtils::SetSize(FrameMsk,aSize,EAspectRatioPreservedAndUnusedSpaceRemoved); 

	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyIconControl::SizeChanged()
{
	TRect MyRect(Rect());
	MyRect.Shrink(4,4);
	
	TFileName ImgFileName;
	iBgProvided.GetFileName(ImgFileName);
	
	if(ImgFileName.Length())
	{	
		TSize NmberSiz(((MyRect.Width() * 2) / 9),MyRect.Height());

		iNumbers.ResetAndDestroy();
		
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dZero,EMbmYapp_a000312dZero_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dOne,EMbmYapp_a000312dOne_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dTwo,EMbmYapp_a000312dTwo_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dThree,EMbmYapp_a000312dThree_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dFour,EMbmYapp_a000312dFour_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dFive,EMbmYapp_a000312dFive_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dSix,EMbmYapp_a000312dSix_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dSeven,EMbmYapp_a000312dSeven_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dEight,EMbmYapp_a000312dEight_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dNine,EMbmYapp_a000312dNine_mask,NmberSiz));
	
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dColon,EMbmYapp_a000312dColon_mask,TSize((NmberSiz.iWidth / 2),NmberSiz.iHeight)));
	
		iNumRect0 = iNumRect1 = iNumRect2 = iNumRect3 = iColRect = MyRect;
	
		TInt NumWidth(0);
	
		for(TInt i=0;i < iNumbers.Count(); i++)
		{
			if(iNumbers[i])
			{
				if(iNumbers[i]->Bitmap())
				{
					if(iNumbers[i]->Bitmap()->Handle())
					{
						TSize ImgSiz(iNumbers[i]->Bitmap()->SizeInPixels());
					
						if(ImgSiz.iWidth > NumWidth)
						{
							NumWidth = ImgSiz.iWidth;
						}
					}
				}
			}
		}
	
		if(NumWidth <= 0)
		{
			NumWidth = NmberSiz.iWidth;
		}
	
		iNumRect0.iBr.iX = (iNumRect0.iTl.iX + NumWidth);
		
		iNumRect1.iTl.iX = iNumRect0.iBr.iX; 
		iNumRect1.iBr.iX = (iNumRect1.iTl.iX + NumWidth);
		
		iColRect.iTl.iX = iNumRect1.iBr.iX;
		iColRect.iBr.iX = (iColRect.iTl.iX + (NmberSiz.iWidth / 2));
		
		iNumRect2.iTl.iX = iColRect.iBr.iX; 
		iNumRect2.iBr.iX = (iNumRect2.iTl.iX + NumWidth);
	
		iNumRect3.iTl.iX = iNumRect2.iBr.iX; 
		iNumRect3.iBr.iX = (iNumRect3.iTl.iX + NumWidth);
	}
} 
	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyIconControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	TRect SourceRect(0,0,0,0);
	CFbsBitmap* BgBitMap = iBgProvided.BagImageL(this,SourceRect);
	if(BgBitMap)
	{
		if(BgBitMap->Handle())
		{
			gc.DrawBitmap(Rect(),BgBitMap,SourceRect);
		}
	}
	
	TTime NowTime;
	NowTime.HomeTime();
	
	TInt HourTen = (NowTime.DateTime().Hour() / 10);
	TInt Hours = (NowTime.DateTime().Hour() % 10);
	TInt MinTen= (NowTime.DateTime().Minute() / 10);
	TInt Min = (NowTime.DateTime().Minute() % 10);
	
	if(iNumbers.Count() > HourTen)
	{
		DrawNumber(iNumRect0,gc,iNumbers[HourTen]);
	}
	
	if(iNumbers.Count() > Hours)
	{
		DrawNumber(iNumRect1,gc,iNumbers[Hours]);
	}
	
	if(iNumbers.Count() > 10)
	{
		DrawNumber(iColRect,gc,iNumbers[10]);
	}
	
	if(iNumbers.Count() > MinTen)
	{
		DrawNumber(iNumRect2,gc,iNumbers[MinTen]);
	}
	
	if(iNumbers.Count() > Min)
	{
		DrawNumber(iNumRect3,gc,iNumbers[Min]);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyIconControl::DrawNumber(const TRect& aRect,CWindowGc& aGc,CGulIcon* aIcon) const
{
	if(aIcon)
	{
		if(aIcon->Bitmap() && aIcon->Mask())
		{
			if(aIcon->Bitmap()->Handle() && aIcon->Mask()->Handle())
			{
				TSize ImgSiz(aIcon->Bitmap()->SizeInPixels());
			
				TRect DbgRect(aRect);
				
				DbgRect.iTl.iX = (DbgRect.iTl.iX + ((DbgRect.Width() - ImgSiz.iWidth) / 2));
				DbgRect.iBr.iX = (DbgRect.iTl.iX + ImgSiz.iWidth);
				
				DbgRect.iTl.iY = (DbgRect.iTl.iY + ((DbgRect.Height()- ImgSiz.iHeight)/ 2));
				DbgRect.iBr.iY = (DbgRect.iTl.iY + ImgSiz.iHeight);
				
				// have the match in here..
				aGc.DrawBitmapMasked(DbgRect,aIcon->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),aIcon->Mask(),EFalse);			
			}
		}
	}	
}


