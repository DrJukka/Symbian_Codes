/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#include <barsread.h>

#include <EIKSPANE.H>
#include <AknAppUi.h>
#include <EIKBTGPC.H>
#include <aknnavi.h> 
#include <aknnavide.h> 
#include <akntabgrp.h> 
#include <aknlists.h> 
#include <akniconarray.h> 
#include <eikmenub.h>
#include <aknquerydialog.h> 
#include <AknIconArray.h>
#include <bautils.h>
#include <stringloader.h> 
#include <APGCLI.H>

#include "YApps_E8876015.hrh"                   // own resource header
#include "YApps_E8876015.h"
#include "Main_Container.h" 
#include "Profile_Settings.h"
#include "Default_settings.h" 
#include "Help_Container.h"


#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_2002B0B1.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_E8876015.rsg>
	#else
		#include <YApps_E8876015.rsg>
	#endif
#endif
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(CEikButtonGroupContainer* aCba,const TBool& aDimPreview)
:iCba(aCba),iDimPreview(aDimPreview)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
    
	delete iProfileBox;
    iItemArray.ResetAndDestroy();
    iKeyArray.ResetAndDestroy();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(void)
{
	CreateWindowL();
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	MakeProfileBoxL();
	
	ActivateL();
	SetMenuL();
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	if(iProfileBox)
	{
		iProfileBox->SetRect(Rect());
	}
	
	if(iDefaultSettings)
	{
		iDefaultSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iProfileSettings)
	{
		iProfileSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleResourceChange(TInt aType)
{
	if ( aType==KEikDynamicLayoutVariantSwitch )
	{  	
		TRect rect;
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
	
		SetRect(rect);
	}
	
	CCoeControl::HandleResourceChange(aType);
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::MakeProfileBoxL()
{
	TInt MySetIndex(0);
	
	if(iProfileBox)
	{
		MySetIndex = iProfileBox->CurrentItemIndex();
	}
    
	delete iProfileBox;
	iProfileBox = NULL;
	
    iProfileBox   = new( ELeave ) CAknDoubleTimeStyleListBox();
	iProfileBox->ConstructL(this,EAknListBoxSelectionList);
	iProfileBox->Model()->SetItemTextArray(GetProfilesArrayL());
	
    iProfileBox->Model()->SetOwnershipType(ELbmOwnsItemArray);;
	iProfileBox->CreateScrollBarFrameL( ETrue );
    iProfileBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iProfileBox->SetRect(Rect());
	iProfileBox->ActivateL();

	TInt ItemsCount = iProfileBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iProfileBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iProfileBox->SetCurrentItemIndex(0);
	
	UpdateScrollBar(iProfileBox);
	iProfileBox->DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArray* CMainContainer::GetProfilesArrayL(void)
{
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(MyArray);

	iItemArray.ResetAndDestroy();
    
	GetValuesFromStoreL();
	
    CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
	CleanupStack::PushL(ScheduleDB);
	ScheduleDB->ConstructL();
  	ScheduleDB->ReadDbItemsL(iItemArray);	
    CleanupStack::PopAndDestroy(ScheduleDB);
    
    
    TBuf<250> Bufff;
    TBuf<100> HjelpBuf1;
    
    for(TInt ii=0; ii < iKeyArray.Count(); ii++)
    {
    	if(iKeyArray[ii])
    	{		
    		Bufff.Zero();
    		Bufff.Num((ii + 1));
    		Bufff.Append(_L("\t"));
			Bufff.Append(iKeyArray[ii]->iLabel);
    		    			
    		Bufff.Append(_L("\t"));
    			
    		if(iKeyArray[ii]->iNunmber)
    		{
    			if(iKeyArray[ii]->iNunmber->Des().Length() > 50)
    				Bufff.Append(iKeyArray[ii]->iNunmber->Des().Left(50));
    			else
    				Bufff.Append(*iKeyArray[ii]->iNunmber);
    		}
    			
    		Bufff.Append(_L("\t"));
    		
    		if(iKeyArray[ii]->iEnabled)
    		{
    			if(iKeyArray[ii]->iName)
    			{
    				if(iKeyArray[ii]->iName->Des().Length() > 50)
    					Bufff.Append(iKeyArray[ii]->iName->Des().Left(50));
    				else
    					Bufff.Append(*iKeyArray[ii]->iName);
    			}
    		}
    		else
    		{
    			StringLoader::Load(HjelpBuf1,R_SH_STR_DISABLED);
    			Bufff.Append(HjelpBuf1);
    		}
    			
    		MyArray->AppendL(Bufff);
    	}
    }
    
	for(TInt i=0; i < iItemArray.Count(); i++)
	{
		if(iItemArray[i])
		{
		
			Bufff.Zero();
		
			Bufff.Num(iItemArray[i]->iTime.DateTime().Hour());
			Bufff.Append(_L(":"));
			if(iItemArray[i]->iTime.DateTime().Minute() < 10)
			{
				Bufff.AppendNum(0);	
			}
			Bufff.AppendNum(iItemArray[i]->iTime.DateTime().Minute());
			Bufff.Append(_L("\t"));
			
			if(iItemArray[i]->iRepeat == 0)
			{
				if(iItemArray[i]->iTime.DateTime().Day() < 9)
				{
					Bufff.AppendNum(0);	
				}
				Bufff.AppendNum(iItemArray[i]->iTime.DateTime().Day() + 1);
				//Bufff.Append(DateChar);
				Bufff.Append(_L("/"));
				
				TMonthNameAbb ThisMonth(iItemArray[i]->iTime.DateTime().Month());
				
				Bufff.Append(ThisMonth);
				
				Bufff.Append(_L("/"));
				
				TInt Yearrr = iItemArray[i]->iTime.DateTime().Year();
				TInt Hudreds = Yearrr / 100;
				
				Yearrr = (Yearrr - (Hudreds * 100));
				if(Yearrr < 10)
				{
					Bufff.AppendNum(0);	
				}
				
				Bufff.AppendNum(Yearrr);
			}
			else
			{
				STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetRepeatTypeName(iItemArray[i]->iRepeat,HjelpBuf1);
				Bufff.Append(HjelpBuf1);
			}
			
			Bufff.Append(_L("\t"));
			
			if(iItemArray[i]->iNunmber)
			{
				if(iItemArray[i]->iNunmber->Des().Length() > 50)
					Bufff.Append(iItemArray[i]->iNunmber->Des().Left(50));
				else
					Bufff.Append(*iItemArray[i]->iNunmber);
			}
			
			Bufff.Append(_L("\t"));
			
			if(iItemArray[i]->iEnabled)
			{
				if(iItemArray[i]->iName)
				{
					if(iItemArray[i]->iName->Des().Length() > 50)
						Bufff.Append(iItemArray[i]->iName->Des().Left(50));
					else
						Bufff.Append(*iItemArray[i]->iName);
				}
			}
			else
			{
				StringLoader::Load(HjelpBuf1,R_SH_STR_DISABLED);
				Bufff.Append(HjelpBuf1);
			}
			
			MyArray->AppendL(Bufff);
		}
	}
	
	CleanupStack::Pop(MyArray);
	return MyArray;
}
	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::UpdateScrollBar(CEikListBox* aListBox)
    {
    if (aListBox)
        {   
        TInt pos(aListBox->View()->CurrentItemIndex());
        if (aListBox->ScrollBarFrame())
            {
            aListBox->ScrollBarFrame()->MoveVertThumbTo(pos);
            }
        }
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(Rect());
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iDefaultSettings)
	{
		Ret = iDefaultSettings->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iProfileSettings)
	{
		Ret = iProfileSettings->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			if(iProfileBox){
				TInt command(0);
				if(HandlePopUpMenuSelectionL(command)){
					HandleViewCommandL(command);
				}
			}else{
				CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			}
			break;
		case EKeyRightArrow:
		case EKeyLeftArrow:
		default:
			if(iProfileBox)
			{
				Ret = iProfileBox->OfferKeyEventL(aKeyEvent,aEventCode);
			}
			break;
		}
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMainContainer::HandlePopUpMenuSelectionL(TInt& aCommand)
{
	TBool ret(EFalse);
	
	TBool isEnable(EFalse),isDisable(EFalse),isRemove(EFalse),isaPreview(EFalse);
	ShowMenuItemsL(isEnable,isDisable,isRemove,isaPreview);
	
	if(!isRemove && !isaPreview){
		ret = ETrue;
		aCommand = EProfModify;
	}else{
	
		TBuf<100> hjelpper;
		CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
		CleanupStack::PushL(list);
	
		CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
		CleanupStack::PushL(popupList);
	
		list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
		list->CreateScrollBarFrameL(ETrue);
		list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
	
		CArrayFixFlat<TInt>* cmdArray = new (ELeave) CArrayFixFlat<TInt>(5);
		CleanupStack::PushL(cmdArray);
		CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(5);
		CleanupStack::PushL(Array);

		if(isEnable){
			StringLoader::Load(hjelpper,R_CMD_ENABLE);
			Array->AppendL(hjelpper);
			cmdArray->AppendL(EProfEnable);
		}else if(isDisable){
			StringLoader::Load(hjelpper,R_CMD_DISABLE);
			Array->AppendL(hjelpper);
			cmdArray->AppendL(EProfDisable);
		}
		
		if(isaPreview && !iDimPreview){
			StringLoader::Load(hjelpper,R_CMD_MODIFY);
			Array->AppendL(hjelpper);
			cmdArray->AppendL(EProfModify);
	
			StringLoader::Load(hjelpper,R_CMD_PREVIEW);
			Array->AppendL(hjelpper);
			cmdArray->AppendL(EProfTest);		
		}

		if(isRemove){
			StringLoader::Load(hjelpper,R_CMD_DELETE);
			Array->AppendL(hjelpper);
			cmdArray->AppendL(EProfRemove);		
		}

		list->Model()->SetItemTextArray(Array);
		CleanupStack::Pop();//Array
		list->Model()->SetOwnershipType(ELbmOwnsItemArray);
	
		if (popupList->ExecuteLD())
		{
			TInt Selected = list->CurrentItemIndex();
			if(Selected >= 0 && Selected < cmdArray->Count())
			{
				ret = ETrue;
				aCommand = cmdArray->At(Selected);
			}
		}
	
		CleanupStack::PopAndDestroy(1); // cmdArray
		CleanupStack::Pop();            // popuplist
		CleanupStack::PopAndDestroy(1); // list
	}
	
	return ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleViewCommandL(TInt aCommand)
{
	TBuf<60> Hjelpper;

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
			iMyHelpContainer = CMyHelpContainer::NewL(0);
		}
		SetMenuL();
		DrawNow();    		
		break;	
	case EProfTest:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr < 4)
			{
				if(Curr >= 0 && Curr < iKeyArray.Count())
				{
					if(iKeyArray[Curr] && iKeyArray[Curr]->iNunmber)
					{
						TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
						TApaTask task = taskList.FindApp(KUidCallUIApp);
						if ( task.Exists() )
						{
							TBuf<200> hjelpper;
																		
							if(iKeyArray[Curr]->iNunmber)
								hjelpper.Copy(*iKeyArray[Curr]->iNunmber);
											
							task.SwitchOpenFile(hjelpper);
						}
						else
						{
							//Do start UI now.
							TThreadId app_threadid;
							CApaCommandLine* cmdLine; 
							cmdLine=CApaCommandLine::NewLC();
							cmdLine->SetExecutableNameL(KtxCallUIAppFileName);
													
							if(iKeyArray[Curr]->iNunmber)
								cmdLine->SetDocumentNameL(*iKeyArray[Curr]->iNunmber);
													
							cmdLine->SetCommandL( EApaCommandRun );
							RApaLsSession ls;
							User::LeaveIfError(ls.Connect());
							ls.StartApp(*cmdLine,app_threadid);
							ls.Close();
							CleanupStack::PopAndDestroy(); // cmdLine
						}
					}
				}			
			}
			else
			{
				Curr = (Curr - 4);
				if(Curr >= 0 && Curr < iItemArray.Count())
				{
					if(iItemArray[Curr])
					{
						TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
						TApaTask task = taskList.FindApp(KUidCallUIApp);
						if ( task.Exists() )
						{
							TBuf<200> hjelpper;
														
							if(iItemArray[Curr]->iNunmber)
								hjelpper.Copy(*iItemArray[Curr]->iNunmber);
						
							task.SwitchOpenFile(hjelpper);
						}
						else
						{
							//Do start UI now.
							TThreadId app_threadid;
							CApaCommandLine* cmdLine; 
							cmdLine=CApaCommandLine::NewLC();
							cmdLine->SetExecutableNameL(KtxCallUIAppFileName);
								
							if(iItemArray[Curr]->iNunmber)
								cmdLine->SetDocumentNameL(*iItemArray[Curr]->iNunmber);
								
							cmdLine->SetCommandL( EApaCommandRun );
							RApaLsSession ls;
							User::LeaveIfError(ls.Connect());
							ls.StartApp(*cmdLine,app_threadid);
							ls.Close();
							CleanupStack::PopAndDestroy(); // cmdLine
						}
					}
				}
			}
		}
		break;		
	case EProfModify:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr < 4)
			{
				if(Curr >= 0 && Curr < iKeyArray.Count())
				{
					if(iKeyArray[Curr])
					{
						// get current and add data to settings..
						delete iProfileSettings;
						iProfileSettings = NULL;
						iProfileSettings = new(ELeave)CProfileSettings(iCba,EFalse);
						iProfileSettings->SetMopParent(this);
						iProfileSettings->ConstructL();
						iProfileSettings->SetDataL(iKeyArray[Curr]);
					}
				}
			}
			else
			{
				Curr = (Curr - 4);
				if(Curr >= 0 && Curr < iItemArray.Count())
				{
					if(iItemArray[Curr])
					{
						// get current and add data to settings..
						delete iProfileSettings;
						iProfileSettings = NULL;
						iProfileSettings = new(ELeave)CProfileSettings(iCba,ETrue);
						iProfileSettings->SetMopParent(this);
						iProfileSettings->ConstructL();
						iProfileSettings->SetDataL(iItemArray[Curr]);
					}
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EProfRemove:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr < 4)
			{
				// none removable		
			}
			else
			{
				Curr = (Curr - 4);
				if(Curr >= 0 && Curr < iItemArray.Count())
				{
					if(iItemArray[Curr])
					{
						StringLoader::Load(Hjelpper,R_STR_REMMESSAGE);
					
						CAknQueryDialog* dlg = CAknQueryDialog::NewL();
						if(dlg->ExecuteLD(R_QUERY,Hjelpper))
						{
							CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
							CleanupStack::PushL(ScheduleDB);
							ScheduleDB->ConstructL();
							ScheduleDB->DeleteFromDatabaseL(iItemArray[Curr]->iIndex);
							CleanupStack::PopAndDestroy(ScheduleDB);
						
							MakeProfileBoxL();
						}
					}
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EProfNew:
		{		
			delete iProfileSettings;
			iProfileSettings = NULL;
			iProfileSettings = new(ELeave)CProfileSettings(iCba,ETrue);
			iProfileSettings->SetMopParent(this);
			iProfileSettings->ConstructL();
		}
		SetMenuL();
		DrawNow();
		break;
	case ESettOk2:
	case ESettOk:
		if(iProfileSettings)
		{	
			if(aCommand == ESettOk)
			{
				CMsgSched* newIttem = new(ELeave)CMsgSched();
				CleanupStack::PushL(newIttem);
							
				newIttem->iIndex = -1;
				newIttem->iTime.HomeTime();	
				
				iProfileSettings->GetValuesL(newIttem);
				newIttem->iEnabled = ETrue;
				
				CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				ScheduleDB->ConstructL();
				
				if(newIttem->iIndex >= 0)
					ScheduleDB->UpdateDatabaseL(newIttem);
				else
					ScheduleDB->SaveToDatabaseL(newIttem);
				
				CleanupStack::PopAndDestroy(ScheduleDB);
				CleanupStack::PopAndDestroy(newIttem);
			}else{ // ESettOk2
				TInt Curr = iProfileBox->CurrentItemIndex();
			
				if(Curr >= 0 && Curr < iKeyArray.Count())
				{
					if(iKeyArray[Curr])
					{
						iProfileSettings->GetValuesL(iKeyArray[Curr]);
						iKeyArray[Curr]->iEnabled = ETrue;	
						SaveValuesL();
					}
				}
			}
			
			MakeProfileBoxL();
		}
	case ESettCancel:
		{
			delete iProfileSettings;
			iProfileSettings = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EProfDisable:
	case EProfEnable:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr < 4)
			{
				if(Curr >= 0 && Curr < iKeyArray.Count())
				{
					if(iKeyArray[Curr])
					{
						if(aCommand == EProfEnable)
							iKeyArray[Curr]->iEnabled = ETrue;	
						else // EProfDisable
							iKeyArray[Curr]->iEnabled = EFalse;
						
						SaveValuesL();
						iProfileBox->Model()->SetItemTextArray(GetProfilesArrayL());
					}
				}
			}
			else
			{
				Curr = (Curr - 4);
				if(Curr >= 0 && Curr < iItemArray.Count())
				{
					if(iItemArray[Curr])
					{
						if(aCommand == EProfEnable)
							iItemArray[Curr]->iEnabled = ETrue;	
						else // EProfDisable
							iItemArray[Curr]->iEnabled = EFalse;	
							
						CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
						CleanupStack::PushL(ScheduleDB);
						ScheduleDB->ConstructL();								
						ScheduleDB->UpdateDatabaseL(iItemArray[Curr]);
		
						CleanupStack::PopAndDestroy(ScheduleDB);
							
						iProfileBox->Model()->SetItemTextArray(GetProfilesArrayL());
					}
				}
			}
		}
		DrawNow();
		break;	
	case ESett2Ok:
		if(iDefaultSettings)
		{
			iDefaultSettings->SaveValuesL();
		}		
	case ESett2Cancel:
		{
			delete iDefaultSettings;
			iDefaultSettings = NULL;
		}
		SetMenuL();
		DrawNow();
		break;		
	case EDefSettings:
		{
			iDefaultSettings = new(ELeave)CDefaultSettings(iCba);
			iDefaultSettings->ConstructL();
		}
		SetMenuL();
		DrawNow();		
		break;		
	default:
		if(iDefaultSettings)
		{
			iDefaultSettings->HandleViewCommandL(aCommand);
		}
		else if(iProfileSettings)
		{
			iProfileSettings->HandleViewCommandL(aCommand);
		}
		break;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl( TInt /*aIndex*/) const
{
	CCoeControl* RetCont(NULL);

	if(iMyHelpContainer)
	{
		RetCont = iMyHelpContainer;		
	}
	else if(iDefaultSettings)
	{
		RetCont = iDefaultSettings;
	}
	else if(iProfileSettings)
	{
		RetCont = iProfileSettings;
	}
	else 
	{
		RetCont = iProfileBox;
	}
	
	return RetCont;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	TInt RetCount(0);
	
	if(iMyHelpContainer)
	{
		RetCount = 1;	
	}
	else if(iDefaultSettings)
	{
		RetCount = 1;
	}
	else if(iProfileSettings)
	{
		RetCount = 1;
	}
	else 
	{
		RetCount = 1;
	}
 
	return RetCount;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetValuesFromStoreL(void)
{	
	iKeyArray.ResetAndDestroy();

	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxKeySettingsFile, KNullDesC))
	{
		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));
	
		TFileName hljBuffer;
		TUint32 TmpValue(0);
				
		CKeyCapcap* ittem1 = new(ELeave)CKeyCapcap();
		iKeyArray.Append(ittem1);
		StringLoader::Load(ittem1->iLabel,R_STR_GREENBUTTON);
		
		GetValuesL(MyDStore,0x0011,TmpValue);
							
		if(TmpValue > 50)
			ittem1->iEnabled = ETrue;
		else// if(TmpValue > 10)
			ittem1->iEnabled = EFalse;
		
		GetValuesL(MyDStore,0x0012,hljBuffer);
		ittem1->iNunmber = hljBuffer.AllocL();

		GetValuesL(MyDStore,0x0013,hljBuffer);
		ittem1->iName = hljBuffer.AllocL();
		
		TmpValue = 0;
		GetValuesL(MyDStore,0x0014,TmpValue);
									
		if(TmpValue > 3599){
			TmpValue = 3599;
		}
		
		ittem1->iDelay = TmpValue;
		
		CKeyCapcap* ittem2 = new(ELeave)CKeyCapcap();
		iKeyArray.Append(ittem2);
		StringLoader::Load(ittem2->iLabel,R_STR_REDBUTTON);
		
		GetValuesL(MyDStore,0x0021,TmpValue);
							
		if(TmpValue > 50)
			ittem2->iEnabled = ETrue;
		else// if(TmpValue > 10)
			ittem2->iEnabled = EFalse;
		
		GetValuesL(MyDStore,0x0022,hljBuffer);
		ittem2->iNunmber = hljBuffer.AllocL();

		GetValuesL(MyDStore,0x0023,hljBuffer);
		ittem2->iName = hljBuffer.AllocL();
				
		TmpValue = 0;
		GetValuesL(MyDStore,0x0024,TmpValue);
											
		if(TmpValue > 3599){
			TmpValue = 3599;
		}
				
		ittem2->iDelay = TmpValue;
		
		CKeyCapcap* ittem3 = new(ELeave)CKeyCapcap();
		iKeyArray.Append(ittem3);
		StringLoader::Load(ittem3->iLabel,R_STR_OFFBUTTON);
		
		GetValuesL(MyDStore,0x0031,TmpValue);
							
		if(TmpValue > 50)
			ittem3->iEnabled = ETrue;
		else// if(TmpValue > 10)
			ittem3->iEnabled = EFalse;
		
		GetValuesL(MyDStore,0x0032,hljBuffer);
		ittem3->iNunmber = hljBuffer.AllocL();

		GetValuesL(MyDStore,0x0033,hljBuffer);
		ittem3->iName = hljBuffer.AllocL();		
	
		TmpValue = 0;
		GetValuesL(MyDStore,0x0034,TmpValue);
											
		if(TmpValue > 3599){
			TmpValue = 3599;
		}
				
		ittem3->iDelay = TmpValue;		
		
		CKeyCapcap* ittem4 = new(ELeave)CKeyCapcap();
		iKeyArray.Append(ittem4);
		StringLoader::Load(ittem4->iLabel,R_STR_CAMERABUTTON);
		
		GetValuesL(MyDStore,0x0041,TmpValue);
							
		if(TmpValue > 50)
			ittem4->iEnabled = ETrue;
		else// if(TmpValue > 10)
			ittem4->iEnabled = EFalse;
		
		GetValuesL(MyDStore,0x0042,hljBuffer);
		ittem4->iNunmber = hljBuffer.AllocL();

		GetValuesL(MyDStore,0x0043,hljBuffer);
		ittem4->iName = hljBuffer.AllocL();	

		TmpValue = 0;
		GetValuesL(MyDStore,0x0044,TmpValue);
											
		if(TmpValue > 3599){
			TmpValue = 3599;
		}
				
		ittem4->iDelay = TmpValue;		
		
		CleanupStack::PopAndDestroy(1);// Store
	}
}

		
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TDes& aValue)
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
void CMainContainer::GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue)
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
		aValue = 0;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(void)
{
	TFindFile AufFolder(CCoeEnv::Static()->FsSession());
	if(KErrNone == AufFolder.FindByDir(KtxKeySettingsFile, KNullDesC))
	{
		User::LeaveIfError(CCoeEnv::Static()->FsSession().Delete(AufFolder.File()));

		CDictionaryFileStore* MyDStore = CDictionaryFileStore::OpenLC(CCoeEnv::Static()->FsSession(),AufFolder.File(), TUid::Uid(0x102013AD));

		if(iKeyArray.Count() > 0){
			if(iKeyArray[0]){
				if(iKeyArray[0]->iEnabled)
					SaveValuesL(MyDStore,0x0011,0x100);	
				else
					SaveValuesL(MyDStore,0x0011,0x000);
				
				if(iKeyArray[0]->iNunmber){
					SaveValuesL(MyDStore,0x0012,*iKeyArray[0]->iNunmber);
				}
				
				if(iKeyArray[0]->iName){
					SaveValuesL(MyDStore,0x0013,*iKeyArray[0]->iName);
				}
				
				SaveValuesL(MyDStore,0x0014,iKeyArray[0]->iDelay);
			}
		}
		
		if(iKeyArray.Count() > 1){
			if(iKeyArray[1]){
				if(iKeyArray[1]->iEnabled)
					SaveValuesL(MyDStore,0x0021,0x100);	
				else
					SaveValuesL(MyDStore,0x0021,0x000);
				
				if(iKeyArray[1]->iNunmber){
					SaveValuesL(MyDStore,0x0022,*iKeyArray[1]->iNunmber);
				}
				
				if(iKeyArray[1]->iName){
					SaveValuesL(MyDStore,0x0023,*iKeyArray[1]->iName);
				}		
				
				SaveValuesL(MyDStore,0x0024,iKeyArray[1]->iDelay);
			}
		}	

		if(iKeyArray.Count() > 2){
			if(iKeyArray[2]){
				if(iKeyArray[2]->iEnabled)
					SaveValuesL(MyDStore,0x0031,0x100);	
				else
					SaveValuesL(MyDStore,0x0031,0x000);
				
				if(iKeyArray[2]->iNunmber){
					SaveValuesL(MyDStore,0x0032,*iKeyArray[2]->iNunmber);
				}
				
				if(iKeyArray[2]->iName){
					SaveValuesL(MyDStore,0x0033,*iKeyArray[2]->iName);
				}	
				
				SaveValuesL(MyDStore,0x0034,iKeyArray[2]->iDelay);
			}
		}
		
		if(iKeyArray.Count() > 3){
			if(iKeyArray[3]){
				if(iKeyArray[3]->iEnabled)
					SaveValuesL(MyDStore,0x0041,0x100);	
				else
					SaveValuesL(MyDStore,0x0041,0x000);
				
				if(iKeyArray[3]->iNunmber){
					SaveValuesL(MyDStore,0x0042,*iKeyArray[3]->iNunmber);
				}
				
				if(iKeyArray[3]->iName){
					SaveValuesL(MyDStore,0x0043,*iKeyArray[3]->iName);
				}	
				
				SaveValuesL(MyDStore,0x0044,iKeyArray[3]->iDelay);
			}
		}
				
		MyDStore->CommitL();
		CleanupStack::PopAndDestroy(1);// Store
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, const TDesC& aValue)
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
	out2 << aValue;
	out2.CommitL(); 	
	CleanupStack::PopAndDestroy(1);// out2
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue)
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
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	if(iMyHelpContainer)
	{
		if(iCba)
		{
			iCba->SetCommandSetL(R_APPHELP_CBA);
			iCba->DrawDeferred();
		}
	}
	else if(iDefaultSettings)
	{
		iDefaultSettings->SetMenuL();
	}
	else if(iProfileSettings)
	{
		iProfileSettings->SetMenuL();
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
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ShowMenuItemsL(TBool& aEnable, TBool& aDisable, TBool& aRemove, TBool& aPreview)
{
	aEnable = aDisable = aRemove = aPreview = EFalse;
	
	if(iProfileBox)
	{
		TInt Curr = iProfileBox->CurrentItemIndex();
		if(Curr < 4)
		{
			aRemove = EFalse;
			
			if(Curr >= 0 && Curr < iKeyArray.Count())
			{
				if(iKeyArray[Curr])
				{
					if(iKeyArray[Curr]->iEnabled){
						aEnable = EFalse;
						aDisable = ETrue;
					}else{
						aEnable = ETrue;
						aDisable = EFalse;					
					}
					if(iKeyArray[Curr]->iNunmber && iKeyArray[Curr]->iNunmber->Des().Length()){
						aPreview = ETrue;								
					}else{
						aPreview = EFalse;						
					}
				}
			}
		}
		else
		{
			Curr = (Curr - 4);
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				if(iItemArray[Curr])
				{
					if(iItemArray[Curr]->iEnabled){
						aEnable = EFalse;
						aDisable = ETrue;
					}else{
						aEnable = ETrue;
						aDisable = EFalse;					
					}	
					
					if(iItemArray[Curr]->iNunmber && iItemArray[Curr]->iNunmber->Des().Length()){
						aPreview = ETrue;
						aRemove = ETrue;
					}else{
						aPreview = EFalse;	
						aRemove = EFalse;
					}		
				}
			}
		}		
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		TBool isEnable(EFalse),isDisable(EFalse),isRemove(EFalse),isaPreview(EFalse);
		
		ShowMenuItemsL(isEnable,isDisable,isRemove,isaPreview);
		
#ifdef SONE_VERSION
    if(!KAppIsTrial){
        aMenuPane->SetItemDimmed(EVisitDrJukka,ETrue);
    } 
#else

#endif  
        
		if(!isEnable){
			aMenuPane->SetItemDimmed(EProfEnable,ETrue);
		}
		
		if(!isDisable){
			aMenuPane->SetItemDimmed(EProfEnable,ETrue);
		}

		if(!isRemove){
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
		}

		if(!isaPreview){
			aMenuPane->SetItemDimmed(EProfTest,ETrue);
		}
		
		if(iDimPreview){
            aMenuPane->SetItemDimmed(EProfTest,ETrue);
		}
		
		//aMenuPane->SetItemDimmed(EProfModify,ETrue);
	}
	
	if(iDefaultSettings)
	{
		iDefaultSettings->InitMenuPanelL(aResourceId,aMenuPane);
	}
	
	if(iProfileSettings)
	{
		iProfileSettings->InitMenuPanelL(aResourceId,aMenuPane);
	}
}


		

