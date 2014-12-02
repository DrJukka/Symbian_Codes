/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */

#include <AknUtils.h>
#include <w32std.h>
#include <coemain.h>
#include <aknsutils.h>
#include <StringLoader.h>

#include "ThemePreview_Container.h"
#include <S125AniLauncher_200027A9.rsg>

#include "S125AniLauncher.h" 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CPreviewContainer::CPreviewContainer(TInt aSpeed,TUint32 aThemeId)
:iSpeed(aSpeed),iThemeId(aThemeId)
{
 
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CPreviewContainer::~CPreviewContainer()
{
	if(iTimeOutTimer)
		iTimeOutTimer->Cancel();
		
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	delete iFrameReader;    
    iFrameReader = NULL;
    
    iBackImgages.ResetAndDestroy();

	delete iBgContext;
	iBgContext = NULL;
	
	iAknsSrvSession.Close();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::ConstructL(const TRect& aRect)
{	
    CreateWindowL();
    
    iAknsSrvSession.Connect(this);
  	iAknsSrvSession.EnableSkinChangeNotify();
  	
    GetCurrentSkinIdL(iCurrentThemeId,iCurrentLocation);
    
    if(!SetThemeOnNowL(iThemeId,EAknsSrvPhone))
	{
		SetThemeOnNowL(iThemeId,EAknsSrvMMC);
	}

    iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);

    SetRect(aRect);
    ActivateL();
	DrawNow();	
	
	iReadAlready = EFalse;
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	iTimeOutTimer->After(iSpeed);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CPreviewContainer::SetThemeOnNowL(TUint32& aThemeId,TAknSkinSrvSkinPackageLocation aLocation)
{
	TBool FoundAlready(EFalse);
    
	CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = iAknsSrvSession.EnumerateSkinPackagesL(aLocation);
	CleanupStack::PushL( skinInfoArray );
	  	
    for(TInt i=0; i < skinInfoArray->Count(); i++)
	{
		if(((TUint32)skinInfoArray->At(i)->PID().Uid().iUid)  == aThemeId)
		{
			TAknsPkgID pkgId = skinInfoArray->At(i)->PID();
			
			iAknsSrvSession.SetAllDefinitionSets( pkgId );	
			FoundAlready = ETrue;
			SetNewSkinIdL( pkgId, aLocation);
        }
	}

	skinInfoArray->ResetAndDestroy();        
	CleanupStack::PopAndDestroy(1); //skinInfoArray
	
	return FoundAlready;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::GetCurrentSkinIdL(TAknsPkgID& aSkinId,TAknSkinSrvSkinPackageLocation aLocation)
{
    TAknsPkgIDBuf pidBuf;            
    
    CRepository* repository = CRepository::NewL( KCRUidPersonalisation );                    
    repository->Get( KPslnActiveSkinUid, pidBuf );
    
    TInt MyLocation(0);
    repository->Get( KPslnActiveSkinLocation, MyLocation);
    
    if(MyLocation == 2)
    	aLocation = EAknsSrvMMC;
    else
    	aLocation = EAknsSrvPhone;
    
    delete repository;
    repository = NULL;
    
    aSkinId.SetFromDesL( pidBuf );
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::SetNewSkinIdL(TAknsPkgID aPkgId,TAknSkinSrvSkinPackageLocation aLocation)
{
    TAknsPkgIDBuf pidBuf;            
    aPkgId.CopyToDes( pidBuf );
    CRepository* repository = CRepository::NewL( KCRUidPersonalisation );                    

    TInt retVal = repository->Set( KPslnActiveSkinUid, pidBuf );
    retVal = repository->Set( KPslnActiveSkinLocation,aLocation);
    // KPslnActiveSkinLocation has to be changed also
    // if new skin resides on mmc whereas old one resided in phone mem
    // ...and vice versa
    
    delete repository;
    repository = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::SkinContentChanged ()
{
	TAknSkinSrvSkinPackageLocation Dummy;
	TAknsPkgID 	NowThemeId;
	GetCurrentSkinIdL(NowThemeId,Dummy);
	
	if(iThemeId != (TUint32)NowThemeId.iNumber)
	{
		ReadAnimationImagesL();
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CPreviewContainer::SkinConfigurationChanged (const TAknsSkinStatusConfigurationChangeReason aReason)
{
	TAknSkinSrvSkinPackageLocation Dummy;
	TAknsPkgID 	NowThemeId;
	GetCurrentSkinIdL(NowThemeId,Dummy);
	
	if(iThemeId != (TUint32)NowThemeId.iNumber)
	{
		ReadAnimationImagesL();
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CPreviewContainer::SkinPackageChanged (const TAknsSkinStatusPackageChangeReason aReason)
{
	TAknSkinSrvSkinPackageLocation Dummy;
	TAknsPkgID 	NowThemeId;
	GetCurrentSkinIdL(NowThemeId,Dummy);
	
	if(iThemeId != (TUint32)NowThemeId.iNumber)
	{
		ReadAnimationImagesL();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::SizeChanged()
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
TTypeUid::Ptr CPreviewContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CPreviewContainer::ReadAnimationImagesL()
{
	delete iFrameReader;
	iFrameReader = NULL;
	
	TRect Rectangle(Rect());
 		
 	if(CCoeEnv::Static()->ScreenDevice())
 	{
 		CCoeEnv::Static()->ScreenDevice()->GetScreenModeSizeAndRotation(CCoeEnv::Static()->ScreenDevice()->CurrentScreenMode(),iSizeAndRotation);
		
		Rectangle.iTl.iY = 0;
		Rectangle.iTl.iX = 0;
		Rectangle.iBr.iY = iSizeAndRotation.iPixelSize.iHeight;
		Rectangle.iBr.iX = iSizeAndRotation.iPixelSize.iWidth;
		
		if(CFbsBitGc::EGraphicsOrientationNormal == iSizeAndRotation.iRotation)
		{
			AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, Rectangle);
	
			TInt Multip = (iSizeAndRotation.iPixelSize.iHeight / 208);
	 		Rectangle.iTl.iY = Rectangle.iTl.iY + (13 * Multip);
		}
 	}
	
	iFrameReader = CFrameReader::NewL(*this,Rectangle);
	iFrameReader->StartReadingL();	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::AllDoneL(TInt /*aError*/)
{
	if(!iBackImgages.Count())
	{
		TBuf<60> TmpBuffer;
		StringLoader::Load(TmpBuffer,R_STR_NOANIMATION);

		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->ShowNoteL(TmpBuffer);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::TimerExpired()
{
	if(!iReadAlready)
	{
		iReadAlready = ETrue;
		ReadAnimationImagesL();
		
		if(iTimeOutTimer)
		{
			iTimeOutTimer->After(iSpeed);
		}
	}
	else
	{
		TPixelsTwipsAndRotation SizeAndRotation;
	 	if(CCoeEnv::Static()->ScreenDevice())
	 	{
	 		CCoeEnv::Static()->ScreenDevice()->GetScreenModeSizeAndRotation(CCoeEnv::Static()->ScreenDevice()->CurrentScreenMode(),SizeAndRotation);
	 	}
	 	TRect Rectangle(0,0,SizeAndRotation.iPixelSize.iWidth,SizeAndRotation.iPixelSize.iHeight);
	 				
		if(iSizeAndRotation.iRotation != SizeAndRotation.iRotation)
		{
			ReadAnimationImagesL();
		}
		else if(iBackImgages.Count())
		{		
			iCurrent = iCurrent + 1;
			if(iCurrent >= iBackImgages.Count())
			{
				iCurrent = 0;
			}
			
			DrawNow();	
		
			if(iTimeOutTimer)
			{
				iTimeOutTimer->After(iSpeed);
			}
		}
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPreviewContainer::Draw(const TRect& /*aRect*/ ) const 
{
    TRect drawingRect = Rect();
    CWindowGc& gc = SystemGc();
	
	if(iCurrent < iBackImgages.Count())
	{
		if(iBackImgages[iCurrent])
		{	
			if(iBackImgages[iCurrent]->Handle())
			{
				gc.DrawBitmap(drawingRect,iBackImgages[iCurrent]);	
			}
		}
	}
	else if(iBackImgages.Count())
	{
		if(iBackImgages[0])
		{	
			if(iBackImgages[0]->Handle())
			{
				gc.DrawBitmap(drawingRect,iBackImgages[0]);	
			}
		}
	}
	else
	{
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, drawingRect);
	}
}	
