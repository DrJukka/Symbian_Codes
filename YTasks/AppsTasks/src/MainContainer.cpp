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

#include "MainServerSession.h"
#include "MainContainer.h"

#include "YTools_A0000F60.hrh"
#include "TasksContainer.h"
#include "AppsContainer.h"
#include "Help_Container.h"

#include "Common.h"

#ifdef SONE_VERSION
	#include <YTools_2002B0A7_res.rsg>
#else
	#ifdef LAL_VERSION
		#include <YTools_A0000F60_res.rsg>
	#else
		#include <YTools_A0000F60_res.rsg>
	#endif
#endif



#include <eikstart.h>
	


const TInt KAknExListFindBoxTextLength = 20;
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{
	iTabGroup = NULL;
	
	if (!iNaviPane)
    {
        CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
        iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
    }

	if(iNaviPane)
	    iNaviPane->Pop(NULL); 
	
	iNaviPane = NULL;
	
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
		
    delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    
    delete iFirstControl;
    delete iSecondControl;
    
	delete iBgContext;
	iBgContext = NULL;
	
	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}


/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(CEikButtonGroupContainer* aCba)
{
	iCba = aCba;
	
 	CreateWindowL();
 	
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KAppTaResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	MakeNavipanelL();
	
	delete iFirstControl;
	iFirstControl = NULL;
	iFirstControl = CMainListView::NewL();
	
	ActivateL();
	SetMenuL();
	DrawNow();
	
	ChangeIconAndNameL();
	
	RMainServerClient MainServerClient;
	if(MainServerClient.Connect() == KErrNone)
	{
		iHasServer = ETrue;
		MainServerClient.Close();
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

_LIT(KtxTabText1			,"Apps.");
_LIT(KtxTabText2			,"Tasks");	
void CMainContainer::MakeNavipanelL(void)
{
	if(iTabGroup)
	{
		if (!iNaviPane)
	    {
	        CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	        iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	    }

		if(iNaviPane)
		    iNaviPane->Pop(NULL); 
		
		iNaviPane = NULL;
		
	    delete iNaviDecoratorForTabs;
	    iNaviDecoratorForTabs = NULL;
	    delete iNaviDecorator;
	    iNaviDecorator = NULL;
	}

	CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

	CAknNavigationDecorator* iNaviDecoratorForTabsTemp;
	iNaviDecoratorForTabsTemp = iNaviPane->CreateTabGroupL();
	delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
	iNaviDecoratorForTabs = iNaviDecoratorForTabsTemp;
   	iTabGroup = STATIC_CAST(CAknTabGroup*, iNaviDecoratorForTabs->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(KTabWidthWithTwoTabs);

	iTabGroup->AddTabL(0,KtxTabText1);
	iTabGroup->AddTabL(1,KtxTabText2);

	iTabGroup->SetActiveTabByIndex(0);
	iNaviPane->PushL(*iNaviDecoratorForTabs);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::ChangeIconAndNameL(void)
{	
 	CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	if(sp)
	{
		CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
		TitlePane->SetTextL(KAppTaApplicationName);
			
    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KAppTaIconsFileName, KNullDesC))
			{
				TInt BgIndex(0),BgMask(1);
				
			/*	if(iServerIsOn)
				{
					BgIndex = 2;
					BgMask = 3;
				}
			*/	
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
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
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

 		if(iFirstControl)
		{
			iFirstControl->SetRect(rect);
		}

		if(iSecondControl)
		{
			iSecondControl->SetRect(rect);
		}    
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CMainContainer::MopSupplyObject(TTypeUid aId)
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
void CMainContainer::ForegroundChangedL(TBool /*aForeground*/)
{
	
}
/*
-------------------------------------------------------------------------
if(iInfoView)
	{
		iCba.SetCommandSetL(R_INFO_CBA);
		iCba.DrawDeferred();
	}
-------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();

	TInt MenuRes(R_MAIN_MENUBAR);
	TInt ButtomRes(R_MAIN_CBA);

	if(iMyHelpContainer)
	{
		ButtomRes = R_APPHELP_CBA;
	}
	else if(iTabGroup)
	{
		switch(iTabGroup->ActiveTabIndex())
		{
		case 1:
			MenuRes = R_MAIN_MENUBAR;
			ButtomRes = R_MAIN_CBA;
			break;
		case 2:
			MenuRes = R_MAIN_MENUBAR;
			ButtomRes = R_MAIN_CBA;
			break;
		default:
			//R_MAIN_MENUBAR
			break;
		}
	}
	
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
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
		if(iSecondControl)
		{
			aMenuPane->SetItemDimmed(ELaunchApp,ETrue);
		}
		else
		{
			aMenuPane->SetItemDimmed(ETasksReFresh,ETrue);
			aMenuPane->SetItemDimmed(ECloseTask,ETrue);
			aMenuPane->SetItemDimmed(EBringToFrg,ETrue);
			aMenuPane->SetItemDimmed(EKillTask,ETrue);
		}
		
		if(!iHasServer)
		{
			aMenuPane->SetItemDimmed(EKillTask,ETrue);
		}
	}
	
	if(iTabGroup)
	{
		switch(iTabGroup->ActiveTabIndex())
		{
		case 1:
			if(iSecondControl)
			{
				iSecondControl->InitMenuPanelL(aResourceId,aMenuPane);
			}
			break;
		default:
			if(iFirstControl)
			{
				iFirstControl->InitMenuPanelL(aResourceId,aMenuPane);
			}
			break;
		}	
	}
}

/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CMainContainer::HandleCommandL(TInt aCommand)
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
    case EShowInfo:
    	{
    		TBuf<200> appTitle;
			HBufC* Abbout(NULL);
			
		    if(iSecondControl)
	    	{
	    		Abbout = iSecondControl->GetTaskInfoL(appTitle);
	    	}
	    	else if(iFirstControl)
			{
				Abbout = iFirstControl->GetApplicationInfoL(appTitle);	
			}
    	
    		if(Abbout)
			{
				CleanupStack::PushL(Abbout);
				
				TPtr Pointter(Abbout->Des());
				CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
				dlg->PrepareLC(R_ABOUT_HEADING_PANE);
				dlg->SetHeaderTextL(appTitle);  
				dlg->RunLD();
			
				CleanupStack::PopAndDestroy(Abbout);
			}
    	}
		SetMenuL();
		DrawNow();
		break;
    case EAbout:
    	{
    		HBufC* Abbout = KAppTaAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KAppTaApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EBacktoYTasks:
    	if(iTasksHandlerExit)
    	{
    		iTasksHandlerExit->HandlerExitL(this);	
    	}
    	break; 
	default:
		if(iTabGroup)
		{
			switch(iTabGroup->ActiveTabIndex())
			{
			case 1:
				if(iSecondControl)
				{
					iSecondControl->HandleViewCommandL(aCommand);
				}
				break;
			default:
				if(iFirstControl)
				{
					iFirstControl->HandleViewCommandL(aCommand);
				}
				break;
			}	
		}
		break;
    };
}


/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		switch (aKeyEvent.iCode)
		{
		case EKeyLeftArrow:
		case EKeyRightArrow:
			if(iTabGroup)
			{
				TInt IndexNum = iTabGroup->ActiveTabIndex();
				
				if(aKeyEvent.iCode == EKeyRightArrow)
					IndexNum = IndexNum + 1;
				else
					IndexNum = IndexNum - 1;
				
				if(IndexNum < 0)
					IndexNum = 0;
				
				if(IndexNum > 1) 
					IndexNum = 1;
				
				iTabGroup->SetActiveTabByIndex(IndexNum);
				
				if(iFirstControl)
				{
					switch(iTabGroup->ActiveTabIndex())
					{
					case 1:
						{
							iFirstControl->MakeVisible(EFalse);
							iFirstControl->SetFocus(EFalse);
							
							delete iSecondControl;
							iSecondControl = NULL;
							iSecondControl = new(ELeave)CTasksContainer();
							iSecondControl->ConstructL();
						}
						break;
					default:
						{
							delete iSecondControl;
							iSecondControl = NULL;
							iFirstControl->MakeVisible(ETrue);
							iFirstControl->SetFocus(ETrue);
						}
						break;
					};
				}
				
				SetMenuL();
				DrawNow();
			}
			break;
		default:
			if(iTabGroup)
			{
				switch(iTabGroup->ActiveTabIndex())
				{
				case 1:
					if(iSecondControl)
					{
						Ret = iSecondControl->OfferKeyEventL(aKeyEvent,aType);
					}
					break;
				default:
					if(iFirstControl)
					{
						Ret = iFirstControl->OfferKeyEventL(aKeyEvent,aType);
					}
					break;
				}	
			}
			break;
		};
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	if(iBgContext)
	{
	 	CWindowGc& gc = SystemGc();
	  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	TInt RetCount(0);
	
	if(iMyHelpContainer)
		RetCount = 1;
	else if(iTabGroup)
	{
		switch(iTabGroup->ActiveTabIndex())
		{
		case 1:
			if(iSecondControl)
			{
				RetCount = 1;
			}
			break;
		default:
			if(iFirstControl)
			{
				RetCount = 1;
			}
			break;
		}	
	}
 
	return RetCount;
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl(TInt /*aIndex*/) const
{
	CCoeControl* RetCont(NULL);

	if(iMyHelpContainer)
		RetCont = iMyHelpContainer;
	else if(iTabGroup)
	{
		switch(iTabGroup->ActiveTabIndex())
		{
		case 1:
			RetCont = iSecondControl;
			break;
		default:
			RetCont = iFirstControl;
			break;
		}	
	}
	
	return RetCont;
}

/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYTasksContainer* NewFileHandler()
    {
    return (new CMainContainer);
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
	IMPLEMENTATION_PROXY_ENTRY(0x0F60,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x0F60}, NewFileHandler}
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

