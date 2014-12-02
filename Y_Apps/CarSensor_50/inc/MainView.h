/*
============================================================================

============================================================================
*/

#ifndef __MOVINGBALLAPPVIEW_h__
#define __MOVINGBALLAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>
#include <MdaAudioTonePlayer.h>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include <sensrvchannelfinder.h>
#include <sensrvchannel.h>
#include <sensrvmagnetometersensor.h>
#include <sensrvgeneralproperties.h>
#include <sensrvmagnetometersensor.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvorientationsensor.h>
#include <sensrvdatalistener.h> 
#include <sensrvtappingsensor.h> 
#include <sensrvtypes.h> 

// FORWARD DECLARATIONS
class CRDAccelerometer;
class CMyHelpContainer;
class CEikButtonGroupContainer;

// CLASS DECLARATION
class CMovingBallAppView : public CCoeControl,MSensrvDataListener
	{
	public: // New methods
		static CMovingBallAppView* NewL( const TRect& aRect,CEikButtonGroupContainer* aCba);
		static CMovingBallAppView* NewLC( const TRect& aRect,CEikButtonGroupContainer* aCba);
		virtual ~CMovingBallAppView();
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		void HandleViewCommandL(TInt aCommand);
		void SetMenuL(void);
	public:  // Functions from base classes
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl( TInt /*aIndex*/) const;
		void SetLights(TBool aOn);
		TBool LightOn(void){return iLightOn;};
		void Reset(void);
		void Draw( const TRect& aRect ) const;
		virtual void SizeChanged();
		void ShowNoteL(const TDesC&  aMessage);
	private: // New Functions
		TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
	private: // Constructors
		TBool HandleAccelerationL( TInt aX, TInt aY, TInt aZ, TInt& aZAngle, TInt& aAngle);
		
		void HandleAccelerationL( TInt aX, TInt aY, TInt aZ);
		void ConstructL(const TRect& aRect);
		CMovingBallAppView(CEikButtonGroupContainer* aCba);
	 	void DataReceived( CSensrvChannel& aChannel, TInt aCount, TInt aDataLost );
	    void DataError( CSensrvChannel& aChannel, TSensrvErrorSeverity aError );
	    void GetDataListenerInterfaceL( TUid /*aInterfaceUid*/, TAny*& /*aInterface*/ ){};	
	private: // Data
		CEikButtonGroupContainer* 	iCba;
		CSensrvChannel*			iSensrvSensorChannel;		
		TInt 			iAngle1,iAngle2;
		TInt 			iResetAngle1,iResetAngle2;

		TInt iCount,iCount2,iCount3,iCount4;
		TInt64 iXAvg,iYAvg,iZAvg;
		TInt64 iXAvg2,iYAvg2,iZAvg2;
		TInt64 iXAvg3,iYAvg3,iZAvg3;
		TInt64 iXAvg4,iYAvg4,iZAvg4;
				
		CFbsBitmap* 	iFrameImg;
		CFbsBitmap* 	iFrameMsk;
		CFbsBitmap* 	iCarImg;
		CFbsBitmap* 	iCarMsk;
		CAknsBasicBackgroundControlContext*	iBgContext;
		TBool			iLightOn,iResetNow;
		CMyHelpContainer*		iMyHelpContainer;
		TBuf<100> iDbgBuf;
	};

#endif // __MOVINGBALLAPPVIEW_h__

// End of File
