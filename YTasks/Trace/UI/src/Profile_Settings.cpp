
#include "Profile_Settings.h"            // own definitions


#include "YTools_A0003129.hrh"
          // own resource header


#include <AknGlobalNote.h>
#include <eikclbd.h>
#include <apgcli.h>
#include <APMREC.H>
#include <hal.h> 
#include <aknmfnesettingpage.h> 
#include <aknmfnesettingpage.h>
#include <AknQueryDialog.h>
#include <AknSettingItemList.h>
#include <AknQueryDialog.h>
#include <BAUTILS.H>
#include <eikappui.h> 
#include <eikapp.h>
#include <s32file.h>
#include <aknlists.h> 
#include <AknIconArray.h>

#include "MainContainer.h"
#include "ExampleServerSession.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A4_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A0003129_res.rsg"
	#else
		#include "YTools_A0003129_res.rsg"
	#endif
#endif

// Settings
//
CAknSettingItem* CAknSettingsListListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case EDrawStyle:
		settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(aIdentifier,iDrawStyle);
		break;
	case ESettingOn:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iEnabledOn);
		break;
	case ESettingHor:
		settingItem = new (ELeave) CAknSliderSettingItem(aIdentifier,iHorSlider);
		break;
	case ESettingVer:
		settingItem = new (ELeave) CAknSliderSettingItem(aIdentifier,iVerSlider);
		break;
	case EFontSize:
		settingItem = new (ELeave) CAknSliderSettingItem(aIdentifier,iFontSize);
		break;
	}	
	
	return settingItem;
}


/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CProfileSettings::CProfileSettings()
{	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CProfileSettings::~CProfileSettings()
{		
	delete iListBox;
	iListBox = NULL;
	
	delete iBgContext;
	iBgContext = NULL;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CProfileSettings::ConstructL(void)
{
    CreateWindowL();
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDSkinBmpMainPaneUsual, TRect(0,0,1,1), ETrue);
    
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	ActivateL();
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::MakeListboxL(TInt aSelected)
{			
    delete iListBox;
    iListBox = NULL;
    iListBox = new (ELeave) CAknSettingsListListbox();
    iListBox->SetMopParent(this); 
    
    GetValuesFromStoreL();
	
	iListBox->ConstructFromResourceL(R_PROF_SETTING);

	iListBox->MakeVisible(ETrue);
    iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	if(aSelected >= 0 && aSelected < 3)
	{
		iListBox->ListBox()->SetCurrentItemIndex(aSelected);
	}
    iListBox->ActivateL();
	iListBox->LoadSettingsL();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}

	if(iListBox)
	{
		iListBox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	else
	{
		MakeListboxL(-1);
	}
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CProfileSettings::MopSupplyObject(TTypeUid aId)
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
void CProfileSettings::HandleResourceChange(TInt aType)
{
	TRect rect;
	TBool SetR(EFalse);
	
    if ( aType == 0x101F8121 )
    {
    	SetR = ETrue;
    	rect = CEikonEnv::Static()->EikAppUi()->ClientRect();
    } 
 
 	if(SetR)
 	{	    
    	SetRect(rect);	
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CProfileSettings::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iListBox)// && iRegStatus != KInvalidVers)
	{
		Ret = iListBox->OfferKeyEventL(aKeyEvent,aType);
	}
	 
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CProfileSettings::CountComponentControls() const
{	
	if(iListBox)
		return 1;
	else
		return 0;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CProfileSettings::ComponentControl( TInt aIndex) const
{
	return iListBox; 
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
	if(iBgContext){
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect());
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::GetValuesFromStoreL(void)
{	
	if(!iListBox)
		return;
	
	TFileName DefaultFile;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KTraceSetFileName, KNullDesC))
	{
		DefaultFile.Copy(AppFile.File());
	}
	else
	{
		TFindFile AppFile2(CCoeEnv::Static()->FsSession());
		if(KErrNone == AppFile2.FindByDir(KTraceIconsFileName, KNullDesC))
		{
			TParsePtrC Hejlp(AppFile2.File());
			DefaultFile.Copy(Hejlp.Drive());
			DefaultFile.Append(KTraceSetFileName);
		}
	}
		
	if(DefaultFile.Length())
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),DefaultFile, TUid::Uid(0x102013AD));

		TInt32 TmpValue(-1);

		GetValuesL(MyDStore,0x7777,TmpValue);
		
		if(TmpValue > 50)
			iListBox->iEnabledOn = ETrue;
		else 
			iListBox->iEnabledOn  = EFalse;
		
		TmpValue = -1;		
		GetValuesL(MyDStore,0x5555,TmpValue);
		
		if(TmpValue < 0 || TmpValue > 2)
		{
			TmpValue = 0;
		}
	
		iListBox->iDrawStyle = TmpValue;
		
		
		TmpValue = -1;
		GetValuesL(MyDStore,0x8877,TmpValue);
		
		iListBox->iHorSlider = TmpValue;
		
		if(iListBox->iHorSlider < 0)
			iListBox->iHorSlider = 0;
		else if(iListBox->iHorSlider > 100)
			iListBox->iHorSlider = 100;

		TmpValue = -1;
		GetValuesL(MyDStore,0x7788,TmpValue);
		
		iListBox->iVerSlider = TmpValue;
		
		if(iListBox->iVerSlider < 0)
			iListBox->iVerSlider = 0;
		else if(iListBox->iVerSlider > 100)
			iListBox->iVerSlider = 100;
		
		TmpValue = -1;
		GetValuesL(MyDStore,0x6666,TmpValue);
		
		iListBox->iFontSize = TmpValue;
		
		if(iListBox->iFontSize < 10)
			iListBox->iFontSize = 10;
		else if(iListBox->iFontSize > 150)
			iListBox->iFontSize = 150;	
	
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TInt32& aValue)
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
		aValue = -1;
}
	
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CProfileSettings::SaveValuesL(void)
{
	if(!iListBox)
		return;

	iListBox->StoreSettingsL();

	TFileName DefaultFile;
	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KTraceSetFileName, KNullDesC))
	{
		DefaultFile.Copy(AppFile.File());
	}
	else
	{
		TFindFile AppFile2(CCoeEnv::Static()->FsSession());
		if(KErrNone == AppFile2.FindByDir(KTraceIconsFileName, KNullDesC))
		{
			TParsePtrC Hejlp(AppFile2.File());
			DefaultFile.Copy(Hejlp.Drive());
			DefaultFile.Append(KTraceSetFileName);
		}
	}
		
	if(DefaultFile.Length())
	{
		User::LeaveIfError(CCoeEnv::Static()->FsSession().Delete(DefaultFile));

		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),DefaultFile, TUid::Uid(0x102013AD));
				
		if(iListBox->iEnabledOn)
			SaveValuesL(MyDStore,0x7777,0x100);	
		else
			SaveValuesL(MyDStore,0x7777,0x000);

		SaveValuesL(MyDStore,0x5555,iListBox->iDrawStyle);		
					
		SaveValuesL(MyDStore,0x8877,iListBox->iHorSlider);

		SaveValuesL(MyDStore,0x7788,iListBox->iVerSlider);
		
		SaveValuesL(MyDStore,0x6666,iListBox->iFontSize);		
		
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CProfileSettings::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TInt32 aValue)
{
	if(aDStore == NULL)
		return;
		
	TUid FileUid = {0x10};
	FileUid.iUid = aUID;

	if(aDStore->IsPresentL(FileUid))
	{
		aDStore->Remove(FileUid);
		aDStore->CommitL();
	}
			
	RDictionaryWriteStream out2;
	out2.AssignLC(*aDStore,FileUid);
	out2.WriteInt32L(aValue);
	out2.CommitL(); 	
	CleanupStack::PopAndDestroy(1);// out2
}



