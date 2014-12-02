#include <AknUtils.h>
#include <aknappui.h>
#include <hal.h>
#include <BAUTILS.H>
#include <CNTITEM.H>
#include <CNTFLDST.H>
#include <MProEngEngine.h>
#include <ProEngFactory.h>
#include <MProEngProfile.h>
#include <MProEngToneSettings.h>
#include <mproengtones.h>
#include <GULICON.H>
#include <akntitle.h> 
#include <aknkeylock.h> 
#include <eiklabel.h> 
#include <stringloader.h> 
#include <aknbiditextutils.h> 
#include <aknutils.h>

#include "Play_Container.h"
#include "YApps_E8876018.h"
#include "YApps_E8876018.h"
#include "YApps_E8876018.hrh"

#include "Help_Container.h"

#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_2002DD34.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_E8876018.rsg>
	#else
		#include <YApps_E8876018.rsg>
	#endif
#endif


#include <centralrepository.h> 
#include <ProfileEngineSDKCRKeys.h>

const TUid		EMyContactsImageID	= { 0x101f8841 };

const TInt 		KTimerFirst 		= 2000000; // 2 sec.
const TInt 		KTimerDelay 		= 500000; // 0.5 sec.
const TInt 		KVibratimeValue		= 500; // 0.5 sec.

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CPlayContainer* CPlayContainer::NewL(const TRect& aRect,CEikButtonGroupContainer* aCba)
    {
    CPlayContainer* self = CPlayContainer::NewLC(aRect,aCba);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CPlayContainer* CPlayContainer::NewLC(const TRect& aRect,CEikButtonGroupContainer* aCba)
    {
    CPlayContainer* self = new (ELeave) CPlayContainer(aCba);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CPlayContainer::CPlayContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
    {
	
    }

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CPlayContainer::~CPlayContainer()
{	
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
	delete iBgContext;
	iBgContext = NULL;

	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	delete iPlayerAdapter;
	iPlayerAdapter = NULL;
	
	delete iImageBuffer;
	iImageBuffer = NULL;
	
	delete iHWRMLight;
	iHWRMLight = NULL;
	delete iHWRMVibra;
	iHWRMVibra = NULL;
	
	if(iMdaImageDescToBitmapUtility)
	{
		iMdaImageDescToBitmapUtility->Cancel();
			
		delete iMdaImageDescToBitmapUtility;
		iMdaImageDescToBitmapUtility = NULL;
	}
	
	if(iMdaImageFileToBitmapUtility)
	{
		iMdaImageFileToBitmapUtility->Cancel();
			
		delete iMdaImageFileToBitmapUtility;
		iMdaImageFileToBitmapUtility = NULL;
	}	
	
	delete iCallerImgTmp;
	iCallerImgTmp = NULL;
	
	delete iContactsDb;
	iContactsDb = NULL;
#ifdef __S60_50__	
	delete iLeftButton;
	iLeftButton = NULL;
	
	delete iRightButton;
	iRightButton = NULL;
#else
	
#endif
	
	delete iGreenPhone;
	delete iYellowPhone;
	delete iBluePhone;
	delete iGetOperatorName;
	
	if(iButtonFont)
	{
		CEikonEnv::Static()->ScreenDevice()->ReleaseFont(iButtonFont); 
	}

	if(iTextFont)
	{
		CEikonEnv::Static()->ScreenDevice()->ReleaseFont(iTextFont); 
	}
	
	
	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Close();
	}
}
	
/*
-----------------------------------------------------------------------
  //  iRightButton->SetTextAndIconAlignment( CAknButton::EIconBeforeText );
  //  iRightButton->SetTextHorizontalAlignment( CGraphicsContext::ECenter );
-----------------------------------------------------------------------
*/
void CPlayContainer::ConstructL(const TRect& aRect)
{
    CreateWindowL();
    
/*    _LIT(KRecFilename			,"C:\\CallUI_Test.txt");
    CCoeEnv::Static()->FsSession().Delete(KRecFilename);		
    iDebugFile.Create(CCoeEnv::Static()->FsSession(),KRecFilename,EFileWrite|EFileShareAny);	
  

    if(iDebugFile.SubSessionHandle())
    {
    	iDebugFile.Write(_L8("started\n\r"));
    }
*/    	
    iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMainIdle,TRect(0,0,1,1), ETrue);
    CheckNormalSettingsL();
  
    iMirroRed = AknLayoutUtils::LayoutMirrored();
    
    SetRect(aRect);
  
#ifdef __S60_50__	   
    TFontSpec spec2 = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontSpecInTwips();
    spec2.iHeight = ((spec2.iHeight * 95) / 100);
    
    CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(iTextFont, spec2);
    
    TFontSpec spec = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont)->FontSpecInTwips();
    spec.iHeight = ((spec.iHeight * 90) / 100);
    
    CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(iButtonFont, spec);

    TBuf<60> hjelpper;
    StringLoader::Load(hjelpper,R_SH_STR_ANSWER);
    
    iLeftButton = CAknButton::NewL(NULL,NULL,NULL,NULL,hjelpper,_L(""),0,0);
    iLeftButton->SetContainerWindowL(*this);
    
    if(iMirroRed){
        iLeftButton->SetRect(iRightBtOut);
    }else{
        iLeftButton->SetRect(iLeftBtOut);
    }
    iLeftButton->SetObserver(this);
    
    iLeftButton->SetIconScaleMode( EAspectRatioPreserved );  
    CGulIcon* lefticcon = GetButtonIconL(KAknsIIDQgnIndiButtonAnswer,TSize(50,50));
	CAknButtonState* state = iLeftButton->State(iLeftButton->StateIndex());
    state->SetIcon(lefticcon); 
  
    iLeftButton->SetTextFont(iButtonFont);
    iLeftButton->SetTextAndIconAlignment( CAknButton::EIconOverText );
    iLeftButton->SetTextHorizontalAlignment( CGraphicsContext::ECenter );
    
    iLeftButton->MakeVisible(ETrue);
    iLeftButton->ActivateL();

    StringLoader::Load(hjelpper,R_SH_STR_SILENSE);
    
    iRightButton = CAknButton::NewL(NULL,NULL,NULL,NULL,hjelpper,_L(""),0,0);//KAknButtonStateHasLatchedFrame
    iRightButton->SetContainerWindowL(*this);
    
    if(iMirroRed){
        iRightButton->SetRect(iLeftBtOut);
    }else{
        iRightButton->SetRect(iRightBtOut);
    }
    
    iRightButton->SetObserver(this);
    
    CGulIcon* righticcon = GetButtonIconL(KAknsIIDQgnIndiButtonSilence,TSize(50,50));
    CAknButtonState* state2 = iRightButton->State(iRightButton->StateIndex());
    state2->SetIcon(righticcon); 
    
    iRightButton->SetIconScaleMode( EAspectRatioPreserved );    

    iRightButton->SetTextFont(iButtonFont);
    iRightButton->SetTextAndIconAlignment( CAknButton::EIconOverText );
    iRightButton->SetTextHorizontalAlignment( CGraphicsContext::ECenter );
            
    iRightButton->MakeVisible(ETrue);
    iRightButton->ActivateL();

    TFindFile PrivFolder(CCoeEnv::Static()->FsSession());
    if(KErrNone == PrivFolder.FindByDir(KMyIconFile, KNullDesC))// finds the drive
    {	
    	iBluePhone = GetPhoneIconL(PrivFolder.File(),0,1,TSize(20,25));
	}
    
    iGreenPhone = GetPhoneIconL(_L("z:\\resource\\apps\\callstatus.mif"),16384, 16385,TSize(50,50));
    iYellowPhone = GetPhoneIconL(_L("z:\\resource\\apps\\callstatus.mif"),16396,16397,TSize(50,50)); 
#else

    if(!iTextFont){
        TFontSpec spec2 = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontSpecInTwips();
        spec2.iHeight = ((spec2.iHeight * 85) / 100);
    
        CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(iTextFont, spec2);
    }
    
    if ( iBgContext )
    {
        iBgContext->SetRect(Rect());
    
        if ( &Window() )
        {
            iBgContext->SetParentPos( PositionRelativeToScreen() );
        }
    }
        
    iGreenPhone = GetPhoneIconL(_L("z:\\resource\\apps\\callstatus.mif"),16384, 16385,TSize(40,40));
    iYellowPhone = GetPhoneIconL(_L("z:\\resource\\apps\\callstatus.mif"),16396,16397,TSize(40,40));
        
#endif     
  	ActivateL();
  	
    CEikStatusPane*     sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
    if(sp)
    {       
        if(sp->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_IDLE)//R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE)
        {
            sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_IDLE);
        }
    }  	
  	
    iGetOperatorName = CGetOperatorName::NewL(*this);
    
  	//iHWRMLight = CHWRMLight::NewL();
  	iHWRMVibra = CHWRMVibra::NewL(this);
  	
    if(iDebugFile.SubSessionHandle())
    {
    	iDebugFile.Write(_L8("And running\n\r"));
    }
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::HandleControlEventL( CCoeControl* aControl, TCoeEvent aEventType )
{
#ifdef __S60_50__		
    switch ( aEventType ) 
    { 
    	case EEventRequestFocus:
    		if(aControl ==iRightButton)
    		{
    		}
    		break;
        case EEventStateChanged:
        {
            if(aControl == iLeftButton)
            {
                STATIC_CAST(CAknAppUi*,CEikonEnv::Static()->AppUi())->HandleCommandL(EEikCmdExit);
            }
            else if(aControl ==iRightButton)
            {            
				iRightButton->SetDimmed(ETrue);
            
				delete iPlayerAdapter;
				iPlayerAdapter = NULL;
            }
        }   break; 
 
        default: 
             break; 
    }
#else
	
#endif    
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::SnoozeL()
{
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("SnoozeL\n\r"));
    }
    
	iSnoozeOn = ETrue;
	CCoeEnv::Static()->RootWin().SetOrdinalPosition(-1,0);
	
	if(iTimeOutTimer)
	{
		delete iTimeOutTimer;
		iTimeOutTimer = NULL;
	}
	
	delete iPlayerAdapter;
	iPlayerAdapter = NULL;
	
	if(iMdaImageFileToBitmapUtility)
	{
		iMdaImageFileToBitmapUtility->Cancel();
				
		delete iMdaImageFileToBitmapUtility;
		iMdaImageFileToBitmapUtility = NULL;
	}
	
	if(iMdaImageDescToBitmapUtility)
	{
		iMdaImageDescToBitmapUtility->Cancel();
				
		delete iMdaImageDescToBitmapUtility;
		iMdaImageDescToBitmapUtility = NULL;
	}
	
	iImageOk = EFalse;
	delete iCallerImgTmp;
	iCallerImgTmp = NULL;	
		
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityRealTime, *this);
	iTimeOutTimer->After(iSnoozeDelay);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::OperatorNameL(const TDesC& aOpName, TInt aError)
{
    if(aError == KErrNone && aOpName.Length())
	{
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{
			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(aOpName);
		}
	}

	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CGulIcon* CPlayContainer::GetPhoneIconL(const TDesC& aFilename,const TInt& aIcon,const TInt& aMask,const TSize& aSize)
{
	CFbsBitmap* Bitmap(NULL);// = new(ELeave) CFbsBitmap;
	CFbsBitmap* Mask(NULL);// = new(ELeave) CFbsBitmap;
    
	AknIconUtils::CreateIconLC(Bitmap, Mask, aFilename, aIcon,aMask);

	if(Bitmap && Mask)
    {
		AknIconUtils::SetSize(Bitmap,aSize);
    	AknIconUtils::SetSize(Mask,aSize);
    }
	
	CleanupStack::Pop(2);
	
	return CGulIcon::NewL(Bitmap,Mask);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CGulIcon* CPlayContainer::GetButtonIconL(const TAknsItemID& aIcon,const TSize& aSize)
{
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();

	CGulIcon* iccon = AknsUtils::CreateGulIconL(skin,aIcon,ETrue);
	if(iccon && iccon->Bitmap() && iccon->Mask())
    {
		AknIconUtils::SetSize(iccon->Bitmap(),aSize);
    	AknIconUtils::SetSize(iccon->Mask(),aSize);
    }
	
	return iccon;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::AddContactL(const TDesC& aNumber)
{
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("AddContactL\n\r"));
    }
    
	if(iTimeOutTimer)
	{
		delete iTimeOutTimer;
		iTimeOutTimer = NULL;
	}
	
	delete iPlayerAdapter;
	iPlayerAdapter = NULL;
		
	if(iMdaImageFileToBitmapUtility)
	{
		iMdaImageFileToBitmapUtility->Cancel();
					
		delete iMdaImageFileToBitmapUtility;
		iMdaImageFileToBitmapUtility = NULL;
	}
		
	if(iMdaImageDescToBitmapUtility)
	{
		iMdaImageDescToBitmapUtility->Cancel();
					
		delete iMdaImageDescToBitmapUtility;
		iMdaImageDescToBitmapUtility = NULL;
	}
		
	iImageOk = EFalse;
	delete iCallerImgTmp;
	iCallerImgTmp = NULL;
		
	delete iImageBuffer;
	iImageBuffer = NULL;
	
	iImageFile.Zero();
	iRingtone.Zero();
	
	iName.Zero();
	iNumber.Copy(aNumber);

	if(iMirroRed){    
        AknBidiTextUtils::ConvertToVisualAndClipL(iNumber,*AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont), Rect().Width(), Rect().Width(), AknBidiTextUtils::ERightToLeft );
    }
	
	if(!iContactsDb){
		// Opens the default contact database           
		iContactsDb = CContactDatabase::OpenL();
	}
	
	if(iNumber.Length()){
		
		TInt searchLength(7);
		
		if(iNumber.Length() < 7){
			searchLength = iNumber.Length();
		}
	
		CContactIdArray* myIdArray= iContactsDb->MatchPhoneNumberL(iNumber,searchLength);
		CleanupStack::PushL(myIdArray);
		
		if(myIdArray->Count()){
			TContactItemId id = (*myIdArray)[0];

			// Read contact using id
			CContactItem* contact = iContactsDb->ReadContactL(id);
			CleanupStack::PushL(contact);
		 
			TInt cIndex = contact->CardFields().Find(KUidContactFieldGivenName);
			TInt bIndex = contact->CardFields().Find(KUidContactFieldFamilyName);

			if (cIndex != KErrNotFound || bIndex != KErrNotFound)
			{
				if (cIndex != KErrNotFound)
				{
					CContactTextField* NameField = ((contact->CardFields())[cIndex]).TextStorage();					
					if(NameField)
					{
						iName.Copy(NameField->Text());
					}
				}
						
				if (bIndex != KErrNotFound)
				{
					CContactTextField* NameField = ((contact->CardFields())[bIndex]).TextStorage();					
					if(NameField)
					{
						if(iName.Length()){
							iName.Append(_L(" "));
						}
						
						iName.Append(NameField->Text());
					}
				}
				
			    if(iMirroRed){    
			        AknBidiTextUtils::ConvertToVisualAndClipL(iName,*AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont), Rect().Width(), Rect().Width(), AknBidiTextUtils::ERightToLeft );
			    }				
			}
			
			TInt tmpIndex = contact->CardFields().Find(EMyContactsImageID);
			if(tmpIndex!= KErrNotFound)
			{
				if(((contact->CardFields())[tmpIndex]).StorageType() == KStorageTypeText)
				{
					if(((contact->CardFields())[tmpIndex]).TextStorage())
					{
						iImageFile.Copy(((contact->CardFields())[tmpIndex]).TextStorage()->Text());
						if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iImageFile)){
							iImageFile.Zero();
						}
					}
				}
			}
			
			if(!iImageFile.Length())
			{
				TInt ImIndex = contact->CardFields().Find(KUidContactFieldVCardMapPHOTO);			
				if(ImIndex!= KErrNotFound)
				{
					if (ImIndex != KErrNotFound
					&&  ((contact->CardFields())[ImIndex]).StorageType() == KStorageTypeStore 
					&&  ((contact->CardFields())[ImIndex]).StoreStorage()->IsFull())
					{
						HBufC8* buf = ((contact->CardFields())[ImIndex]).StoreStorage()->Thing();
						if(buf)
						{
							 iImageBuffer = buf->Des().AllocL();
						}
					}
				}
			}
			
			TInt RIndex = contact->CardFields().Find(KUidContactFieldRingTone);
			if(RIndex!= KErrNotFound)
			{
				if(((contact->CardFields())[RIndex]).TextStorage())
				{
					iRingtone.Copy(((contact->CardFields())[RIndex]).TextStorage()->Text());
					if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iRingtone)){
						iRingtone.Zero();
					}else{
					// check if it is video ?
					}
				}
			}			
					 
			CleanupStack::PopAndDestroy();
		}
		
		CleanupStack::PopAndDestroy(myIdArray);
	}
	
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("AddContactL, Done\n\r"));
    }	
}
/*
-----------------------------------------------------------------------------
const TUid KUidTelephony = {0x101fd64c}; // 3rd edition - standby screen
	
	TApaTaskList taskList(CEikonEnv::Static()->WsSession()); // iWsSession is a RWsSession type of object and a connect is already done on it.
	TApaTask task = taskList.FindApp (KUidTelephony );
	
	if (task.Exists ( ) ) // this shud definitely be true as it is the telephony app...
	{
		task.BringToForeground ( );
	}
-----------------------------------------------------------------------------
*/
void CPlayContainer::StartTimerL(void)
{
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("StartTimerL\n\r"));
    }    
    
	TRAPD(Errr,
	RAknKeyLock keyLock;
	if(keyLock.Connect() == KErrNone){
		if (keyLock.IsKeyLockEnabled())
		{
			keyLock.DisableWithoutNote();
		}
		keyLock.Close();
	});
	
	iStartTime.HomeTime();

	iSnoozeOn = EFalse;
	iLightsAreOn = ETrue;
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityRealTime, *this);
	iTimeOutTimer->After(KTimerFirst);
	
//	CCoeEnv::Static()->RootWin().SetOrdinalPosition(0,0);
	CCoeEnv::Static()->RootWin().SetOrdinalPosition(1, ECoeWinPriorityAlwaysAtFront +1);

    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("StartTimerL, Done\n\r"));
    } 
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::CheckProfileSettingsL()
{	
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("CheckProfileSettingsL\n\r"));
    }     
    
	iRepeat = iAscending = iJustBeep = EFalse;

	// Create an engine instance:
	 MProEngEngine* engine = ProEngFactory::NewEngineLC();
	 // Get the settings of the active profile:
	 MProEngProfile* activeProfile = engine->ActiveProfileLC();
	 MProEngToneSettings& ts = activeProfile->ToneSettings();
	 
	 TBuf8<255> hkjelp;
	 	 	
	 iVibraOn = ts.VibratingAlert();
	 iRingVolume = ts.RingingVolume();
	 
	 switch(ts.RingingType())
	 {
	 case EProfileRingingTypeRinging:
		 iRepeat = ETrue;
		 break;
	 case EProfileRingingTypeAscending:
		 iAscending = ETrue;
		 iRepeat = ETrue;
		 break;		 
	 case EProfileRingingTypeRingingOnce:
		 iRepeat = EFalse;
		 break;
	 case EProfileRingingTypeBeepOnce:
		 iRepeat = EFalse;
		 iJustBeep = ETrue;
		 break;
	 case EProfileRingingTypeSilent: 
		 iRingVolume = 0;
		 break;
	 };
	 

	 hkjelp.Append(_L8("Vol: "));
	 hkjelp.AppendNum(iRingVolume);
	 
	 if(iVibraOn)
		 hkjelp.Append(_L8(", Vibra on"));
	 else
		 hkjelp.Append(_L8(", No Vibra"));
	 
	 
	if(iDebugFile.SubSessionHandle())
	{
	 	hkjelp.Append(_L8("\n\r"));
	 	iDebugFile.Write(hkjelp);
 	}
	 
	if(iJustBeep)
	{
		MProEngTones& tones = activeProfile->ProfileTones();
		iRingtone.Copy(tones.RingingTone1());
	}
	else if(!iRingtone.Length())
    {
		 if(IsOffLineProfileOnL())
		 {
			 iRingtone.Copy(iFallBackRingTone);
		 }
		 else
	     {
			 MProEngTones& tones = activeProfile->ProfileTones();
			 iRingtone.Copy(tones.RingingTone1());
	     }
     }
	
	if(!BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iRingtone)){
		iRingtone.Copy(iFallBackRingTone);
	}
	 
	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Write(_L8("Selected ringtone"));
	 	hkjelp.Copy(iRingtone);
	 	iDebugFile.Write(hkjelp);
	 	iDebugFile.Write(_L8("\n\r"));
	}
	 
	CleanupStack::PopAndDestroy(2); // activeProfile, engine
	
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("CheckProfileSettingsL, Done\n\r"));
    } 	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CPlayContainer::IsOffLineProfileOnL()
{
	TInt value;

	 CRepository *repository = CRepository::NewLC( KCRUidProfileEngine );
	 repository->Get( KProEngActiveProfile, value );
	 CleanupStack::PopAndDestroy( repository );
	 
	 if ( value == 5 )
		 return ETrue;
	 else
		 return EFalse;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::TimerExpired()
{
	if(iSnoozeOn)
	{
		if( STATIC_CAST(CCallEffectsAppUi*, CCoeEnv::Static()->AppUi())->IsCallOn())
		{
			SnoozeL();
		}
		else
		{
			iSnoozeOn = EFalse;	
			StartTimerL();
			CheckProfileSettingsL();
			StartRingtoneL();
			StartCallerImageL();
		}
	}
	else
	{
		TTime nowwTim;
		nowwTim.HomeTime();
	
		TTimeIntervalSeconds ringtim(0);
		
		nowwTim.SecondsFrom(iStartTime,ringtim);
		
		if(ringtim.Int() >= iMaxTime)
		{
			CEikonEnv::Static()->EikAppUi()->HandleCommandL(EAknSoftkeyExit);
		}
		else
		{
			if(iYellowOn)
				iYellowOn = EFalse;
			else
				iYellowOn = ETrue;
		
			if(iTimCount > 3)
			{
				iTimCount = 0;
			
				if(iFlashLights && iHWRMLight)
				{
					if(iLightsAreOn)
					{
						iLightsAreOn = EFalse;
						iHWRMLight->LightOffL(CHWRMLight::EPrimaryDisplay);
					}
					else
					{
						iLightsAreOn = ETrue;
						iHWRMLight->LightOnL(CHWRMLight::EPrimaryDisplay);
					}
				}
				else
				{
					User::ResetInactivityTime();
				}
			
				if(iVibraOn && iHWRMVibra)
				{
					TRAPD(Err,iHWRMVibra->StartVibraL(KVibratimeValue));
				}
				
				if(iAscending && (iRingVolumeRamp < iRingVolume) && iPlayerAdapter)
				{
					iRingVolumeRamp = (iRingVolumeRamp + 1);
					iPlayerAdapter->SetVolume(iRingVolumeRamp);
				}
			}
			else
			{
				iTimCount++;
			}
		}
		
		iTimeOutTimer->After(KTimerDelay);
	}

	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::StartRingtoneL(void)
{
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("StartRingtoneL\n\r"));
    }     
    
#ifdef __S60_50__		
	if(iRightButton)
	{
		iRightButton->SetDimmed(EFalse);
	}
#else
	
#endif
	delete iPlayerAdapter;
	iPlayerAdapter = NULL;
	
	if(iRingVolume > 0)
	{
		if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iRingtone) && iRingtone.Length()){
			iPlayerAdapter = CPlayerAdapter::NewL(this);
			iPlayerAdapter->OpenL(iRingtone);
		}
	}
	
	if(iHWRMVibra && iVibraOn)
	{
		TRAPD(Err,iHWRMVibra->ReserveVibraL());
	}
	
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("StartRingtoneL, Done\n\r"));
    }  	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::NotifyPlayComplete(TInt aError)
{
	if(iPlayerAdapter && iRepeat)
	{
		iPlayerAdapter->SetVolume(iRingVolumeRamp);
		iPlayerAdapter->PlayL();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::NotifyFileOpened(TInt aError)
{
	if(iPlayerAdapter)
	{	
		if(iAscending){
			iRingVolumeRamp = 1; // increase to iRingVolume with timer
		}else{
			iRingVolumeRamp = iRingVolume;
		}
	
		iPlayerAdapter->SetVolume(iRingVolumeRamp);
		iPlayerAdapter->PlayL();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::StartCallerImageL(void)
{
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("StartCallerImageL\n\r"));
    }  
    
	if(iMdaImageFileToBitmapUtility)
	{
		iMdaImageFileToBitmapUtility->Cancel();
				
		delete iMdaImageFileToBitmapUtility;
		iMdaImageFileToBitmapUtility = NULL;
	}
	
	if(iMdaImageDescToBitmapUtility)
	{
		iMdaImageDescToBitmapUtility->Cancel();
				
		delete iMdaImageDescToBitmapUtility;
		iMdaImageDescToBitmapUtility = NULL;
	}
	
	iImageOk = EFalse;
	delete iCallerImgTmp;
	iCallerImgTmp = NULL;
		
	if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iImageFile) && iImageFile.Length()){
		
		iMdaImageFileToBitmapUtility = CMdaImageFileToBitmapUtility::NewL(*this);
		iMdaImageFileToBitmapUtility->OpenL(iImageFile);
	}else if(iImageBuffer){		
		iMagePoint.Set(iImageBuffer->Des());
			
		iMdaImageDescToBitmapUtility = CMdaImageDescToBitmapUtility::NewL(*this);
		iMdaImageDescToBitmapUtility->OpenL(iMagePoint);
	}
	
    if(iDebugFile.SubSessionHandle())
    {
        iDebugFile.Write(_L8("StartCallerImageL, Done\n\r"));
    } 	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::MiuoConvertComplete(TInt aError)
{
	if(iMdaImageDescToBitmapUtility)
	{
		delete iMdaImageDescToBitmapUtility;
		iMdaImageDescToBitmapUtility = NULL;		
	}

	if(iMdaImageFileToBitmapUtility)
	{
		delete iMdaImageFileToBitmapUtility;
		iMdaImageFileToBitmapUtility = NULL;		
	}
	
	if(aError != KErrNone)
	{
		delete iCallerImgTmp;
		iCallerImgTmp = NULL;
	}
	else
	{
	
		 if(iDebugFile.SubSessionHandle())
		 {
			 TBuf8<100> hkjelp;

			 hkjelp.Copy(_L8("Image siz: "));
			 hkjelp.AppendNum(iCallerImgTmp->SizeInPixels().iWidth);
			 hkjelp.Append(_L8("x"));
			 hkjelp.AppendNum(iCallerImgTmp->SizeInPixels().iHeight);
			 hkjelp.Append(_L8("\n\r"));
			 iDebugFile.Write(hkjelp);
			 
			 hkjelp.Copy(_L8("Rect siz: "));
			 hkjelp.AppendNum(iImageRect.Width());
			 hkjelp.Append(_L8("x"));
			 hkjelp.AppendNum(iImageRect.Height());
			 hkjelp.Append(_L8("\n\r"));
			 iDebugFile.Write(hkjelp);
			 
			 
			 TRect imgRects(TPoint(0,0),iCallerImgTmp->SizeInPixels());
			 TRect DrawRects(iImageRect);
			 			
			if(imgRects.Width() > DrawRects.Width()
			|| imgRects.Height() > DrawRects.Height()){
				
				if(imgRects.Width() > imgRects.Height()){
					TInt valTmp = ((DrawRects.Width() * imgRects.Height())/ DrawRects.Height());
					
					TInt hhGap = ((imgRects.Width() - valTmp) / 2);
						
					imgRects.iTl.iX = (imgRects.iTl.iX + hhGap);
					imgRects.iBr.iX = (imgRects.iBr.iX - hhGap);
				}else{			
					TInt valTmp = ((DrawRects.Height() * imgRects.Width())/ DrawRects.Width());
								
					TInt hhGap = ((imgRects.Height() - valTmp) / 2);
						
					imgRects.iTl.iY = (imgRects.iTl.iY + hhGap);
					imgRects.iBr.iY = (imgRects.iBr.iY - hhGap);
				}
			}else{ //if(imgRects.Width() <= DrawRects.Width()){
			 		TInt wwGap = ((DrawRects.Width() - imgRects.Width()) / 2);
			 		DrawRects.iTl.iX = (DrawRects.iTl.iX + wwGap);
			 		DrawRects.iBr.iX = (DrawRects.iTl.iX + imgRects.Width());
			 				
			 		TInt hGap = ((DrawRects.Height() - imgRects.Height()) / 2);
			 		DrawRects.iTl.iY = (DrawRects.iTl.iY + hGap);
			 		DrawRects.iBr.iY = (DrawRects.iTl.iY + imgRects.Height());
			 }
			 
			 hkjelp.Copy(_L8("Image Rect: "));
			 hkjelp.AppendNum(imgRects.iTl.iX);
			 hkjelp.Append(_L8("x"));
			 hkjelp.AppendNum(imgRects.iTl.iY);
			 hkjelp.Append(_L8("-"));
			 hkjelp.AppendNum(imgRects.iBr.iX);
			 hkjelp.Append(_L8("x"));
			 hkjelp.AppendNum(imgRects.iBr.iY);
			 hkjelp.Append(_L8("\n\r"));
			 iDebugFile.Write(hkjelp);
						 
			 hkjelp.Copy(_L8("Draw Rect: "));
			 hkjelp.AppendNum(DrawRects.iTl.iX);
			 hkjelp.Append(_L8("x"));
			 hkjelp.AppendNum(DrawRects.iTl.iY);
			 hkjelp.Append(_L8("-"));
			 hkjelp.AppendNum(DrawRects.iBr.iX);
			 hkjelp.Append(_L8("x"));
			 hkjelp.AppendNum(DrawRects.iBr.iY);
			 hkjelp.Append(_L8("\n\r"));
			 iDebugFile.Write(hkjelp); 
		 }
		
		 if(iCallerImgTmp){
		 	if(iCallerImgTmp->Handle()){
		 		iImageOk = ETrue;
		 	}
		 }
		// its ok...
	}

	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::MiuoOpenComplete(TInt aError)
{
	if(iMdaImageDescToBitmapUtility && aError == KErrNone)
	{
		TInt frameCount = iMdaImageDescToBitmapUtility->FrameCount();
	    if( frameCount > 0 )
	    {	
	    	TFrameInfo ThisInfo;
	    	iMdaImageDescToBitmapUtility->FrameInfo(0,ThisInfo);
	    	
	    	TSize imagetargetSize = ThisInfo.iOverallSizeInPixels;
	    	
	    	if(ThisInfo.iOverallSizeInPixels.iHeight > imagetargetSize.iHeight
	    	|| ThisInfo.iOverallSizeInPixels.iWidth > imagetargetSize.iWidth)
	    	{
				imagetargetSize = iImageRect.Size();
				imagetargetSize.iHeight = ((imagetargetSize.iHeight * ThisInfo.iOverallSizeInPixels.iHeight) / ThisInfo.iOverallSizeInPixels.iWidth);
	    	}
	    	
		    delete iCallerImgTmp;
			iCallerImgTmp = NULL;
		    iCallerImgTmp = new(ELeave)CFbsBitmap();//iWsSession);
	    	iCallerImgTmp->Create(imagetargetSize,EColor16M);
			
		    iMdaImageDescToBitmapUtility->ConvertL(*iCallerImgTmp);
	    }
	}
	else if(iMdaImageFileToBitmapUtility && aError == KErrNone)
	{
		TInt frameCount = iMdaImageFileToBitmapUtility->FrameCount();
	    if( frameCount > 0 )
	    {	
	    	TFrameInfo ThisInfo;
	    	iMdaImageFileToBitmapUtility->FrameInfo(0,ThisInfo);
	    	
	    	TSize imagetargetSize = ThisInfo.iOverallSizeInPixels;
	    	
	    	if(ThisInfo.iOverallSizeInPixels.iHeight > imagetargetSize.iHeight
	    	|| ThisInfo.iOverallSizeInPixels.iWidth > imagetargetSize.iWidth)
	    	{
				imagetargetSize = iImageRect.Size();
				imagetargetSize.iHeight = ((imagetargetSize.iHeight * ThisInfo.iOverallSizeInPixels.iHeight) / ThisInfo.iOverallSizeInPixels.iWidth);
	    	}
	    	
		    delete iCallerImgTmp;
			iCallerImgTmp = NULL;
		    iCallerImgTmp = new(ELeave)CFbsBitmap();//iWsSession);
	    	iCallerImgTmp->Create(imagetargetSize,EColor16M);
			
	    	iMdaImageFileToBitmapUtility->ConvertL(*iCallerImgTmp);
	    }
	}
	else
	{
		DrawNow();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::MiuoCreateComplete(TInt /*aError*/)
{
}
/*
-----------------------------------------------------------------------------
AknsDrawUtils::PrepareFrame( skin,
                                         iOuterRect,
                                         iInnerRect,
                                         iFrameId,
                                         KAknsIIDDefault );
-----------------------------------------------------------------------------
*/
void CPlayContainer::SizeChanged()
{		
	iImageRect = Rect();
	
#ifdef __S60_50__	

	iImageRect.iBr.iY = (iImageRect.iTl.iY + ((iImageRect.Height() / 12) * 9));

	iCallOut = iImageRect;

    iCallOut.iTl.iX = (iCallOut.iTl.iX);
    iCallOut.iBr.iX = (iCallOut.iBr.iX);
    iCallOut.iTl.iY = (iCallOut.iBr.iX - (iImageRect.Height()/4));
	    	    
    TInt minHeight = (AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontMaxHeight() * 4);
	    
    if(iCallOut.Height() < minHeight)
    {
        iCallOut.iTl.iY = (iCallOut.iBr.iY - minHeight);
    }
	    
    iCallIn = iCallOut;
    iCallIn.Shrink(5,5);
	    
    TInt gappi = (iImageRect.Width() / 15);
	
    if ( iBgContext )
    {
        iBgContext->SetRect(Rect());
    
        if ( &Window() )
        {
            iBgContext->SetParentPos( PositionRelativeToScreen() );
        }
    }
    
	iLeftBtOut = Rect();
	iLeftBtOut.iTl.iY = iCallOut.iBr.iY;
	iLeftBtOut.Shrink(3,3);
	
	iLeftBtOut.iTl.iX = (iLeftBtOut.iTl.iX  + gappi);
	iLeftBtOut.iBr.iX = (iLeftBtOut.iTl.iX  + (gappi * 6));
	
	TInt Gappi2 = (iLeftBtOut.Height() / 8);
	
	iLeftBtOut.iTl.iY = (iLeftBtOut.iTl.iY + Gappi2);
	iLeftBtOut.iBr.iY = (iLeftBtOut.iBr.iY - Gappi2);

	iRightBtOut.iTl.iX = (iLeftBtOut.iBr.iX + gappi);
	iRightBtOut.iBr.iX = (iRightBtOut.iTl.iX  + (gappi * 6));
	iRightBtOut.iTl.iY = iLeftBtOut.iTl.iY;
	iRightBtOut.iBr.iY = iLeftBtOut.iBr.iY;
	
//	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
//	AknsDrawUtils::PrepareFrame( skin,iCallOut,iCallIn,KAknsIIDQsnFrCall2Bubble,KAknsIIDDefault );
//	AknsDrawUtils::PrepareFrame( skin,iLeftBtOut,iLeftBtIn,KAknsIIDQsnFrCall3Rect,KAknsIIDDefault );
//	AknsDrawUtils::PrepareFrame( skin,iRightBtOut,iRightBtIn,KAknsIIDQsnFrCall3Rect,KAknsIIDDefault );

	if(iLeftButton)
	{
		iLeftButton->SetRect(iLeftBtOut);
	}
	
	if(iRightButton)
	{
		iRightButton->SetRect(iRightBtOut);
	}
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
#else
	
    if(!iTextFont){
        TFontSpec spec2 = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontSpecInTwips();
        spec2.iHeight = ((spec2.iHeight * 85) / 100);
    
        CEikonEnv::Static()->ScreenDevice()->GetNearestFontInTwips(iTextFont, spec2);
    }
    
    iCallOut = iImageRect;
    
    TInt minHeight = 10;

    if(iTextFont){    
        minHeight = (iTextFont->FontMaxHeight() * 5);
    }else{
        minHeight = (AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontMaxHeight() * 5);
    }
    
	iCallOut.iTl.iY = (iCallOut.iBr.iY - minHeight);
   
    iCallOut.iTl.iX = (iCallOut.iTl.iX + 5);
    iCallOut.iTl.iY = (iCallOut.iTl.iY - 5);
    iCallOut.iBr.iY = (iCallOut.iBr.iY - 5);    
    iCallOut.iBr.iX = (iCallOut.iBr.iX - 5);
	    
    iCallIn = iCallOut;
    iCallIn.Shrink(5,5);
#endif	

    
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::CheckNormalSettingsL(void)
{	
	iFlashLights = EFalse;//ETrue;//
	
	iFallBackRingTone.Zero();
	iSnoozeDelay = 30000000; // 30 sec..
	iMaxTime = 30; // 30 sec..
	
	OperatorNameL(_L("   "), KErrNone); // clear the appplication name away..

	{
		TFindFile AufFolder(CCoeEnv::Static()->FsSession());
		if(KErrNone == AufFolder.FindByDir(KtxDefSettingsFile, KNullDesC))
		{
			CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
			
			TFileName hljBuffer;
			GetValuesL(MyDStore,0x0110,hljBuffer);
			OperatorNameL(hljBuffer, KErrNone);
			
			GetValuesL(MyDStore,0x1100,hljBuffer);
			
			if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),hljBuffer) && hljBuffer.Length()){
				iFallBackRingTone.Copy(hljBuffer);
			}
			
			TUint32 TmpValue(0);
			GetValuesL(MyDStore,0x0101,TmpValue);
			
			iSnoozeDelay = (TmpValue * 1000000);
			
			TmpValue = 0;
			GetValuesL(MyDStore,0x0111,TmpValue);
									
			iMaxTime = TmpValue;
			
			GetValuesL(MyDStore,0x1234,hljBuffer);
			
			if(iCba && hljBuffer.Length())
			{
				MEikButtonGroup* buttonGroup = iCba->ButtonGroup();
				TInt cmdId = buttonGroup->CommandId(2);
				CCoeControl* button = buttonGroup->GroupControlById( cmdId );
				if( button && buttonGroup->IsCommandVisible( cmdId ))
				{
					CEikLabel* label = static_cast<CEikLabel*>( button->ComponentControl(0) );
					label->SetTextL(hljBuffer);
				}
			}

			CleanupStack::PopAndDestroy(1);// Store
		}
	}
}
		
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue)
{
	aValue.Zero();
	
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		in >> aValue;
		CleanupStack::PopAndDestroy(1);// in
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		RDictionaryReadStream in;
		in.OpenLC(*aDStore,FileUid);
		aValue = in.ReadInt32L();
		CleanupStack::PopAndDestroy(1);// in
	}
	else
		aValue = 30;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CPlayContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
TKeyResponse CPlayContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasConsumed;
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else 
	{		
		switch (aKeyEvent.iCode)
		{
		case EKeyRightArrow:
	
			break;
		case EKeyLeftArrow:
	
			break;
		case EKeyDownArrow:
	
			break;
		case EKeyUpArrow:
	
			break;	
		default:
	
			break;
		}
	}
	
	return Ret;
} 


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EAppHelpBack:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EAppHelp:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    		
			iMyHelpContainer = CMyHelpContainer::NewL(2);
		}
		SetMenuL();
		DrawNow();    		
		break;	
	};
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayContainer::SetMenuL(void)
{
	if(iMyHelpContainer)
	{
		if(iCba)
		{
			iCba->SetCommandSetL(R_APPHELP_CBA);
			iCba->DrawDeferred();
		}
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();
	
		TInt MenuRes(R_MAIN_MENUBAR);
		TInt ButtomRes(R_MAIN_CBA);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(MenuRes);
		
		if(iCba)
		{
			iCba->SetCommandSetL(ButtomRes);
			iCba->DrawDeferred();
		}
	}
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TInt CPlayContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
		return 1;	
#ifdef __S60_50__		
	else if(iLeftButton && iRightButton)
		return 2;
#else
	
#endif	
	else
		return 0;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CCoeControl* CPlayContainer::ComponentControl(TInt aIndex) const
{	
	if(iMyHelpContainer)
		return iMyHelpContainer;	
#ifdef __S60_50__				
	else if(aIndex)
		return iLeftButton;
	else
		return iRightButton;
#else
	else
		return NULL;
	
#endif	
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CPlayContainer::Draw(const TRect& /*aRect*/) const
{
 	CWindowGc& gc = SystemGc();
 	// draw background skin first.
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
 	AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );

 	
 	
 	if(iCallerImgTmp && iImageOk){
		if(iCallerImgTmp->Handle())
		{
			TRect imgRects(TPoint(0,0),iCallerImgTmp->SizeInPixels());
			TRect DrawRects(iImageRect);
			
			if(imgRects.Width() > DrawRects.Width()
			|| imgRects.Height() > DrawRects.Height()){
			
				if(imgRects.Width() > imgRects.Height()){
					TInt valTmp = ((DrawRects.Width() * imgRects.Height())/ DrawRects.Height());
				
					TInt hhGap = ((imgRects.Width() - valTmp) / 2);
					
					imgRects.iTl.iX = (imgRects.iTl.iX + hhGap);
					imgRects.iBr.iX = (imgRects.iBr.iX - hhGap);
				}else{			
					TInt valTmp = ((DrawRects.Height() * imgRects.Width())/ DrawRects.Width());
							
					TInt hhGap = ((imgRects.Height() - valTmp) / 2);
					
					imgRects.iTl.iY = (imgRects.iTl.iY + hhGap);
					imgRects.iBr.iY = (imgRects.iBr.iY - hhGap);
				}
			}else{ //if(imgRects.Width() <= DrawRects.Width()){
				TInt wwGap = ((DrawRects.Width() - imgRects.Width()) / 2);
				DrawRects.iTl.iX = (DrawRects.iTl.iX + wwGap);
				DrawRects.iBr.iX = (DrawRects.iTl.iX + imgRects.Width());
				
				TInt hGap = ((DrawRects.Height() - imgRects.Height()) / 2);
				DrawRects.iTl.iY = (DrawRects.iTl.iY + hGap);
				DrawRects.iBr.iY = (DrawRects.iTl.iY + imgRects.Height());
			}
			
			
			gc.DrawBitmap(DrawRects,iCallerImgTmp, imgRects);
		}
 	}
 	
 	AknsDrawUtils::DrawFrame( skin,(CWindowGc&) gc,iCallOut,iCallIn,KAknsIIDQsnFrCall2Bubble,KAknsIIDDefault );

 	TInt leftMargin(0);
 	
 	if(iGreenPhone && iGreenPhone->Bitmap() && iGreenPhone->Mask()
 	&& iYellowPhone && iYellowPhone->Bitmap() && iYellowPhone->Mask())
	{
		TSize ImgSiz(iGreenPhone->Bitmap()->SizeInPixels());

    #ifdef __S60_50__   
		TPoint startpp((iCallOut.iTl.iX),(iCallOut.iTl.iY));
	#else
		TPoint startpp((iCallOut.iTl.iX),(iCallOut.iTl.iY - (ImgSiz.iHeight / 6)));
    #endif  		

		if(iMirroRed){
            startpp.iX = (iCallOut.iBr.iX - ImgSiz.iWidth - 5);
		}

		TRect DestRect(startpp,ImgSiz);
		TRect SrcRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight);
	
		if(iYellowOn)
			gc.DrawBitmapMasked(DestRect,iYellowPhone->Bitmap(),SrcRect,iYellowPhone->Mask(), EFalse);
		else
			gc.DrawBitmapMasked(DestRect,iGreenPhone->Bitmap(),SrcRect,iGreenPhone->Mask(), EFalse);
	
		if(iMirroRed){
            leftMargin = DestRect.iTl.iX;
		}else{
            leftMargin = DestRect.iBr.iX;		
		}
	}
#ifdef __S60_50__   	
 	if(iBluePhone && iBluePhone->Bitmap() && iBluePhone->Mask())
	{
		TSize ImgSiz(iBluePhone->Bitmap()->SizeInPixels());
	
		TPoint startpp((iCallOut.iTl.iX + 25),(iCallOut.iTl.iY + 25));
		
		if(iMirroRed){
            startpp.iX = (iCallOut.iBr.iX - ImgSiz.iWidth - 5);
		}
		
		
		TRect DestRect(startpp,ImgSiz);
		TRect SrcRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight);

		gc.DrawBitmapMasked(DestRect,iBluePhone->Bitmap(),SrcRect,iBluePhone->Mask(), ETrue);
	}
#endif  
 	
 	const CFont* useFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);
 	
 	if(iTextFont){
		useFont = iTextFont;
 	}
 	
 	gc.UseFont(useFont);
 	
 	TRgb textcol;
 	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG51);
 	gc.SetPenColor(TLogicalRgb(textcol));
 	
    CGraphicsContext::TTextAlign allign(CGraphicsContext::ELeft);
    
    if(iMirroRed){
        allign = (CGraphicsContext::ERight);
    } 	
 	
#ifdef __S60_50__       
    TRect DestRect(leftMargin,(iCallOut.iTl.iY + 25),iCallOut.iBr.iX,iCallOut.iBr.iY);
 
    if(iMirroRed){
        DestRect.iTl.iX = iCallOut.iTl.iX;
        DestRect.iBr.iX = leftMargin;
    }
    
    TInt GappText = ((DestRect.Height() - 25 - (useFont->AscentInPixels() * 3)) / 3);
        
    if(iName.Length())
    {
        gc.DrawText(iName,DestRect,useFont->AscentInPixels(), allign, 0);
        DestRect.iTl.iY = (DestRect.iTl.iY + useFont->FontMaxHeight() + GappText);
    }

    gc.DrawText(iNumber,DestRect,useFont->AscentInPixels(), allign, 0);
    
    TBuf<60> hjelpper;
    StringLoader::Load(hjelpper,R_SH_STR_CALLING);
    
    if(iMirroRed){    
        //AknTextUtils::CurrentScriptDirectionality()
        AknBidiTextUtils::ConvertToVisualAndClipL(hjelpper,*useFont, DestRect.Width(), DestRect.Width(), AknBidiTextUtils::ERightToLeft );
    }
    
    DestRect.iTl.iY = (DestRect.iTl.iY + useFont->FontMaxHeight() + GappText);
    gc.DrawText(hjelpper,DestRect,useFont->AscentInPixels(), allign, 0);
#else 		
 	
    TInt GappText(10);
    TRect DestRect2(leftMargin,iCallOut.iTl.iY,iCallOut.iBr.iX,iCallOut.iBr.iY);
    
    if(iMirroRed){
        DestRect2.iTl.iX = iCallOut.iTl.iX;
        DestRect2.iBr.iX = leftMargin;
    }    
    
    if(iName.Length())
        GappText = ((DestRect2.Height() - (useFont->FontMaxHeight() * 3)) / 4);
    else
        GappText = ((DestRect2.Height() - (useFont->FontMaxHeight() * 2)) / 3);
    
    DestRect2.iTl.iY = (DestRect2.iTl.iY + GappText);
 
 	if(iName.Length())
 	{
 		gc.DrawText(iName,DestRect2,useFont->AscentInPixels(), allign, 0);
 		DestRect2.iTl.iY = (DestRect2.iTl.iY + useFont->FontMaxHeight() + GappText);
 	}

 	gc.DrawText(iNumber,DestRect2,useFont->AscentInPixels(), allign, 0);
 	
 	TBuf<60> hjelpper;
 	StringLoader::Load(hjelpper,R_SH_STR_CALLING);
 	
    if(iMirroRed){    
        AknBidiTextUtils::ConvertToVisualAndClipL(hjelpper,*useFont, DestRect2.Width(), DestRect2.Width(), AknBidiTextUtils::ERightToLeft );
    }
    
 	DestRect2.iTl.iY = (DestRect2.iTl.iY + useFont->FontMaxHeight() + GappText);
 	gc.DrawText(hjelpper,DestRect2,useFont->AscentInPixels(), allign, 0);
#endif  	
}



	
