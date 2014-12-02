/* Copyright (c) 2001 - 2005 , Solution One Telecom LTd. All rights reserved */

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
#include <APGCLI.H>
#include <StringLoader.h>

#include "Main_Container.h"                     // own definitions
#include "ThemeDataBase.h"
#include "commonstrings.h"



#include "S125AniLauncher.h"                     // own definitions
#include "S125AniLauncher.hrh"                   // own resource header
#include <S125AniLauncher_200027A9.rsg>



/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(CEikButtonGroupContainer* aCba,TUint aRegStatus,TInt aSpeed)
:iCba(aCba),iRegStatus(aRegStatus),iSpeed(aSpeed)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{    
	delete iPreviewContainer;
	iPreviewContainer = NULL;
	 
    delete iProfileBox;
    
    iItemArray.Reset();
//    iItemArray.Close();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(void)
{
	CreateWindowL();

	GetCurrentSkinIdL();
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		
	ActivateL();
	SetMenuL();
	DrawNow();

	TInt MySetIndex(-1);
	if(iProfileBox)
	{
		MySetIndex = iProfileBox->CurrentItemIndex();
	}
	
	if(MySetIndex < 0)
	{
		STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->HandleCommandL(ESettingBack);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	MakeProfileBoxL();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandleResourceChange(TInt aType)
{
	TRect rect;
	TBool SetR(EFalse);
	
#ifdef __SERIES60_3X__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
    }
#else
    if ( aType == 0x101F8121 )
    {
    	SetR = ETrue;
    	rect = CEikonEnv::Static()->EikAppUi()->ClientRect();
    }
#endif   
 
 	if(SetR)
 	{
 		if(iPreviewContainer)
 		{
 			iPreviewContainer->SetRect(rect);
 		}
 		
	 	if(iThemeSettings)
	    {
	    	iThemeSettings->SetRect(rect);
	    }
	    
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
TBool CMainContainer::SelectTimesL(TInt& aTimes)
{
	TBool Ret(EFalse);
	
	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	CleanupStack::PushL(popupList);

	list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
	list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

	CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(Array);

	Array->AppendL(_L("every 12 hours"));
	Array->AppendL(_L("every 8 hours"));
	Array->AppendL(_L("every 6 hours"));
	Array->AppendL(_L("every 4 hours"));
	
	list->Model()->SetItemTextArray(Array);
	CleanupStack::Pop();//Array
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	TBuf<60> TmpBuffer(_L("Change theme"));
//	StringLoader::Load(TmpBuffer,R_SH_STR_SELTHEME);
	
	popupList->SetTitleL(TmpBuffer);
	if (popupList->ExecuteLD())
    {
    	Ret = ETrue;
    	
		switch(list->CurrentItemIndex())
		{
		case 1:
			aTimes = 3;
			break;
		case 2:
			aTimes = 4;
			break;
		case 3:
			aTimes = 6;
			break;
		default:
			aTimes = 2;
			break;
		}
		
	}

	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1);  // list

	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMainContainer::SelectStartTimeL(TInt& aHour)
{
	TBool Ret(EFalse);
	
	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	CleanupStack::PushL(popupList);

	list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
	list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

	CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(Array);

	Array->AppendL(_L("6 am"));
	Array->AppendL(_L("8 am"));
	Array->AppendL(_L("10 am"));
	Array->AppendL(_L("12 am"));
	
	list->Model()->SetItemTextArray(Array);
	CleanupStack::Pop();//Array
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	TBuf<60> TmpBuffer(_L("Start Time"));
//	StringLoader::Load(TmpBuffer,R_SH_STR_SELTHEME);
	
	popupList->SetTitleL(TmpBuffer);
	if (popupList->ExecuteLD())
    {
    	Ret = ETrue;
    	
		switch(list->CurrentItemIndex())
		{
		case 1:
			aHour = 8;
			break;
		case 2:
			aHour = 10;
			break;
		case 3:
			aHour = 12;
			break;
		default:
			aHour = 6;
			break;
		}
		
	}

	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1);  // list

	return Ret;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::SetThemesStuffL(CScheduleDB& aScheduleDB)
{
	TInt Times(0);
	TInt Hour(0);
	
	if(SelectTimesL(Times))
	{
		if(SelectStartTimeL(Hour))
		{
			if(Times > 0 && Hour > 0)
			{
				aScheduleDB.DeleteAllL();
				
				CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->SkinInfoArray();
				
				TInt MyIndex(-1);
				TTime ScheduleT(TDateTime(2001,TMonth(0),1,Hour,0,0,0));
				TInt BetweenHours = (24 / Times);
				
				TInt iii(0);
				for(TInt i=0; i < Times; i++)
				{
					if(iii >= skinInfoArray->Count())
					{
						iii = 0;
					}
					
					MyIndex = -1;
						
					ScheduleT = ScheduleT + TTimeIntervalHours(BetweenHours);
						
					aScheduleDB.SaveToDatabaseL((TUint32)skinInfoArray->At(iii)->PID().Uid ().iUid,ScheduleT,MyIndex);
						
					iii++;
				}
			}
		}
	}
}			
				
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CDesCArray* CMainContainer::GetProfilesArrayL(void)
{
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(MyArray);

	iItemArray.Reset();

	RArray<TThemeSched> TmpArray;
	
	CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
	CleanupStack::PushL(ScheduleDB);
	ScheduleDB->ConstructL();
	ScheduleDB->ReadDbItemsL(TmpArray);	
	
	if(!TmpArray.Count())
	{
		SetThemesStuffL(*ScheduleDB);
		ScheduleDB->ReadDbItemsL(TmpArray);	
	}
	
	CleanupStack::PopAndDestroy(ScheduleDB);

	if(TmpArray.Count())
	{
	   	TTime NextSchedule;
	    TInt NextIndex(0);
	    do
	    {	
	    	NextSchedule.HomeTime();
	    	NextIndex = 0;
	    	ResolveNextSchduleTimeL(NextSchedule,NextIndex,TmpArray);
	    	
	    	if(NextIndex < 0 && NextIndex >= TmpArray.Count())
	    	{
	    		NextIndex = 0;
	    	}
	    	
	    	if(NextIndex >= 0 && NextIndex < TmpArray.Count())
	    	{    		
	    		iItemArray.Append(TmpArray[NextIndex]);	
	    		TmpArray.Remove(NextIndex);
	    	}
	    	
	    	
	    }while(TmpArray.Count());
	   
	   	TmpArray.Reset();
	   
	    TBuf<250> Bufff;
	    TBuf<100> HjelpBuf1,HjelpBuf2;
	    
		for(TInt i=0; i < iItemArray.Count(); i++)
		{
			Bufff.Zero();
		
			STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->GetThemeNameL(iItemArray[i].iThemeId,HjelpBuf2);
			
			Bufff.Num(iItemArray[i].iTime.DateTime().Hour());
			Bufff.Append(_L(":"));
			if(iItemArray[i].iTime.DateTime().Minute() < 10)
			{
				Bufff.AppendNum(0);	
			}
			Bufff.AppendNum(iItemArray[i].iTime.DateTime().Minute());
			Bufff.Append(_L("\t"));
			Bufff.Append(HjelpBuf1);
			Bufff.Append(_L("\t"));
			Bufff.Append(HjelpBuf2);
			Bufff.Append(_L("\t"));

			
			MyArray->AppendL(Bufff);
		}
	}
	CleanupStack::Pop(MyArray);
	return MyArray;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
TBool CMainContainer::GetSetThemeAndCurrentL(TUint32& aCurrent,TUint32& aSet)
{
	TBool Ret(EFalse);
	
	if(iItemArray.Count())
	{
		aCurrent = (TUint32)iCurrentThemeId.Uid().iUid;
	
		TTime NextSchedule;
		NextSchedule.HomeTime();
		TInt NextIndex(0);
		ResolvePreviousSchduleTimeL(NextSchedule,NextIndex,iItemArray);
	
		if(NextIndex >=0 && NextIndex < iItemArray.Count())
		{
			Ret = ETrue;
			aSet = iItemArray[NextIndex].iThemeId;
		}
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::ResolvePreviousSchduleTimeL(TTime& aNextSchedule, TInt& aIndex,RArray<TThemeSched>& aItemArray)
{
	TBool SetAlready(EFalse);
	
	TTime NowTime;
	NowTime.HomeTime();
		
	TTimeIntervalMinutes Interval(0);
	TTimeIntervalSeconds SchedInterval1(0);
	TTimeIntervalSeconds SchedInterval2(0);
	
	for(TInt i=0; i < aItemArray.Count(); i++)
	{
		TTime ScheduleT(TDateTime(NowTime.DateTime().Year(),NowTime.DateTime().Month(),NowTime.DateTime().Day(),aItemArray[i].iTime.DateTime().Hour(),aItemArray[i].iTime.DateTime().Minute(),0,0));
		
		NowTime.MinutesFrom(ScheduleT,Interval);
		if(Interval.Int() <= 0)
		{
			ScheduleT = ScheduleT + TTimeIntervalDays(1);
		}
		
		if(!SetAlready)
		{
			SetAlready = ETrue;
			aNextSchedule = ScheduleT;
			aIndex = i;
		}
		else 
		{
			NowTime.SecondsFrom(ScheduleT,SchedInterval1);
			NowTime.SecondsFrom(aNextSchedule,SchedInterval2);
			
			if(SchedInterval2.Int() > SchedInterval1.Int())
			{
				aNextSchedule = ScheduleT;
				aIndex = i;	
			}
		}
	}	
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CMainContainer::ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aIndex,RArray<TThemeSched>& aItemArray)
{
	TBool SetAlready(EFalse);
	
	TTime NowTime;
	NowTime.HomeTime();
	
	NowTime = NowTime + TTimeIntervalSeconds(30);
	
	TTimeIntervalMinutes Interval(0);
	TTimeIntervalSeconds SchedInterval(0);
	
	for(TInt i=0; i < aItemArray.Count(); i++)
	{
		TTime ScheduleT(TDateTime(NowTime.DateTime().Year(),NowTime.DateTime().Month(),NowTime.DateTime().Day(),aItemArray[i].iTime.DateTime().Hour(),aItemArray[i].iTime.DateTime().Minute(),0,0));
		
		ScheduleT.MinutesFrom(NowTime,Interval);
		if(Interval.Int() <= 0)
		{
			ScheduleT = ScheduleT + TTimeIntervalDays(1);
		}
		
		if(!SetAlready)
		{
			SetAlready = ETrue;
			aNextSchedule = ScheduleT;
			aIndex = i;
		}
		else 
		{
			aNextSchedule.SecondsFrom(ScheduleT,SchedInterval);
		
			if(SchedInterval.Int() > 0)
			{
				aNextSchedule = ScheduleT;
				aIndex = i;	
			}
		}
	}	
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

	if(iPreviewContainer)
	{
		return iPreviewContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iThemeSettings)
	{
		return iThemeSettings->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iRegStatus != KInvalidVers)
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	    	HandleViewCommandL(EThemChnageThem);
			//CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
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
void CMainContainer::CheckChangesOnValues(TBool RegOk)
{
	if(!IsDrawerOn())
	{
		CApaCommandLine* cmdLine = CApaCommandLine::NewL();
		cmdLine->SetExecutableNameL(KtxAniDrawerFileName);
		cmdLine->SetCommandL(EApaCommandRun);

		RApaLsSession ls;
		if(KErrNone == ls.Connect())
		{
			ls.StartApp(*cmdLine);	
			ls.Close();
		}
				
		delete cmdLine;
		cmdLine = NULL;
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CMainContainer::IsDrawerOn(void)
{
	TBool Ret(EFalse);
	
	TFileName res;
	TFindProcess find;
  	while(find.Next(res) == KErrNone)
    {
    	RProcess ph;
	  	ph.Open(res);
	  	
	  	if(ph.SecureId() == 0x200027AC)
	  	{
	  		Ret = ETrue;
	  		break;
	  	}
	  	
	  	ph.Close();
    }
    
    return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetCurrentSkinIdL(void)
{
    TAknsPkgIDBuf pidBuf;            
    
    CRepository* repository = CRepository::NewL( KCRUidPersonalisation );                    
    repository->Get( KPslnActiveSkinUid, pidBuf );
    
    delete repository;
    repository = NULL;
    
    iCurrentThemeId.SetFromDesL( pidBuf );
    
//   STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->ShowNoteL(pidBuf);			
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
	
TBool CMainContainer::SetOriginalSkinOnL(TAknSkinSrvSkinPackageLocation aLocation)
{
	TBool Ret(EFalse);
	
	RAknsSrvSession AknsSrvSession = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->AknsSrvSession();
	
	CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = AknsSrvSession.EnumerateSkinPackagesL(aLocation);
	CleanupStack::PushL( skinInfoArray );
	
//	STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->ShowNoteL(_L("Startt"));
	
    for(TInt i=0; i < skinInfoArray->Count(); i++)
	{
		if(((TUint32)skinInfoArray->At(i)->PID().Uid().iUid)  == ((TUint32)iCurrentThemeId.iNumber))
		{
//			STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->ShowNoteL(_L("Set"));
			
			TAknsPkgID pkgId = skinInfoArray->At(i)->PID();
			
			AknsSrvSession.SetAllDefinitionSets( pkgId );	
			
			TAknsPkgIDBuf pidBuf;            
		    iCurrentThemeId.CopyToDes( pidBuf );
		    CRepository* repository = CRepository::NewL( KCRUidPersonalisation );                    

		    TInt retVal = repository->Set( KPslnActiveSkinUid, pidBuf );
		    retVal = repository->Set( KPslnActiveSkinLocation, aLocation );
		    // KPslnActiveSkinLocation has to be changed also
		    // if new skin resides on mmc whereas old one resided in phone mem
		    // ...and vice versa
		    
		    delete repository;
		    repository = NULL;
		    
//		    STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->ShowNoteL(pidBuf);
        }
	}

    skinInfoArray->ResetAndDestroy();        
	CleanupStack::PopAndDestroy(1); //skinInfoArray
	
	return Ret;
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
	
void CMainContainer::HandleViewCommandL(TInt aCommand)
{
	if(iRegStatus != KInvalidVers)
	{
		switch(aCommand)
		{
		case EThemNewSched:
			{
				CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
				CleanupStack::PushL(ScheduleDB);
				
				ScheduleDB->ConstructL();
				SetThemesStuffL(*ScheduleDB);
				
				CleanupStack::PopAndDestroy(ScheduleDB);
			
				MakeProfileBoxL();
			}
			SetMenuL();
			DrawNow();
			break;
		case EThemShowBack:
			{
				delete iPreviewContainer;
				iPreviewContainer = NULL;
				
				if(!SetOriginalSkinOnL(EAknsSrvPhone))
				{
					SetOriginalSkinOnL(EAknsSrvMMC);
				}
			}
			SetMenuL();
			DrawNow();
			break;
		case EThemShow:
			if(iProfileBox)
			{
				TInt Curr = iProfileBox->CurrentItemIndex();
				if(Curr >= 0 && Curr < iItemArray.Count())
				{
					iPreviewContainer = new(ELeave)CPreviewContainer(iSpeed,iItemArray[Curr].iThemeId);
					iPreviewContainer->ConstructL(CEikonEnv::Static()->EikAppUi()->ClientRect());
				}
			}
			SetMenuL();
			DrawNow();
			break;
		case EThemChnageThem:
			if(iProfileBox)
			{
				TInt Curr = iProfileBox->CurrentItemIndex();
				if(Curr >= 0 && Curr < iItemArray.Count())
				{
					TUint32 ThemeId(0);
	        		if(SelectThemeL(ThemeId))
	        		{
			        	CScheduleDB* ScheduleDB = new(ELeave)CScheduleDB();
						CleanupStack::PushL(ScheduleDB);
						ScheduleDB->ConstructL();
					 	   		
						ScheduleDB->UpdateDatabaseL(ThemeId,iItemArray[Curr].iTime,iItemArray[Curr].iIndex);
	        
						CleanupStack::PopAndDestroy(ScheduleDB);
					
						MakeProfileBoxL();
	        		}
				}
			}
			break;
		default:

			break;
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMainContainer::SelectThemeL(TUint32& aThemeId)
{
	TBool Ret(EFalse);
	
	CArrayPtr< CAknsSrvSkinInformationPkg >* skinInfoArray = STATIC_CAST(CMgAppUi*,CEikonEnv::Static()->EikAppUi())->SkinInfoArray();

	CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
	CleanupStack::PushL(list);

	CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_OK_CANCEL);
	CleanupStack::PushL(popupList);

	list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	list->CreateScrollBarFrameL(ETrue);
	list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);

	CDesCArrayFlat* Array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL(Array);
	
    for(TInt i=0; i < skinInfoArray->Count(); i++)
    {
		Array->AppendL(skinInfoArray->At( i )->Name());
	}
	
	list->Model()->SetItemTextArray(Array);
	CleanupStack::Pop();//Array
	list->Model()->SetOwnershipType(ELbmOwnsItemArray);

	TBuf<60> TmpBuffer;
	StringLoader::Load(TmpBuffer,R_SH_STR_SELTHEME);
	
	popupList->SetTitleL(TmpBuffer);
	if (popupList->ExecuteLD())
    {
		TInt Selected = list->CurrentItemIndex();
		if(Selected >= 0 && Selected < skinInfoArray->Count())
		{
			aThemeId = skinInfoArray->At(Selected)->PID().Uid ().iUid;
			Ret = ETrue;
		}
	}

	CleanupStack::Pop();            // popuplist
	CleanupStack::PopAndDestroy(1);  // list

	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl( TInt /*aIndex*/) const
{
	CCoeControl* RetCont(NULL);
	
	if(iPreviewContainer)
	{
		RetCont = iPreviewContainer;
	}
	else if(iThemeSettings)
	{
		RetCont = iThemeSettings;
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
	
	if(iPreviewContainer)
	{
		RetCount = 1;
	}
	else if(iThemeSettings)
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
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();

	if(iPreviewContainer)
	{
		if(iCba)
		{
			iCba->SetCommandSetL(R_AUTO_VIEW_CBA);
			iCba->DrawDeferred();
		}
	}
	else if(iThemeSettings)
	{
		iThemeSettings->SetMenuL();
	}
	else
	{
		TInt MenuRes(R_AUTO_MENUBAR);
		TInt ButtomRes(R_AUTO_CBA);
	
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
	if(aResourceId == R_AUTO_MENU)
	{
		TBool Dim(ETrue);
	
		if(iProfileBox)
		{
			if(iProfileBox->CurrentItemIndex() >= 0)
			{
				Dim = EFalse;
			}
		}
	
		if(Dim)
		{
			aMenuPane->SetItemDimmed(EThemShow,ETrue);
			aMenuPane->SetItemDimmed(EThemChnageThem,ETrue);
		}
	}
}
