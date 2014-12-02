/* Copyright (c) 2001-2009, Dr. Jukka Silvennoinen, All rights reserved */


#include <eikedwin.h>
#include <aknenv.h>
#include <stringloader.h>
#include <AknUtils.h>
#include <BARSREAD.H>
#include <TXTRICH.H>
#include <eikappui.h>
#include <stringloader.h> 

#include "Help_Container.h"

#include "Common.h"
#ifdef SONE_VERSION
	#include <YApps_2002B0B1.rsg>
#else
	#ifdef LAL_VERSION
		#include <YApps_E8876015.rsg>
	#else
		#include <YApps_E8876015.rsg>
	#endif
#endif

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyHelpContainer* CMyHelpContainer::NewL(const TInt& aScreen)
	{
	CMyHelpContainer* self = new(ELeave)CMyHelpContainer(aScreen);
	self->ConstructL();
	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

CMyHelpContainer::CMyHelpContainer(const TInt& aScreen):iScreen(aScreen)
{

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
    
    iMirroRed = AknLayoutUtils::LayoutMirrored();
    
	iEditor = new (ELeave) CEikRichTextEditor;
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	iEditor->SetContainerWindowL(*this);
	iEditor->ConstructL(this,0,0,0);
	
	iEditor->CreateScrollBarFrameL();
	iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	    
	TCharFormat		charFormat;	
    TCharFormatMask charFormatMask;	
	
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
		
		if(iMirroRed){
            rect.iTl.iX = (rect.iTl.iX + ScrollBarRect.Width());
		}else{
            rect.iBr.iX = (rect.iBr.iX - ScrollBarRect.Width());
		}
		
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
		if(iScreen == 0)
		{
			HBufC* line1 = StringLoader::LoadLC(R_STR_HELP1);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line1);
			CleanupStack::PopAndDestroy();
	
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		
			HBufC* line2 = StringLoader::LoadLC(R_STR_HELP2);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line2);
			CleanupStack::PopAndDestroy();
			
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
	
			HBufC* line3 = StringLoader::LoadLC(R_STR_HELP3);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line3);
			CleanupStack::PopAndDestroy();
	
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			
			HBufC* line4 = StringLoader::LoadLC(R_STR_HELP4);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line4);
			CleanupStack::PopAndDestroy();
			
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			
			HBufC* line5 = StringLoader::LoadLC(R_STR_HELP5);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line5);
			CleanupStack::PopAndDestroy();
					
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			
			HBufC* line6 = StringLoader::LoadLC(R_STR_HELP6);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line6);
			CleanupStack::PopAndDestroy();
								
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		else if(iScreen == 1) // Schedlue settings
		{
			HBufC* line7 = StringLoader::LoadLC(R_STR_HELP7);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line7);
			CleanupStack::PopAndDestroy();
			
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
					
			HBufC* line8 = StringLoader::LoadLC(R_STR_HELP8);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line8);
			CleanupStack::PopAndDestroy();
					
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
					
			HBufC* line9 = StringLoader::LoadLC(R_STR_HELP9);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line9);
			CleanupStack::PopAndDestroy();
							
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
					
			HBufC* line10 = StringLoader::LoadLC(R_STR_HELP10);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line10);
			CleanupStack::PopAndDestroy();
										
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		else if(iScreen == 2) // key settings
		{
			HBufC* line11 = StringLoader::LoadLC(R_STR_HELP11);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line11);
			CleanupStack::PopAndDestroy();
												
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			
			HBufC* line12 = StringLoader::LoadLC(R_STR_HELP12);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line12);
			CleanupStack::PopAndDestroy();
															
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
								
			HBufC* line13 = StringLoader::LoadLC(R_STR_HELP13);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line13);
			CleanupStack::PopAndDestroy();
																		
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);								
		}
		else if(iScreen == 3) // Default settings
		{
			HBufC* line14 = StringLoader::LoadLC(R_STR_HELP14);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line14);
			CleanupStack::PopAndDestroy();
																	
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
										
			HBufC* line15 = StringLoader::LoadLC(R_STR_HELP15);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line15);
			CleanupStack::PopAndDestroy();
																				
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);	
					
			HBufC* line16 = StringLoader::LoadLC(R_STR_HELP16);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line16);
			CleanupStack::PopAndDestroy();
																				
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
													
			HBufC* line17 = StringLoader::LoadLC(R_STR_HELP17);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line17);
			CleanupStack::PopAndDestroy();
																							
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);	

			HBufC* line18 = StringLoader::LoadLC(R_STR_HELP18);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), *line18);
			CleanupStack::PopAndDestroy();
																							
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);	
		}
	}
}
