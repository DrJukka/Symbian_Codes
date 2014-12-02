/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/

#include "Splash_Screen.h"
#include <EIKENV.H>
#include <EIKAPPUI.H>
#include <aknutils.h>
#include <eikapp.h>
#include <AknGlobalNote.h>
#include <APGCLI.H>
#include <GULICON.H>


#include "YApp_E8876001.h"



// CMySplashScreen
//
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMySplashScreen::~CMySplashScreen()
{
	delete iBackGround;
	iBackGround = NULL;
	
	iBackAdd.ResetAndDestroy();
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMySplashScreen::CMySplashScreen()
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMySplashScreen* CMySplashScreen::NewL()
    {
    CMySplashScreen* self = new(ELeave)CMySplashScreen();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMySplashScreen::ConstructL(void)
    {
    CreateWindowL();
    
	SetRect(CEikonEnv::Static()->EikAppUi()->ApplicationRect());

	ActivateL();	
	DrawNow();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMySplashScreen::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize)
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
void CMySplashScreen::SizeChanged()
{	
	TFindFile privFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == privFile.FindByDir(KtxIconFileName, KNullDesC))
	{	
		delete iBackGround;
		iBackGround = NULL;		
		iBackGround = AknIconUtils::CreateIconL(privFile.File(), EMbmYapp_a000312dSplash_back);
		AknIconUtils::SetSize(iBackGround,Rect().Size(),EAspectRatioNotPreserved);	
		
		iBackAdd.ResetAndDestroy();
		iBackAdd.Append(LoadImageL(privFile.File(),EMbmYapp_a000312dSplash,EMbmYapp_a000312dSplash_mask,Rect().Size()));
		iBackAdd.Append(LoadImageL(privFile.File(),EMbmYapp_a000312dSplash2,EMbmYapp_a000312dSplash2_mask,Rect().Size()));
		iBackAdd.Append(LoadImageL(privFile.File(),EMbmYapp_a000312dSplash3,EMbmYapp_a000312dSplash3_mask,Rect().Size()));
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::HandleResourceChange(TInt aType)
{
	TRect rect;
#ifdef __SERIES60_3X__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {    
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
    }
#else
    if ( aType == 0x101F8121 )
    {
    	rect = CEikonEnv::Static()->EikAppUi()->ClientRect();
    	SetRect(rect);
    }
#endif 

	CCoeControl::HandleResourceChange(aType);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::NextImage(void)
{
	iCurrent++;
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
    
	if(iBackGround)
	{
		if(iBackGround->Handle())
		{
			gc.DrawBitmap(Rect(),iBackGround);
		}
	}
	
	CGulIcon* TmpDrawMe(NULL);
	if(iBackAdd.Count())
	{
		if(iCurrent < iBackAdd.Count())
		{
			TmpDrawMe = iBackAdd[iCurrent];
		}
		else
		{
			TmpDrawMe = iBackAdd[0];
		}
	}
			
	DrawImages(gc,Rect(),TmpDrawMe);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::DrawImages(CWindowGc& aGc,const TRect& aRect,CGulIcon* aIcon) const
{
	if(aIcon)
	{
		if(aIcon->Bitmap() && aIcon->Mask())
		{
			if(aIcon->Bitmap()->Handle() && aIcon->Mask()->Handle())
			{
				TSize ImgSiz(aIcon->Bitmap()->SizeInPixels());
				
				TInt LFtMargin = aRect.iTl.iX + ((aRect.Width() - ImgSiz.iWidth) / 2);
				TInt RghMargin = aRect.iTl.iY + ((aRect.Height() - ImgSiz.iHeight) / 2);
				
				TRect DrwRect(LFtMargin,RghMargin,(ImgSiz.iWidth + LFtMargin),(ImgSiz.iHeight + RghMargin));
				
				aGc.DrawBitmapMasked(DrwRect,aIcon->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),aIcon->Mask(),EFalse);
			}
		}
	}
}

