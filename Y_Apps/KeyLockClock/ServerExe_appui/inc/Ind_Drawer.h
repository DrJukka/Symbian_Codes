


#ifndef __CLOCK_DRAWER_H__
#define __CLOCK_DRAWER_H__

#include <e32std.h>
#include <w32std.h>
#include <coedef.h>
#include <apgwgnam.h>
#include <coecntrl.h>
// INCLUDES
#include <HWRMLight.h> // Link against HWRMLightClient.lib.

#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <Etel3rdParty.h>

#include <mprofileengine.h>
#include <mprofilesnamesarray.h>
#include <mprofile.h>
#include <mprofilename.h>


#include "CTimeOutTimer.h"
#include "Drawer_Observers.h"
#include "Signal_Observer.h"

_LIT(KtxClockWGName				,"Key lock Clock");

struct TScreenVals
	{
		TInt 	iLightVal,iLightOn,iLightTime,iLightVal2,iShowDate,iScreenMode;
		TRgb 	iBgColorRgb, iFontColorRgb;
		TUint 	iMajor,iMinor;
	};

struct TMonitorVals
	{
		TInt iLightTimeOut,iMissedCalls,iEmailStatus,iSMSCount,iMMSCount;
		CTelephony::TBatteryStatus	iBatStatus;
		TUint iBatteryCharge;
	};	

class CClockDrawer : public CActive,MTimeOutNotify, MKeyCallBack,MSignalObsCallBack
{
public:
	static CClockDrawer* NewL(MLayoutChnageCallBack& aCallback,const TDes& aFont,const TScreenVals& aSettings);
	static CClockDrawer* NewLC(MLayoutChnageCallBack& aCallback,const TDes& aFont,const TScreenVals& aSettings);
	~CClockDrawer();
	void SetValuesL(const TScreenVals& aSettings);
	void SetMonitorValues(const TMonitorVals& aMonitorVals);
	void ReFreshL();
	void ReFreshhhL(void);
public:
	void ForegroundChanged(const TBool& aVisible);
protected:	
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId); // needed for skins stuff
protected:	
	void SignalStatus(TInt32 aStrength,TInt8 aBars);
	TBool KeyCaptured(TWsEvent aEvent);
	void DoCancel();
	TInt RunError(TInt aError);
	void RunL();
	void TimerExpired();
private:

	void DrawReally(void);
	void CreateBackBufferL(const TSize& aSize);
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,TSize aSize);
	void GetTimeBuffer(TDes& aBuffer, const TTime& aTime);
	void my5800HackForLights();
	void GetFontL(const TInt& aPresentage,const TDes& aFont);
	CClockDrawer(MLayoutChnageCallBack& aCallback,const TScreenVals& aSettings);
	void ConstructL(const TDes& aFont);
	void Listen();
	void DrawIcon(CFbsBitGc* aGc,TRect& aRect, CGulIcon* aIcon,const TBool& aDoDraw,const TBool& aRotated);
	void DrawNotifications(CFbsBitGc* aGc,const TRect& aRect, const TBool& aCalls,const TBool& aEmails,const TBool& aMmsMsg,const TBool& aSmsMsg,const TBool& aRotated);
	void Draw(void);
private:
	MLayoutChnageCallBack& 			iCallback;
	RWsSession 						iWsSession; 
	CWindowGc* 						iGc;
	CWsScreenDevice* 				iScreen;
	RWindowGroup 					iWg; 
	RWindow 						iMyWindow; 
	TBool    						full_redraw,iChargerIsOn;
	CTimeOutTimer* 					iTimeOutTimer;
	TSize							iScreenSize;
	RFs								iFsSession;
	TPixelsTwipsAndRotation 		iSizeAndRotation;
	TInt							iLightTimeOut;
	CKeyObserver*					iKeyObserver;
	CHWRMLight* 					iHWRMLight;
	CFont*							iUseFont;
	HBufC*							iDateFormatString;
	HBufC*							iTimeFormatString;
	TBool                       iMirroRed;
	RFile							iFile;
	TInt							iDbgInt;	
	CAknsBasicBackgroundControlContext*	iBgContext; // skins are drawn with this one
	CMySignalObserver*				iMySignalObserver;
	TInt8 							iSignalBars;
	TScreenVals						iSettings;
	TMonitorVals					iMonitorVals;
	TInt							iOriginalScreenMode;
	CGulIcon* 						iEmailIcon;
	CGulIcon* 						iCallIcon;
	CGulIcon* 						iMMSIcon;
	CGulIcon* 						iSMSIcon;
	CFbsBitmap*						iBatteryBitmap;
	CFbsBitmap*						iBatteryBitmapMask;
	CFbsBitmap*						iSignalBitmap;
	CFbsBitmap*						iSignalBitmapMask;
	CFbsBitmap*						iNoSignalBitmapMask;
	CFbsBitmap*						iNoSignalBitmap;
	CFbsBitmap*						iBatteryIcon;
	CFbsBitmap*						iBatteryIconMask;
	TBool							iRotated;
	MProfileEngine* 				iProfileEngine;
	CFbsBitmapDevice*               iBackBufferDevice;
	CFbsBitGc*                      iBackBufferContext;
	CFbsBitmap*                     iBackBuffer;

	TSize							iBackBufferSize;
};






#endif //__CLOCK_DRAWER_H__
