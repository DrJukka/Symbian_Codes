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

#include "YApps_E8876012.hrh"                   // own resource header
#include "YApps_E8876012.h"
#include "Main_Container.h" 
#include "Profile_Settings.h"
#include "Help_Container.h"

#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_20028858.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_E8876012.rsg>
	#else
		#include <YApps_E8876012.rsg>
	#endif
#endif
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
    
    CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
	CleanupStack::PushL(ScheduleDB);
	ScheduleDB->ConstructL();
  	ScheduleDB->ReadDbItemsL(iItemArray);	
    CleanupStack::PopAndDestroy(ScheduleDB);
    
    
    TBuf<250> Bufff;
    TBuf<100> HjelpBuf1;
    
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
			
			if(iItemArray[i]->iMessage)
			{
				if(iItemArray[i]->iMessage->Des().Length() > 50)
					Bufff.Append(iItemArray[i]->iMessage->Des().Left(50));
				else
					Bufff.Append(*iItemArray[i]->iMessage);
			}
			
			Bufff.Append(_L("\t"));
			
			if(iItemArray[i]->iEnabled)
			{
				if(iItemArray[i]->iNunmber)
				{
					if(iItemArray[i]->iNunmber->Des().Length() > 50)
						Bufff.Append(iItemArray[i]->iNunmber->Des().Left(50));
					else
						Bufff.Append(*iItemArray[i]->iNunmber);
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
	else if(iProfileSettings)
	{
		Ret = iProfileSettings->OfferKeyEventL(aKeyEvent,aEventCode);
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
			iMyHelpContainer = CMyHelpContainer::NewL(0);
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
        	
        	TBool okToSave(EFalse);
        	if(CheckMessageForUnicodeL(newIttem->iMessage,okToSave))
        	{	
				newIttem->iUnicode = ETrue;
        	}
        	else
        	{
				newIttem->iUnicode = EFalse;
				okToSave = ETrue;
        	}
        	
        	if(okToSave)
        	{
				CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				ScheduleDB->ConstructL();
			
				if(newIttem->iIndex >= 0)
					ScheduleDB->UpdateDatabaseL(newIttem);
				else
					ScheduleDB->SaveToDatabaseL(newIttem);
			
				CleanupStack::PopAndDestroy(ScheduleDB);
        	}
        	
			CleanupStack::PopAndDestroy(newIttem);
			
			if(okToSave)
			{
				MakeProfileBoxL();
			}
			else
			{
				break;
			}
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
TBool CMainContainer::CheckMessageForUnicodeL(HBufC* aMessage, TBool& aSaveNow)
{
	TBool ret(EFalse);
	aSaveNow = ETrue;
	
	if(aMessage){
		
		TInt cunt(0);

		ConvertFromUnicodeL(aMessage->Des(),cunt);
		
		if(cunt)
		{
			ret = ETrue;
		
			if((aMessage->Des().Length() > 70))
			{	
				TBuf<200> Hjelppersmal;
				StringLoader::Load(Hjelppersmal,R_STR_MSGTRUNK);
						
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,Hjelppersmal))
				{
					aSaveNow = ETrue;
				}	
				else
				{
					aSaveNow = EFalse;
				}
			}
			else
			{
				aSaveNow = ETrue;
			}
		}
	}
	
	return ret;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::ConvertFromUnicodeL(const TDes& aFrom,TInt& aUncorvetible) 
{
	if(aFrom.Length())
	{
		HBufC8* hjelpper = HBufC8::NewLC(aFrom.Length());
		TPtr8 hjelpperPtr(hjelpper->Des());
	
	//	TRAP(Err,
		CCnvCharacterSetConverter* CSConverter = CCnvCharacterSetConverter::NewLC();
			
		TPtrC Remainder(aFrom);
	
		if (CSConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit,CEikonEnv::Static()->FsSession()) != CCnvCharacterSetConverter::EAvailable)
		{
			CSConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit,CEikonEnv::Static()->FsSession());
		}
			
		for(;;)
		{	
			TInt unConv(0);
			const TInt returnValue = CSConverter->ConvertFromUnicode(hjelpperPtr,Remainder,unConv);
			
			aUncorvetible = aUncorvetible + unConv;
			
			if (returnValue <= 0) // < error
			{
				break;
			}
			Remainder.Set(Remainder.Right(returnValue));
		}

		CleanupStack::PopAndDestroy(2);//hjelpper,CSConverter
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
		TBool Dim(ETrue),DimEnable(ETrue);
	
		if(iProfileBox)
		{
			TInt CurrInd = iProfileBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dim = EFalse;
			}
		
			if(CurrInd >= 0 && CurrInd < iItemArray.Count())
			{
				if(iItemArray[CurrInd])
				{
					DimEnable = iItemArray[CurrInd]->iEnabled;
				}
			}
		}

#ifdef SONE_VERSION
    if(!KAppIsTrial){
        aMenuPane->SetItemDimmed(EVisitDrJukka,ETrue);
    } 
#else

#endif   		
		
		if(Dim)
		{
			aMenuPane->SetItemDimmed(EProfRemove,ETrue);
			aMenuPane->SetItemDimmed(EProfModify,ETrue);
			aMenuPane->SetItemDimmed(EProfEnable,ETrue);
			aMenuPane->SetItemDimmed(EProfDisable,ETrue);
		}
		else
		{
			if(DimEnable)
				aMenuPane->SetItemDimmed(EProfEnable,ETrue);
			else
				aMenuPane->SetItemDimmed(EProfDisable,ETrue);
		}
	}
	
	if(iProfileSettings)
	{
		iProfileSettings->InitMenuPanelL(aResourceId,aMenuPane);
	}
}

		

