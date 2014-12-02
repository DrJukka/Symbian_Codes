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
#include <AknGlobalNote.h>
#include <eikclbd.h>

#include "Messages_Container.h"                     // own definitions

#include "YApps_E8876012.hrh"                     // own definitions
#include "YApps_E8876012.h"  

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
----------------------------------------------------------------------------
*/
CMessagesContainer::CMessagesContainer(CEikButtonGroupContainer* aCba):iCba(aCba)
{		

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CMessagesContainer::~CMessagesContainer()
{  	
	iItemArray.ResetAndDestroy();
    delete iMyListBox;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMessagesContainer::ConstructL()
{
	CreateWindowL();	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		
	ActivateL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMessagesContainer::SizeChanged()
{
	MakeListBoxL();	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMessagesContainer::MakeListBoxL()
{
	TInt MySetIndex(0);
	
	if(iMyListBox)
	{
		MySetIndex = iMyListBox->CurrentItemIndex();
	}
    
	delete iMyListBox;
	iMyListBox = NULL;

    iMyListBox   = new( ELeave ) CAknSingleNumberStyleListBox();
	iMyListBox->ConstructL(this,EAknListBoxSelectionList);

	iMyListBox->Model()->SetItemTextArray(GetArrayL());
   	iMyListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
	iMyListBox->CreateScrollBarFrameL( ETrue );
    iMyListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iMyListBox->SetRect(Rect());
	
//	iMyListBox->View()->SetListEmptyTextL(KtxNoData);

	iMyListBox->ActivateL();

	TInt ItemsCount = iMyListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iMyListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iMyListBox->SetCurrentItemIndex(0);
		
	UpdateScrollBar(iMyListBox);
	iMyListBox->DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CDesCArray* CMessagesContainer::GetArrayL(void)
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
    
    for(TInt i=0; i < iItemArray.Count(); i++)
    {
    	if(iItemArray[i])
    	{
			Bufff.Num(i + 1);
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
    
	// Append Text string to MyArray in here as "1\tMytext", where 1 is the zero based icon index
	// also remember to load and add icons in here... listbox will automatically re-size icons
	// thus no need to set any size for them.
	
	CleanupStack::Pop(MyArray);
	return MyArray;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMessagesContainer::UpdateScrollBar(CEikListBox* aListBox)
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
----------------------------------------------------------------------------
*/
void CMessagesContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(Rect());
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMessagesContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	
	
	if(iMyListBox)
	{
		Ret = iMyListBox->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMessagesContainer::GetMessageL(TDes& aMessage)
{
	if(iMyListBox)
	{
		TInt CurrInd = iMyListBox->CurrentItemIndex();
		if(CurrInd >= 0 && CurrInd < iItemArray.Count())
		{
			if(iItemArray[CurrInd])
		    {
				if(iItemArray[CurrInd]->iMessage)
				{
					aMessage.Copy(*iItemArray[CurrInd]->iMessage);
				}
		    }
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMessagesContainer::HandleViewCommandL(TInt aCommand)
{
	TBuf<255> newMessage;
	TBuf<60> Hjelpper;
	
	switch(aCommand)
	{
	case EMsgStoreNew:
		{
			StringLoader::Load(Hjelpper,R_STR_MESSAGE);
			CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(newMessage,CAknQueryDialog::ENoTone);
			Dialog->PrepareLC(R_ASK_NAME2_DIALOG);
			Dialog->SetPromptL(Hjelpper);
			if(Dialog->RunLD())
			{
				CMsgStored* newItem= new(ELeave)CMsgStored();
				CleanupStack::PushL(newItem);
				
				newItem->iIndex = -1;
				newItem->iMessage = newMessage.AllocL();	
			
				CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				ScheduleDB->ConstructL();
			  	ScheduleDB->SaveToDatabaseL(newItem);
			  	
			    CleanupStack::PopAndDestroy(2);//ScheduleDB, newItem
			
				MakeListBoxL();	
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EMsgStoreModify:
		if(iMyListBox)
		{
			TInt CurrInd = iMyListBox->CurrentItemIndex();
			if(CurrInd >= 0 && CurrInd < iItemArray.Count())
			{
				if(iItemArray[CurrInd])
			    {
					if(iItemArray[CurrInd]->iMessage)
					{
						newMessage.Copy(*iItemArray[CurrInd]->iMessage);
					}
					
					StringLoader::Load(Hjelpper,R_STR_MESSAGE);
					CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(newMessage,CAknQueryDialog::ENoTone);
					Dialog->PrepareLC(R_ASK_NAME2_DIALOG);
					Dialog->SetPromptL(Hjelpper);
					if(Dialog->RunLD())
					{
						delete iItemArray[CurrInd]->iMessage;
						iItemArray[CurrInd]->iMessage = NULL;
						iItemArray[CurrInd]->iMessage = newMessage.AllocL();	
								
						CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
						CleanupStack::PushL(ScheduleDB);
						ScheduleDB->ConstructL();
					  	ScheduleDB->UpdateDatabaseL(iItemArray[CurrInd]);
								  	
					    CleanupStack::PopAndDestroy(ScheduleDB);//
								
						MakeListBoxL();	
					}
			    }
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EMsgStoreDelete:
		if(iMyListBox)
		{
			TInt CurrInd = iMyListBox->CurrentItemIndex();
			if(CurrInd >= 0 && CurrInd < iItemArray.Count())
			{
				if(iItemArray[CurrInd])
				{
					StringLoader::Load(Hjelpper,R_STR_REMMESSAGE);
		
					CAknQueryDialog* dlg = CAknQueryDialog::NewL();
					if(dlg->ExecuteLD(R_QUERY,Hjelpper))
					{
						CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
						CleanupStack::PushL(ScheduleDB);
						ScheduleDB->ConstructL();
						ScheduleDB->DeleteFromDatabaseL(iItemArray[CurrInd]->iIndex,ETrue);
													  	
						CleanupStack::PopAndDestroy(ScheduleDB);//
					
						MakeListBoxL();	
					}
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	default:
		break;
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CCoeControl* CMessagesContainer::ComponentControl( TInt /*aIndex*/) const
{
	return iMyListBox;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TInt CMessagesContainer::CountComponentControls() const
{	
	if(iMyListBox)
		return 1;
	else
		return 0;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMessagesContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();
	
	TInt MenuRes(R_MSGS_MENUBAR);
	TInt ButtomRes(R_MSGS_CBA);
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(MenuRes);
		
	if(iCba)
	{
		iCba->SetCommandSetL(ButtomRes);
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMessagesContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MSGS_MENU)
	{
		TBool Dim(ETrue);
	
		if(iMyListBox)
		{
			TInt CurrInd = iMyListBox->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dim = EFalse;
			}
		}
		
		if(Dim)
		{
			aMenuPane->SetItemDimmed(EMsgStoreOk,ETrue);
			aMenuPane->SetItemDimmed(EMsgStoreModify,ETrue);
			aMenuPane->SetItemDimmed(EMsgStoreDelete,ETrue);
		}
	}
}

