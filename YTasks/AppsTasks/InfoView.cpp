/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/
#include <eikedwin.h>
#include <aknenv.h>
#include <stringloader.h>
#include <AknUtils.h>
#include <BARSREAD.H>
#include <TXTRICH.H>
#include <eikappui.h>
#include <APGCLI.H>
#include <documenthandler.h> 

#include "InfoView.h"

#include "AppsContainer.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CInfoView* CInfoView::NewL(const TUid aAppUid)
	{
	CInfoView* self = new(ELeave)CInfoView(aAppUid);
	self->ConstructL();
	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CInfoView::CInfoView(const TUid aAppUid)
:iAppUid(aAppUid)
    {
	// no implementation required
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

CInfoView::~CInfoView()
{
	delete iEditor;
	iEditor=NULL;
	delete iParaformMe;
	delete iCharformMe;
	delete iRichMessageMe; 
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CInfoView::ConstructL(void)
    {
    CreateWindowL();
    
    SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());

	ActivateL();
	DrawNow();
 }
 /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CInfoView::MakeEditorL(void)
 {
 	iTitleFont = AknLayoutUtils::FontFromId(EAknLogicalFontTitleFont );

	TRect MyRect(Rect());
	
	MyRect.iTl.iY = (MyRect.iTl.iY  + ((iTitleFont->HeightInPixels() * 3) /2));
	
	delete iEditor;
	iEditor = NULL;
	iEditor = new (ELeave) CEikRichTextEditor;
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	iEditor->SetContainerWindowL(*this);
	iEditor->SetAvkonWrap(ETrue);
	iEditor->ConstructL(this,10,0,EAknEditorFlagDefault,EGulFontControlAll,EGulAllFonts);
	iEditor->SetBorder(TGulBorder::ESingleBlack);
	
	iEditor->SetRect(MyRect);

	TCharFormat		charFormat;	
    TCharFormatMask charFormatMask;	

	charFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontSpecInTwips();
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
	
	TBuf<200> TitleName;
	HBufC* Abbout = GetApplicationInfoLC(iAppUid,iNameBuffer);
	
	AddTextToEditorL(*Abbout);
	
	CleanupStack::PopAndDestroy(Abbout);
	
	iEditor->SetDocumentContentL(*iRichMessageMe);
	iEditor->SetCursorPosL(0,EFalse);
	
	iEditor->SetFocus(EFalse);
	iEditor->SetFocusing(ETrue);
	iEditor->ClearSelectionL();	
	iEditor->SetCursorPosL(0,EFalse);
	
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
 }
 
 /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC* CInfoView::GetApplicationInfoLC(TUid aAppUid,TDes& aTitle)
{
	HBufC* RetBuff = HBufC::NewLC(10000);
	
	RApaLsSession ls;
	User::LeaveIfError(ls.Connect());
	CleanupClosePushL(ls);
	
	TApaAppInfo AppInfo;
		
	ls.GetAppInfo(AppInfo,aAppUid);
	
	TParsePtrC TitleHelp(AppInfo.iFullName);
	
	aTitle.Copy(TitleHelp.NameAndExt());
	
	RetBuff->Des().Copy(KtxAppUid);
	RetBuff->Des().AppendNum((TUint)iAppUid.iUid,EHex);
	
	RetBuff->Des().Append(KtxAppCaption);
	RetBuff->Des().Append(AppInfo.iCaption);
	RetBuff->Des().Append(KtxAppShortCaption);
	RetBuff->Des().Append(AppInfo.iShortCaption);
	
	TPckgBuf<TApaAppCapability> TApaAppCapabilityBuf;
	
	ls.GetAppCapability(TApaAppCapabilityBuf,aAppUid);
	
	RetBuff->Des().Append(KtxAppEmbeddability);
	
	switch(TApaAppCapabilityBuf().iEmbeddability)
	{
	case TApaAppCapability::ENotEmbeddable: /** An application cannot be embedded. */
		RetBuff->Des().Append(KtxAppNotEmbeddable);
		break;
	case TApaAppCapability::EEmbeddable:/** An application can be run embedded or standalone and can read/write embedded document-content. */
		RetBuff->Des().Append(KtxAppEmbeddable);
		break;
	case TApaAppCapability::EEmbeddableOnly:/** An application can only be run embedded and can read/write embedded document-content. */
		RetBuff->Des().Append(KtxAppEmbeddableOnly);
		break;
	case TApaAppCapability::EEmbeddableUiOrStandAlone:/** An application can be run embedded or standalone and cannot read/write embedded document-content. */
		RetBuff->Des().Append(KtxAppEmbeddableUiOrStandAlone);
		break;
	case TApaAppCapability::EEmbeddableUiNotStandAlone:/** An application can only be run embedded and cannot read/write embedded document-content. */
	 	RetBuff->Des().Append(KtxAppEmbeddableUiNotStandAlone);
	 	break;
	};
		
	RetBuff->Des().Append(KtxAppSupportsNewFile);

	if(TApaAppCapabilityBuf().iSupportsNewFile)
	{
		RetBuff->Des().Append(KtxYes);
	}
	else
	{
		RetBuff->Des().Append(KtxNo);
	}
	
	RetBuff->Des().Append(KtxAppHidden);
	
	if(TApaAppCapabilityBuf().iAppIsHidden)
	{
		RetBuff->Des().Append(KtxYes);
	}
	else
	{
		RetBuff->Des().Append(KtxNo);
	}
	
	RetBuff->Des().Append(KtxAppLaunchBackGround);
	
	if(TApaAppCapabilityBuf().iLaunchInBackground)	
	{
		RetBuff->Des().Append(KtxYes);
	}
	else
	{
		RetBuff->Des().Append(KtxNo);
	}
	
	if(TApaAppCapabilityBuf().iGroupName.Length())
	{
		RetBuff->Des().Append(KtxAppGroupName);
		RetBuff->Des().Append(TApaAppCapabilityBuf().iGroupName);
	}

	TLanguage appLanguage(ELangTest);
	
	ls.ApplicationLanguage(aAppUid,appLanguage);
	
	RetBuff->Des().Append(KtxAppLanguageCode);
	RetBuff->Des().AppendNum(appLanguage,EDecimal);

	CDesCArrayFlat* AppOwnedFiles = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(AppOwnedFiles);
	
	ls.GetAppOwnedFiles(*AppOwnedFiles,aAppUid);
	
	if(AppOwnedFiles->MdcaCount())
	{
		RetBuff->Des().Append(KtxAppOwedFiles);
	}
	
	for(TInt f=0; f < AppOwnedFiles->MdcaCount(); f++)
	{
		RetBuff->Des().Append(KtxLineFeedAndTab);
		RetBuff->Des().Append(AppOwnedFiles->MdcaPoint(f));
	}
	
	if(AppOwnedFiles->MdcaCount())
	{
		RetBuff->Des().Append(KtxLineFeed);
	}
	
	CleanupStack::PopAndDestroy(AppOwnedFiles);

	CDataTypeArray* allDataTypes = new(ELeave)CDataTypeArray(50);
	CleanupStack::PushL(allDataTypes);
	ls.GetSupportedDataTypesL(*allDataTypes);
	
	TBool HasHeaderAdded(EFalse);
	
	for(TInt i=0; i < allDataTypes->Count(); i++)
	{
		TUid HandlerAppUid;
		
		ls.AppForDataType(allDataTypes->At(i),HandlerAppUid);
		
		if(HandlerAppUid == aAppUid)
		{
			if(!HasHeaderAdded)
			{
				HasHeaderAdded = ETrue;
				RetBuff->Des().Append(KtxAppSupportsFileTypes);
			}
			
			RetBuff->Des().Append(allDataTypes->At(i).Des());
			RetBuff->Des().Append(KtxLineFeedAndTab);
		}
	}
	
	CleanupStack::PopAndDestroy(allDataTypes);
	
	RetBuff->Des().Append(KtxLineFeed);
	RetBuff->Des().Append(KtxLineFeed);
	
	
	CleanupStack::PopAndDestroy();//ls
	return RetBuff;	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoView::SizeChanged()
{
	MakeEditorL();
}

	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CInfoView::CountComponentControls() const
    {
	if(iEditor)
		return 1;
	else
		return 0;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoView::Draw(const TRect& aRect) const
    {    
	TRect rect(aRect);
	rect.SetHeight(22);
	rect.Shrink(1,1);
	rect.SetWidth(rect.Width()-2);
    CWindowGc& gc = SystemGc();
	gc.Clear();
	
    gc.SetBrushColor(KRgbBlue);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.DrawRect(rect);

	if(iTitleFont)
	{
		gc.UseFont(iTitleFont);
		
		TInt FreeSpace = (iTitleFont->HeightInPixels() / 6);
		TRect TxtRect(2,FreeSpace,(Rect().Size().iWidth - 2),(FreeSpace + iTitleFont->HeightInPixels()));
		
		gc.DrawText(iNameBuffer,TxtRect,iTitleFont->AscentInPixels(), CGraphicsContext::ELeft, 0);
		gc.DiscardFont();
	}
}
   
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CInfoView::ComponentControl(TInt /*aIndex*/) const
    {
       return iEditor;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

TKeyResponse CInfoView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
{
	TKeyResponse  MyRet = EKeyWasNotConsumed;

	
	switch (aKeyEvent.iCode)
    {
	case EKeyUpArrow:
		if(iEditor)
		{
			iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	        iEditor->UpdateScrollBarsL();
			iEditor->DrawNow();
			MyRet = EKeyWasConsumed;
		}
		DrawNow();
		break;
	case EKeyDownArrow:
		if(iEditor)
		{
			iEditor->MoveCursorL(TCursorPosition::EFPageDown,EFalse);
			iEditor->UpdateScrollBarsL();
			iEditor->DrawNow();
			MyRet = EKeyWasConsumed;
		}
		DrawNow();
		break;
	default:
		break;
	}        

	return MyRet;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoView::AddTextToEditorL(const TDesC& aStuff)
{
	
	if (iRichMessageMe)
	{	
		TBuf<1> LineFeedBuf(_L("0"));
		LineFeedBuf[0] = 0x000A;

		TInt Ret = aStuff.Find(LineFeedBuf);
		if(Ret != KErrNotFound)
		{
			TInt i = 0;

			do
			{
				if(iRichMessageMe)
				{
					iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),aStuff.Mid(i,Ret));
					iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),CEditableText::ELineBreak);
				}

				i = i + Ret + 1;
				Ret = aStuff.Right(aStuff.Length() - i).Find(LineFeedBuf);

			}while(Ret != KErrNotFound && i < aStuff.Length());

			if(aStuff.Length() > i && iRichMessageMe)
				iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),aStuff.Right(aStuff.Length() - i));
		}
		else if(iRichMessageMe)
			iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),aStuff);	
	
		 TDataType testType(_L8("audio/basic"));
		
		 CDocumentHandler* dbgtest = CDocumentHandler::NewL();
		 if(dbgtest->CanOpenL(testType))
		 {
		 	TUid DbgAppUid;
		 	TInt Errr = dbgtest->HandlerAppUid(DbgAppUid);
			
			
			TBuf<100> bdgbuf;
			bdgbuf.Num(Errr);
			bdgbuf.Append(_L(", UID = "));
		 	bdgbuf.AppendNum((TUint)DbgAppUid.iUid);
		 	
		 	TApaAppInfo AppInfo;
		 	
		 	RApaLsSession ls;
			ls.Connect();
			ls.GetAppInfo(AppInfo,DbgAppUid);
			ls.Close();
			
		 	iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),bdgbuf);	
			iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),AppInfo.iCaption);	
		 }
		 else
		 {
		 	iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),_L("Can not"));	
		 }
		 
		 delete dbgtest;
	}
}


