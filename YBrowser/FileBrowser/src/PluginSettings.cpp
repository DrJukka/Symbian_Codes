/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
*/

#include <GULUTIL.H>
#include <APGCLI.H>
#include <APMREC.H>
#include <AknIconArray.h>
#include <AknSelectionList.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknglobalnote.h> 
#include <eikclbd.h>
#include <BAUTILS.H>
#include <aknnavide.h>
#include <akntabgrp.h>

#include "PluginSettings.h"
#include "IconHandler.h"
#include "YuccaBrowser.h"
#include "YuccaBrowser.hrh"

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


_LIT(KTxSupNewFile	,"(Supports new files)");


_LIT(KTxJustALine	,"-");
_LIT(KTxJustAZero	,"0");
_LIT(KTxIndexZero	,"0\t");
_LIT(KTxIndexOne	,"1\t");


/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CPluInListbox ::CPluInListbox()
{
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CPluInListbox::~CPluInListbox()
{		

}				
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CAknSettingItem* CPluInListbox ::CreateSettingItemL(TInt aIdentifier)
{
	CAknSettingItem* settingItem = NULL;

	switch(aIdentifier)
	{
	case EPlugInName:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iName);
		break;
	case EPlugInTypes:
    	settingItem = new (ELeave) CAknTextSettingItem(aIdentifier,iTypes);
		break;
	case EPlugUnRecHand:
		settingItem = new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,iUnRecognized);
		break;
	}	
	
	return settingItem;
}




/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CPluInListbox::SizeChanged()
    {
    if (ListBox()) 
        {
        ListBox()->SetRect(Rect());
        }
    }
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CPluInListbox::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
{
	if(ListBox())
	{	
		if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked)
		{
			if(ListBox()->View()->CurrentItemIndex() == 0)// Name
			{
				CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			}
			else if(ListBox()->View()->CurrentItemIndex() == 1)// types
			{
				CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();	
			}
			else
			{
				CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
			}
		}
		else
		{
			CAknSettingItemList::HandleListBoxEventL(aListBox,aEventType);
		}
	}
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CPluInListbox::SelectApplicationL(void) 
{
}
/*
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
*/
CPluInContainer::~CPluInContainer()
{
	RemoveNavipanel();
	
	delete iMyDBClass;
	iMyDBClass = NULL;
	
	iTmpSupArray.ResetAndDestroy();
	
	delete iEditor;
	delete iParaformMe;
	delete iCharformMe;
	delete iRichMessageMe; 
			
	delete iTypesListbox;
	iTypesListbox = NULL;
	
	delete iSysListBox;
	iSysListBox = NULL;
	
	delete iSysFilTypArray;
	iSysFilTypArray = NULL;
	
	delete iOwnListBox;
	delete iOwnFilTypArray;
	
	iRecognizerArray.ResetAndDestroy();
	iHandlerArray.ResetAndDestroy();
	
	delete iBgContext;
	iBgContext = NULL;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CPluInContainer ::CPluInContainer(CEikButtonGroupContainer* aCba,CIconHandler* aIconHandler)
:iCba(aCba),iIconHandler(aIconHandler)
{
	
}	
/*
-------------------------------------------------------------------------
This will be called when this file shandler is preraped to be used for, 
opening or constructing a file. The open/new etc. functions are called
strait after calling this function.
-------------------------------------------------------------------------
*/
void CPluInContainer::ConstructL(void)
{	
 	CreateWindowL();
 	
 	MakeNavipanelL();
 	
 	iMyDBClass = new(ELeave)CMyDBClass();
	iMyDBClass->ConstructL();
	
	RPointerArray<CDisabledItem> DisabledArray;

	iMyDBClass->GetDisabledL(DisabledArray);
	
 	ReReadHandlersL(DisabledArray);
	ReReadRecognizersL(DisabledArray);
		
	DisabledArray.ResetAndDestroy();
		
	
 	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);
	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); 	
		
	ActivateL();
	SetMenuL();
	DrawNow();	
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CAknSingleGraphicStyleListBox* CPluInContainer::ConstructListBoxL(TInt aIndex,CDesCArray* aArray)
{
	TInt MySetIndex(aIndex);

	CAknSingleGraphicStyleListBox* ListBox   = new( ELeave )CAknSingleGraphicStyleListBox();	
	ListBox->ConstructL(this,EAknListBoxMarkableList);	
	
	
	ListBox->Model()->SetItemTextArray(aArray);
    ListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    
	ListBox->CreateScrollBarFrameL( ETrue );
    ListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	ListBox->SetRect(Rect());
	ListBox->ItemDrawer()->ColumnData()->SetIconArray(iIconHandler->GetSelectionIconArrayL());
	ListBox->ItemDrawer()->ColumnData()->EnableMarqueeL(ETrue);
	
	ListBox->ActivateL();

	TInt ItemsCount = ListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		ListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		ListBox->SetCurrentItemIndex(0);
	
	UpdateScrollBar(ListBox);
	ListBox->DrawNow();
	
	return ListBox;
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CPluInContainer::ConstructEditorL(void)
{
	iTmpPlugIn= NULL;
	iTmpSupArray.ResetAndDestroy();

	delete iEditor;
	iEditor = NULL;
	iEditor = new (ELeave) CEikGlobalTextEditor;
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	iEditor->SetContainerWindowL(*this);
	iEditor->SetAvkonWrap(ETrue);
	iEditor->ConstructL(this,10,0,EAknEditorFlagDefault,EGulFontControlAll,EGulAllFonts);
	iEditor->SetBorder(TGulBorder::ESingleBlack);
	iEditor->SetRect(Rect());
	
	TCharFormat		charFormat;	
    TCharFormatMask charFormatMask;	

	charFormat.iFontSpec.iTypeface.SetIsProportional(ETrue); // Arial is proportional 
	charFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont)->FontSpecInTwips();;

	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	TRgb textcol;
	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
	charFormat.iFontPresentation.iTextColor = TLogicalRgb(textcol);
	
    charFormatMask.SetAll();
	
	delete iParaformMe;
	iParaformMe = NULL;
	iParaformMe = CParaFormatLayer::NewL(); 
	
	delete iCharformMe;
	iCharformMe = NULL;
	iCharformMe = CCharFormatLayer::NewL(charFormat,charFormatMask); 
	
	delete iRichMessageMe;
	iRichMessageMe = NULL;
	iRichMessageMe  = CRichText::NewL(iParaformMe,iCharformMe);    
  
	TBool IsHandler(EFalse);  
  	CImplementationInformation* Sel= CurrPlugIn(IsHandler);
	if(Sel)
	{	
		iTmpPlugIn= Sel;
		AddPlugInInfoL(*iRichMessageMe,*Sel,IsHandler);
	}

	iEditor->SetDocumentContentL(*iRichMessageMe);
	iEditor->SetCursorPosL(0,EFalse);

	iEditor->SetFocus(EFalse);
	iEditor->SetFocusing(ETrue);
	iEditor->ClearSelectionL();
	
	iEditor->UpdateScrollBarsL();
	iEditor->DrawNow();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CImplementationInformation* CPluInContainer::CurrPlugIn(TBool& aHandler)
{
	CImplementationInformation* Ret(NULL);
	
	if(iTabGroup)
	{	
		TInt Inddex = GetSelectedIndex();
			
		if(iTabGroup->ActiveTabIndex() == 1)
		{
			aHandler = EFalse;
			if(Inddex >= 0 && Inddex < iRecognizerArray.Count())
			{
				Ret = iRecognizerArray[Inddex];
			}
		}
		else
		{
			aHandler = ETrue;
			if(Inddex >= 0 && Inddex < iHandlerArray.Count())
			{
				Ret = iHandlerArray[Inddex];
			}
		}
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CPluInContainer::AddPlugInInfoL(CRichText& aText,CImplementationInformation& aInfo,const TBool& aHandler)
{	
	iWasDefHand = iIsOpenWith = iCanHandModify = EFalse;
	
	TBuf<255> smallHjelp;
	
	StringLoader::Load(smallHjelp,R_SH_STR_NAMEDD);
	aText.InsertL(aText.LdDocumentLength(),smallHjelp);
	aText.InsertL(aText.LdDocumentLength(),aInfo.DisplayName());
	aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
	
	StringLoader::Load(smallHjelp,R_SH_STR_STATUSDD);
	aText.InsertL(aText.LdDocumentLength(),smallHjelp);
	
	if(aInfo.Disabled())
		StringLoader::Load(smallHjelp,R_SH_STR_DISABLED);
	else
		StringLoader::Load(smallHjelp,R_SH_STR_ENABLED);
	
	aText.InsertL(aText.LdDocumentLength(),smallHjelp);
	
	aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);

	TFileName hjelpBuffer;
	if(aHandler)
	{
		TRAPD(ErrNum,
		TInt ItemStart 	= aInfo.OpaqueData().Find(KtxStrtYTools);
		TInt ItemEnd 	= aInfo.OpaqueData().Find(KtxEnddYTools);
		
		if(ItemStart != KErrNotFound
		&& ItemEnd != KErrNotFound)
		{
			ItemStart = ItemStart + KtxStrtYTools().Length();
			ItemEnd = (ItemEnd - ItemStart);
			
			if(ItemEnd > 0)
			{
				StringLoader::Load(smallHjelp,R_SH_STR_PLVERSION);
				aText.InsertL(aText.LdDocumentLength(),smallHjelp);
				
				hjelpBuffer.Zero();
				iIconHandler->GetVersionL(aInfo.OpaqueData().Mid(ItemStart,ItemEnd),hjelpBuffer);
				
				aText.InsertL(aText.LdDocumentLength(),hjelpBuffer);
				
				if(iIconHandler->CheckVersionL(aInfo.OpaqueData().Mid(ItemStart,ItemEnd)))
					StringLoader::Load(smallHjelp,R_SH_STR_VEROK);
				else
					StringLoader::Load(smallHjelp,R_SH_STR_VERNOTOK);
				
				aText.InsertL(aText.LdDocumentLength(),smallHjelp);
					
				aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
				
				CFHandlerItem* NewItem = new(ELeave)CFHandlerItem();
				CleanupStack::PushL(NewItem);
		
				if(iIconHandler->CheckSupportedTypesL(aInfo.OpaqueData().Mid(ItemStart,ItemEnd),*NewItem))
				{
					StringLoader::Load(smallHjelp,R_SH_STR_PLTYPE);
					aText.InsertL(aText.LdDocumentLength(),smallHjelp);
					aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
					
					if(NewItem->iFlags & EYBFilehandlerSupportsNewFileNoFiles
					|| NewItem->iFlags & EYBFilehandlerSupportsNewFileMultiple
					|| NewItem->iFlags & EYBFilehandlerSupportsAddFiles
					|| NewItem->iFlags & EYBFilehandlerSupportsRFile
					|| (NewItem->iFlags == 0))
					{
						StringLoader::Load(smallHjelp,R_SH_STR_FILHAND);
						aText.InsertL(aText.LdDocumentLength(),smallHjelp);
						aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
					
						iCanHandModify = ETrue;
					}
					
					if(NewItem->iFlags & EYBFilehandlerSupportsSendFiles)
					{
						StringLoader::Load(smallHjelp,R_SH_STR_FILSEND);
						aText.InsertL(aText.LdDocumentLength(),smallHjelp);
						aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
					}
					
					if(NewItem->iFlags & EYBFilehandlerSupportsRecvFiles)
					{
						StringLoader::Load(smallHjelp,R_SH_STR_FILRECE);
						aText.InsertL(aText.LdDocumentLength(),smallHjelp);
						aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
					}
					
					if(NewItem->iFlags & EYBFilehandlerAddtoFoldersList)
					{	
						StringLoader::Load(smallHjelp,R_SH_STR_FILFLDR);
						aText.InsertL(aText.LdDocumentLength(),smallHjelp);
						aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
					}
					
					if(NewItem->iFlags & EYBFilehandlerSupportsOpenWith)
					{
						StringLoader::Load(smallHjelp,R_SH_STR_OPWTHHAND);
						aText.InsertL(aText.LdDocumentLength(),smallHjelp);
						aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
						
						iIsOpenWith = iCanHandModify = ETrue;
						
						smallHjelp.Zero();
						TUint32 tmpHjlp(0);
						
						if(iMyDBClass->GetDefaultHandL(smallHjelp,tmpHjlp))
						{
							if(aInfo.DisplayName() == smallHjelp
							&& ( tmpHjlp == (TUint32)aInfo.ImplementationUid().iUid))
							{
								iWasDefHand = ETrue;
							}
						}
					}
					
					aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
						
					iIconHandler->CheckSupArrayL(aInfo.DisplayName(),((TUint32)aInfo.ImplementationUid().iUid),NewItem->iSupArray,*iMyDBClass);
				
					if(NewItem->iSupArray.Count())
					{	
						StringLoader::Load(smallHjelp,R_SH_STR_SUPFTYUPS);
						aText.InsertL(aText.LdDocumentLength(),smallHjelp);
						aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
						
						for(TInt s=0; s < NewItem->iSupArray.Count(); s++)
						{
							if(NewItem->iSupArray[s])
							{
								if(NewItem->iSupArray[s]->iIdString)
								{
									aText.InsertL(aText.LdDocumentLength(),KTxJustALine);
									aText.InsertL(aText.LdDocumentLength(),*NewItem->iSupArray[s]->iIdString);
									
									if(NewItem->iSupArray[s]->iFlags & EYBFilehandlerSupportsNewFileNoFiles)
									{
										StringLoader::Load(smallHjelp,R_SH_STR_SUPNFILS);
										aText.InsertL(aText.LdDocumentLength(),smallHjelp);
									}
									
									aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
								}
							
							
								iTmpSupArray.Append(NewItem->iSupArray[s]);
								NewItem->iSupArray[s] = NULL;
							}
						}
					}
				}
				
				CleanupStack::PopAndDestroy(NewItem);
			}
		});
		
	}
	else
	{
		StringLoader::Load(smallHjelp,R_SH_STR_PLVERSION);
		aText.InsertL(aText.LdDocumentLength(),smallHjelp);

		hjelpBuffer.Zero();
		iIconHandler->GetVersionL(aInfo.OpaqueData(),hjelpBuffer);
		
		aText.InsertL(aText.LdDocumentLength(),hjelpBuffer);

		if(iIconHandler->CheckVersionL(aInfo.OpaqueData()))
			StringLoader::Load(smallHjelp,R_SH_STR_VEROK);
		else
			StringLoader::Load(smallHjelp,R_SH_STR_VERNOTOK);
		
		aText.InsertL(aText.LdDocumentLength(),smallHjelp);	
		aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
					
		StringLoader::Load(smallHjelp,R_SH_STR_PLTYPE);
		aText.InsertL(aText.LdDocumentLength(),smallHjelp);
		aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
		
		StringLoader::Load(smallHjelp,R_SH_STR_FILRECO);
		aText.InsertL(aText.LdDocumentLength(),smallHjelp);
		aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
		
		TRAPD(ErrNum,
		TAny* impl = REComSession::CreateImplementationL(aInfo.ImplementationUid(), _FOFF(CYBrowserFileRecognizer,iDestructorIDKey));
		if ( impl )
		{
			CYBrowserFileRecognizer* pluGin = STATIC_CAST(CYBrowserFileRecognizer*,impl);
		
			HBufC* credits = pluGin->GetCreditsL();
			if(credits)
			{
				StringLoader::Load(smallHjelp,R_MNU_ABOUT);
				
				aText.InsertL(aText.LdDocumentLength(),smallHjelp);
				aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
				
				TBuf<1> LineFeedBuf(KTxJustAZero);
				LineFeedBuf[0] = 0x000A;

				TInt Ret = credits->Des().Find(LineFeedBuf);
				if(Ret != KErrNotFound)
				{
					TInt i = 0;

					do
					{
						aText.InsertL(aText.LdDocumentLength(),credits->Des().Mid(i,Ret));
						aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
						
						i = i + Ret + 1;
						Ret = credits->Des().Right(credits->Des().Length() - i).Find(LineFeedBuf);

					}while(Ret != KErrNotFound && i < credits->Des().Length());

					if(credits->Des().Length() > i && iRichMessageMe)
						aText.InsertL(aText.LdDocumentLength(),credits->Des().Right(credits->Des().Length() - i));
				}
				else if(iRichMessageMe)
					aText.InsertL(aText.LdDocumentLength(),credits->Des());

				aText.InsertL(aText.LdDocumentLength(),CEditableText::ELineBreak);
			}
			delete credits;
			credits = NULL;
			delete pluGin;
			pluGin = NULL;
		});	
	}
}

	/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CPluInContainer::UpdateScrollBar(CEikListBox* aListBox)
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
Y-Browser will call setsize for client rect when lay-out or size changes,
This will be then called by the framework
-----------------------------------------------------------------------------
*/
void CPluInContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	if(iOwnFilTypArray)
	{
		TInt SelInd(-1);
		
		if(iOwnListBox)
		{
			SelInd = iOwnListBox->View()->CurrentItemIndex();
		}
		
		delete iOwnListBox;
		iOwnListBox = NULL;
	 	iOwnListBox = ConstructListBoxL(SelInd,iOwnFilTypArray);
	}	
	
	if(iSysFilTypArray)
	{
		TInt SelInd(-1);
		
		if(iSysListBox)
		{
			SelInd = iSysListBox->View()->CurrentItemIndex();
		}
		
		delete iSysListBox;
		iSysListBox = NULL;
	 	iSysListBox = ConstructListBoxL(SelInd,iSysFilTypArray);
	}
	
	if(iTypesListbox)
	{
		iTypesListbox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	if(iEditor)
	{
		iEditor->SetRect(Rect());
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CPluInContainer::MopSupplyObject(TTypeUid aId)
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
void CPluInContainer::SetMenuL(void)
{
	if(iEditor)
	{
//		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_PLUGINMOD_MENUBAR);
		
		if(iCanHandModify)
			iCba->SetCommandSetL(R_PLUGININFO2_CBA);
		else
			iCba->SetCommandSetL(R_PLUGININFO1_CBA);
		
		iCba->DrawDeferred();
	}
	else if(iTypesListbox)
	{	
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_PLUGINMOD_MENUBAR);
		iCba->SetCommandSetL(R_PLUGINMOD_CBA);
		iCba->DrawDeferred();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_PLUGIN_MENUBAR);				
		iCba->SetCommandSetL(R_PLUGIN_CBA);
		iCba->DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPluInContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(aResourceId == R_PLUGIN_MENU)
	{
		TBool Dimopen(ETrue),DimEna(ETrue);
		
		TBool IsAHand(EFalse);
		CImplementationInformation* Sel= CurrPlugIn(IsAHand);
		if(Sel)
		{
			if(Sel->Disabled())
			{
				DimEna = EFalse;
			}
		}
	
		if(CurrentListBox())
		{
			TInt CurrInd = CurrentListBox()->View()->CurrentItemIndex();
			if(CurrInd >= 0)
			{
				Dimopen = EFalse;		
			}
		}
		
		if(Dimopen)
		{
			aMenuPane->SetItemDimmed(EPluginInfo,ETrue);
			aMenuPane->SetItemDimmed(EPluginEnable,ETrue);
			aMenuPane->SetItemDimmed(EPluginDisable,ETrue);
		}
		
		if(DimEna)
			aMenuPane->SetItemDimmed(EPluginEnable,ETrue);
		else
			aMenuPane->SetItemDimmed(EPluginDisable,ETrue);
	}
}

	
	
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
void CPluInContainer::RemoveNavipanel(void) 
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
	
    delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
    delete iNaviDecorator;
    iNaviDecorator = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPluInContainer::MakeNavipanelL(void)
{
	CEikStatusPane *sp = ((CAknAppUi*)iEikonEnv->EikAppUi())->StatusPane();
	iNaviPane = (CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

	CAknNavigationDecorator* iNaviDecoratorForTabsTemp = iNaviPane->CreateTabGroupL();
	delete iNaviDecoratorForTabs;
    iNaviDecoratorForTabs = NULL;
	iNaviDecoratorForTabs = iNaviDecoratorForTabsTemp;
   	iTabGroup = STATIC_CAST(CAknTabGroup*, iNaviDecoratorForTabs->DecoratedControl());
	iTabGroup->SetTabFixedWidthL(EAknTabWidthWithTwoLongTabs);//EAknTabWidthWithOneTab);

	TBuf<60> smallHjelp;
	StringLoader::Load(smallHjelp,R_SH_STR_HANDLERS);
	iTabGroup->AddTabL(0,smallHjelp);
	
	StringLoader::Load(smallHjelp,R_SH_STR_RECOGNIZERS);
	iTabGroup->AddTabL(1,smallHjelp);
	
	if(iRecOpen)	
		iTabGroup->SetActiveTabByIndex(1);
	else
		iTabGroup->SetActiveTabByIndex(0);
	
	iNaviPane->PushL(*iNaviDecoratorForTabs);
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPluInContainer::ShowNoteL(const TDesC&  aMessage, TBool aError)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
		
	TInt NoteId(0); 
	
	if(aError)
		NoteId = dialog->ShowNoteL(EAknGlobalErrorNote,aMessage);
	else
		NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	
	User::After(1200000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
TBool CPluInContainer::CheckReFresh(void)
{
	return iChanged;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/	
void CPluInContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EPluginEnable:
	case EPluginDisable:
		if(iMyDBClass)
		{
			TBool IsHandler(EFalse);
			CImplementationInformation* Sel= CurrPlugIn(IsHandler);
			if(Sel)
			{
				TInt selIndex = GetSelectedIndex();
				
				TBool Recognizer(ETrue);
				if(IsHandler)
				{
					Recognizer = EFalse;
				}
				
				TBool Changes(EFalse);
				
				if(aCommand == EPluginEnable)
				{
					if(iMyDBClass->SetEnabledL(Sel->DisplayName(),((TUint32)Sel->ImplementationUid().iUid),Recognizer))
					{
						Sel->SetDisabled(EFalse);
						Changes = ETrue;
					}
				}
				else
				{
					TInt NewIndex(-1);

					iMyDBClass->SetDisabledL(Sel->DisplayName(),((TUint32)Sel->ImplementationUid().iUid),Recognizer,NewIndex);
				
					if(NewIndex >= 0)
					{		
						Changes= ETrue;		
						Sel->SetDisabled(ETrue);
					}	
				}
				
				if(Changes)
				{
					iChanged = ETrue;
								
					RPointerArray<CDisabledItem> DisabledArray;

					iMyDBClass->GetDisabledL(DisabledArray);
		
					if(IsHandler)
					{
	 					ReReadHandlersL(DisabledArray);
						if(iSysFilTypArray)
						{
							delete iSysListBox;
							iSysListBox = NULL;
						 	iSysListBox = ConstructListBoxL(selIndex,iSysFilTypArray);
						}
					}
					else
					{
						ReReadRecognizersL(DisabledArray);
						if(iOwnFilTypArray)
						{
							delete iOwnListBox;
							iOwnListBox = NULL;
						 	iOwnListBox = ConstructListBoxL(selIndex,iOwnFilTypArray);
						}	
					}
					
					DisabledArray.ResetAndDestroy();
				}
			}
		}
		SetMenuL();
		DrawNow();
		break;
	case EPluginInfo:
		{
			iRecOpen = EFalse;
			if(iTabGroup)
			{
				if(iTabGroup->ActiveTabIndex() == 1)
				{
					iRecOpen = ETrue;
				}
			}
		
			ConstructEditorL();
			RemoveNavipanel();
		}
		SetMenuL();
		DrawNow();
		break;
	case EPluginInfoBack:
		{
			MakeNavipanelL();
			iTmpPlugIn= NULL;
			iTmpSupArray.ResetAndDestroy();
			delete iEditor;
			iEditor = NULL;
			
		}
		SetMenuL();
		DrawNow();
		break;
	case EPluginShowFileTypes:
		if(iTypesListbox)
		{
			ShowFileTypesL(iTmpSupArray);
			iTypesListbox->iTypes.Num(iTmpSupArray.Count());
			iTypesListbox->LoadSettingsL();
		}
		DrawNow();
		break;
	case EPluginAddFileTypes:
		if(iTypesListbox)
		{
			AddFileTypesL(iTmpSupArray);
			iTypesListbox->iTypes.Num(iTmpSupArray.Count());
			iTypesListbox->LoadSettingsL();
		}
		DrawNow();
		break;
	case EPluginFileAss:
		{
			// do not delete now yet..or make navi panel just yet.
			//iTmpSupArray.ResetAndDestroy();
			//MakeNavipanelL();
			
			delete iEditor;
			iEditor = NULL;
			
			delete iTypesListbox;
		    iTypesListbox = NULL;

		    iTypesListbox = new (ELeave) CPluInListbox;
		    
		    iTypesListbox->iTypes.Zero();
		    iTypesListbox->iName.Zero();
		    
			if(iTmpPlugIn)
			{
				iTypesListbox->iName.Copy(iTmpPlugIn->DisplayName());
				iTypesListbox->iUid = (TUint32)iTmpPlugIn->ImplementationUid().iUid;
			}
			
			// get change information from DB & open with handler additional info.
			
			iTypesListbox->iTypes.Num(iTmpSupArray.Count());
			
		    if(iIsOpenWith)
		    {
			    iTypesListbox->iUnRecognized = iWasDefHand;
		    }
		    
		   	if(iIsOpenWith)
		    	iTypesListbox->ConstructFromResourceL(R_PLUGIN2_SETTING);	    
		   	else
		   		iTypesListbox->ConstructFromResourceL(R_PLUGIN1_SETTING);
		   	
			iTypesListbox->MakeVisible(ETrue);
		    iTypesListbox->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		    iTypesListbox->ActivateL();			
			iTypesListbox->LoadSettingsL();
		    iTypesListbox->DrawNow();
		}
		SetMenuL();
		DrawNow();
		break;
	case EPluginResetTypes:
		if(iTypesListbox)
		{
			if(iTypesListbox->iUid > 0 && iMyDBClass)
			{
				iMyDBClass->DeleteHandlerTypesOWRL(iTypesListbox->iName, iTypesListbox->iUid);			
			}
		
			iTmpPlugIn = NULL;
			iTmpSupArray.ResetAndDestroy();
			MakeNavipanelL();
			delete iTypesListbox;
			iTypesListbox = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
		break;
	case EPluginModBack:
		if(iTypesListbox)
		{
			TBool DisableMe(EFalse);
			
			iTypesListbox->StoreSettingsL();
			
			if(iIsOpenWith)
			{
				if(iTypesListbox->iUnRecognized && !iWasDefHand)
				{
					iChanged = ETrue;
					iMyDBClass->SetDefaultHandL(iTypesListbox->iName, iTypesListbox->iUid);
				}
				else if (iWasDefHand && !iTypesListbox->iUnRecognized)
				{
					iChanged = ETrue;
					iMyDBClass->RemoveDefaultHandL();		
				}
			
				iWasDefHand = EFalse;
			}
			
			if(iFtypesChanged && iTypesListbox->iUid > 0 && iMyDBClass)
			{
				iChanged = ETrue;
				
				CDesCArrayFlat* tmpArr = new(ELeave)CDesCArrayFlat(10);
				CleanupStack::PushL(tmpArr);
					
				iMyDBClass->DeleteHandlerTypesOWRL(iTypesListbox->iName, iTypesListbox->iUid);
				
				if(iTmpSupArray.Count())
				{
					for(TInt cc=0; cc < iTmpSupArray.Count(); cc++)
					{
						if(iTmpSupArray[cc])
						{
							if(iTmpSupArray[cc]->iIdString)
							{
								tmpArr->AppendL(*iTmpSupArray[cc]->iIdString);
							}
						}
					}
				}
				
				if(!tmpArr->MdcaCount())
				{
					if(!iIsOpenWith)
					{
						DisableMe = ETrue;		
					}
				}
				else
				{
					iMyDBClass->SetHandlerTypesOWRL(iTypesListbox->iName, iTypesListbox->iUid,*tmpArr);
				}
				
				CleanupStack::PopAndDestroy(tmpArr);			
			}
		
			iTmpPlugIn = NULL;
			iTmpSupArray.ResetAndDestroy();
			MakeNavipanelL();
			delete iTypesListbox;
			iTypesListbox = NULL;
			
			if(DisableMe)
			{
				HandleViewCommandL(EPluginDisable);
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
void CPluInContainer::ShowFileTypesL(RPointerArray<CSupportTypeItem>& aSupArray)
{
	CArrayFixFlat<TInt>* SelectedItems = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelectedItems);
	CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
	CleanupStack::PushL(SelectArrray);

	TBuf<255> hjelpp;
	for(TInt p=0; p < aSupArray.Count() ; p++)
	{
		hjelpp.Copy(_L("1\t"));
		
		if(aSupArray[p])
		{
			if(aSupArray[p]->iIdString)
			{
				hjelpp.Append(*aSupArray[p]->iIdString);
			}
		}
		
		SelectedItems->AppendL(p);
		SelectArrray->AppendL(hjelpp);
	}

	CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(SelectedItems);

	dialog->PrepareLC(R_OWRTYPES_SELECTION_QUERY);

	dialog->SetItemTextArray(SelectArrray); 
	dialog->SetOwnershipType(ELbmDoesNotOwnItemArray); 
	dialog->SetIconArrayL(iIconHandler->GetSelectionIconArrayL()); 

	if (dialog->RunLD ())
	{
		for(TInt i=0; i < SelectedItems->Count(); i++)
		{
			iFtypesChanged = ETrue;
			
			TInt selected = SelectedItems->At(i);
			
			if(selected >= 0 && selected < aSupArray.Count())
			{
				CSupportTypeItem* tmp = aSupArray[selected];
				aSupArray[selected] = NULL;
				delete tmp;
				tmp = NULL;
			}
		}
		
		for(TInt pp=(aSupArray.Count() - 1); pp > -1 ; pp--)
		{
			if(!aSupArray[pp])
			{
				aSupArray.Remove(pp);
			}
		}
	}

	CleanupStack::PopAndDestroy(2);  //SelectedItems,SelectArrray					
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CPluInContainer::AddFileTypesL(RPointerArray<CSupportTypeItem>& aSupArray)
{
	CArrayFixFlat<TInt>* SelectedItems = new(ELeave)CArrayFixFlat<TInt>(10);
	CleanupStack::PushL(SelectedItems);
	
	CDesCArrayFlat* SelectArrray = new(ELeave)CDesCArrayFlat(20);
	CleanupStack::PushL(SelectArrray);
	
	RPointerArray<CFFileTypeItem> ItemTypeArray  = iIconHandler->GetItemTypeArray();
	
	CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(SelectedItems);
	dialog->PrepareLC(R_TYPES_SELECTION_QUERY);
	
	TBool AddNow(ETrue);
	
	TBuf<255> Hjelpper;
	for(TInt i=0; i < ItemTypeArray.Count(); i++)
	{
		if(ItemTypeArray[i])
		{
			if(ItemTypeArray[i]->iIdString) 
			{
				AddNow = ETrue;
				
				for(TInt pp=0; pp < aSupArray.Count(); pp++)
				{
					if(aSupArray[pp])
					{
						if(aSupArray[pp]->iIdString)
						{
							if(aSupArray[pp]->iIdString->Des() == ItemTypeArray[i]->iIdString->Des())
							{
								AddNow = EFalse;
							}
						}
					}
				}
				
				if(AddNow)
				{
					Hjelpper.Num(ItemTypeArray[i]->iTypeId);
					Hjelpper.Append(_L("\t"));
					Hjelpper.Append(ItemTypeArray[i]->iIdString->Des());
				
					SelectArrray->AppendL(Hjelpper);
				}
				else
				{
					ItemTypeArray.Remove(i);
				}
			}
		}
	}

	dialog->SetItemTextArray(SelectArrray); 
	dialog->SetOwnershipType(ELbmDoesNotOwnItemArray); 
	dialog->SetIconArrayL(iIconHandler->GetIconArrayL()); 

	if (dialog->RunLD ())
	{
		for(TInt i=0; i < SelectedItems->Count(); i++)
		{
			TInt SelType = SelectedItems->At(i);
		
			if(SelType >=0 && SelType < ItemTypeArray.Count())
			{
				if(ItemTypeArray[SelType])
				{
					if(ItemTypeArray[SelType]->iIdString) 
					{	
						CSupportTypeItem* SpIttem = new(ELeave)CSupportTypeItem();
						CleanupStack::PushL(SpIttem);

						SpIttem->iIdString = ItemTypeArray[SelType]->iIdString->Des().AllocL();
						SpIttem->iPartialSting = EFalse;		
						SpIttem->iFlags = 0;		

						CleanupStack::Pop();//SpIttem
						aSupArray.Append(SpIttem);
						iFtypesChanged = ETrue;
					}
				}
			}
		}
	}

	CleanupStack::PopAndDestroy(2);  //SelectedItems,SelectArrray
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CPluInContainer::ReReadHandlersL(RPointerArray<CDisabledItem>& aArray) 
{
	delete iSysFilTypArray;
	iSysFilTypArray = NULL;
	iSysFilTypArray = new(ELeave)CDesCArrayFlat(10);

	iHandlerArray.ResetAndDestroy();
	
	TFileName myHejlper;
	
	REComSession::ListImplementationsL(KUidYHandlerUID, iHandlerArray);
	
	for ( TInt i = 0; i < iHandlerArray.Count(); i++ )
	{
		myHejlper.Copy(KTxIndexZero);
		if(iHandlerArray[i])
		{
			for(TInt p=0; p < aArray.Count(); p++)
			{
				if(aArray[p])
				{
					if(aArray[p]->iUid == ((TUint32)iHandlerArray[i]->ImplementationUid().iUid)
					&& aArray[p]->iName
					&& !aArray[p]->iRecognizer)
					{
						if(aArray[p]->iName->Des() == iHandlerArray[i]->DisplayName())
						{
							myHejlper.Copy(KTxIndexOne);
							iHandlerArray[i]->SetDisabled(ETrue);	
						}
					}
				}
			}
		
			myHejlper.Append(iHandlerArray[i]->DisplayName());
		}
		
		iSysFilTypArray->AppendL(myHejlper);
	}
}
   
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CPluInContainer::ReReadRecognizersL(RPointerArray<CDisabledItem>& aArray) 
{
	delete iOwnFilTypArray;
	iOwnFilTypArray = NULL;
	iOwnFilTypArray = new(ELeave)CDesCArrayFlat(10);

	iRecognizerArray.ResetAndDestroy();
	
	TFileName myHejlper;
	
	REComSession::ListImplementationsL(KUidYRecogzrUID, iRecognizerArray);
	
	for ( TInt i = 0; i < iRecognizerArray.Count(); i++ )
	{
		myHejlper.Copy(KTxIndexZero);
		if(iRecognizerArray[i])
		{
			for(TInt p=0; p < aArray.Count(); p++)
			{
				if(aArray[p])
				{
					if(aArray[p]->iUid == ((TUint32)iRecognizerArray[i]->ImplementationUid().iUid)
					&& aArray[p]->iName
					&& aArray[p]->iRecognizer)
					{
						if(aArray[p]->iName->Des() == iRecognizerArray[i]->DisplayName())
						{
							myHejlper.Copy(KTxIndexOne);
							iRecognizerArray[i]->SetDisabled(ETrue);	
						}
					}
				}
			}
			
			myHejlper.Append(iRecognizerArray[i]->DisplayName());
		}
		
		iOwnFilTypArray->AppendL(myHejlper);
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TInt CPluInContainer::GetSelectedIndex(void) 
{
	TInt Ret(-1);
	
	if(CurrentListBox())
	{
		Ret = CurrentListBox()->View()->CurrentItemIndex();		
	}	
	
	return Ret;
}
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
CAknSingleGraphicStyleListBox* CPluInContainer::CurrentListBox(void) const
{
	CAknSingleGraphicStyleListBox* Ret = iSysListBox;
	if(iTabGroup)
	{
		if(iTabGroup->ActiveTabIndex() == 1)
		{
			Ret = iOwnListBox;
		}
	}
	
	return Ret;
}
				
/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
TKeyResponse CPluInContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	if(iEditor)
	{
		switch (aKeyEvent.iCode)
    	{
		case EKeyUpArrow:
			if(iEditor)
			{
				iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		        iEditor->UpdateScrollBarsL();
		//		iEditor->DrawNow();
				Ret = EKeyWasConsumed;
			}
			DrawNow();
			break;
		case EKeyDownArrow:
			if(iEditor)
			{
				iEditor->MoveCursorL(TCursorPosition::EFPageDown,EFalse);
				iEditor->UpdateScrollBarsL();
		//		iEditor->DrawNow();
				Ret = EKeyWasConsumed;
			}
			DrawNow();
		default:
			break;
		}
	}
	else if(iTypesListbox)
	{
		Ret = iTypesListbox->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
		case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case '2':
			if(CurrentListBox())
			{
				CurrentListBox()->View()->MoveCursorL(CListBoxView::ECursorPreviousPage,CListBoxView::ENoSelection);
				CurrentListBox()->UpdateScrollBarsL();
				CurrentListBox()->DrawNow();
			}
			DrawNow();
			break;
		case '8':
			if(CurrentListBox())
			{
				CurrentListBox()->View()->MoveCursorL(CListBoxView::ECursorNextPage,CListBoxView::ENoSelection);
				CurrentListBox()->UpdateScrollBarsL();
				CurrentListBox()->DrawNow();
			}
			DrawNow();
			break;
		case EKeyRightArrow:
			if(iTabGroup)
			{
				if(iTabGroup->ActiveTabIndex() != 1)
				{
					iTabGroup->SetActiveTabByIndex(1);
				}
			}
			DrawNow();
			break;
		case EKeyLeftArrow:
			if(iTabGroup)
			{
				if(iTabGroup->ActiveTabIndex() != 0)
				{
					iTabGroup->SetActiveTabByIndex(0);
				}
			}
			DrawNow();
			break;
		case EKeyDownArrow:
		case EKeyUpArrow:
		default:
			if(CurrentListBox())
			{
				Ret = CurrentListBox()->OfferKeyEventL(aKeyEvent,aType);
			}
			break;
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------

-----------------------------------------------------------------------
*/
void CPluInContainer::Draw(const TRect& /*aRect*/) const
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
TInt CPluInContainer::CountComponentControls() const
{
	if(iEditor)
		return 1;
	else if(iTypesListbox)
		return 1;
	else if(CurrentListBox())
		return 1;
	else
		return 0;
}
/*
-------------------------------------------------------------------------

-------------------------------------------------------------------------
*/
CCoeControl* CPluInContainer::ComponentControl(TInt /*aIndex*/	) const
{
	if(iEditor)
		return iEditor;
	else if(iTypesListbox)
		return iTypesListbox;
	else
		return CurrentListBox();
}

