/*
* ============================================================================

* ============================================================================
*/

// INCLUDE FILES

#include <StringLoader.h>
#include <BAUTILS.H>
#include <MGFetch.h>
#include <mtclreg.h>              	// for CClientMtmRegistry 
#include <msvids.h>               	// for Message type IDs
#include <mmsclient.h>            	// for CMmsClientMtm
#include <AknQueryDialog.h>       	// for CAknTextQueryDialog
#include <barsread.h>            	// for TResourceReader
#include <f32file.h>				
#include <coeutils.h>			  	// Check the file exist
#include <TXTRICH.H>
#include <APMREC.H>
#include <APGCLI.H>
#include <EIKMENUB.H>
#include <aknedsts.h> 
#include <aknindicatorcontainer.h> 
#include <AknEditStateIndicator.h>
#include <AknEnv.h>

#include "MMSExampleContainer.h"
#include "Help_Container.h"

#include "App_2002B0B0.h"
#include <App_2002B0B0.rsg>
#include "App_2002B0B0.hrh"



CMMSExampleContainer::CMMSExampleContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
{
	
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CMMSExampleContainer::~CMMSExampleContainer()
    { 
    if(iProgressDialog)
    {
    	iProgressDialog->ProcessFinishedL(); 
    }
    
    iProgressDialog = NULL;
    iProgressInfo = NULL;
    
    delete iMyHelpContainer;
    iMyHelpContainer = NULL;
    	
    delete iMessage;
    iMessage = NULL;
    delete iSMSSender;
    iSMSSender = NULL;
	delete iContacsContainer;
    
    delete iRecEditor;
    iRecEditor = NULL;
    delete iRecParaformMe;
    delete iRecCharformMe;
    delete iRecRichMessageMe;
    	
    delete iEditor;
    iEditor=NULL;
    delete iParaformMe;
    delete iCharformMe;
    delete iRichMessageMe; 
     
    iContactArray.ResetAndDestroy();
    delete iBgContext;
    iBgContext = NULL;
    }

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMMSExampleContainer::ConstructL(const TRect& aRect)
    {
    CreateWindowL();
   
    iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMainMessage,TRect(0,0,1,1), ETrue);

    iCurrCount = 160;
    iToBuffer.Copy(_L("To:"));
    
 	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
 	TRgb textcol;
 	TRgb Linecol;
 	
 #ifdef __SERIES60_3X__
 	iUseAllFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
 	
 	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
 	AknsUtils::GetCachedColor(skin,Linecol,KAknsIIDQsnLineColors,EAknsCIQsnLineColorsCG1);
 #else
	#ifdef __SERIES60_28__
		iUseAllFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
		AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
		 AknsUtils::GetCachedColor(skin,Linecol,KAknsIIDQsnLineColors,EAknsCIQsnLineColorsCG1);
	#else
		iUseAllFont = LatinPlain12();
		
		const TInt EAknsCIQsnTextColorsCG9    = 8;
		const TInt EAknsMinorQsnTextColors    = 0x3300;
		
		static const TAknsItemID KAknsIIDQsnTextColors =
		    { EAknsMajorSkin, EAknsMinorQsnTextColors };
		
		AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG9);
		AknsUtils::GetCachedColor(skin,Linecol,KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG9);
	#endif	 
#endif	
 	
 	iTextColor = TLogicalRgb(textcol);
 	iLineColor = iTextColor;//TLogicalRgb(Linecol);
 	
	SetRect(aRect);
    ActivateL();
}



/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
				
void CMMSExampleContainer::HandleEdwinEventL(CEikEdwin* /*aEdwin*/, TEdwinEvent aEventType)
{
	switch(aEventType)
	{
	case EEventFormatChanged:
		break;
	case EEventNavigation:
		break;
	case EEventTextUpdate:
		break;
	};
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
				
void CMMSExampleContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{       
	TBool handled(EFalse);

    if(aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
		if((iRecleRect.iTl.iX <= aPointerEvent.iPosition.iX) && (iRecleRect.iBr.iX >= aPointerEvent.iPosition.iX ))
		{
			if((iRecleRect.iTl.iY <= aPointerEvent.iPosition.iY) && (iRecleRect.iBr.iY >= aPointerEvent.iPosition.iY ))
			{
				HandleCommandL(ESelectRecipient);
				handled = ETrue;
			}
		}
    }
    
    if(!handled){
		CCoeControl::HandlePointerEventL(aPointerEvent);
    }
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMMSExampleContainer::MakeRecipientEdL(void)
{	
	if(iUseAllFont)
	{
		delete iRecEditor;
		iRecEditor = NULL;
		
		if(!iRecParaformMe
		|| !iRecCharformMe
		|| !iRecRichMessageMe)
		{
			delete iRecParaformMe;
			iRecParaformMe = NULL;
			delete iRecCharformMe;
			iRecCharformMe = NULL;
			delete iRecRichMessageMe;
			iRecRichMessageMe = NULL;
			
			TCharFormat		charFormat;	
		    TCharFormatMask charFormatMask;	

			TFontSpec MyeFontSpec = iUseAllFont->FontSpecInTwips();
			charFormat.iFontSpec = MyeFontSpec;
			charFormat.iFontPresentation.iTextColor= iTextColor;
		   	charFormatMask.SetAll();

			iRecParaformMe = CParaFormatLayer::NewL(); 	
			iRecCharformMe = CCharFormatLayer::NewL(charFormat,charFormatMask); 			
			iRecRichMessageMe  = CRichText::NewL(iRecParaformMe,iRecCharformMe);
		}
		
		TRect EditorRect(iRecleRect);
		EditorRect.Shrink(4,4);
		
		iRecEditor = new (ELeave) CEikRichTextEditor;
//		iRecEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
		iRecEditor->SetContainerWindowL(*this);
		iRecEditor->SetAvkonWrap(ETrue);
		iRecEditor->ConstructL(this,10,0,EAknEditorFlagDefault,EGulFontControlAll,EGulAllFonts);
		iRecEditor->SetBorder(TGulBorder::ESingleBlack);	
		iRecEditor->SetRect(EditorRect);	
		iRecEditor->SetDocumentContentL(*iRecRichMessageMe);
		iRecEditor->SetFocus(EFalse);
		iRecEditor->SetFocusing(EFalse);
		iRecEditor->ClearSelectionL();	
		iRecEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
//		iRecEditor->UpdateScrollBarsL();
	}
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMMSExampleContainer::MakeEditorL(void)
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
		iEditor->SetEdwinObserver(this);
		iEditor->SetMaxLength(160);
		 
		TRect EditorRect(iEditorRect);
		EditorRect.Shrink(4,4);
		iEditor->SetRect(EditorRect);
		
		iEditor->SetDocumentContentL(*iRichMessageMe);
		iEditor->SetFocus(ETrue);
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
void CMMSExampleContainer::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}

	if(iContacsContainer)
	{
		iContacsContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
	}
	
	
	if(iUseAllFont)
	{
		TInt RecHeight = (iUseAllFont->HeightInPixels() * 2);
		TInt ToLength  = (iUseAllFont->TextWidthInPixels(iToBuffer) + 8);
				
		TRect WholeRect(Rect());
		WholeRect.Shrink(4,4);
			
		iRecleRect.iTl.iX = (WholeRect.iTl.iX + ToLength);
		iRecleRect.iTl.iY = WholeRect.iTl.iY;
		iRecleRect.iBr.iX = WholeRect.iBr.iX;
		iRecleRect.iBr.iY = (WholeRect.iTl.iY + RecHeight);
			
		iEditorRect.iTl.iX= WholeRect.iTl.iX;
		iEditorRect.iTl.iY= (iRecleRect.iBr.iY + 4); 
		iEditorRect.iBr   = WholeRect.iBr;
			
		iLinePoint1.iX = (iRecleRect.iTl.iX - 2); 
		iLinePoint1.iY = WholeRect.iTl.iY;
			
		iLinePoint2.iX = iLinePoint1.iX;
		iLinePoint2.iY = (iEditorRect.iTl.iY - 2);
			
		iLinePoint3.iX = WholeRect.iBr.iX;
		iLinePoint3.iY = iLinePoint2.iY;
			
		iToRect.iTl	   = WholeRect.iTl;
		iToRect.iBr.iX = (iLinePoint1.iX - 2);
		iToRect.iBr.iY = iLinePoint2.iY;
		
		if(iEditor)
		{
			TRect EditorRect(iEditorRect);
			EditorRect.Shrink(4,4);
			iEditor->SetRect(EditorRect);
		}
		else
		{
			MakeEditorL();
		}
		
		if(iRecEditor)
		{
			TRect EditorRect(iRecleRect);
			EditorRect.Shrink(4,4);
			iRecEditor->SetRect(EditorRect);
		}
		else
		{
			MakeRecipientEdL();
		}
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
TTypeUid::Ptr CMMSExampleContainer::MopSupplyObject(TTypeUid aId)
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
void CMMSExampleContainer::Draw(const TRect& /*aRect*/) const
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
  	
  	if(iUseAllFont)
  	{
  		gc.SetPenColor(iTextColor);
  		gc.SetPenStyle(CGraphicsContext::ESolidPen);
  		gc.UseFont(iUseAllFont);
  		
  		gc.DrawText(iToBuffer,iToRect,iUseAllFont->AscentInPixels(), CGraphicsContext::ERight, 0);
  	}
  	
  	gc.SetPenColor(iLineColor);
  	gc.SetPenStyle(CGraphicsContext::ESolidPen);
  	gc.DrawLine(iLinePoint1,iLinePoint2);
  	gc.DrawLine(iLinePoint2,iLinePoint3);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/		
void CMMSExampleContainer::SetRecipientL(const TDesC& aRecipient)
{
	if (iRecRichMessageMe && iRecEditor)
	{
		iRecRichMessageMe->Reset();
		iRecRichMessageMe->InsertL(iRecRichMessageMe->LdDocumentLength(),aRecipient);
	
		iRecEditor->SetDocumentContentL(*iRecRichMessageMe);
		iRecEditor->SetCursorPosL(0,EFalse);
		iRecEditor->ClearSelectionL();	
		iRecEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iRecEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iRecEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
		iRecEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	//	iRecEditor->UpdateScrollBarsL();
		iRecEditor->DrawNow();
	}
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TInt CMMSExampleContainer::CountComponentControls() const
{
	if(iMyHelpContainer)
		return 1;		
	else if(iContacsContainer)
		return 1;
	else if(iRecEditor && iEditor)
		return 2; // return number of controls inside this container
	else
		return 0;
    }
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CCoeControl* CMMSExampleContainer::ComponentControl(TInt aIndex) const
{
	if(iMyHelpContainer)
		return iMyHelpContainer;		
	else if(iContacsContainer)
		return iContacsContainer;
	else
	{
		if(aIndex == 1)
			return iRecEditor;
		else
			return iEditor;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMMSExampleContainer::HandleStatusChange(MMsvObserver::TStatus aStatus)
{
	switch(aStatus)
	{
	case ECreated:
		break;
	case EMovedToOutBox:
		break;
	case EScheduledForSend:
		break;
	case ESent:
		{
			SendNextMessageL();
		}
		break;
	case EDeleted:
		break;
	case EScheduled:
		break;
	case EGotReply:
		break;
    };
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMMSExampleContainer::HandleError(const TInt& aError)
{
	TBuf<200> errBuf;

	switch(aError)
    {
    case EScheduleFailed:
    	{
			StringLoader::Load(errBuf,R_STR_FAIL_1);
    	}
    	break;
    case ESendFailed:
    	{
			StringLoader::Load(errBuf,R_STR_FAIL_2);
    	}
    	break;
    case ENoServiceCentre:
    	{
			StringLoader::Load(errBuf,R_STR_FAIL_3);
    	}
    	break;
    case EFatalServerError:
    	{
			StringLoader::Load(errBuf,R_STR_FAIL_4);
    	}
    	break;
    default:
    	if(aError < 0)
    	{
			StringLoader::Load(errBuf,R_STR_FAIL_5);
			errBuf.AppendNum(aError);
    	}
    	break;
	};
	
	if(errBuf.Length()){
		STATIC_CAST(CMMSExampleAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(errBuf);
	}
	
	SendNextMessageL();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMMSExampleContainer::DialogDismissedL (TInt /*aButtonId*/)
{
	iProgressDialog = NULL;
	iProgressInfo = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMMSExampleContainer::SendNextMessageL(void)
{		
	TBool OkToContinue(EFalse);
	do{
		iCurrIndex++;
		
		if(iContactArray.Count() <= iCurrIndex){
			OkToContinue = ETrue;
		}else if(iMessage){
			if(iContactArray[iCurrIndex]){
				if(iContactArray[iCurrIndex]->iNumber){
					OkToContinue = ETrue;
				}
			}
		}
	}while(!OkToContinue);
	
	if(iContactArray.Count() > iCurrIndex && iMessage && iContactArray[iCurrIndex] && iContactArray[iCurrIndex]->iNumber){
		
		if(iProgressDialog){
			if(iContactArray[iCurrIndex]->iName){
				iProgressDialog->SetTextL(*iContactArray[iCurrIndex]->iName);
			}else{
				iProgressDialog->SetTextL(*iContactArray[iCurrIndex]->iNumber);
			}
		}
	
		if(iProgressInfo)
		{
			iProgressInfo->SetAndDraw(iCurrIndex);	
		}
		
		if(!iSMSSender){
			iSMSSender = CSMSSender::NewL(*this);
		}
		iSMSSender->SendMsgL(*iMessage,*iContactArray[iCurrIndex]->iNumber,iIsUnoCode,ETrue);
		
	}else{
		if(iProgressDialog)
		{
			iProgressDialog->ProcessFinishedL(); 
		}
		iProgressDialog = NULL;
		iProgressInfo = NULL;
		
		TBuf<200> errBuf;
		StringLoader::Load(errBuf,R_STR_ALLSENT);
		STATIC_CAST(CMMSExampleAppUi*,CEikonEnv::Static()->AppUi())->ShowNoteL(errBuf);
	}
}

void CMMSExampleContainer::HandleCommandL(TInt aCommand)
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
    		iMyHelpContainer = CMyHelpContainer::NewL(0);
    	}
    	SetMenuL();
    	DrawNow();    		
    	break;	        
    case EMMSExampleCmdSend:
    	if(iContactArray.Count())
    	{
			delete iSMSSender;
			iSMSSender = NULL;
			delete iMessage;
			iMessage = NULL;
			
			iMessage = GetMessageL();
			iCurrIndex = -1;
			
			if(iMessage){
				
				iIsUnoCode = EFalse;
				TBool okToSave(EFalse);
				if(CheckMessageForUnicodeL(*iMessage,okToSave))
				{	
					iIsUnoCode = ETrue;
				}
				else
				{
					iIsUnoCode = EFalse;
					okToSave = ETrue;
				}
				
				if(okToSave){
					
					iProgressDialog = new (ELeave) CAknProgressDialog((REINTERPRET_CAST(CEikDialog**, &iProgressDialog)),ETrue);
					iProgressDialog->PrepareLC(R_PROGRESS_NOTE);
					iProgressInfo = iProgressDialog->GetProgressInfoL();
					iProgressDialog->SetCallback(this);
					iProgressDialog->RunLD();
					iProgressInfo->SetFinalValue(iContactArray.Count());	
				
					SendNextMessageL();
				}
			}			
    	}
    	break;
    case ENewRecipient:
    	{
			TBuf<150> hjelpper;
    	
			HBufC* titleBuf = StringLoader::LoadLC(R_STR_SH_PHONENUMBER);
    					
    		CAknTextQueryDialog* dialog = CAknTextQueryDialog::NewL(hjelpper,CAknQueryDialog::ENoTone);
    		dialog->PrepareLC(R_GETNUMBER_DIALOG);
    		dialog->SetPromptL(*titleBuf);
    		if(dialog->RunLD())
    		{
				CMyContact* tmpp = new(ELeave)CMyContact();
				iContactArray.Append(tmpp);
				tmpp->iId 	 = -1;
				tmpp->iName	 = hjelpper.AllocL();
				tmpp->iNumber= hjelpper.AllocL();
			
				if(iContactArray.Count())
				{
					if(iContactArray.Count() > 1)
					{
						TBuf<60> titHelp;
						StringLoader::Load(titHelp,R_SH_STR_CONTACTS); 
					
						hjelpper.Num(iContactArray.Count());
						hjelpper.Append(titHelp);
					}
				}
			
				SetRecipientL(hjelpper);
    		}
    		
    		CleanupStack::PopAndDestroy();//titleBuf
    	}
    	DrawNow();
    	break;
    case ERemoveRecipient:
		{
			iContactArray.ResetAndDestroy();
			SetRecipientL(_L(""));
		}
    	DrawNow();
        break;
    case ESelectRecipient:
    	{
			delete iContacsContainer;
    		iContacsContainer = NULL;
    		
    		RPointerArray<CMyCItem>& array = STATIC_CAST(CMMSExampleAppUi*,CEikonEnv::Static()->AppUi())->GetContactsArrayL();
    		
    		
    		iContacsContainer = new(ELeave)CContacsContainer(iCba,array);
    		iContacsContainer->ConstructL();
        }
    	SetMenuL();
    	DrawNow();
        break;
    case EContOkOne:
    	if(iContacsContainer)
    	{
			iContacsContainer->GetSelectedArrayLC(iContactArray);
			
			TBuf<150> hjelpper;
			
			if(iContactArray.Count())
			{
				if(iContactArray.Count() > 1)
				{
					TBuf<60> titHelp;
					StringLoader::Load(titHelp,R_SH_STR_CONTACTS); 
				
					hjelpper.Num(iContactArray.Count());
					hjelpper.Append(titHelp);
				}
				else if(iContactArray[0])
				{
					if(iContactArray[0]->iName && iContactArray[0]->iName->Des().Length())
						hjelpper.Copy(iContactArray[0]->iName->Des());
					else if(iContactArray[0]->iNumber && iContactArray[0]->iNumber->Des().Length())
						hjelpper.Copy(iContactArray[0]->iNumber->Des());
				}
			}
			
			SetRecipientL(hjelpper);
        }
    case EContBackOne:
    	{
			delete iContacsContainer;
			iContacsContainer = NULL;
        }
    	SetMenuL();
    	DrawNow();
    	break;

    default:
    	if(iContacsContainer)
    	{
			iContacsContainer->HandleViewCommandL(aCommand);
    	}
        break;
        }
    }


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CMMSExampleContainer::CheckMessageForUnicodeL(const TDesC& aMessage, TBool& aSaveNow)
{
	TBool ret(EFalse);
	aSaveNow = ETrue;
	
	TInt cunt(0);

	ConvertFromUnicodeL(aMessage,cunt);
		
	if(cunt)
	{
		ret = ETrue;
		
		if((aMessage.Length() > 70))
		{	
			TBuf<200> Hjelppersmal;
			StringLoader::Load(Hjelppersmal,R_STR_MSGTRUNK);
						
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if(dlg->ExecuteLD(R_QUERY,Hjelppersmal))
			{
				aSaveNow = ETrue;
			}	
			else
			{
				aSaveNow = EFalse;
			}
		}
		else
		{
			aSaveNow = ETrue;
		}
	}
	
	return ret;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMMSExampleContainer::ConvertFromUnicodeL(const TDesC& aFrom,TInt& aUncorvetible) 
{
	if(aFrom.Length())
	{
		HBufC8* hjelpper = HBufC8::NewLC(aFrom.Length());
		TPtr8 hjelpperPtr(hjelpper->Des());
	
	//	TRAP(Err,
		CCnvCharacterSetConverter* CSConverter = CCnvCharacterSetConverter::NewLC();
			
		TPtrC Remainder(aFrom);
	
		if (CSConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit,CEikonEnv::Static()->FsSession()) != CCnvCharacterSetConverter::EAvailable)
		{
			CSConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit,CEikonEnv::Static()->FsSession());
		}
			
		for(;;)
		{	
			TInt unConv(0);
			const TInt returnValue = CSConverter->ConvertFromUnicode(hjelpperPtr,Remainder,unConv);
			
			aUncorvetible = aUncorvetible + unConv;
			
			if (returnValue <= 0) // < error
			{
				break;
			}
			Remainder.Set(Remainder.Right(returnValue));
		}

		CleanupStack::PopAndDestroy(2);//hjelpper,CSConverter
	}
 }

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TKeyResponse CMMSExampleContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse ret(EKeyWasNotConsumed);
	
	if(iMyHelpContainer)
	{
		ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iContacsContainer)
	{
		ret = iContacsContainer->OfferKeyEventL(aKeyEvent,aType);
	}
	else if(iEditor)
	{
		ret = iEditor->OfferKeyEventL(aKeyEvent, aType);
		
		CRichText* RichMe  = iEditor->RichText();
		if(RichMe)
		{
			iCurrCount = 160 - RichMe->LdDocumentLength();
			UpdateCharacterCountL();
		}
	}
			
	return ret;	
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMMSExampleContainer::UpdateCharacterCountL(void)
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
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
HBufC* CMMSExampleContainer::GetMessageL(void)
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
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMMSExampleContainer::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    if (aResourceId == R_MMSEXAMPLE_OPTIONS_MENU)
    	{
			TBool dimSend(ETrue);
			if(iContactArray.Count())
			{
				dimSend = EFalse;
			}
			
			if(dimSend)
			{
				aMenuPane->SetItemDimmed(EMMSExampleCmdSend, ETrue);
			}
    	}

}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMMSExampleContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();				
	
	if(iMyHelpContainer)
	{
		if(iCba)
		{
			iCba->SetCommandSetL(R_APPHELP_CBA);
			iCba->DrawDeferred();
		}
	}
	else if(iContacsContainer)
	{
		iContacsContainer->SetMenuL();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_MMSEXAMPLE_MENUBAR);

		iCba->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
		iCba->DrawDeferred();
	}
}

