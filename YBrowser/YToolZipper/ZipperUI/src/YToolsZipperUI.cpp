/* 
Copyright (c) 2007 Dr. Jukka Silvennoinen www.DrJukka.com

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. THE SOFTWARE IS ALSO SUBJECT TO CHANGE WITHOUT PRIOR NOTICE, 
NO COMPATIBILITY BETWEEN MODIFICATIONS IS GUARANTEED. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name(s) of the above copyright holders 
shall not be used in advertising or otherwise to promote the sale, use or other 
dealings in this Software without prior written authorization.

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
#include <eikmenub.h> 
#include <stringloader.h>

#include "YToolsZipperUI.h"
#include "YuccaBrowser.hrh"

#include "CZipCompressor.h"
#include "CZipUncompressor.h"
#include "ZipReader.h"
#include "Common.h"
#include "icons.mbg"
#include "Help_Container.h"

#include "Common.h"
#ifdef SONE_VERSION
    #include <YFB_2002B0AA.rsg>
#else
    #ifdef LAL_VERSION

    #else
        #ifdef __YTOOLS_SIGNED
            #include <YuccaBrowser_2000713D.rsg>
        #else
            #include <YuccaBrowser.rsg>
        #endif
    #endif
#endif


#ifdef __SERIES60_3X__
	#include <eikstart.h>
#else
#endif

/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CYBrowserFileHandler1::~CYBrowserFileHandler1()
{
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;

	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
	
	delete iZipReader;
	iZipReader = NULL;
	
	delete iZipCompressor;
	iZipCompressor = NULL;
	
	delete iZipUncompressor;
	iZipUncompressor  = NULL;
	
	delete iFilesArray;
	iFilesArray = NULL;
	
	delete iSelectionBox;
	iSelectionBox = NULL;
	
	delete iSelectionBoxArray;
	iSelectionBoxArray = NULL;
		
	if(iResId > 0)
	{
		CEikonEnv::Static()->DeleteResourceFile(iResId);
	}
	
	if(iRFile.SubSessionHandle())
	{
		iRFile.Close();
	}

	if(iDebugFile.SubSessionHandle())
	{
		iDebugFile.Write(_L8("Bye, bye, "));	
	  	iDebugFile.Close();
	}
	
	REComSession::DestroyedImplementation(iDestructorIDKey);
}
/*
-------------------------------------------------------------------------
this will be called right after first constructor has finished
there shouldn't be any need to change this one.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils)
{
	iFileHandlerUtils = aFileHandlerUtils;
}

/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ConstructL()
{
 	CreateWindowL();
 	
 	iCanAddFiles = EFalse;
 	iChangesMade = MYBrowserFileHandlerUtils::ENoChangesMade;
 	
 	iResId = -1;
 	TFindFile AppFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == AppFile.FindByDir(KMyResourceFileName, KNullDesC))
	{
 		TFileName resourceName(AppFile.File());
		BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(), resourceName);
		iResId = CEikonEnv::Static()->AddResourceFileL(resourceName);
	}
	
//	_LIT(KRecFilename			,"C:\\ZipUI.txt");
//	CEikonEnv::Static()->FsSession().Delete(KRecFilename);		
//	User::LeaveIfError(iDebugFile.Create(CEikonEnv::Static()->FsSession(),KRecFilename,EFileWrite|EFileShareAny));	

	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("start, "));
	}
	
	// just to make sure we can lear the navicontrol in here
	iFileHandlerUtils->GetFileUtils().SetNaviTextL(_L(""));	
	
	// we should only use client rect area with plug-ins...
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
	
 	ActivateL();
	SetMenuL();
}
/*
-----------------------------------------------------------------------
just a internal function for constructing UI listbox
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::DisplayListBoxL(void)
{	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Make box, "));
	}
	
	TInt Selected(-1);
	
	if(iSelectionBox)
	{
		Selected = iSelectionBox->CurrentItemIndex();
	}
	
	delete iSelectionBox;
	iSelectionBox = NULL;
	iSelectionBox   = new( ELeave ) CAknDoubleGraphicStyleListBox();
	iSelectionBox->ConstructL(this,EAknListBoxMarkableList);
		
	if(!iSelectionBoxArray)
	{
		iSelectionBoxArray = new(ELeave)CDesCArrayFlat(10);
	}
	
	iSelectionBox->Model()->SetItemTextArray(iSelectionBoxArray);
    iSelectionBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
	iSelectionBox->CreateScrollBarFrameL( ETrue );
    iSelectionBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iSelectionBox->SetRect(Rect());	
	
	iSelectionBox->ItemDrawer()->ColumnData()->SetIconArray(iFileHandlerUtils->GetIconUtils().GetIconArrayL(TSize(0,0)));
	iSelectionBox->ActivateL();
	
	if(Selected >= 0)
	{
		iSelectionBox->SetCurrentItemIndex(Selected);
	}
	
	UpdateScrollBar(iSelectionBox);
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("made, "));
	}
}
/*
-----------------------------------------------------------------------
This was used for 2nd edition when the icons actually needed to be sized
-----------------------------------------------------------------------
*/
TSize CYBrowserFileHandler1::GetIconSize(void)
{
	TSize IconSiz(13,13);
	TInt Multiple = (CEikonEnv::Static()->ScreenDevice()->SizeInPixels().iWidth / 170);
	TInt MultipleTwo = (CEikonEnv::Static()->ScreenDevice()->SizeInPixels().iHeight/ 200);

	if(MultipleTwo <= Multiple && MultipleTwo > 1)
	{
		IconSiz.iWidth  = IconSiz.iWidth * 2;
		IconSiz.iHeight = IconSiz.iHeight * 2;
	}
	
	return IconSiz;
}
/*
-----------------------------------------------------------------------------
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SizeChanged()
{
	DisplayListBoxL();
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
	}
}
/*
-------------------------------------------------------------------------------
just a internal function for updating listbox scroll bars after changes
-------------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::UpdateScrollBar(CEikListBox* aListBox)
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
-------------------------------------------------------------------------
Y-Browser will call this when focus changes.

i.e. When you ask y-browser to open a file, and it will be opened by
other y-browser handler, then you will be called with focus false, 
right before new handler takes focus.

And when it returns back to y-browser, this function will be called with
ETrue,
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetFocusL(TBool aFocus)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("focusing, "));
	}
	
	if(aFocus)
	{
		CEikStatusPane*		sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
		if(sp)
		{
			CAknTitlePane* TitlePane = STATIC_CAST(CAknTitlePane*,sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
			TitlePane->SetTextL(KtxApplicationName);
				
	    	CAknContextPane* 	ContextPane = (CAknContextPane *)sp->ControlL(TUid::Uid(EEikStatusPaneUidContext));
			if(ContextPane)
			{
				TFindFile AppFile(CCoeEnv::Static()->FsSession());
				if(KErrNone == AppFile.FindByDir(KtxIconsFileName, KNullDesC))
				{
					ContextPane->SetPictureFromFileL(AppFile.File(),EMbmIconsConicon,EMbmIconsConicon_m);  
				}
			}				
		}		
	
		SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		SetMenuL();
		DrawNow();	
	}
}
/*
-------------------------------------------------------------------------
internal function for updating menu and CBA's for the application
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SetMenuL(void)
{
	if(iFileHandlerUtils)
	{
		if(iMyHelpContainer)
		{
			iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_APPHELP_CBA);				
			iFileHandlerUtils->GetCba().DrawDeferred();
		}
		else
		{
			iFileHandlerUtils->GetCba().SetCommandSetL(R_PLUG_MAIN_CBA);				
			iFileHandlerUtils->GetCba().DrawDeferred();
		}
	}
	
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_ZI_MAIN_MENUBAR);	
}
/*
-------------------------------------------------------------------------
Normal DynInitMenuPaneL, which Y-Browser will call.

See SDK documentation for more information
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("DynMenu, "));
	}
	
	if(aResourceId == R_ZI_MAIN_MENU)
	{
		TBool DimExtract(ETrue);
		if(iSelectionBox)
		{
			if(iSelectionBox->CurrentItemIndex() >= 0)
			{
				DimExtract = EFalse;
			}
		}
		
		if(DimExtract)
		{
			aMenuPane->SetItemDimmed(EZIExtract,ETrue);	
		}
		
		if(!iCanAddFiles)
		{			
			aMenuPane->SetItemDimmed(EZIAddFiles,ETrue);
		}
	}
}

/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(const TDesC& aFileName,const TDesC& /*aFileTypeID*/)	
{
	iCanAddFiles = EFalse;
	
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Openfile, "));
	}
	
	if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),aFileName))
	{
		iZipFileName.Copy(aFileName);
		
		delete iZipReader;
		iZipReader = NULL;
		iZipReader = CZipReader::NewL(CCoeEnv::Static()->FsSession(),*this);
		iZipReader->ReadFilesL(iZipFileName);
		
		iWaitDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), ETrue);
		iWaitDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);
		iWaitDialog->SetCallback(this);
		iWaitDialog->RunLD();
	}
	else
	{
		if(iFileHandlerUtils)
		{
			iFileHandlerUtils->GetFileUtils().ShowFileErrorNoteL(aFileName,KErrNotFound);
		}
		HandleCommandL(EBacktoYBrowser);
	}
}
/*
-------------------------------------------------------------------------
we only support one file type, so we don't care what aFileTypeID is
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::OpenFileL(RFile& aOpenFile,const TDesC& /*aFileTypeID*/)
{	
	iCanAddFiles = EFalse;

#ifdef __SERIES60_3X__
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Openfile, "));
	}
	
	if(aOpenFile.SubSessionHandle())
	{
		iRFile.Duplicate(aOpenFile);
	
		delete iZipReader;
		iZipReader = NULL;
		iZipReader = CZipReader::NewL(CCoeEnv::Static()->FsSession(),*this);
		iZipReader->ReadFilesL(iRFile);
		
		iWaitDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), ETrue);
		iWaitDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);
		iWaitDialog->SetCallback(this);
		iWaitDialog->RunLD();	
	}
	else
	{
		if(iFileHandlerUtils)
		{
			iFileHandlerUtils->GetFileUtils().ShowFileErrorNoteL(KNullDesC,KErrBadHandle);
		}
		
		HandleCommandL(EBacktoYBrowser);
	}
#else
	HandleCommandL(EBacktoYBrowser);
#endif
}

/*
-------------------------------------------------------------------------
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::NewFileL(const TDesC& aPath,const TDesC& aFileTypeID)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Newfile, "));
	}
	
	TBool FileNameOk(EFalse);	
	TBuf<50> FileNameAndExt(KtxtDefaultFileExtension);
	
	TBuf<100> Hjrlppp;
	StringLoader::Load(Hjrlppp,R_STR_ZPFILNAME);
	
	do
	{
		CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(FileNameAndExt,CAknQueryDialog::ENoTone);
		Dialog->PrepareLC(R_ASK_NAME_DIALOG);
		Dialog->SetPromptL(Hjrlppp);
		if(Dialog->RunLD())
		{
			iZipFileName.Copy(aPath);
			iZipFileName.Append(FileNameAndExt);
			if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iZipFileName))
			{
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,KtxtFileExistsReplace))
				{
					FileNameOk = ETrue;
				}
			}
			else
			{
				FileNameOk = ETrue;
			}
		}
		else
		{
			break;
		}
		
	}while(!FileNameOk);


	if(FileNameOk)
	{
		iCanAddFiles = ETrue;
		HandleCommandL(EZIAddFiles);
	}
	else//ExitNow
	{
		HandleCommandL(EBacktoYBrowser);
	}
}
/*
-------------------------------------------------------------------------
Called  by Y-Browser when usr wants to make New file
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::NewFileL(const TDesC& aPath,const TDesC& /*aFileTypeID*/,MDesCArray& aFileArray)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Newfile, "));
	}
	
	TBool FileNameOk(EFalse);	
	TBuf<50> FileNameAndExt(KtxtDefaultFileExtension);
	
	TBuf<100> Hjrlppp;
    StringLoader::Load(Hjrlppp,R_STR_ZPFILNAME);
	    
	do
	{
		CAknTextQueryDialog* Dialog = CAknTextQueryDialog::NewL(FileNameAndExt,CAknQueryDialog::ENoTone);
		Dialog->PrepareLC(R_ASK_NAME_DIALOG);
		Dialog->SetPromptL(Hjrlppp);
		if(Dialog->RunLD())
		{
			iZipFileName.Copy(aPath);
			iZipFileName.Append(FileNameAndExt);
			if(BaflUtils::FileExists(CCoeEnv::Static()->FsSession(),iZipFileName))
			{
				CAknQueryDialog* dlg = CAknQueryDialog::NewL();
				if(dlg->ExecuteLD(R_QUERY,KtxtFileExistsReplace))
				{
					FileNameOk = ETrue;
				}
			}
			else
			{
				FileNameOk = ETrue;
			}
		}
		else
		{
			break;
		}
		
	}while(!FileNameOk);


	if(FileNameOk)
	{
		delete iZipCompressor;
		iZipCompressor = NULL;
		
		if(aFileArray.MdcaCount())// we do not support new files without input files
		{
			iZipCompressor = CZipCompressor::NewL(CCoeEnv::Static()->FsSession(),iZipFileName,aFileArray,*this);
	
			iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
			iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
			iProgressInfo = iProgressDialog->GetProgressInfoL();
			iProgressDialog->SetCallback(this);
			iProgressDialog->RunLD();
			iProgressInfo->SetFinalValue(aFileArray.MdcaCount() + 1);
		}
		else
		{
			iCanAddFiles = ETrue;
			HandleCommandL(EZIAddFiles);
		}
	}
	else//ExitNow
	{
		HandleCommandL(EBacktoYBrowser);
	}	
}

/*
-------------------------------------------------------------------------
File selection has finished and user selected some files.
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SelectedFilesL(const CDesCArray& aArray)
{
	delete iZipCompressor;
	iZipCompressor = NULL;
	
	if(aArray.MdcaCount())// we do not support new files without input files
	{
		iCanAddFiles = EFalse;
		iZipCompressor = CZipCompressor::NewL(CCoeEnv::Static()->FsSession(),iZipFileName,aArray,*this);
	
		iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
		iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
		iProgressInfo = iProgressDialog->GetProgressInfoL();
		iProgressDialog->SetCallback(this);
		iProgressDialog->RunLD();
		iProgressInfo->SetFinalValue(aArray.MdcaCount() + 1);
	}

	SetFocusL(ETrue);
}
/*
-------------------------------------------------------------------------
Folder selection has finished, and user selected a folder
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SelectedFolderL(const TDesC& aFolderName)
{
	SetFocusL(ETrue);
	
	if(aFolderName.Length())
	{
		iChangesMade = MYBrowserFileHandlerUtils::EChangesMade;
		
		delete iZipUncompressor;
		iZipUncompressor = NULL;
		
		delete iFilesArray;
		iFilesArray = NULL;
		iFilesArray = GetSelectedFilesL();
		
		iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**,&iProgressDialog)),ETrue);
		iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
		iProgressInfo = iProgressDialog->GetProgressInfoL();
		iProgressDialog->SetCallback(this);
		iProgressDialog->RunLD();
		
		TInt FileVal(0);
		
		if(iFilesArray)
		{// count of selected files (+1 so progress shows it better)
			FileVal = iFilesArray->Count() + 1;
		}
		else if(iSelectionBoxArray)
		{// get total number of files..
			FileVal = iSelectionBoxArray->Count() + 1;
		}
		
		iProgressInfo->SetFinalValue(FileVal);
		
		iZipUncompressor = CZipUncompressor::NewL(CCoeEnv::Static()->FsSession(),*this);	
		
		if(iRFile.SubSessionHandle())
			iZipUncompressor->EncompressL(aFolderName,iRFile,iFilesArray);
		else
			iZipUncompressor->EncompressL(aFolderName,iZipFileName,iFilesArray);	
	}	
}
/*
-------------------------------------------------------------------------
Y-Browser tells us that user cancelled file additions
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::SelectorCancelledL(void)
{
	SetFocusL(ETrue);
}
/*
-------------------------------------------------------------------------
we are not supporting adding files to existing zip files
so no need to implement this function
-------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::AddFilesL(const TDesC& /*aFileName*/,MDesCArray& /*aFileArray*/)
{	
	HandleCommandL(EBacktoYBrowser);
}
/*
-------------------------------------------------------------------------
Handle command called by Y-Browser,make sure you set your menu & CBA's 
when focus chnages, and that your commands are started at least with
first command as 0x7000.

-------------------------------------------------------------------------
*/

void CYBrowserFileHandler1::HandleCommandL(TInt aCommand)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("cmd, "));
	}
	
	switch (aCommand)
    { 
	case EPlugAppHelpBack:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EPlugAppHelp:
		{
			delete iMyHelpContainer;
			iMyHelpContainer = NULL;    		
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;
    case EZIAddFiles:
    	if(iFileHandlerUtils)
    	{
    		iCanAddFiles = ETrue;
    		// start the Y-Broser file selector
    		iFileHandlerUtils->GetFileUtils().StartSelectionViewL(this,EFalse,_L(""),KTxtAddFiles,NULL);
    	}
    	break;
    case EPlugAbout:
    	{
    		HBufC* Abbout = KtxAbbout().AllocLC();
			TPtr Pointter(Abbout->Des());
			CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(Pointter);
			dlg->PrepareLC(R_ABOUT_HEADING_PANE);
			dlg->SetHeaderTextL(KtxApplicationName);  
			dlg->RunLD();
			
			CleanupStack::PopAndDestroy(Abbout);
    	}
    	break;
    case EZIExtract:
    	if(iFileHandlerUtils)
    	{	
    		// start the Y-Broser folder selector
    		iFileHandlerUtils->GetFileUtils().StartSelectionViewL(this,ETrue,_L(""),KTxtSelectFolder,NULL);    	
    	}
    	break;
    case EBacktoYBrowser:
    	if(iFileHandlerUtils)
    	{
    		// tell Y-Browser that we want to exit now.
    		iFileHandlerUtils->HandleExitL(this,iChangesMade);	
    	}
    	break; 
	default:
		break;
    };
}

/*
-------------------------------------------------------------------------
internal function for getting selected file names (column no. 1 in listbox)
-------------------------------------------------------------------------
*/
CDesCArrayFlat* CYBrowserFileHandler1::GetSelectedFilesL(void)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("Getfile, "));
	}
	
	CDesCArrayFlat* FilesArray(NULL);
	
	if(iSelectionBox)
	{
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
		TBool NoSelection(ETrue);
				
		const CListBoxView::CSelectionIndexArray* SelStuff = iSelectionBox->SelectionIndexes();
		if(SelStuff)
		{
			if(SelStuff->Count())
			{
				FilesArray = new(ELeave)CDesCArrayFlat(SelStuff->Count());
				
				NoSelection = EFalse;
				
				for(TInt i=0; i < SelStuff->Count(); i++)
				{
					if(itemArray->Count() > SelStuff->At(i))
					{
						TPtrC ItName,Item;
						Item.Set(itemArray->MdcaPoint(SelStuff->At(i)));
						if(KErrNone == TextUtils::ColumnText(ItName,1,&Item))
						{
							FilesArray->AppendL(ItName);
						}	
					}
				}
			}
		}
		
		if(NoSelection)
		{
            TBuf<200> Hjrlppp;
		    StringLoader::Load(Hjrlppp,R_STR_ZPEXTRACALL);
		    
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(!dlg->ExecuteLD(R_QUERY,Hjrlppp))
			{	
				TInt Selected = iSelectionBox->CurrentItemIndex();
				
				if(itemArray->Count() > Selected)
				{
					TPtrC ItName,Item;
					Item.Set(itemArray->MdcaPoint(Selected));
					if(KErrNone == TextUtils::ColumnText(ItName,1,&Item))
					{
						if(FilesArray == NULL)
						{
							FilesArray = new(ELeave)CDesCArrayFlat(1);
						}
						
						FilesArray->AppendL(ItName);
					}
				}
			}
		}
	}
			
	return FilesArray;
}
/*
-----------------------------------------------------------------------
normal implementation of the OfferKeyEventL, 
called  by Y-Browser when we are in focus

See SDK documentations for more information
-----------------------------------------------------------------------
*/
TKeyResponse CYBrowserFileHandler1::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
			if(iSelectionBox)
			{
				TInt CurrSel = iSelectionBox->CurrentItemIndex();
				if(CurrSel >= 0)
				{	
					iSelectionBox->View()->ToggleItemL(CurrSel);
				}
			}
			break;
		default:
			if(iSelectionBox)
			{
				iSelectionBox->OfferKeyEventL(aKeyEvent,aType);
			}
			break;
		};
	}
	return Ret;
}
/*
-----------------------------------------------------------------------
just normal Draw

See SDK documentations for more information
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::Draw(const TRect& aRect) const
{
 	CWindowGc& gc = SystemGc();
 	gc.Clear(aRect);
}
/*
-------------------------------------------------------------------------
just normal CountComponentControls

See SDK documentations for more information
-------------------------------------------------------------------------
*/
TInt CYBrowserFileHandler1::CountComponentControls() const
{
	if(iMyHelpContainer)
		return 1;
	else if(iSelectionBox)
		return 1;
	else
		return 0;
}
/*
-------------------------------------------------------------------------
just normal ComponentControl

See SDK documentations for more information
-------------------------------------------------------------------------
*/
CCoeControl* CYBrowserFileHandler1::ComponentControl(TInt /*aIndex*/) const
{
	if(iMyHelpContainer)
		return iMyHelpContainer;
	else
		return iSelectionBox;
}

/*
-----------------------------------------------------------------------
internal interface function which is used to update Zip file info
read process.
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::AddFileToListL(TUint32 aCompressedSize,TUint32 aUncompressedSize,const TDesC& aCurrentFile,TInt aIconId)
{
	if(iDebugFile.SubSessionHandle())
	{	
		iDebugFile.Write(_L8("AddtoList, "));
	}
	
	if(iSelectionBox)
	{
		TInt UnSiz  = (aUncompressedSize / 1024);
		TInt ComSiz = (aCompressedSize / 1024);
		
		TFileName AddBuffer;
		
		if(UnSiz > 0 && ComSiz > 0)
		{
			AddBuffer.Format(_L("%d\t%S\t%d Kb./%d Kb."),aIconId,&aCurrentFile,ComSiz,UnSiz);
		}
		else
		{
			AddBuffer.Format(_L("%d\t%S\t%d b./%d b."),aIconId,&aCurrentFile,aCompressedSize,aUncompressedSize);
		}
		
		CDesCArray* itemArray = STATIC_CAST( CDesCArray* ,iSelectionBox->Model()->ItemTextArray());
		itemArray->AppendL(AddBuffer);
	}
}

/*
-----------------------------------------------------------------------
internal interface function used for updating process note while 
compressing/uncompressing zip file.
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::CompressProcessL(TInt aFileCount,const TDesC& aCurrentFile)
{
	if(iProgressDialog)
	{
		TParsePtrC NameHelp(aCurrentFile);
		
		iProgressDialog->SetTextL(NameHelp.NameAndExt());
	}
	
	if(iProgressInfo)
	{
		iProgressInfo->SetAndDraw(aFileCount);	
	}
}
/*
-----------------------------------------------------------------------
internal interface function used for updating process note when
compressing/uncompressing/reading zip file finishes.
-----------------------------------------------------------------------
*/
void CYBrowserFileHandler1::ProcessFinnishedL(CBase* aObject)
{
	if(iZipCompressor == aObject)
	{// compression has finished.
		delete iFilesArray;
		iFilesArray = NULL;
	}
	
	if(iZipUncompressor == aObject)
	{// extracting process has finished
		
	}
	else if(iSelectionBox) // reading has finished
	{
		iSelectionBox->HandleItemAdditionL();
		UpdateScrollBar(iSelectionBox);
	}
	
	if(iWaitDialog)
	{
		iWaitDialog->ProcessFinishedL(); 
	}
	
	iWaitDialog = NULL;
	
	if(iProgressDialog)
	{
		iProgressDialog->ProcessFinishedL(); 
	}

	iProgressDialog = NULL;
	iProgressInfo = NULL;
	DrawNow();
}
	

/*
-----------------------------------------------------------------------------
normal MProgressDialogCallback callback function.

See SDK documentations for more information
----------------------------------------------------------------------------
*/
void CYBrowserFileHandler1::DialogDismissedL (TInt /*aButtonId*/)
{		  		
	iProgressDialog = NULL;
	iProgressInfo = NULL;
	iWaitDialog = NULL;
	
	if(iZipUncompressor)
	{
		if(iZipUncompressor->IsActive())
		{
			iZipUncompressor->Cancel();
		}
	}
	
	if(iZipCompressor)
	{
		if(iZipCompressor->IsActive())
		{
			iZipCompressor->Cancel();
		}
	}
	
	if(iZipReader)
	{
		if(iZipReader->IsActive())
		{
			iZipReader->Cancel();
		}
	}	
}
/*
-------------------------------------------------------------------------
interface function for getting the implementation instance
-------------------------------------------------------------------------
*/
CYBrowserFileHandler* NewFileHandler()
    {
    return (new CYBrowserFileHandler1);
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
	IMPLEMENTATION_PROXY_ENTRY(0x0F6E,NewFileHandler)
};
#else

// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
    {
    {{0x0F6E}, NewFileHandler}
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

