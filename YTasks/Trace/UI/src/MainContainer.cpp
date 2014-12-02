/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

// INCLUDE FILES
#include <avkon.hrh>
#include <APPARC.H>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <BAUTILS.H> 
#include <ecom.h>
#include <implementationproxy.h>
#include <aknmessagequerydialog.h> 
#include <akniconarray.h> 
#include <akntitle.h> 
#include <akncontext.h> 
#include <TXTRICH.H>
#include <CHARCONV.H>
#include <eikmenub.h> 
#include <aknglobalconfirmationquery.h> 
#include "MainContainer.h"

#include "MemoryControl.h"
#include "CPUControl.h"
#include "Profile_Settings.h"

#include "ExampleServerSession.h"
#include "Help_Container.h"
#include "YTools_A0003129.hrh"

#include "Common.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A4_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A0003129_res.rsg"
	#else
		#include "YTools_A0003129_res.rsg"
	#endif
#endif
	

const TInt KAknExListFindBoxTextLength = 20;
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CTraceContainer::~CTraceContainer()
{
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	
	iArray.ResetAndDestroy();

	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	    
	delete iProfileSettings;
	iProfileSettings = NULL;
	
	delete iCPUControl;
	iCPUControl = NULL;

	delete iMemControl;
	iMemControl = NULL;
	
	delete iBgContext;
	iBgContext = NULL;
	
	delete iMaskImg;
	iMaskImg = NULL;
	
	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}


/*
-------------------------------------------------------------------------
This will be called when this file shandler is paped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CTraceContainer::ConstructL(CEikButtonGroupContainer* aCba)
{
	iCba = aCba;
	
	TMemoryInfoV1Buf   memInfoBuf;
	UserHal::MemoryInfo(memInfoBuf);
	iTotalRamInBytes = (memInfoBuf().iTotalRamInBytes / 1000);
	
 	CreateWindowL();
 	
 	iServerIsOn = IsServerOn();
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KTraceResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	TFindFile ImgFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == ImgFile.FindByDir(KTraceIconsFileName, KNullDesC))
	{
		TRect ThisRect(Rect());
		ThisRect.Shrink(4,4);
		TInt HeightMe = (ThisRect.Height() / 2);
		TSize MskImgSiz(ThisRect.Width(),HeightMe);
		
		iMaskImg= LoadImageL(ImgFile.File(), 4, 5,MskImgSiz,EAspectRatioNotPreserved);
	}
		
	ActivateL();
	SetMenuL();
	DrawNow();
	
	iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal,*this);
	
	ChangeIconAndNameL();
	
	if(iServerIsOn)
	{
		ForegroundChangedL(ETrue);
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CTraceContainer::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aAspect)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	TInt MaskId(aMsk);
	TInt UseIndex(aImg);
			
	if(AknIconUtils::IsMifFile(aFileName))
	{
		AknIconUtils::ValidateLogicalAppIconId (aFileName, UseIndex, MaskId); 
	}
	
	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, UseIndex, MaskId);

	if(aAspect)
	{
		AknIconUtils::SetSize(FrameImg,aSize,aAspect);  
		AknIconUtils::SetSize(FrameMsk,aSize,aAspect); 
	}
	else
	{
		AknIconUtils::SetSize(FrameImg,aSize,aAspect);  
		AknIconUtils::SetSize(FrameMsk,aSize,aAspect); 	
	}
	
	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTraceContainer::ChangeIconAndNameL(void)
{	
 	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(KTraceApplicationName);
			
    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KTraceIconsFileName, KNullDesC))
			{
				TInt BgIndex(0),BgMask(1);
				
				if(iServerIsOn)
				{
					BgIndex = 2;
					BgMask = 3;
				}
				
				if(AknIconUtils::IsMifFile(AppFile.File()))
				{
					AknIconUtils::ValidateLogicalAppIconId(AppFile.File(), BgIndex,BgMask); 
				}
			
				ContextPane->SetPictureFromFileL(AppFile.File(),BgIndex,BgMask);  
			}
		}				
	}	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CTraceContainer::IsServerOn()
{
	TBool Ret(EFalse);
	
	TFileName res;
	TFindProcess find;
  	while(find.Next(res) == KErrNone)
    {
    	RProcess ph;
	  	ph.Open(res);
	  	  			
	  	if(ph.SecureId() == (TUint32)KUidTraceServerUID.iUid)
	 	{
	 		TExitType Exxit =ph.ExitType();
	 		if(Exxit == EExitPending)
	 		{
		  		Ret = ETrue;
		  		break;
	 		}
	  	}
	  	
	  	ph.Close();
    }
 
    return Ret;
}

/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTraceContainer::AddItemListL(TExampleItem& aItem)
{
	CExampleItem* NewIttem = new(ELeave)CExampleItem();
	
	NewIttem->iCpuLoad = aItem.iCpuLoad;
    NewIttem->iMemory  = aItem.iMemory;
    NewIttem->iTime    = aItem.iTime;
	
	iArray.Append(NewIttem);
}

/*
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CTraceContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	if(iProfileSettings)
	{
		iProfileSettings->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	//set rects...
	TRect ThisRect(Rect());
	ThisRect.Shrink(4,4);
	TInt HeightMe = (ThisRect.Height() / 2);
	
	if(iMemControl)
	{
		iMemControl->SetRect(TRect(ThisRect.iTl.iX,ThisRect.iTl.iY,ThisRect.iBr.iX,(ThisRect.iTl.iY + (HeightMe - 1))));
	}
	
	if(iCPUControl)
	{
		iCPUControl->SetRect(TRect(ThisRect.iTl.iX,(ThisRect.iTl.iY + (HeightMe + 1)),ThisRect.iBr.iX,ThisRect.iBr.iY));
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
void CTraceContainer::HandleResourceChange(TInt aType)
{ 		
	TRect rect;
	TBool SetR(EFalse);
	
#ifdef __SERIES60_3X__
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	SetR = ETrue;
    	
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPaneBottom, Brect);
//		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane, Srect);
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
	    SetRect(rect);
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CTraceContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CTraceContainer::SetMenuL(void)
{
	if(iMyHelpContainer)
	{
		iCba->SetCommandSetL(R_APPHELP_CBA);
		iCba->DrawDeferred();
	}
	else if(iProfileSettings)
	{
		iCba->SetCommandSetL(R_SETTINGS_CBA);
		iCba->DrawDeferred();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MAIN_MENUBAR);				
		iCba->SetCommandSetL(R_MAIN_CBA);
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTraceContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		aMenuPane->SetItemDimmed(EItemInfo,ETrue);
		aMenuPane->SetItemDimmed(EResetMe,ETrue);
		
		if(iArray.Count() < 1)
		{
			aMenuPane->SetItemDimmed(EClearMe,ETrue);
		}
		
		if(iServerIsOn)
		{
			aMenuPane->SetItemDimmed(ESetMeOn,ETrue);
		}
		else
		{
			aMenuPane->SetItemDimmed(ESetMeOff,ETrue);
		}
	}
}

/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CTraceContainer::ForegroundChangedL(TBool aForeground)
{
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}

	if(aForeground)
	{	
		if(iTimeOutTimer == NULL)
		{
			iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal,*this);
		}

		iTimeOutTimer->After(1000000);
	}
	else
	{
		
	}
}
/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CTraceContainer::GetInfoBufferL(TExampleItem& aItem,TDes& aBuffer)
{
	aBuffer.Zero();

	
	aBuffer.Append(_L("\n"));
	aBuffer.Append(_L("Time:\n"));
	aBuffer.AppendNum(aItem.iTime.DateTime().Hour());
	aBuffer.Append(_L(":"));
	if(aItem.iTime.DateTime().Minute() < 10)
	{
		aBuffer.AppendNum(0);	
	}
	aBuffer.AppendNum(aItem.iTime.DateTime().Minute());
	aBuffer.Append(_L(":"));
	if(aItem.iTime.DateTime().Second() < 10)
	{
		aBuffer.AppendNum(0);	
	}
	aBuffer.AppendNum(aItem.iTime.DateTime().Second());
	
	aBuffer.Append(_L(" - "));

	if(aItem.iTime.DateTime().Day() < 9)
	{
		aBuffer.AppendNum(0);	
	}
	aBuffer.AppendNum(aItem.iTime.DateTime().Day() + 1);
	aBuffer.Append(_L("/"));
	
	TMonthNameAbb ThisMonth(aItem.iTime.DateTime().Month());
	aBuffer.Append(ThisMonth);
	aBuffer.Append(_L("/"));
	
	TInt Yearrr = aItem.iTime.DateTime().Year();
	TInt Hudreds = Yearrr / 100;
	
	Yearrr = (Yearrr - (Hudreds * 100));
	if(Yearrr < 10)
	{
		aBuffer.AppendNum(0);	
	}
	
	aBuffer.AppendNum(Yearrr);
	aBuffer.Append(_L("\n"));
}

/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CTraceContainer::TimerExpired()
{	
	RExampleServerClient CrashClient;
	CrashClient.Connect();

	CrashClient.GetCPUMax(iMaxCpuCount);

	TExampleItem ItemBuffer;

	TFileName Bufferrr;
	TInt ItemCount(0);
	CrashClient.GetItemCount(ItemCount);
	
	TInt GotCunt = iArray.Count();
	
	for(TInt i = (GotCunt - 1); i < ItemCount; i++)
	{
		ItemBuffer.iIndex = i;
		CrashClient.GetItemL(ItemBuffer);	
		AddItemListL(ItemBuffer);
	}

	if(iServerIsOn)
	{
		iTimeOutTimer->After(1000000);
	}

	CrashClient.Close();
	DrawNow();
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CTraceContainer::HandleCommandL(TInt aCommand)
{	
	switch (aCommand)
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
    case EItemInfo:
    	{
    /*		RExampleServerClient CrashClient;
			CrashClient.Connect();
			
			TExampleItem ItemBuffer;
			
			TFileName Bufferrr;
			TInt ItemCount(0);
			CrashClient.GetItemCount(ItemCount);
			for(TInt i=0; i < ItemCount; i++)
			{
				ItemBuffer.iIndex = i;
				CrashClient.GetItemL(ItemBuffer);	
				
				AddItemListL(ItemBuffer);
			}

			if(!iServerIsOn)
			{
				CrashClient.ShutServerDownNow();
			}

			CrashClient.Close();*/
    	}
    
    		
/*		if(iSelectionBox)
		{
			TInt CurrInd = iSelectionBox->CurrentItemIndex();
			if(CurrInd >= 0 && CurrInd <= iArray.Count())
			{
				if(iArray[CurrInd])
				{
		    		HBufC* Informmm = HBufC::NewLC(3000);
		    		
		    		TPtr Pointter(Informmm->Des());
		    		GetInfoBufferL(*iArray[CurrInd],Pointter);
		    		
					TPtr NewPointter(Informmm->Des());
					CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(NewPointter);
					dlg->PrepareLC(R_ABOUT_HEADING_PANE);
					dlg->SetHeaderTextL(KTraceApplicationName);  
					dlg->RunLD();
					
					CleanupStack::PopAndDestroy(Informmm);
		    	}
			}
		}*/
    	break;
    case ESettingsMe:
    	{
			iProfileSettings = new(ELeave)CProfileSettings();
			iProfileSettings->ConstructL();
    	}
    	SetMenuL();
    	DrawNow();
    	break;
    case ESettingsOk:
    	{
			if(iProfileSettings)
			{
				iProfileSettings->SaveValuesL();
				
			//	if(iServerIsOn)
				{
					RExampleServerClient CrashClient;
					CrashClient.Connect();
					CrashClient.ReReadSettings();
					CrashClient.Close();
				}
			}
    	
			delete iProfileSettings;
			iProfileSettings = NULL;
    	}
    	SetMenuL();
    	DrawNow();
    	break;
    case ESetMeOn:
    	{
    		iServerIsOn = ETrue;
    		RExampleServerClient CrashClient;
			CrashClient.Connect();
			
			CrashClient.StartTrace();
			
			CrashClient.Close();
			
			ChangeIconAndNameL();
			
			if(iTimeOutTimer)
			{
				iTimeOutTimer->Cancel();
			}
			else
			{
				iTimeOutTimer = CTimeOutTimer::NewL(EPriorityNormal,*this);
			}

			iTimeOutTimer->After(1000000);
    	}
    	break;
	case ESetMeOff:
		{
			iServerIsOn = EFalse;
    		RExampleServerClient CrashClient;
			CrashClient.Connect();
			CrashClient.ShutServerDownNow();
			CrashClient.Close();
			
			ChangeIconAndNameL();
			
			if(iTimeOutTimer)
			{
				iTimeOutTimer->Cancel();
			}
    	}
    	break;
	case EClearMe:
		{
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(dlg->ExecuteLD(R_QUERY,_L("Clear list")))
			{
	    		RExampleServerClient CrashClient;
				CrashClient.Connect();
				CrashClient.ClearItemList();

				if(!iServerIsOn)
				{
					CrashClient.ShutServerDownNow();
				}
				else
				{	// re-start the trace
					CrashClient.StopTrace();
					CrashClient.StartTrace();
				}

				CrashClient.Close();
				
				iArray.ResetAndDestroy();
				// refresh info
			}
    	}
    	break;
	case EResetMe:
		{

				
    		// refresh info
    	}
    	break;
    case EAbout:
    	{
    		HBufC* Abbout = KTraceAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KTraceApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYTasks:
    	if(iTasksHandlerExit)
    	{
    		if(!iServerIsOn)
    		{
    			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,_L("Set Trace on")))
				{
					RExampleServerClient CrashClient;
					CrashClient.Connect();
					CrashClient.Close();
				}
    		}
    	
    		iTasksHandlerExit->HandlerExitL(this);	
    	}
    	break; 
	default:
		break;
    };
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTraceContainer::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalConfirmationQuery* pQ = CAknGlobalConfirmationQuery::NewL();
	CleanupStack::PushL(pQ);

	TRequestStatus theStat = KRequestPending;
	pQ->ShowConfirmationQueryL(theStat,aMessage, R_AVKON_SOFTKEYS_OK_CANCEL);
	User::WaitForRequest(theStat);
   
	CleanupStack::PopAndDestroy(pQ);
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CTraceContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iProfileSettings)
	{
		Ret = iProfileSettings->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		switch (aKeyEvent.iCode)
		{
		case EKeyDevice3:
			; 
			break;
		case EKeyLeftArrow:
	
			break;
		case EKeyRightArrow:
	
			break;
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			break;
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CTraceContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	
	if(iBgContext)
	{ 	
	  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
	
	TRect DrawRect(Rect());
	DrawRect.Shrink(2,2);
	gc.SetPenColor(KRgbBlack);
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY)); 
	
	DrawRect.Shrink(2,2);

	TInt HeightMe = (DrawRect.Height() / 2);
	
	TRect CpuRect(DrawRect.iTl.iX,DrawRect.iTl.iY,DrawRect.iBr.iX,(DrawRect.iTl.iY + (HeightMe - 1)));
	TRect MemRect(DrawRect.iTl.iX,(DrawRect.iTl.iY + (HeightMe + 1)),DrawRect.iBr.iX,DrawRect.iBr.iY);

	DrawCPU(CpuRect,gc);
	DrawMemory(MemRect,gc);
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CTraceContainer::DrawCPU(const TRect& aRect, CWindowGc& gc) const
{
	if(iMaskImg)
	{
		if(iMaskImg->Bitmap() && iMaskImg->Mask())
		{
			if(iMaskImg->Bitmap()->Handle() && iMaskImg->Mask()->Handle())
			{
				TSize ButtonSiz(iMaskImg->Bitmap()->SizeInPixels());
				gc.DrawBitmapMasked(aRect,iMaskImg->Bitmap(),TRect(0,0,ButtonSiz.iWidth,ButtonSiz.iHeight),iMaskImg->Mask(),EFalse);
			}
		}
	}
	
	gc.SetPenColor(KRgbBlack);
	gc.DrawLine(aRect.iTl,TPoint(aRect.iTl.iX,aRect.iBr.iY));
	gc.DrawLine(aRect.iTl,TPoint(aRect.iBr.iX,aRect.iTl.iY));
	gc.DrawLine(aRect.iBr,TPoint(aRect.iTl.iX,aRect.iBr.iY));
	gc.DrawLine(aRect.iBr,TPoint(aRect.iBr.iX,aRect.iTl.iY)); 

	TInt Prosentages(0);

	if(iArray.Count() && iMaxCpuCount)
	{
		if(iArray[iArray.Count() - 1])
		{
			Prosentages = ((iArray[iArray.Count() - 1]->iCpuLoad * 100) / iMaxCpuCount);
			Prosentages = (100 - Prosentages);
		}
	}
	
	TInt SizPie = (aRect.Height() / 3);
	//TPoint(aRect.iTl.iX, (aRect.iBr.iY - SizPie))
	PieDrawer(gc,aRect.iTl,SizPie,Prosentages,ETrue);
	
	gc.SetPenColor(KRgbRed);
	
	TInt StartX(0);
	
	if(iArray.Count() > aRect.Width())
	{
		StartX = (iArray.Count() - aRect.Width());
	}

	TPoint MePoint1(aRect.iTl.iX,aRect.iBr.iY);
	TPoint MePoint2(aRect.iTl);
	
	TInt Valll(0);
	
	if(iMaxCpuCount > 0)
	{
		for(TInt i = StartX; i < iArray.Count(); i++)
		{
			if(iArray[i])
			{
				TUint32 Curr = (iMaxCpuCount - iArray[i]->iCpuLoad);
				if(Curr < 0)
				{
					Curr = 0;
				}
				else if(iMaxCpuCount < Curr)
				{
					Curr = iMaxCpuCount;
				}
				
				Valll = ((aRect.Height() * Curr) / iMaxCpuCount);
			}
		
			MePoint2.iY = (aRect.iBr.iY - Valll);
			
			if(StartX != 1)
			{
				gc.DrawLine(MePoint1,MePoint2);
			}
		
			MePoint1 = MePoint2;
			MePoint2.iX = MePoint2.iX + 1;	
		}
	}
}

/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CTraceContainer::DrawMemory(const TRect& aRect, CWindowGc& gc) const
{
	if(iMaskImg)
	{
		if(iMaskImg->Bitmap() && iMaskImg->Mask())
		{
			if(iMaskImg->Bitmap()->Handle() && iMaskImg->Mask()->Handle())
			{
				TSize ButtonSiz(iMaskImg->Bitmap()->SizeInPixels());
				gc.DrawBitmapMasked(aRect,iMaskImg->Bitmap(),TRect(0,0,ButtonSiz.iWidth,ButtonSiz.iHeight),iMaskImg->Mask(),EFalse);
			}
		}
	}
	
	gc.SetPenColor(KRgbBlack);
	
	gc.DrawLine(aRect.iTl,TPoint(aRect.iTl.iX,aRect.iBr.iY));
	gc.DrawLine(aRect.iTl,TPoint(aRect.iBr.iX,aRect.iTl.iY));
	gc.DrawLine(aRect.iBr,TPoint(aRect.iTl.iX,aRect.iBr.iY));
	gc.DrawLine(aRect.iBr,TPoint(aRect.iBr.iX,aRect.iTl.iY)); 
	
	gc.SetPenColor(KRgbRed);
	
	TInt Prosentages(0);
	
	TBuf<100> FreeBuffer(_L("Free: "));
	TBuf<100> TotalBuffer(_L("Total: "));
	if(iArray.Count())
	{
		if(iArray[iArray.Count() - 1])
		{
			FreeBuffer.AppendNum(iArray[iArray.Count() - 1]->iMemory,EDecimal);
			TotalBuffer.AppendNum(iTotalRamInBytes,EDecimal);
			
			Prosentages = ((iArray[iArray.Count() - 1]->iMemory * 100) / iTotalRamInBytes);
		}
	}
	
	TInt SizPie = (aRect.Height() / 3);
	PieDrawer(gc,aRect.iTl,SizPie,Prosentages,EFalse);
	
	const CFont* MuFonr = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);

	TInt TextHeight= MuFonr->HeightInPixels();
	TInt StartX = (aRect.iTl.iX + SizPie);
	TInt StartY = (aRect.iTl.iY + 2);

	gc.SetPenColor(KRgbBlack);
	gc.UseFont(MuFonr);
	
	gc.DrawText(FreeBuffer,TRect(StartX,StartY,aRect.iBr.iX,StartY + TextHeight),MuFonr->AscentInPixels(), CGraphicsContext::ELeft, 0);			
	gc.DrawText(TotalBuffer,TRect(StartX,(StartY + TextHeight),aRect.iBr.iX,(StartY + (TextHeight * 2))),MuFonr->AscentInPixels(), CGraphicsContext::ELeft, 0);			

	gc.SetPenColor(KRgbRed);
	
	StartX = 0;
	
	if(iArray.Count() > aRect.Width())
	{
		StartX = (iArray.Count() - aRect.Width());
	}

	TPoint MePoint1(aRect.iTl.iX,aRect.iBr.iY);
	TPoint MePoint2(aRect.iTl);
	
	TInt Valll(0);
	
	for(TInt i = StartX; i < iArray.Count(); i++)
	{
		if(iArray[i] && iTotalRamInBytes > 0)
		{
			Valll = ((aRect.Height() * iArray[i]->iMemory) / iTotalRamInBytes);
		}
	
		MePoint2.iY = (aRect.iBr.iY - Valll);
		
		if(StartX != 1)
		{
			gc.DrawLine(MePoint1,MePoint2);
		}
		
		MePoint1 = MePoint2;
		MePoint2.iX = MePoint2.iX + 1;	
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTraceContainer::PieDrawer(CWindowGc& aGc, TPoint aLeftTop, TInt aSize, TInt aProsentages, const TBool& aReverceColor) const
{
	TBuf<50> Buffer;
	Buffer.Num(aProsentages);
	Buffer.Append(_L("%"));

	aGc.SetPenColor(KRgbBlack);
	
	if(aReverceColor)
		aGc.SetBrushColor(KRgbGreen);
	else
		aGc.SetBrushColor(KRgbRed);
	
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawEllipse(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize));

	if(aProsentages < 0)
	{
		Buffer.Copy(_L("N/A"));
		
		if(aReverceColor)
			aGc.SetBrushColor(KRgbGreen);
		else
			aGc.SetBrushColor(KRgbRed);
	}
	else
	{
		TInt HelpPie = 0;
		
		if(aReverceColor)
			aGc.SetBrushColor(KRgbRed);
		else
			aGc.SetBrushColor(KRgbGreen);

		TInt Angle =((aProsentages * 360) / 100);

		if(Angle == 360)
		{
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),aLeftTop,aLeftTop);
		}
		else if(Angle == 45)
		{
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX,aLeftTop.iY));
		}
		else if(Angle == 135)
		{
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX,aLeftTop.iY + aSize));
		}
		else if(Angle == 225)
		{
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX + aSize,aLeftTop.iY + aSize));
		}
		else if(Angle == 315)
		{
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX + aSize,aLeftTop.iY));
		}
		else if(Angle > 45 && Angle < 135)
		{				
			HelpPie = ((((Angle - 45) * aSize) / 90) + aLeftTop.iY);
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX,HelpPie));
		}
		else if(Angle > 135 && Angle < 225)
		{
			HelpPie = ((((Angle - 135) * aSize) / 90) + aLeftTop.iX);
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(HelpPie,aLeftTop.iY + aSize));
		}
		else if(Angle > 225 && Angle < 315)
		{
			HelpPie = (((aLeftTop.iY + aSize) - ((Angle - 225) * aSize) / 90));
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX + aSize,HelpPie));
		}
		else if(Angle > 0 && Angle < 45)
		{
			HelpPie = (((aLeftTop.iX + (aSize/2)) - ((Angle) * (aSize/2)) / 45));
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(HelpPie,aLeftTop.iY));
		}
		else if(Angle > 315 && Angle < 360)
		{
			HelpPie = (((aLeftTop.iX + aSize) - ((Angle - 315) * (aSize/2)) / 45));
			aGc.DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(HelpPie,aLeftTop.iY));
		}

		if(aReverceColor)
		{
			if(Angle > 180)
				aGc.SetBrushColor(KRgbGreen);
			else	
				aGc.SetBrushColor(KRgbRed);
		}
		else
		{
			if(Angle > 180)
				aGc.SetBrushColor(KRgbRed);
			else	
				aGc.SetBrushColor(KRgbGreen);
		}		
	}

	const CFont* MuFonr = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);

	TInt TextHeight= MuFonr->HeightInPixels();
	TInt TextWidth = MuFonr->TextWidthInPixels(Buffer);

	TInt StartX = (aLeftTop.iX + ((aSize - TextWidth) / 2));
	TInt StartY = (aLeftTop.iY + ((aSize - TextHeight) / 2));

	aGc.UseFont(MuFonr);
	aGc.SetPenColor(KRgbBlack);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	
	aGc.DrawText(Buffer,TRect(StartX,StartY,StartX + TextWidth,StartY + TextHeight),MuFonr->AscentInPixels(), CGraphicsContext::ELeft, 0);			

	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
TInt CTraceContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
	{
		return 1;
	}
	else if(iProfileSettings)
	{
		return 1;
	}
	else if(iCPUControl && iMemControl)
	{
		return 2;
	}
	else
	{
		return 0;
	}
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CTraceContainer::ComponentControl(TInt aIndex) const
{
	if(iMyHelpContainer)
	{
		return iMyHelpContainer;
	}
	else if(iProfileSettings)
	{
		return iProfileSettings;
	}
	else if(aIndex)
	{
		return iCPUControl;
	}
	else
	{
		return iMemControl;
	}
}

/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYTasksContainer* NewFileHandler()
    {
    return (new CTraceContainer);
    }
/*
-------------------------------------------------------------------------
ECom ImplementationTable function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
#ifdef __SERIES60_3X__
LOCAL_D const TImplementationProxy ImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY(0x3129,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x3129}, NewFileHandler}
    };
    
// DLL Entry point
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
    {
    return(KErrNone);
    }
#endif


/*
-------------------------------------------------------------------------
ECom ImplementationGroupProxy function

See SDK documentations for more information
-------------------------------------------------------------------------
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
	aTableCount = sizeof(ImplementationTable)/sizeof(ImplementationTable[0]);
	return ImplementationTable;
}

