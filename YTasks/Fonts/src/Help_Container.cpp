/* Copyright (c) 2001-2009, Dr. Jukka Silvennoinen, All rights reserved */


#include <eikedwin.h>
#include <aknenv.h>
#include <stringloader.h>
#include <AknUtils.h>
#include <BARSREAD.H>
#include <TXTRICH.H>
#include <eikappui.h>

#include "Help_Container.h"

_LIT(KtxDisclaimer_text1			,"This plug-in is used for showing the look and feel of all fonts available in the device.");
_LIT(KtxDisclaimer_text2			,"The text for the fonts can e imported from a file, pasted from clipboard or entered with text dialog. The font can be changed with Change font option in the options menu.");
_LIT(KtxDisclaimer_text3			,"System fonts include all non-scalable system fonts, logical fonts include normal scalable fonts used by the device, and the typeface font list includes all typefaces registered for the device. With typeface fonts the font height can also be set, and the default size for the font is 100.");
_LIT(KtxDisclaimer_text4			,"Note that the exit option will exit the whole application where as the back will just close the plug-in.");

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyHelpContainer* CMyHelpContainer::NewL()
	{
	CMyHelpContainer* self = new(ELeave)CMyHelpContainer();
	self->ConstructL();
	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

CMyHelpContainer::~CMyHelpContainer()
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
void CMyHelpContainer::ConstructL()
    {
    CreateWindowL();
    iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);
    
	iEditor = new (ELeave) CEikRichTextEditor;
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	iEditor->SetContainerWindowL(*this);
	iEditor->ConstructL(this,0,0,0);
	
	iEditor->CreateScrollBarFrameL();
	iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	    
	TCharFormat		charFormat;	
    TCharFormatMask charFormatMask;	

//	_LIT(KFontArial,"Arial");
	
	TRgb textcol;
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
	
	charFormat.iFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->FontSpecInTwips();
	charFormat.iFontPresentation.iTextColor = TLogicalRgb(textcol);;
	charFormatMask.SetAll();

	iParaformMe = CParaFormatLayer::NewL(); 
	iCharformMe = CCharFormatLayer::NewL(charFormat,charFormatMask); 
	iRichMessageMe  = CRichText::NewL(iParaformMe,iCharformMe);
	
	AddTextToEditorL();
	iEditor->SetDocumentContentL(*iRichMessageMe);
	iEditor->SetDocumentContentL(*iRichMessageMe);
	iEditor->SetCursorPosL(0,EFalse);
	
	iEditor->SetFocus(EFalse);
	iEditor->SetFocusing(ETrue);
	
	
	TRect MyRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	SetRect(MyRect);
	ActivateL();
	DrawNow();
	
	SizeChanged();
	iEditor->ClearSelectionL();	
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);	
	iEditor->UpdateScrollBarsL();
	iEditor->DrawNow();
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyHelpContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());
	
		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}

	if(iEditor)
	{
		TRect rect(Rect());//CEikonEnv::Static()->EikAppUi()->ClientRect());
		
		TRect ScrollBarRect = iEditor->ScrollBarFrame()->VerticalScrollBar()->Rect();
		
		rect.iBr.iX = (rect.iBr.iX - ScrollBarRect.Width());
		
		iEditor->SetRect(rect);
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CMyHelpContainer::MopSupplyObject(TTypeUid aId)
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
TInt CMyHelpContainer::CountComponentControls() const
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
void CMyHelpContainer::Draw(const TRect& /*aRect*/) const
{    
	CWindowGc& gc = SystemGc();
    
  	// draw background skin first.
  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsDrawUtils::Background( skin, iBgContext, this, gc, Rect() );
}
   
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMyHelpContainer::ComponentControl(TInt /*aIndex*/) const
    {
       return iEditor;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

TKeyResponse CMyHelpContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
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
void CMyHelpContainer::AddTextToEditorL()
{
	if (iRichMessageMe)
	{	
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), KtxDisclaimer_text1);

		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
	
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), KtxDisclaimer_text2);
		
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), KtxDisclaimer_text3);

		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), KtxDisclaimer_text4);
		
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
	}
}
