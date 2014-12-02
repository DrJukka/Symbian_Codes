/* 
	Copyright (c) 2001 - 2008, 
	Dr. Jukka Silvennoinen. 
	All rights reserved 
*/

#include <barsread.h>
#include <CHARCONV.H>
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
#include <EIKPROGI.H>
#include <AknQueryDialog.h>
#include <stringloader.h> 
#include <APGCLI.H>
#include <TXTRICH.H>
#include <eikclbd.h>
#include <EIKFUTIL.H>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <aknindicatorcontainer.h> 
#include <AknEditStateIndicator.h>
#include <eikrted.h> 

#include "SMS_Container.h"

#include "YApps_E8876008.h"
#include "YApps_E8876008.hrh"

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

const TInt KMaxMessageLength = 250;
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSMSContainer::CSMSContainer(void)
{		

}
	
	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSMSContainer::~CSMSContainer()
{  	
	delete iEditor;
	iEditor=NULL;
	delete iParaformMe;
	delete iCharformMe;
	delete iRichMessageMe; 
	
	delete iBgContext;
	iBgContext = NULL;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSContainer::ConstructL()
{
	CreateWindowL();	
	
	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMainMessage,TRect(0,0,1,1), ETrue);
	
	
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	TRgb textcol;
	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
	iTextColor = TLogicalRgb(textcol);
		
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
			
	ActivateL();
	
	iLastCount = 0;
	iCurrCount = KMaxMessageLength;
	
	DrawNow();
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	iUseAllFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);
	
	MakeEditorL();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSContainer::AddTextToEditorL(const TDesC& aText)
{
	if (iRichMessageMe && iEditor)
	{
		iRichMessageMe->Reset();
		SetTextsL(aText,*iRichMessageMe);
		
		iRichMessageMe->InsertL(iRichMessageMe->LdDocumentLength(),CEditableText::ELineBreak);
				
		iEditor->SetDocumentContentL(*iRichMessageMe);
		iEditor->SetCursorPosL(0,EFalse);
		iEditor->ClearSelectionL();	
		iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iEditor->UpdateScrollBarsL();
		iEditor->DrawNow();

		iCurrCount = KMaxMessageLength - iRichMessageMe->LdDocumentLength();
		UpdateCharacterCountL();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/		
void CSMSContainer::SetTextsL(const TDesC& aText,CRichText& aRichText)
{
	// this will be used to parse the text and set colors etc.


	TBuf<1> LineFeedBuf(_L("0"));
	LineFeedBuf[0] = 0x000A;

	TInt Ret = aText.Find(LineFeedBuf);
	if(Ret != KErrNotFound)
	{
		TInt i = 0;

		do
		{
			aRichText.InsertL(aRichText.LdDocumentLength(),aText.Mid(i,Ret));
			aRichText.InsertL(aRichText.LdDocumentLength(),CEditableText::ELineBreak);
			
			i = i + Ret + 1;
			Ret = aText.Right(aText.Length() - i).Find(LineFeedBuf);

		}while(Ret != KErrNotFound && i < aText.Length());

		if(aText.Length() > i)
		{
			aRichText.InsertL(aRichText.LdDocumentLength(),aText.Right(aText.Length() - i));
		}
	}
	else 
	{
		aRichText.InsertL(aRichText.LdDocumentLength(),aText);
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CSMSContainer::UpdateCharacterCountL(void)
{
	if(iLastCount != iCurrCount)
	{
		MAknEditingStateIndicator *ei = CAknEnv::Static()->EditingStateIndicator();
		if( ei)
		{
			TBuf<255> msgSize;
			msgSize.Num(iCurrCount);
		
			CAknIndicatorContainer *ic = ei->IndicatorContainer();
			ic->SetIndicatorValueL(TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ), msgSize);
			ic->SetIndicatorState(TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),EAknIndicatorStateOn);
		
			iLastCount = iCurrCount;
		}
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSMSContainer::MakeEditorL(void)
{	
	if(iUseAllFont)
	{
		delete iEditor;
		iEditor = NULL;
		
		if(!iParaformMe
		|| !iCharformMe
		|| !iRichMessageMe)
		{
			delete iParaformMe;
			iParaformMe = NULL;
			delete iCharformMe;
			iCharformMe = NULL;
			delete iRichMessageMe;
			iRichMessageMe = NULL;
			
			TCharFormat		charFormat;	
		    TCharFormatMask charFormatMask;	

			TFontSpec MyeFontSpec = iUseAllFont->FontSpecInTwips();

			charFormat.iFontSpec = MyeFontSpec;	   	
		   	charFormat.iFontPresentation.iTextColor = iTextColor;
		   	
		   	charFormatMask.SetAll();

			iParaformMe = CParaFormatLayer::NewL(); 	
			iCharformMe = CCharFormatLayer::NewL(charFormat,charFormatMask); 		
			iRichMessageMe  = CRichText::NewL(iParaformMe,iCharformMe);
		}
		
		iEditor = new (ELeave) CEikRichTextEditor;
		iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
		iEditor->SetContainerWindowL(*this);
		iEditor->SetAvkonWrap(ETrue);
		iEditor->ConstructL(this,10,0,EAknEditorFlagDefault,EGulFontControlAll,EGulAllFonts);
		iEditor->SetBorder(TGulBorder::ESingleBlack);	

		TRect EditorRect(Rect());
		EditorRect.Shrink(4,4);
		iEditor->SetRect(EditorRect);
		iEditor->SetMaxLength(KMaxMessageLength);
		
		iEditor->SetDocumentContentL(*iRichMessageMe);
		iEditor->SetFocus(EFalse);
		iEditor->SetFocusing(ETrue);
		iEditor->ClearSelectionL();	
		iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iEditor->UpdateScrollBarsL();
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CSMSContainer::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSMSContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	
  	if(iBgContext)
  	{
  		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
  	}
  	else
  	{
  		gc.Clear();
  	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CSMSContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			break;
		case EKeyUpArrow:
		case EKeyDownArrow:
		default:
		 	if(iEditor)
			{
	 			iEditor->OfferKeyEventL(aKeyEvent, aEventCode);
	 			
	 			CRichText* RichMe  = iEditor->RichText();
	 			if(RichMe)
	 			{
	 				iCurrCount = KMaxMessageLength - RichMe->LdDocumentLength();
	 				UpdateCharacterCountL();
	 			}		 			
			}
			break;
		}
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
HBufC* CSMSContainer::GetSMSMessageL(void)
{
	HBufC* Ret(NULL);
	if(iEditor)
	{
		CRichText* RichRec = iEditor->RichText();	
		if(RichRec)
		{
			Ret = RichRec->Read(0,RichRec->LdDocumentLength()).AllocL();
		}
	}
	
	return Ret;
}
			
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CSMSContainer::ComponentControl( TInt /*aIndex*/) const
{
	return iEditor;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CSMSContainer::CountComponentControls() const
{	
	return 1;
}

