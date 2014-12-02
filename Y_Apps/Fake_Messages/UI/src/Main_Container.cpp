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

#include "YApps_E8876008.hrh"                   // own resource header

#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_20028851.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_20022085.rsg>
	#else
		#include <YApps_E8876008.rsg>
	#endif
#endif

#include "YApps_E8876008.h"
#include "Main_Container.h" 
#include "Profile_Settings.h"
#include "Help_Container.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
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


    	
#ifdef LAL_VERSION    
    delete iApi;
#endif  
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(void)
{
	CreateWindowL();

#ifdef SONE_VERSION
	iAddIsValid = ETrue;
#else
	#ifdef LAL_VERSION
		iAddIsValid = EFalse;
		iApi=MAdtronicApiClient::NewL((const TUint8* const)"0A67C531");
	#else
		iAddIsValid = ETrue;
	#endif
#endif
		
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	
	MakeProfileBoxL();
	
	ActivateL();
	SetMenuL();
	
#ifdef LAL_VERSION
	iApi->StatusL(this);
//	installationValid();
#endif
	
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
	
	if(iProfileSettings)
	{
		iProfileSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
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
void CMainContainer::installationValid()
{
	iAddIsValid = ETrue;
	MakeProfileBoxL();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::installationInvalid()
{
	iAddIsValid = EFalse;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(_L("You need to install and activate adtronic"));
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::adtronicNotInstalled()
{
	iAddIsValid = EFalse;
	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(_L("You need to install and activate adtronic"));
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

	if(iAddIsValid)
	{
		CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
		CleanupStack::PushL(ScheduleDB);
		ScheduleDB->ConstructL();
		ScheduleDB->ReadDbItemsL(iItemArray);	
		CleanupStack::PopAndDestroy(ScheduleDB);
		
		
		TBuf<250> Bufff;
		TBuf<100> HjelpBuf1,HjelpBuf2;
		
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
				
				
				Bufff.Append(_L("\t"));
				
				if(iItemArray[i]->iName)
				{
					if(iItemArray[i]->iName->Des().Length() > 50)
						Bufff.Append(iItemArray[i]->iName->Des().Left(50));
					else
						Bufff.Append(*iItemArray[i]->iName);
				}
				
				Bufff.Append(_L("\t"));
				
				if(iItemArray[i]->iMessage)
				{
					if(iItemArray[i]->iMessage->Des().Length() > 50)
						Bufff.Append(iItemArray[i]->iMessage->Des().Left(50));
					else
						Bufff.Append(*iItemArray[i]->iMessage);
				}
				
				MyArray->AppendL(Bufff);
			}
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
	else if(iProfileSettings)
	{
		return iProfileSettings->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
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
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;	
	case EProfModify:
		if(iProfileBox)
		{
			TInt Curr = iProfileBox->CurrentItemIndex();
			if(Curr >= 0 && Curr < iItemArray.Count())
			{
				if(iItemArray[Curr])
				{
					// get current and add data to settings..
					delete iProfileSettings;
					iProfileSettings = NULL;
					iProfileSettings = new(ELeave)CProfileSettings(iCba);
					iProfileSettings->SetMopParent(this);
					iProfileSettings->ConstructL();
					iProfileSettings->SetDataL(iItemArray[Curr]);
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
					  	ScheduleDB->DeleteFromDatabaseL(iItemArray[Curr]->iIndex,EFalse);
					    CleanupStack::PopAndDestroy(ScheduleDB);
				    
						MakeProfileBoxL();
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
			iProfileSettings = new(ELeave)CProfileSettings(iCba);
			iProfileSettings->SetMopParent(this);
			iProfileSettings->ConstructL();
		}
		SetMenuL();
		DrawNow();
		break;
	case ESettOk:
		if(iProfileSettings)
		{	
			CMsgSched* newIttem = new(ELeave)CMsgSched();
			CleanupStack::PushL(newIttem);
			        	
        	newIttem->iIndex = -1;
        	newIttem->iTime.HomeTime();	
        	
        	iProfileSettings->GetValuesL(newIttem);
        	
			CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
			CleanupStack::PushL(ScheduleDB);
			ScheduleDB->ConstructL();
			
			if(newIttem->iIndex >= 0)
				ScheduleDB->UpdateDatabaseL(newIttem);
			else
				ScheduleDB->SaveToDatabaseL(newIttem);
			
			CleanupStack::PopAndDestroy(2);//ScheduleDB,newIttem
				    
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
	default:
		if(iProfileSettings)
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
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		TBool Dim(ETrue);
	
		if(iProfileBox)
		{
			TInt CurrInd = iProfileBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dim = EFalse;
			}
		}
		
    #ifdef SONE_VERSION
        if(!KAppIsTrial){
            aMenuPane->SetItemDimmed(EVisitDrJukka,ETrue);
        } 
    #else

    #endif		
  		
		
		if(!iAddIsValid)
		{
			aMenuPane->SetItemDimmed(EProfNew,ETrue);
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
			aMenuPane->SetItemDimmed(EProfModify,ETrue);
		}
		
		if(Dim)
		{
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
			aMenuPane->SetItemDimmed(EProfModify,ETrue);
		}
	}
	
	if(iProfileSettings)
	{
		iProfileSettings->InitMenuPanelL(aResourceId,aMenuPane);
	}
}

		

