/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#include "Splash_Screen.h"
#include <EIKENV.H>
#include <EIKAPPUI.H>
#include <eikapp.h>


#include "YApp_E887600D.h"

// CMySplashScreen
//
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMySplashScreen::~CMySplashScreen()
{
	delete iBgPic;
	iBgPic = NULL;
	delete iBgMsk;
	iBgMsk = NULL;
#ifdef __SERIES60_3X__
	delete iBgContext;
	iBgContext = NULL;
#else

#endif
}   
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMySplashScreen* CMySplashScreen::NewL(void)
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
#ifdef __SERIES60_3X__
	// make first with no size at all
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	// Setting rect will cause SizeChanged to be called
	// and iBgContext size & position is updated accordingly.
#endif
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	GetSplashIconL();
	
	ActivateL();	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMySplashScreen::GetSplashIconL(void)
{

 	delete iBgPic;
	iBgPic = NULL;
	delete iBgMsk;
	iBgMsk = NULL;

#ifdef __SERIES60_3X__	
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  	
    AknsUtils::CreateAppIconLC(skin,KUidYBrowserApp,  EAknsAppIconTypeContext,iBgPic,iBgMsk);

	TSize IconSiz((Rect().Width() /2),(Rect().Height() /2));

	AknIconUtils::SetSize(iBgPic,IconSiz);  
	AknIconUtils::SetSize(iBgMsk,IconSiz);  

	CleanupStack::Pop(2);
#else
/*	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KtxImagesName, KNullDesC))
	{
		iBgPic = new(ELeave)CFbsBitmap();
		iBgPic->Load(AppFile.File(),EMbmSplashSplash);
	}*/
#endif
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::SizeChanged()
{
#ifdef __SERIES60_3X__	
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
#endif

}
#ifdef __SERIES60_3X__
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::HandleResourceChange(TInt aType)
{
	TRect rect;
	TBool SetR(EFalse);
#ifdef __SERIES60_3X__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        
    }
#else
    if ( aType == 0x101F8121 )
    {
    	SetR = ETrue;
    	rect = CEikonEnv::Static()->EikAppUi()->ClientRect();
    }
#endif  

    if (SetR)
    {    
        SetRect(rect);
    }

	CCoeControl::HandleResourceChange(aType);
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CMySplashScreen::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
#endif
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
 
 #ifdef __SERIES60_3X__
  	// draw background skin first.
  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
 #endif
 
    if(iBgPic && iBgMsk)
	{
		if(iBgPic->Handle() && iBgMsk->Handle())
		{
			TSize ImgSiz(iBgPic->SizeInPixels());
			TSize RectSiz();
			
			TInt Xpoint = ((Rect().Width() - ImgSiz.iWidth) / 2);
			TInt Ypoint = ((Rect().Height()- ImgSiz.iHeight)/ 2);
		
			TRect DestRect(Xpoint,Ypoint,(Xpoint + ImgSiz.iWidth),(Ypoint + ImgSiz.iHeight));
			TRect SrcRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight);
			
			#ifdef __SERIES60_3X__
				gc.DrawBitmapMasked(DestRect,iBgPic,SrcRect,iBgMsk, EFalse);
			#else
				gc.BitBltMasked(DestRect.iTl,iBgPic,SrcRect,iBgMsk,EFalse);
			#endif
		}	
	}
	else if(iBgPic)
	{
		if(iBgPic->Handle())
		{
			gc.DrawBitmap(Rect(),iBgPic);
		}	
	}
}

