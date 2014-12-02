/* Copyright (c) 2001 - 2005 , J.P. Silvennoinen. All rights reserved */


#ifndef __THEMEPREVIEW_CONTAINER_H__
#define __THEMEPREVIEW_CONTAINER_H__

#include <coecntrl.h>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <centralrepository.h>

#include <aknseffectanim.h>
#include <aknsitemdef.h>
#include <aknspkgid.h>
#include <aknspkgid.inl>
#include <aknspointerstore.h>
#include <aknsrendererwrapper.h>
#include <aknsskinuid.h>
#include <aknssrvchunklookup.h>
#include <aknssrvclient.h>
#include <aknssrvskininformationpkg.h>
#include <aknssrvvariant.hrh>
#include <AknSkinsInternalCRKeys.h>
#include <aknsbasicbackgroundcontrolcontext.h> 
#include <aknssrvskininformationpkg.h>
#include "CTimeOutTimer.h"
#include "AniFrameReader.h"

class CEikLabel; 
class CFbsBitmap;
class CWsBitmap;

/**
*  CPreviewContainer  container control class.
*  
*/
class CPreviewContainer : public CCoeControl,MFrameReadObserver,MTimeOutNotify
, public MAknsSkinChangeObserver
    {
    public: 
        CPreviewContainer(TInt aSpeed,TUint32 aThemeId);
		void ConstructL(const TRect& aRect);
        ~CPreviewContainer();
 	protected:
    	void SkinContentChanged ();
		void SkinConfigurationChanged (const TAknsSkinStatusConfigurationChangeReason aReason);
		void SkinPackageChanged (const TAknsSkinStatusPackageChangeReason aReason);
    protected:
    	TTypeUid::Ptr MopSupplyObject(TTypeUid aId); 
    	void AllDoneL(TInt aError);
		RPointerArray<CFbsBitmap>& AniArrayL(void){ return iBackImgages;};
    	void TimerExpired();
    private:
    	TBool SetThemeOnNowL(TUint32& aThemeId,TAknSkinSrvSkinPackageLocation aLocation);
    	void SetNewSkinIdL(TAknsPkgID aPkgId,TAknSkinSrvSkinPackageLocation aLocation);
    	void GetCurrentSkinIdL(TAknsPkgID& aSkinId,TAknSkinSrvSkinPackageLocation aLocation);
    	virtual void SizeChanged();
    	void ReadAnimationImagesL();
        void Draw(const TRect& aRect) const;     
   	private: 
   		CAknsBasicBackgroundControlContext*	iBgContext; 
        RPointerArray<CFbsBitmap>	iBackImgages;
        CFrameReader*				iFrameReader;
        CTimeOutTimer*				iTimeOutTimer;
        TInt						iCurrent;
        TTimeIntervalMicroSeconds32 iSpeed;
        TPixelsTwipsAndRotation 	iSizeAndRotation;
        TBool 						iReadAlready;
        TUint32 					iThemeId;
        TAknsPkgID 						iCurrentThemeId;
        TAknSkinSrvSkinPackageLocation 	iCurrentLocation;
        RAknsSrvSession				iAknsSrvSession;
    };

#endif // __THEMEPREVIEW_CONTAINER_H__
