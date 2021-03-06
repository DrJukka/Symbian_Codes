/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#include "ShowString.h"
#include "YTools_A0000F5D.hrh"

#include <EIKENV.H>
#include <EIKAPPUI.H>
#include <AknUtils.h>
#include <BACLIPB.H>
 
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CShowString::CShowString(void)
{	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CShowString::~CShowString()
{
	delete iString;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CShowString::ConstructL(const TDesC& aBuffer)
{
	CreateWindowL();
	
	UpDateSizeL();
	
	if(aBuffer.Length())
	{
		iString = aBuffer.AllocL();
	}
	else
	{
		iString = NULL;
	}

	ActivateL();
	CEikonEnv::Static()->AddWindowShadow(this);
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CShowString::UpDateSizeL(void)
{
	TRect CurrclientRect(STATIC_CAST(CEikAppUi*,CEikonEnv::Static()->AppUi())->ClientRect());
	
	TInt CuWidth = (CurrclientRect.iBr.iX - CurrclientRect.iTl.iX);
	CurrclientRect.iTl.iX = (CurrclientRect.iTl.iX + (CuWidth / 20));
	CurrclientRect.iBr.iX = (CurrclientRect.iBr.iX - (CuWidth / 20));
	
	TInt fontHeight = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->HeightInPixels();
	
	TInt CuHeight = (CurrclientRect.iBr.iY - CurrclientRect.iTl.iY);
	
	CurrclientRect.iTl.iY = ((CuHeight / 2) - fontHeight);
	CurrclientRect.iBr.iY = ((CuHeight / 2) + fontHeight);
	
	SetRect(CurrclientRect);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CShowString::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect);
	gc.DrawRect(aRect);

	if(iString)
	{
		TInt fontHeight =  AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->HeightInPixels();
		
		gc.UseFont(AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont));			
		
		TInt TextToDraw = iString->Des().Length() - iCurrentCut;
			
		if(TextToDraw < 1)
		{
			TextToDraw = 1;
		}
		if(TextToDraw > iString->Des().Length())
		{
			TextToDraw = iString->Des().Length();
		}
		
		gc.DrawText(iString->Des().Right(TextToDraw),TRect(5,(fontHeight / 2),aRect.Width() - 5,((fontHeight / 2) + fontHeight)),AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->AscentInPixels(), CGraphicsContext::ELeft, 0);
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CShowString::HandleViewCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
		case ESSCopyClipBoard:
			if(iString)
			{
				CClipboard* cb = CClipboard::NewForWritingLC(CCoeEnv::Static()->FsSession());
				
				cb->StreamDictionary().At(KClipboardUidTypePlainText);
				
				CPlainText* BPlainText = CPlainText::NewL();
				CleanupStack::PushL(BPlainText);
				
				BPlainText->InsertL(0,iString->Des());
				
				BPlainText->CopyToStoreL(cb->Store(),cb->StreamDictionary(),0,BPlainText->DocumentLength());
				
				CleanupStack::PopAndDestroy(); // CBPlainText
				cb->CommitL();
				CleanupStack::PopAndDestroy(); // cb
			}
			break;
        };
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TKeyResponse CShowString::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
{
	TKeyResponse MyRet = EKeyWasNotConsumed;

	switch (aKeyEvent.iCode)
    {
	case EKeyDevice3:
	case EKeyUpArrow:
	case EKeyDownArrow:
		break;
	case EKeyRightArrow:
		if(iString)
		{
			iCurrentCut++;
			
			if(iCurrentCut > iString->Des().Length())
			{
				iCurrentCut = (iString->Des().Length() - 1);
			}
		}
		break;
	case EKeyLeftArrow:
		{
			iCurrentCut--;
			if(iCurrentCut < 0)
			{
				iCurrentCut = 0;
			}
		}
		break;
	default:
		break;
	} 
	
	DrawNow();
	return MyRet;
}



