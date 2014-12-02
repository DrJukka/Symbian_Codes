/* 
	Copyright (c) 2001 - 2008, 
	Dr. Jukka Silvennoinen. 
	All rights reserved 
*/

#include "Splash_Screen.h"
#include <EIKENV.H>
#include <EIKAPPUI.H>
#include <aknutils.h>
#include <eikapp.h>
#include <AknGlobalNote.h>

#include "Common.h"

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
	delete iBgContext;
	iBgContext = NULL;
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
	// make first with no size at all
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	// Setting rect will cause SizeChanged to be called
	// and iBgContext size & position is updated accordingly.
	SetRect(CEikonEnv::Static()->EikAppUi()->ApplicationRect());
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
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  
 	delete iBgPic;
	iBgPic = NULL;
	delete iBgMsk;
	iBgMsk = NULL;
		
    AknsUtils::CreateAppIconLC(skin,KUidTOneViewer,  EAknsAppIconTypeContext,iBgPic,iBgMsk);

	TSize IconSiz((Rect().Width() /2),(Rect().Height() /2));

	AknIconUtils::SetSize(iBgPic,IconSiz);  
	AknIconUtils::SetSize(iBgMsk,IconSiz);  

	CleanupStack::Pop(2);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::HandleResourceChange(TInt aType)
{
	TRect rect;
	TBool SetR(EFalse);

    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, rect);
        
    } 

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
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMySplashScreen::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
    
  	// draw background skin first.
  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
    
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

