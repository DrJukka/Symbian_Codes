

#include "Ind_Drawer.h"


#include <apgcli.h>

#include <BAUTILS.H>
#include <EIKENV.H>
#include <HAL.H>
#include <aknutils.h> 
#include <GULICON.H>
#include <AknBidiTextUtils.h>

#include <avkon.mbg>

#include <centralrepository.h>
#include <LogsInternalCRKeys.h>
#include <coreapplicationuisdomainpskeys.h>

#include "Common.h"
#include "app_iccon.mig"

const TInt KReFreshSpeed = 1000000;
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CClockDrawer* CClockDrawer::NewL(MLayoutChnageCallBack& aCallback,const TDes& aFont,const TScreenVals& aSettings)
	{
	CClockDrawer* self = CClockDrawer::NewLC(aCallback,aFont,aSettings);
	CleanupStack::Pop(self);
	return self;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CClockDrawer* CClockDrawer::NewLC(MLayoutChnageCallBack& aCallback,const TDes& aFont,const TScreenVals& aSettings)
	{
	CClockDrawer* self = new (ELeave) CClockDrawer(aCallback,aSettings);
	CleanupStack::PushL(self);
	self->ConstructL(aFont);
	return self;
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CClockDrawer::CClockDrawer(MLayoutChnageCallBack& aCallback,const TScreenVals& aSettings)
:CActive(EPriorityHigh),iCallback(aCallback),iSignalBars(0),iSettings(aSettings),iOriginalScreenMode(-1)
{



}//,iLightTimeOut(aLightTime) // can not initialize, devices with screensaver will cause a loop..

	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/


CClockDrawer::~CClockDrawer()
{
	Cancel();

	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Start exit, "));	
	}
	
	if(iTimeOutTimer)
		iTimeOutTimer->Cancel();
	
	if(iMyWindow.WsHandle()){
		iMyWindow.SetVisible(EFalse);
	}
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	delete iMySignalObserver;
	iMySignalObserver = NULL;
	
	delete iKeyObserver;
	iKeyObserver = NULL;
	
	delete iHWRMLight;
	iHWRMLight = NULL;
	
	delete iDateFormatString;
	iDateFormatString = NULL;
	
	delete iTimeFormatString;
	iTimeFormatString = NULL;
	
	delete iBgContext;
	iBgContext = NULL;
	
	delete iEmailIcon;
	iEmailIcon = NULL;
	delete iCallIcon;
	iCallIcon = NULL;
	delete iMMSIcon;
	iMMSIcon = NULL;
	delete iSMSIcon;
	iSMSIcon = NULL;
	
	if(iProfileEngine)
	{
		iProfileEngine->Release();
	}
		
	delete iBatteryBitmap;
	delete iBatteryBitmapMask;	
	
	delete iSignalBitmap;
	delete iSignalBitmapMask;
	
	delete iNoSignalBitmapMask;
	delete iNoSignalBitmap;
	
	delete iBatteryIcon;
	delete iBatteryIconMask;
	
    if(iUseFont && iScreen)
    {
		iScreen->ReleaseFont(iUseFont);
    	iUseFont = NULL; 
    }
    
	delete iGc;
	iGc = NULL;
	
	delete iBackBufferContext;
	iBackBufferContext = NULL;
	delete iBackBufferDevice;
	iBackBufferDevice = NULL;
	delete iBackBuffer;
	iBackBuffer = NULL;
	
	delete iScreen;
	iScreen = NULL;
	
	iMyWindow.Close();
	iWg.Close();

	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Bye, bye, "));	
	  	iFile.Close();
	}
	
	iFsSession.Close();
	iWsSession.Close();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::SignalStatus(TInt32 /*aStrength*/,TInt8 aBars)
{
	iSignalBars = aBars;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::SetValuesL(const TScreenVals& aSettings)
{
	iSettings = aSettings;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::SetMonitorValues(const TMonitorVals& aMonitorVals)
{
	iMonitorVals = aMonitorVals;

	if(iMonitorVals.iBatStatus == CTelephony::EBatteryConnectedButExternallyPowered
	|| iMonitorVals.iBatStatus == CTelephony::ENoBatteryConnected)
		iChargerIsOn = ETrue;
	else
		iChargerIsOn = EFalse;
	
	
	if(iFile.SubSessionHandle())
	{	
		if(iChargerIsOn)
			iFile.Write(_L8("iChargerIsOn\n\r"));
		else
			iFile.Write(_L8("iChargerIs off\n\r"));
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::ConstructL(const TDes& aFont)
{										
	User::LeaveIfError(iFsSession.Connect());
	User::LeaveIfError(iWsSession.Connect());
	
	_LIT(KRecFilename			,"C:\\KeyLockClockUI.txt");
	iFsSession.Delete(KRecFilename);
	
	iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);	
	
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8("start 1, "));
	}
	
	iMirroRed = AknLayoutUtils::LayoutMirrored();
	
	iTimeFormatString = CEikonEnv::Static()->AllocReadResourceL(R_QTN_TIME_USUAL);
	iDateFormatString = CEikonEnv::Static()->AllocReadResourceL(R_QTN_DATE_USUAL);
		
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8("start 2, "));
	}
	
	iScreen=new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScreen->Construct());
	
	User::LeaveIfError(iScreen->CreateContext(iGc));
	
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8(", 3, "));
	}
	
	iWg=RWindowGroup(iWsSession);
	User::LeaveIfError(iWg.Construct((TUint32)&iWg, EFalse));
	
	iWg.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront +101);
	iWg.EnableReceiptOfFocus(EFalse);//ETrue);//

	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8(", 4, "));
	}
	
	CApaWindowGroupName* wn=CApaWindowGroupName::NewLC(iWsSession);
	wn->SetHidden(ETrue);
	wn->SetWindowGroupName(iWg);
	CleanupStack::PopAndDestroy();

	iMyWindow=RWindow(iWsSession);
	User::LeaveIfError(iMyWindow.Construct(iWg, (TUint32)&iMyWindow));
	
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8(", 5, "));
	}
	
	iScreen->GetScreenModeSizeAndRotation(iScreen->CurrentScreenMode(),iSizeAndRotation);
 
 	TRect Rectangle(0,0,iSizeAndRotation.iPixelSize.iWidth,iSizeAndRotation.iPixelSize.iHeight);

	iScreenSize = Rectangle.Size();
	iBackBufferSize.iHeight = iScreenSize.iHeight;
	iBackBufferSize.iWidth  = iScreenSize.iWidth;

	iRotated = EFalse;
	
	if(iSettings.iScreenMode == 0){
		if(iScreenSize.iWidth > iScreenSize.iHeight){
			iRotated = ETrue;
			iBackBufferSize.iHeight = iScreenSize.iWidth;
			iBackBufferSize.iWidth = iScreenSize.iHeight;
		}
	}else if(iSettings.iScreenMode == 1){
		if(iScreenSize.iWidth < iScreenSize.iHeight){
			iRotated = ETrue;
			iBackBufferSize.iHeight = iScreenSize.iWidth;
			iBackBufferSize.iWidth = iScreenSize.iHeight;
		}	
	}
	
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8("bb-buffer, "));
	}

	CreateBackBufferL(iScreenSize);
	
	iMyWindow.Activate();
	iMyWindow.SetExtent(Rectangle.iTl,Rectangle.Size());//
	//iMyWindow.SetExtent(TPoint(0,TopCorner), iScreenSize);
	iMyWindow.SetBackgroundColor(KRgbRed);
	iMyWindow.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront +101);
	iMyWindow.SetNonFading(ETrue);
	iMyWindow.SetPointerCapture(RWindowBase::TCaptureDragDrop);

	CActiveScheduler::Add(this);

	TBool fits(EFalse);
	
	TBuf<50> hjelpperbuf;
	
	TDateTime hjelpTD(2012,EDecember,12,23,59,59,0);
	
	TTime hjelpperTime(hjelpTD);
	GetTimeBuffer(hjelpperbuf,hjelpperTime);
	
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8("do-while font, "));
	}
	
	TInt sizze(100);
	do
	{		
		GetFontL(((iBackBufferSize.iHeight * sizze) / 100),aFont);
	
		if(sizze < 10)
		{
			fits = ETrue; // just to make sure the loop does exit..
		}
		else if(iUseFont)
		{
			TInt needSiz = iUseFont->TextWidthInPixels(hjelpperbuf);
			if(needSiz < iBackBufferSize.iWidth)
			{
				fits = ETrue;
			}
		}
		
		sizze--;
		
	}while(!fits);
	
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,Rectangle, ETrue);
	
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8("re-fresh, "));
	}
	
	ReFreshL();
	
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
	iTimeOutTimer->After(KReFreshSpeed);
	
	if(iFile.SubSessionHandle()){	
		iFile.Write(_L8("listen, "));
	}
	
	Listen();
	
	iKeyObserver = CKeyObserver::NewL(this);
	iHWRMLight = CHWRMLight::NewL();
	
	TFindFile AppFolder(iFsSession);
	if(KErrNone == AppFolder.FindByDir(KtxIcconFile, KNullDesC))
	{
		TSize imgSize(CCoeEnv::Static()->NormalFont()->HeightInPixels(),CCoeEnv::Static()->NormalFont()->HeightInPixels());
	
		iEmailIcon = LoadImageL(AppFolder.File(),EMbmApp_icconEmail,EMbmApp_icconEmail_mask,imgSize);
		iCallIcon = LoadImageL(AppFolder.File(),EMbmApp_icconCalls,EMbmApp_icconCalls_mask,imgSize);
		iMMSIcon = LoadImageL(AppFolder.File(),EMbmApp_icconMms,EMbmApp_icconMms_mask,imgSize);
		iSMSIcon = LoadImageL(AppFolder.File(),EMbmApp_icconSms,EMbmApp_icconSms_mask,imgSize);
	}
	
	TSize signalSiz(40,40);
	
	TRAPD(errnonono,
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsUtils::CreateIconL( skin,
							KAknsIIDQgnIndiBatteryStrength,
							iBatteryBitmap,
							iBatteryBitmapMask,
							_L("z:\\system\\data\\avkon.mbm"),
	                        EMbmAvkonQgn_indi_battery_strength,
	                        EMbmAvkonQgn_indi_battery_strength_mask );	
	if(iRotated){
		if(iScreenSize.iWidth > iScreenSize.iHeight){
			AknIconUtils::SetSizeAndRotation( iBatteryBitmap,signalSiz,EAspectRatioPreserved,270);
		}else{
			AknIconUtils::SetSizeAndRotation( iBatteryBitmapMask,signalSiz,EAspectRatioPreserved,90);	
		}
	}else{
		AknIconUtils::SetSize( iBatteryBitmap,signalSiz,EAspectRatioPreserved );
		AknIconUtils::SetSize( iBatteryBitmapMask,signalSiz,EAspectRatioPreserved );	
	}

	
	 AknsUtils::CreateIconL( skin,
	                         KAknsIIDQgnIndiSignalStrength,
	                         iSignalBitmap,
	                         iSignalBitmapMask,
	                         _L("z:\\system\\data\\avkon.mbm"),
	                         EMbmAvkonQgn_indi_signal_strength,
	                         EMbmAvkonQgn_indi_signal_strength_mask );
	 
	 if(iRotated){
		 if(iScreenSize.iWidth > iScreenSize.iHeight){
			 AknIconUtils::SetSizeAndRotation( iSignalBitmap,signalSiz,EAspectRatioPreserved,270 );
		 }else{
			 AknIconUtils::SetSizeAndRotation( iSignalBitmap,signalSiz,EAspectRatioPreserved,90);
		 }
	 }else{
		 AknIconUtils::SetSize( iSignalBitmap,signalSiz,EAspectRatioPreserved );
		 AknIconUtils::SetSize( iSignalBitmapMask,signalSiz,EAspectRatioPreserved );	 
	 }
	 
		if(iFile.SubSessionHandle()){	
			iFile.Write(_L8("signalobs, "));
		}
		
	iMySignalObserver = CMySignalObserver::NewL(*this);
	iProfileEngine = NULL);//CreateProfileEngineL());
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("started\n\r"));
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CClockDrawer::CreateBackBufferL(const TSize& aSize)
    {
    // Create back buffer bitmap
	delete iBackBufferContext;
	iBackBufferContext = NULL;
    delete iBackBufferDevice;
    iBackBufferDevice = NULL;
	delete iBackBuffer;
    iBackBuffer = NULL;
	iBackBuffer = new (ELeave) CFbsBitmap;
 
    User::LeaveIfError( iBackBuffer->Create(aSize,CEikonEnv::Static()->DefaultDisplayMode()));
 
    // Create back buffer graphics context
    iBackBufferDevice = CFbsBitmapDevice::NewL(iBackBuffer);
    User::LeaveIfError(iBackBufferDevice->CreateContext(iBackBufferContext));
    iBackBufferContext->SetPenStyle(CGraphicsContext::ESolidPen);
}

/*
-------------------------------------------------------------------------------
		AknsUtils::CreateIconL( skin,
							KAknsIIDQgnIndiNoSignal,
							iNoSignalBitmap,
							iNoSignalBitmapMask,
							_L("z:\\system\\data\\avkon.mbm"),
							EMbmAvkonQgn_indi_no_signal,
							EMbmAvkonQgn_indi_no_signal_mask );	
	 
	 AknsUtils::CreateColorIconL( skin,
								KAknsIIDQgnIndiNoSignal,
	                            KAknsIIDQsnIconColors,
	                            EAknsCIQsnIconColorsCG3,
	                            iNoSignalBitmap,
	                            iNoSignalBitmapMask,
	                            _L("z:\\system\\data\\avkon.mbm"),
	                            EMbmAvkonQgn_indi_no_signal,
	                            EMbmAvkonQgn_indi_no_signal_mask,
	                            KRgbGray );	 

	 AknIconUtils::SetSize( iNoSignalBitmap,signalSiz,EAspectRatioPreserved );
	 AknIconUtils::SetSize( iNoSignalBitmapMask,signalSiz,EAspectRatioPreserved );
	 if(iBatteryBitmap){
		 
		 TSize icconSizz(20,20);//iBatteryBitmap->SizeInPixels().iWidth,iBatteryBitmap->SizeInPixels().iWidth);
	 	
		 AknsUtils::CreateIconL( skin,
								 KAknsIIDQgnPropBatteryIcon,
								 iBatteryIcon,
								 iBatteryIconMask,
		                         _L("z:\\system\\data\\avkon.mbm"),
		                        EMbmAvkonQgn_prop_battery_icon,
		                        EMbmAvkonQgn_prop_battery_icon_mask );		 
	 
		 AknIconUtils::SetSize( iBatteryIcon,signalSiz,EAspectRatioPreserved );
		 AknIconUtils::SetSize( iBatteryIconMask,signalSiz,EAspectRatioPreserved );
	 }
-------------------------------------------------------------------------------
*/
CGulIcon* CClockDrawer::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,TSize aSize)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	TInt MaskId(aMsk);
	TInt UseIndex(aImg);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, UseIndex, MaskId);
	AknIconUtils::SetSize(FrameImg,aSize,EAspectRatioPreservedAndUnusedSpaceRemoved);
	AknIconUtils::SetSize(FrameMsk,aSize,EAspectRatioPreservedAndUnusedSpaceRemoved);

	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CClockDrawer::MopSupplyObject(TTypeUid aId)
{	
	return MAknsControlContext::SupplyMopObject(aId, iBgContext );
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::GetFontL(const TInt& aSizze,const TDes& aFont)
{
    if(iUseFont && iScreen)
    {
		iScreen->ReleaseFont(iUseFont);
    	iUseFont = NULL; 
    }
    
    if(aFont.Length())
    {
		TZoomFactor devicemap(CCoeEnv::Static()->ScreenDevice());
    	devicemap.SetZoomFactor(TZoomFactor::EZoomOneToOne);
    	
    	TInt heightInTwips = devicemap.VerticalPixelsToTwips(aSizze);
    	
    	if(heightInTwips < 100)
    	{
    		heightInTwips = 100;
    	}
    	
    	TFontSpec spec(aFont, heightInTwips);
    			
    	if(KErrNone != iScreen->GetNearestFontInTwips(iUseFont, spec))
    	{
			iUseFont = NULL; 
    	}
    }
    
    if(!iUseFont)
    {
		TInt NumTfaces = iScreen->NumTypefaces();

		TTypefaceSupport tfs;
		for(TInt i=0; i < NumTfaces; i++)
		{
			iScreen->TypefaceSupport(tfs, i);
			TZoomFactor devicemap(CCoeEnv::Static()->ScreenDevice());
			devicemap.SetZoomFactor(TZoomFactor::EZoomOneToOne);
		
			TInt heightInTwips = devicemap.VerticalPixelsToTwips(aSizze);
		
			if(heightInTwips < 100)
			{
				heightInTwips = 100;
			}
		
			TFontSpec spec(tfs.iTypeface.iName, heightInTwips);
				
			if(KErrNone == iScreen->GetNearestFontInTwips(iUseFont, spec))
			{	
				break;
			}
		}
    }
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CClockDrawer::KeyCaptured(TWsEvent /*aEvent*/)
	{
	iLightTimeOut = iSettings.iLightTime;

	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("KeyCaptured\n\r"));
	}
	
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
		iTimeOutTimer->After((KReFreshSpeed/10));
	}
	
	return EFalse;
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::ReFreshhhL(void)
	{
	iLightTimeOut = iSettings.iLightTime;

	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("ReFreshLllll\n\r"));
	}
	
	ReFreshL();
	
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
		iTimeOutTimer->After((KReFreshSpeed/10));
	}
	
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::ForegroundChanged(const TBool& aVisible)
{
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
		delete iTimeOutTimer;
		iTimeOutTimer = NULL;
	}


	if(aVisible)
	{
		iMyWindow.SetVisible(ETrue);
		
		TRAPD(errrr,
		iTimeOutTimer = CTimeOutTimer::NewL(EPriorityHigh, *this);
		iTimeOutTimer->After(KReFreshSpeed));
	} 
	else 
	{
		iMyWindow.SetVisible(EFalse);
	}	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::TimerExpired()
{	
/*	TBool disabLeNow(ETrue);
	
	if(iScreen)
	{
		TPixelsTwipsAndRotation SizeAndRotation;
		iScreen->GetScreenModeSizeAndRotation(iScreen->CurrentScreenMode(),SizeAndRotation);
		
		TRect Rectangle(0,0,SizeAndRotation.iPixelSize.iWidth,SizeAndRotation.iPixelSize.iHeight);

		if(iSizeAndRotation.iPixelSize.iWidth != SizeAndRotation.iPixelSize.iWidth
		|| iSizeAndRotation.iPixelSize.iHeight != SizeAndRotation.iPixelSize.iHeight)
		{
			disabLeNow = ETrue;
		}
		else if(!iCallback.IsKeyBoardLocked())
		{
			disabLeNow = ETrue;
		} 
		else
		{
			disabLeNow = EFalse;
		}
		
		
	}
 	*/
	if(iCallback.IsKeyBoardLocked())
	{
	
		TBool lightOn(EFalse);
	
		if((iSettings.iLightVal > 0 && !iChargerIsOn) || (iChargerIsOn && iSettings.iLightVal2 > 0))
		{
			if(iLightTimeOut > 0 && iHWRMLight){
				iLightTimeOut--;
				lightOn = ETrue;
			}else if(iChargerIsOn && (iSettings.iLightOn > 0)){
				lightOn = ETrue;
			}else if(iSettings.iLightOn > 1){
				lightOn = ETrue;
			}
		}
		
		TRAPD(errnono,
		if(lightOn)
		{
			if(iSettings.iMajor == 3){	
				User::ResetInactivityTime();
			}else{
				HAL::Set(HAL::EDisplayState,1); //PowerMgmt
			}
			
			if(iHWRMLight)
			{
				
				if(iChargerIsOn){
					iHWRMLight->LightOnL(CHWRMLight::EPrimaryDisplay, KHWRMInfiniteDuration, iSettings.iLightVal2, EFalse);
				}else{
					iHWRMLight->LightOnL(CHWRMLight::EPrimaryDisplay, KHWRMInfiniteDuration, iSettings.iLightVal, EFalse);
				}
			}
		}
		else
		{
			if(iSettings.iMajor > 3 && iHWRMLight){
				iHWRMLight->LightOffL(CHWRMLight::EPrimaryDisplay);
			}
		}
		
		if(iTimeOutTimer)
		{
			iTimeOutTimer->After(KReFreshSpeed);
		}		
	
		ReFreshL());
	}
	else 
	{
		iMyWindow.SetVisible(EFalse);
		//iCallback.DisableScreen(100000);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::my5800HackForLights()
{	
/*
	TRawEvent event;
	event.Set(TRawEvent::ESwitchOn);
	UserSvr::AddEvent(event); //PowerMgmt, SwEvent*/
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::ReFreshL()
{	
	Draw();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::Listen()
{
	Cancel();
	iStatus=KRequestPending;
	
	iWsSession.EventReady(&iStatus);
	//iWsSession.RedrawReady(&iStatus);
	SetActive();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::RunL()
{
//	TWsRedrawEvent e;
//	iWsSession.GetRedraw(e);
	
	TWsEvent e;
	iWsSession.GetEvent(e);
	
	
	
	//if(!iCallback.EventCaptured(e))
	{
		Draw();
		if (iStatus != KErrCancel) 
		{
			Listen();
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::GetTimeBuffer(TDes& aBuffer, const TTime& aTime)
{
	aBuffer.Zero();

	if(iTimeFormatString)
	{
		TRAPD(Errr,aTime.FormatL(aBuffer, *iTimeFormatString));
	}
	else
	{
		TDateTime datTim = aTime.DateTime();
	 	
		aBuffer.Num(datTim.Hour());
		aBuffer.Append(_L(":"));
			
		if(datTim.Minute() < 10)
			aBuffer.AppendNum(0);
			
		aBuffer.AppendNum(datTim.Minute());
	 }
}
/*
-----------------------------------------------------------------------------
#include <avkon.mbg>


http://symbian.devtricks.mobi/tricks/avkon2_mif_icons/

EMbmAvkonQgn_stat_message
EMbmAvkonQgn_stat_message_mail


EMbmAvkonQgn_stat_message_ps
EMbmAvkonQgn_stat_missed_call_ps


_LIT(KtxAvkonMBM	,"Z:\\resource\\apps\\avkon2.mif");

AknIconUtils::CreateIconL(FrameImg2,FrameMsk2,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_on,EMbmAvkonQgn_indi_checkbox_on_mask);
-----------------------------------------------------------------------------
*/
void CClockDrawer::Draw(void)
{
	if(iBackBufferContext)
	{
		
		TRect aRect(TRect(TPoint(0,0),iScreenSize));
		
		iBackBufferContext->SetBrushColor(iSettings.iBgColorRgb);
		iBackBufferContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
		
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		
		if(0x5ffffff == iSettings.iFontColorRgb.Value()){
			TRgb textcol;
			AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
			iBackBufferContext->SetPenColor(TLogicalRgb(textcol));
		}else{
			iBackBufferContext->SetPenColor(iSettings.iFontColorRgb);
		}
		
		iBackBufferContext->Clear(aRect);
		
		if(0x5ffffff == iSettings.iBgColorRgb.Value()){	
		
			AknsDrawUtils::DrawBackground( AknsUtils::SkinInstance(),
										 iBgContext,
		                                 NULL,
		                                 *iBackBufferContext,
		                                 TPoint(0,0),
		                                		 aRect,
		                                 KAknsDrawParamDefault );
		}
			
	    const CFont* useFont(iUseFont);
	    
	    if(!useFont)
	    {
	    	useFont = CCoeEnv::Static()->NormalFont();
	    }
	    
	    iBackBufferContext->UseFont(useFont);
	 	
	 	TBuf<200> buffff;   
	 	
	 	TTime nowTimme;
	 	nowTimme.HomeTime();
	 	
	 	GetTimeBuffer(buffff, nowTimme);
	 	TInt gapp(0);
	 			
	    if(iRotated){
			gapp = ((aRect.Width() - useFont->FontMaxHeight()) / 2);
				
			if(gapp > 0)
			{
				aRect.iTl.iX = gapp;
				aRect.iBr.iX = (aRect.iTl.iX + useFont->FontMaxHeight());
			}
			if(iMirroRed){    
				AknBidiTextUtils::ConvertToVisualAndClipL(buffff,*useFont, aRect.Height(), aRect.Height(), AknBidiTextUtils::ERightToLeft );
			}
			
			TBool FlipText(EFalse);
			
			if(iScreenSize.iWidth > iScreenSize.iHeight){
				FlipText = ETrue;
			}
			
			iBackBufferContext->DrawTextVertical(buffff,aRect,useFont->FontMaxAscent(),FlipText, CGraphicsContext::ECenter, 0);
	    }else{
			gapp = ((aRect.Height() - useFont->FontMaxHeight()) / 2);
	    	 	
	    	if(gapp > 0)
	    	{
	    		aRect.iTl.iY = gapp;
	    	}
	    	if(iMirroRed){    
	    		AknBidiTextUtils::ConvertToVisualAndClipL(buffff,*useFont, aRect.Width(), aRect.Width(), AknBidiTextUtils::ERightToLeft );
	    	}
	    	    
			iBackBufferContext->DrawText(buffff,aRect,useFont->FontMaxAscent(), CGraphicsContext::ECenter, 0);
	    }
	    
	    useFont = CCoeEnv::Static()->NormalFont();
	    iBackBufferContext->UseFont(useFont);
	    
	    if(iDateFormatString && (iSettings.iShowDate > 0))
	    { 
	    	TRAPD(ErrNumm, nowTimme.FormatL(buffff, *iDateFormatString));
			
	    	if(iSettings.iShowDate > 1){
				buffff.Append(_L("-"));
				buffff.Append(TDayName(nowTimme.DayNoInWeek()));   
	    	}
	    	
	    	TRect dateRect(aRect);
	    	TInt gapp2 = ((gapp - useFont->FontMaxHeight()) / 2);
	    				
	    	
	    					
	    	if(iRotated){
					
				TBool FlipText(EFalse);
	    								
	    		if(iScreenSize.iWidth > iScreenSize.iHeight){
						    			    	
					if(gapp2 > 0)
					{
						dateRect.iTl.iX = gapp2;
					}
					else
					{
						dateRect.iTl.iX = 0;
					}
								
					dateRect.iBr.iX = (dateRect.iTl.iX + useFont->FontMaxHeight());
					
					FlipText = ETrue;
				}else{
					if(gapp2 > 0)
					{
						dateRect.iBr.iX = (iScreenSize.iWidth - gapp2);
					}
					else
					{
						dateRect.iBr.iX = iScreenSize.iWidth;
					}
												
					dateRect.iTl.iX = (dateRect.iBr.iX - useFont->FontMaxHeight());
				}
	    						
				iBackBufferContext->DrawTextVertical(buffff,dateRect,useFont->FontMaxAscent(),FlipText, CGraphicsContext::ECenter, 0);    	
	    	}else{
	    		    	    
	    		if(gapp2 > 0)
	    		{
	    			dateRect.iTl.iY = gapp2;
	    		}
	    		else
	    		{
	    			dateRect.iTl.iY = 0;
	    		}
	    		    	    
	    		dateRect.iBr.iY = (dateRect.iTl.iY + useFont->FontMaxHeight());
				iBackBufferContext->DrawText(buffff,dateRect,useFont->AscentInPixels(), CGraphicsContext::ECenter, 0);
	    	}
	    }
	    
	    buffff.Zero();
	    
	   TBool calls(EFalse),emails(EFalse),mmsMsg(EFalse),smsMsg(EFalse); 
	    
	   //debug
	/*   calls = emails= mmsMsg = smsMsg = ETrue; 
	   iSignalBars = 7;
	   iMonitorVals.iBatteryCharge = 100;
	  */ 
	    if(iMonitorVals.iMissedCalls > 0)
	    {
			calls = ETrue;
	    }
	    
	    switch( iMonitorVals.iEmailStatus )	
	    {
	     case ECoreAppUIsNewEmail:	
	    	 {
				 emails = ETrue;
	    	 }
	         break;
	    }
	    
	    if(iMonitorVals.iSMSCount > 0)
	    {
			smsMsg = ETrue;
	    }

	    if(iMonitorVals.iMMSCount > 0)
	    {
			mmsMsg = ETrue;
	    }
	    
	    TRect missRect(aRect);
	    			
	    TInt gapp3 = ((gapp - useFont->FontMaxHeight()) / 2);
	    
	    
	    if(iRotated){
	    	if(iScreenSize.iWidth > iScreenSize.iHeight){
	    		if(gapp3 > 0){
	    			missRect.iTl.iX = missRect.iBr.iX + gapp3;
	    		}
	    		
	    		missRect.iBr.iX = (missRect.iTl.iX + useFont->FontMaxHeight());				
	    	}else{
	    		if(gapp3 > 0){
	    			missRect.iTl.iX = gapp3;
	    		}else{
	    			missRect.iTl.iX = 0;
	    		}
	    									
	    		missRect.iBr.iX = (missRect.iTl.iX + useFont->FontMaxHeight());
	    	}
	    	
	    	DrawNotifications(iBackBufferContext,missRect,calls,emails,mmsMsg,smsMsg,iRotated);
	    	
	    }else{
	    	
			if(gapp3 > 0){
				missRect.iBr.iY = (missRect.iBr.iY - gapp3);
			}
			
			missRect.iTl.iY = (missRect.iBr.iY - useFont->FontMaxHeight());
					
			DrawNotifications(iBackBufferContext,missRect,calls,emails,mmsMsg,smsMsg,iRotated);
	    }
	    
		if ( iSignalBitmap && iSignalBitmapMask )
		{
			TSize ImgSizz1(iSignalBitmap->SizeInPixels());

			TInt highgh = (ImgSizz1.iHeight - ((ImgSizz1.iHeight * iSignalBars) / 7));
			
			TPoint sigPoint(0, highgh);
			TRect sigRect( 0,highgh,ImgSizz1.iWidth,ImgSizz1.iHeight);
			
			if(iRotated){
				highgh = ((ImgSizz1.iWidth * iSignalBars) / 7);
			
				if(iScreenSize.iWidth > iScreenSize.iHeight){
					sigPoint = TPoint((ImgSizz1.iWidth - highgh), (iScreenSize.iHeight - ImgSizz1.iHeight));
					sigRect = TRect((ImgSizz1.iWidth - highgh),0,ImgSizz1.iWidth,ImgSizz1.iHeight);
				}else{
					sigPoint = TPoint((iScreenSize.iWidth - ImgSizz1.iWidth), 0);
					sigRect = TRect( 0,0,highgh,ImgSizz1.iHeight);
				}
			}

			iBackBufferContext->BitBltMasked(sigPoint,
						iSignalBitmap,
						sigRect,
		                iSignalBitmapMask,
						ETrue );
		}
		
		
		if ( iBatteryBitmap && iBatteryBitmapMask )
		{
			TSize ImgSizz(iBatteryBitmap->SizeInPixels());
			TInt chargeStat = (ImgSizz.iHeight - ((ImgSizz.iHeight * iMonitorVals.iBatteryCharge) / 100));
			
			TPoint batPoint((iScreenSize.iWidth - ImgSizz.iWidth), chargeStat);
			TRect batRect( 0,chargeStat,ImgSizz.iWidth,ImgSizz.iHeight);
							
			if(iRotated){
				chargeStat = ((ImgSizz.iWidth * iMonitorVals.iBatteryCharge) / 100);
				
				if(iScreenSize.iWidth > iScreenSize.iHeight){
					batPoint = TPoint((ImgSizz.iWidth - chargeStat),0);
					batRect = TRect((ImgSizz.iWidth - chargeStat),0,ImgSizz.iWidth,ImgSizz.iHeight);
				}else{
					batPoint = TPoint((iScreenSize.iWidth - ImgSizz.iWidth), (iScreenSize.iHeight - ImgSizz.iHeight));
					batRect = TRect( 0,0,chargeStat,ImgSizz.iHeight);
				}
			}
		
			iBackBufferContext->BitBltMasked(batPoint,
		                	iBatteryBitmap,
		                	batRect,
							iBatteryBitmapMask,
							ETrue );
		
		/*	if ( iBatteryIcon && iBatteryIconMask ){
			
				TSize iconSizz(iBatteryIcon->SizeInPixels());
			
				iBackBufferContext->BitBltMasked(TPoint((iScreenSize.iWidth - ImgSizz.iWidth),ImgSizz.iHeight),
								iBatteryIcon,
					            TRect( 0,0,iconSizz.iWidth,iconSizz.iHeight),
					            iBatteryIconMask,
								ETrue );
			}*/
		}
		
		DrawReally();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::DrawNotifications(CFbsBitGc* aGc,const TRect& aRect, const TBool& aCalls,const TBool& aEmails,const TBool& aMmsMsg,const TBool& aSmsMsg,const TBool& aRotated)
{
	TInt useWidth(0);

	if(iEmailIcon) {
		if(iEmailIcon->Bitmap() && iEmailIcon->Bitmap()->Handle()){
			if(aRotated){
				useWidth = useWidth + iEmailIcon->Bitmap()->SizeInPixels().iHeight;
			}else{
				useWidth = useWidth + iEmailIcon->Bitmap()->SizeInPixels().iWidth;
			}
		}
	}
	
	if(iCallIcon) {
		if(iCallIcon->Bitmap() && iCallIcon->Bitmap()->Handle()){
			if(aRotated){							
				useWidth = useWidth + iCallIcon->Bitmap()->SizeInPixels().iHeight;
			}else{
				useWidth = useWidth + iCallIcon->Bitmap()->SizeInPixels().iWidth;
			}
		}
	}	
	
	if(iMMSIcon) {
		if(iMMSIcon->Bitmap() && iMMSIcon->Bitmap()->Handle()){
			if(aRotated){
				useWidth = useWidth + iMMSIcon->Bitmap()->SizeInPixels().iHeight;
			}else{
				useWidth = useWidth + iMMSIcon->Bitmap()->SizeInPixels().iWidth;
			}
		}
	}	
	
	if(iSMSIcon) {
		if(iSMSIcon->Bitmap() && iSMSIcon->Bitmap()->Handle()){
			if(aRotated){
				useWidth = useWidth + iSMSIcon->Bitmap()->SizeInPixels().iHeight;			
			}else{
				useWidth = useWidth + iSMSIcon->Bitmap()->SizeInPixels().iWidth;
			}
		}
	}
	
	if(useWidth > 0)
	{
		TRect useMeREct(aRect);
		if(aRotated){
			TInt gapp = ((aRect.Height() - useWidth) / 2);
			useMeREct.iTl.iY = (useMeREct.iTl.iY + gapp);			
		}else{		
			TInt gapp = ((aRect.Width() - useWidth) / 2);
			useMeREct.iTl.iX = (useMeREct.iTl.iX + gapp);
		}
		
		DrawIcon(aGc,useMeREct,iCallIcon, aCalls,aRotated);
		DrawIcon(aGc,useMeREct,iEmailIcon, aEmails,aRotated);
		DrawIcon(aGc,useMeREct,iSMSIcon, aSmsMsg,aRotated);
		DrawIcon(aGc,useMeREct,iMMSIcon, aMmsMsg,aRotated);	
	}else if(iProfileEngine){

		TRAPD(Ernono,
		MProfile* lProfile =  iProfileEngine->ActiveProfileL();
		CleanupReleasePushL( *lProfile );
	 
		aGc->UseFont(CCoeEnv::Static()->NormalFont());
		aGc->DrawText(lProfile->ProfileName().ShortName(),aRect,CCoeEnv::Static()->NormalFont()->AscentInPixels(), CGraphicsContext::ECenter, 0);

		CleanupStack::PopAndDestroy());
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::DrawIcon(CFbsBitGc* aGc,TRect& aRect, CGulIcon* aIcon,const TBool& aDoDraw,const TBool& aRotated)
{
	if(aDoDraw && aIcon){
		if(aIcon->Bitmap() && aIcon->Mask()){
			if(aIcon->Bitmap()->Handle() && aIcon->Mask()->Handle()){
			
				TSize ImgSizz(aIcon->Bitmap()->SizeInPixels());
				TRect SourceRect(0,0,ImgSizz.iWidth,ImgSizz.iHeight);
				TRect destRect(aRect.iTl.iX,aRect.iTl.iY,(aRect.iTl.iX + ImgSizz.iWidth),(aRect.iTl.iY + ImgSizz.iHeight));
								
				//aGc->DrawBitmap(destRect,aIcon->Bitmap());
				aGc->DrawBitmapMasked(destRect,aIcon->Bitmap(),SourceRect,aIcon->Mask(),EFalse);
			
				if(aRotated){
					aRect.iTl.iY = (aRect.iTl.iY + ImgSizz.iHeight);
				}else{
					aRect.iTl.iX = (aRect.iTl.iX + ImgSizz.iWidth);
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
#include <avkon.mbg>


http://symbian.devtricks.mobi/tricks/avkon2_mif_icons/

EMbmAvkonQgn_stat_message
EMbmAvkonQgn_stat_message_mail


EMbmAvkonQgn_stat_message_ps
EMbmAvkonQgn_stat_missed_call_ps


_LIT(KtxAvkonMBM	,"Z:\\resource\\apps\\avkon2.mif");

AknIconUtils::CreateIconL(FrameImg2,FrameMsk2,KtxAvkonMBM,EMbmAvkonQgn_indi_checkbox_on,EMbmAvkonQgn_indi_checkbox_on_mask);
-----------------------------------------------------------------------------
*/
void CClockDrawer::DrawReally(void)
{
	if(iGc)
	{
		iWg.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront +101);
		iMyWindow.SetOrdinalPosition(0, ECoeWinPriorityAlwaysAtFront +101);
		
		iGc->Activate(iMyWindow);

		TRect aRect(TRect(TPoint(0,0), iMyWindow.Size()));
		
		full_redraw = EFalse;
		iMyWindow.Invalidate(aRect);
		iMyWindow.BeginRedraw();
		
		if(iBackBuffer && iBackBuffer->Handle()){
			iGc->DrawBitmap(aRect,iBackBuffer);
		}		
		
		iMyWindow.EndRedraw();
		
		iGc->Deactivate();
		iWsSession.Flush();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CClockDrawer::RunError(TInt /*aError*/)
{
	return KErrNone;//aError;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CClockDrawer::DoCancel()
{
//	iWsSession.RedrawReadyCancel();
	iWsSession.EventReadyCancel();
}
