

#ifndef __MYPLAY_CONTAINER_H__
#define __MYPLAY_CONTAINER_H__

#include <COECNTRL.H>		
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <CNTDB.H>
#include <MdaImageConverter.h>
#include <HWRMLight.h> // Link against HWRMLightClient.lib.
#include <HWRMVibra.h>

#ifdef __S60_50__		
	#include <AknButton.h>
#else
	
#endif


#include "OperatorName.h"
#include "playeradapter.h"
#include "CTimeOutTimer.h"
	

class CMyHelpContainer;

/*! 
  @class CPlayContainer
  
  @discussion An instance of the Application View object for the HelloWorld 
  example application
  */
class CPlayContainer : public CCoeControl,MMdaImageUtilObserver,MPlaybackObserver
,MTimeOutNotify, MHWRMVibraObserver , public MCoeControlObserver,MOperatorNameCallback
    {
public:
    static CPlayContainer* NewL(const TRect& aRect,CEikButtonGroupContainer* aCba);
    static CPlayContainer* NewLC(const TRect& aRect,CEikButtonGroupContainer* aCba);
    ~CPlayContainer();
public:
	void HandleViewCommandL(TInt aCommand);
	
	void AddContactL(const TDesC& aNumber);
	void StartTimerL(void);
	void CheckProfileSettingsL();
	void StartRingtoneL(void);
	void StartCallerImageL(void);
	void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
	void SnoozeL();
	void CheckNormalSettingsL(void);
public:  
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId); // needed for skins stuff
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
protected:
	TBool IsOffLineProfileOnL();
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue);
	void OperatorNameL(const TDesC& aOpName, TInt aError);
	void TimerExpired();
	void NotifyPlayComplete(TInt aError);
	void NotifyFileOpened(TInt aError);	
	void MiuoConvertComplete(TInt aError);
	void MiuoOpenComplete(TInt aError);
	void MiuoCreateComplete(TInt aError);	
	void VibraModeChanged(CHWRMVibra::TVibraModeState aState) {};
	void VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus) {};
private:
	CGulIcon* GetButtonIconL(const TAknsItemID& aIcon,const TSize& aSize);
	CGulIcon* GetPhoneIconL(const TDesC& aFilename,const TInt& aIcon,const TInt& aMask,const TSize& aSize);
	void SetMenuL(void);
	
	void Draw(const TRect& aRect) const;
	virtual void SizeChanged();
	void ConstructL(const TRect& aRect);
    CPlayContainer(CEikButtonGroupContainer* aCba);
private:
	CEikButtonGroupContainer* 	iCba;
	CAknsBasicBackgroundControlContext*	iBgContext; // skins are drawn with this one
	CContactDatabase*			iContactsDb;
	TBuf<100>					iNumber,iName;
	HBufC8*						iImageBuffer;
	TFileName					iRingtone,iImageFile,iFallBackRingTone;
	TBool						iVibraOn,iRepeat,iAscending,iJustBeep,iFlashLights,iLightsAreOn,iSnoozeOn;
	TInt						iRingVolume,iRingVolumeRamp,iTimCount,iSnoozeDelay,iMaxTime;
	TBool                       iMirroRed;
	TBool						iImageOk;
	CMdaImageFileToBitmapUtility*	iMdaImageFileToBitmapUtility;
	CMdaImageDescToBitmapUtility*	iMdaImageDescToBitmapUtility;
	TPtrC8 							iMagePoint;
	CFbsBitmap*					iCallerImgTmp;
	TRect						iImageRect;
	
	CTimeOutTimer*				iTimeOutTimer;
	CPlayerAdapter*				iPlayerAdapter;
	
	CHWRMLight* 				iHWRMLight;
	CHWRMVibra*                 iHWRMVibra; 
	
	TRect						iLeftBtOut,iRightBtOut,iCallOut,iCallIn;
#ifdef __S60_50__		
	CAknButton*					iLeftButton;
	CAknButton*					iRightButton;
#else
	
#endif	
	TBool						iYellowOn;
	CGulIcon* 					iGreenPhone;
	CGulIcon* 					iYellowPhone;
	CGulIcon* 					iBluePhone;
	
	CFont*						iButtonFont;
	CFont*						iTextFont;
	
	CGetOperatorName*			iGetOperatorName;
	
	CMyHelpContainer*			iMyHelpContainer;
	TTime						iStartTime;
	
	RFile 	iDebugFile;

};


#endif // __MYPLAY_CONTAINER_H__
