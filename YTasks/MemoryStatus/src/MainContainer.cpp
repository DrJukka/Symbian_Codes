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
#include <aknglobalnote.h> 
#include "MainContainer.h"

#include "YTools_A0003128.hrh"
#include "Help_Container.h"
#include "Common.h"

#ifdef SONE_VERSION
	#include "YTools_2002B0A3_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A0003128_res.rsg"
	#else
		#include "YTools_A0003128_res.rsg"
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
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
		iProgressDialog = NULL;
		iProgressInfo = NULL;
	}

	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
		
	delete iFileWriter;
	iFileWriter = NULL;
	
	delete iRamBuffer;
	iRamBuffer = NULL;
				
	delete iBgSelImg;
	iBgSelImg = NULL;
	
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
 	
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	
	TRgb textcol;
	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
	iTextColor = TLogicalRgb(textcol);
		
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgScreen,TRect(0,0,1,1), ETrue);
	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KMEmStatResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
	ActivateL();
	
	GetDrivesInfoL();
	
	SetMenuL();
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetDrivesInfoL(void)
{
	TVolumeInfo volumeInfo; 

	iCFree = iCTotal = 0;
	iDFree = iDTotal = 0;
	iEFree = iETotal = 0;
	
	if(KErrNone == CCoeEnv::Static()->FsSession().Volume(volumeInfo,EDriveC))
	{						
		iCFree = (volumeInfo.iFree);
		iCTotal = (volumeInfo.iSize);
		iCHasTmp =  HasTempFilesL(_L("C:"));
	}
		
	if(KErrNone == CCoeEnv::Static()->FsSession().Volume(volumeInfo,EDriveD))
	{						
		iDFree = (volumeInfo.iFree);
		iDTotal= (volumeInfo.iSize);
		iDHasTmp =  HasTempFilesL(_L("D:"));
	}
	
	if(KErrNone == CCoeEnv::Static()->FsSession().Volume(volumeInfo,EDriveE))
	{						
		iEFree = (volumeInfo.iFree);
		iETotal = (volumeInfo.iSize);
		iEHasTmp =  HasTempFilesL(_L("E:"));						
	}
	
	TMemoryInfoV1Buf   memInfoBuf;

	if(KErrNone == UserHal::MemoryInfo(memInfoBuf))
	{
		iTotalRam = memInfoBuf().iTotalRamInBytes;
		iFreeRam = memInfoBuf().iFreeRamInBytes;
	}
	else
	{
		iTotalRam = 0;
		iFreeRam = 0;
	}
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
		TitlePane->SetTextL(KMEmStatApplicationName);
			
    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
		if(ContextPane)
		{
			TFindFile AppFile(CCoeEnv::Static()->FsSession());
			if(KErrNone == AppFile.FindByDir(KMEmStatIconsFileName, KNullDesC))
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
			
				
				TRect DrawRect(Rect());
				DrawRect.Shrink(4,4);

				TInt CompWidth = (DrawRect.Width() / 2);
				TInt CompHeight= (DrawRect.Height() / 2);
				
				delete iBgSelImg;
				iBgSelImg = NULL;
				iBgSelImg = LoadImageL(AppFile.File(),2, 3,TSize(CompWidth,CompHeight),EAspectRatioNotPreserved);
			}
		}				
	}	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMainContainer::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aAspect)
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
	
	ChangeIconAndNameL();
	
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

-------------------------------------------------------------------------
*/
void CMainContainer::SetMenuL(void)
{
	if(iMyHelpContainer)
	{
		iCba->SetCommandSetL(R_APPHELP_CBA);
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
void CMainContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_MAIN_MENU)
	{
/*		if(iSelected != 0)
		{
			aMenuPane->SetItemDimmed(EReserveMem,ETrue);
		}
*/		

		switch(iSelected)
		{
		case 0:
			if(!iRamBuffer)
			{
				aMenuPane->SetItemDimmed(EFreeMem,ETrue);
			}
			break;
		case 1:
			if(!iCHasTmp)
			{
				aMenuPane->SetItemDimmed(EFreeMem,ETrue);
			}
			break;
		case 2:
			if(!iDHasTmp)
			{
				aMenuPane->SetItemDimmed(EFreeMem,ETrue);
			}
			break;
		default:
			if(!iEHasTmp)
			{
				aMenuPane->SetItemDimmed(EFreeMem,ETrue);
			}
			break;
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::ReserveRAMMemoryL(TInt aSize)
{
	TInt Size = 0;

	if(iRamBuffer)
	{
		Size = iRamBuffer->Des().Size();
		delete iRamBuffer;
		iRamBuffer = NULL;
	}

	Size = Size + (aSize * 1024);

	User::CompressAllHeaps();

	if(Size > 0)
	{
		TChar FillChar = 'Y';
		iRamBuffer = HBufC8::NewL(Size);
		iRamBuffer->Des().Fill(FillChar,Size);
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::ReserveMemoryL(TDesC& aPath,TInt64 aSize)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
		iProgressDialog = NULL;
		iProgressInfo = NULL;
	}

	if(iFileWriter)
	{
		delete iFileWriter;
		iFileWriter = NULL;
	}

	iFileWriter = new(ELeave)CFileWriter(*this);
	iFileWriter->ConstructL();

	iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)), ETrue);
	iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
	iProgressDialog->SetTextL(_L("Reserving diskspace"));
	iProgressInfo = iProgressDialog->GetProgressInfoL();
	iProgressDialog->SetCallback(this);
	iProgressDialog->RunLD();
	iProgressInfo->SetFinalValue(aSize - 1);

	iFileWriter->StartWritingL(aPath,aSize);
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CMainContainer::IncrementWriteL(TInt64 aDone, TInt64 /*aTotal*/)
{
	if(iProgressInfo)
	{
		iProgressInfo->SetAndDraw(aDone);
	}
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/

void CMainContainer::WriteFinishedL(void)
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
		iProgressDialog = NULL;
		iProgressInfo = NULL;
	}

	GetDrivesInfoL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::DialogDismissedL (TInt /*aButtonId*/)
{
	if(iFileWriter)
	{
		iFileWriter->CancelWrite();
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TBool CMainContainer::HasTempFilesL(const TDesC& aDrive)
{
	TBool Ret(EFalse);
	
	CDir* file_list = NULL;
	TFileName FolderPath;

	FolderPath.Copy(aDrive);
	FolderPath.Append(KtxSystemTemp);
			
	if(CCoeEnv::Static()->FsSession().GetDir(FolderPath, KEntryAttMatchMask,ESortByName,file_list) == KErrNone)
	{
		if(file_list)
		{
			if(file_list->Count())
			{
				Ret = ETrue;
			}
			
			delete file_list;
			file_list = NULL;
		}
	}		
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMainContainer::ClearTempFolderL(const TDesC& aDrive)
{
	CDir* file_list = NULL;
	
	TFileName FolderPath, FileNammm;

	CFileMan* FileMan = CFileMan::NewL(CCoeEnv::Static()->FsSession());
	CleanupStack::PushL(FileMan);

	FolderPath.Copy(aDrive);
	FolderPath.Append(KtxSystemTemp);
			
	if(CCoeEnv::Static()->FsSession().GetDir(FolderPath, KEntryAttMatchMask,ESortByName,file_list) == KErrNone)
	{
		if(file_list)
		{
			if(file_list->Count())
			{
				for (TInt i=0;i<file_list->Count();i++)
				{
					if((*file_list)[i].IsDir()) //Folder
					{
						FileNammm.Copy(FolderPath);
						FileNammm.Append((*file_list)[i].iName);
						FileNammm.Append(_L("\\"));
						FileMan->RmDir(FileNammm);
					}
					else
					{
						FileNammm.Copy(FolderPath);
						FileNammm.Append((*file_list)[i].iName);
						FileMan->Delete(FileNammm);
					}
				}
			}
			
			delete file_list;
			file_list = NULL;
		}
	}		

	CleanupStack::PopAndDestroy(1); // FileMan
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
    case EFreeMem:
    	{
    		switch(iSelected)
			{
			case 0:
				{
					delete iRamBuffer;
					iRamBuffer = NULL;
				
					User::CompressAllHeaps();
				}
				break;
			case 1:
				{
					ClearTempFolderL(_L("C:"));
				}
				break;
			case 2:
				{
					ClearTempFolderL(_L("D:"));
				}
				break;
			default:
				{
					ClearTempFolderL(_L("E:"));
				}
				break;
			}

    		GetDrivesInfoL();
    		DrawNow();
		}
		break;
	case EReserveMem:
		{
			TInt Reserve = 100;
			TFileName PathMe;
			CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(Reserve,CAknQueryDialog::ENoTone);
			Dialog->PrepareLC(R_NUMBER_DIALOG);
			
			switch(iSelected)
			{
			case 0:
				Dialog->SetPromptL(_L("Reserve RAM (Kb.)"));
				Dialog->SetMinimumAndMaximum(5,(iFreeRam / 1024));
				break;
			case 1:
				Dialog->SetPromptL(_L("Reserve C-Drive (Kb.)"));
				Dialog->SetMinimumAndMaximum(5,(iCFree/ 1024));
				PathMe.Copy(_L("C:"));
				break;
			case 2:
				Dialog->SetPromptL(_L("Reserve D-Drive (Kb.)"));
				Dialog->SetMinimumAndMaximum(5,(iDFree/ 1024));
				PathMe.Copy(_L("D:"));
				break;
			default:
				Dialog->SetPromptL(_L("Reserve E-Drive (Kb.)"));
				Dialog->SetMinimumAndMaximum(5,(iEFree/ 1024));
				PathMe.Copy(_L("E:"));
				break;
			}

			if(Dialog->RunLD())
			{			
				if(iSelected == 0) //RAM
	    		{
	    			ReserveRAMMemoryL(Reserve);
	    		}
	    		else
	    		{
	    			PathMe.Append(KtxSystemTemp);
	    			TInt RessMe = (Reserve * 1024);
	    			ReserveMemoryL(PathMe,RessMe);
	    		}
	    		
				GetDrivesInfoL();
	    		DrawNow();
			}
		}
		break;
    case ERamCompress:
    	{
    		TInt64 TmpFree = iFreeRam;
    		
    		User::CompressAllHeaps();
    		
    		GetDrivesInfoL();
    		
    		TInt64 TmpDiff = (iFreeRam - TmpFree);
    		
    		if(TmpDiff < 0)
    		{
    			TmpDiff = 0;
    		}
    		
    		TBuf<200> Jello;
    		
    		GetAmountText(Jello,TmpDiff,_L("Freed: "));
    		
    		ShowNoteL(Jello);
    		DrawNow();
    	}
    	break;
    case EAbout:
    	{
    		
    		
    		HBufC* Abbout = KMEmStatAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KMEmStatApplicationName);  
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
		break;
    };
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (aPointerEvent.iType == TPointerEvent::EButton1Down){
    
		TInt Xxx =  (Rect().Width() / aPointerEvent.iPosition.iX);
		TInt Yxx =  (Rect().Height() / aPointerEvent.iPosition.iY);
		
		if(Xxx < 2){
			if(Yxx < 2){
				iSelected = 3;
			}else{
				iSelected = 1;
			}
		}else{
			if(Yxx < 2){
				iSelected = 2;
			}else{
				iSelected = 0;
			}		
		}
			
				
		DrawNow();
    
    }else{
		CCoeControl::HandlePointerEventL(aPointerEvent);
    }    
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
		case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyRightArrow:
			switch(iSelected)
			{
			case 0:
				iSelected = 1;
				DrawNow();
				break;
			case 1:
				break;
			case 2:
				iSelected = 3;
				DrawNow();
				break;
			default:
				break;
			}
			break;
		case EKeyLeftArrow:
			switch(iSelected)
			{
			case 0:
				break;
			case 1:
				iSelected = 0;
				DrawNow();
				break;
			case 2:
				break;
			default:
				iSelected = 2;
				DrawNow();
				break;
			}
			break;
		case EKeyUpArrow:
			switch(iSelected)
			{
			case 0:
				break;
			case 1:
				break;
			case 2:
				iSelected = 0;
				DrawNow();
				break;
			default:
				iSelected = 1;
				DrawNow();
				break;
			}
			break;
		case EKeyDownArrow:
			switch(iSelected)
			{
			case 0:
				iSelected = 2;
				DrawNow();
				break;
			case 1:
				iSelected = 3;
				DrawNow();
				break;
			case 2:
				break;
			default:
				break;
			}
			break;
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
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	
	if(iBgContext)
	{
	  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
	}
	
	TRect DrawRect(Rect());
	DrawRect.Shrink(2,2);
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY)); 

	DrawRect.Shrink(2,2);
	TInt CompWidth = (DrawRect.Width() / 2);
	TInt CompHeight= (DrawRect.Height() / 2);

	TRect DrawMeRect(DrawRect.iTl.iX,DrawRect.iTl.iY,(DrawRect.iTl.iX + CompWidth),(DrawRect.iTl.iY + CompHeight));

	CompDrawer(gc,DrawMeRect,iFreeRam,iTotalRam,TBool(iSelected == 0),_L("RAM"));
	
	DrawMeRect.iTl.iX = DrawMeRect.iBr.iX;
	DrawMeRect.iBr.iX = (DrawMeRect.iTl.iX + CompWidth);
	
	CompDrawer(gc,DrawMeRect,iCFree,iCTotal,TBool(iSelected == 1),_L("C:\\"));
	
	DrawMeRect= TRect(DrawRect.iTl.iX,(DrawRect.iTl.iY + CompHeight),(DrawRect.iTl.iX + CompWidth),(DrawRect.iTl.iY + (2 * CompHeight)));

	CompDrawer(gc,DrawMeRect,iDFree,iDTotal,TBool(iSelected == 2),_L("D:\\"));
	
	DrawMeRect.iTl.iX = DrawMeRect.iBr.iX;
	DrawMeRect.iBr.iX = (DrawMeRect.iTl.iX + CompWidth);
	CompDrawer(gc,DrawMeRect,iEFree,iETotal,TBool(iSelected == 3),_L("E:\\"));

}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::CompDrawer(CWindowGc& aGc, TRect aRect, TInt64 aValue,TInt64 aTotal, TBool aSelected, const TDesC& aText) const
{
	aGc.SetPenColor(iTextColor);
	
	TRect MyRect(aRect);
	
	if(aSelected && iBgSelImg)
	{
		if(iBgSelImg->Bitmap() && iBgSelImg->Mask())
		{
			if(iBgSelImg->Bitmap()->Handle() && iBgSelImg->Mask()->Handle())
			{	
				TSize ImgSiz(iBgSelImg->Bitmap()->SizeInPixels());
				if(ImgSiz.iWidth > 0 && ImgSiz.iHeight > 0)
				{
					aGc.DrawBitmapMasked(MyRect,iBgSelImg->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iBgSelImg->Mask(),EFalse);					
				}
				else
				{
					aGc.DrawBitmapMasked(MyRect,iBgSelImg->Bitmap(),MyRect,iBgSelImg->Mask(),EFalse);
				}
			}
		}
	}
	
	aGc.DrawLine(MyRect.iTl,TPoint(MyRect.iTl.iX,MyRect.iBr.iY));
	aGc.DrawLine(MyRect.iTl,TPoint(MyRect.iBr.iX,MyRect.iTl.iY));
	aGc.DrawLine(MyRect.iBr,TPoint(MyRect.iTl.iX,MyRect.iBr.iY));
	aGc.DrawLine(MyRect.iBr,TPoint(MyRect.iBr.iX,MyRect.iTl.iY)); 
	MyRect.Shrink(2,2);
	
	const CFont* MuFonr = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);

	TInt TextHeight= MuFonr->HeightInPixels();
	TInt TextWidth = MuFonr->TextWidthInPixels(aText);

	aGc.UseFont(MuFonr);
	
	
	aGc.DrawText(aText,TRect(MyRect.iTl.iX,MyRect.iTl.iY,(MyRect.iTl.iX + TextWidth),(MyRect.iTl.iY + TextHeight)),MuFonr->AscentInPixels(), CGraphicsContext::ELeft, 0);			

	TBuf<40> Hjelppe;
	GetAmountText(Hjelppe,aTotal,_L("Total: "));
	
	TRect TextRect(MyRect);
	
	TextRect.iTl.iY = (TextRect.iBr.iY - TextHeight);
	
	aGc.DrawText(Hjelppe,TextRect,MuFonr->AscentInPixels(), CGraphicsContext::ELeft, 0);			
	
	Hjelppe.Zero();
	GetAmountText(Hjelppe,aValue,_L("Free: "));
	
	TextRect.iBr.iY = TextRect.iTl.iY;
	TextRect.iTl.iY = (TextRect.iBr.iY - TextHeight);
	
	aGc.DrawText(Hjelppe,TextRect,MuFonr->AscentInPixels(), CGraphicsContext::ELeft, 0);			

	TInt Siz(MyRect.Width() - TextWidth);
	if(Siz > (MyRect.Height() - (2 * TextHeight)))
	{
		Siz  = (MyRect.Height() - (2 * TextHeight));
	}
	
	TInt64 Pross(-1);

	if(aTotal > 0)
	{
		Pross = aValue;
		Pross = (Pross * 100);
		Pross = (Pross / aTotal);
	}
	
	PieDrawer(aGc,TPoint((MyRect.iBr.iX - Siz),MyRect.iTl.iY),Siz,(TInt)Pross);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::GetAmountText(TDes& aBuffer,TInt64 aValue,const TDesC& aHeader) const
{
	aBuffer.Copy(aHeader);
	
	if((aValue / (1024 * 1024)) > 10)
	{
		TInt64 ValVal1 =(aValue / (1024 * 1024));
		
		aBuffer.AppendNum(ValVal1 ,EDecimal);
		aBuffer.Append(_L(" Mb."));
	}
	else if((aValue / 1024) > 10)
	{
		TInt64 ValVal2 = (aValue / 1024);
		aBuffer.AppendNum(ValVal2,EDecimal);
		aBuffer.Append(_L(" Kb."));
	}
	else
	{
		aBuffer.AppendNum(aValue,EDecimal);
		aBuffer.Append(_L(" b."));
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::PieDrawer(CWindowGc& aGc, TPoint aLeftTop, TInt aSize, TInt aProsentages) const
{
	TBuf<50> Buffer;
	Buffer.Num(aProsentages);
	Buffer.Append(_L("%"));

	aGc.SetPenColor(KRgbBlack);
	aGc.SetBrushColor(KRgbRed);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawEllipse(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize));

	if(aProsentages < 0)
	{
		Buffer.Copy(_L("N/A"));
		aGc.SetBrushColor(KRgbRed);
	}
	else
	{
		TInt HelpPie = 0;

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

		if(Angle > 180)
			aGc.SetBrushColor(KRgbGreen);
		else
			aGc.SetBrushColor(KRgbRed);
	}

	const CFont* MuFonr = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);

	TInt TextHeight= MuFonr->HeightInPixels();
	TInt TextWidth = MuFonr->TextWidthInPixels(Buffer);

	TInt StartX = (aLeftTop.iX + ((aSize - TextWidth) / 2));
	TInt StartY = (aLeftTop.iY + ((aSize - TextHeight) / 2));

	aGc.UseFont(MuFonr);
	aGc.SetPenColor(KRgbBlack);
	aGc.DrawText(Buffer,TRect(StartX,StartY,StartX + TextWidth,StartY + TextHeight),MuFonr->AscentInPixels(), CGraphicsContext::ELeft, 0);			

	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
}

/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
TInt CMainContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
	{
		return 1;
	}
	else 
		return 0;
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl(TInt /*aIndex*/) const
{
	return iMyHelpContainer;
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
	IMPLEMENTATION_PROXY_ENTRY(0x3128,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x3128}, NewFileHandler}
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

