/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/


#include <EIKENV.H>
#include <EIKAPPUI.H>
#include <aknutils.h>
#include <eikapp.h>
#include <AknGlobalNote.h>
#include <APGCLI.H>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <TXTRICH.H>
#include <EIKMFNE.H>
#include <aknedsts.h> // for CAknEdwinState
#include <eikedwin.h> 
#include <aknindicatorcontainer.h> 
#include <AknEditStateIndicator.h>

#include "MenuNotes_Control.h"
#include "YApp_E8876001.mig"
/*
-----------------------------------------------------------------------------
***************************** CMyMenuControl ******************************
-----------------------------------------------------------------------------
*/
CMyMenuControl::~CMyMenuControl()
{
    if (iFrameReadTimer)
        iFrameReadTimer->Cancel();
    
    delete iFrameReadTimer;
    
	delete iCommands;
	delete iTexts;
	
	iBackAdd.ResetAndDestroy();
	delete iBackGround;
	iBackGround = NULL;
	delete iBackMask;
	iBackMask = NULL;
	delete iListSel;
	iListSel = NULL;
	
	delete iScroller;
	iScroller = NULL;

	delete iScrollBack;
	iScrollBack = NULL;
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyMenuControl::CMyMenuControl(MMyMenuProvided& aMenuProvided,TRgb aFontColor,TRgb aSelColor, TInt aAddInterval)
:iMenuProvided(aMenuProvided),iAddInterval(aAddInterval)
,iFontColor(aFontColor),iSelColor(aSelColor)	
{

}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyMenuControl* CMyMenuControl::NewL(const TRect& aRect,MMyMenuProvided& aMenuProvided,TRgb aFontColor,TRgb aSelColor, TInt aAddInterval)
    {
    CMyMenuControl* self = new(ELeave)CMyMenuControl(aMenuProvided,aFontColor,aSelColor,aAddInterval);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    CleanupStack::Pop(self);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMyMenuControl::ConstructL(const TRect& aRect)
{
    CreateWindowL();
    MakeVisible(EFalse);
    
    iUseFont= AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
		  
    
    
    TRect MyRect(aRect);
    
    SetRect(MyRect);
    
	ActivateL();	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyMenuControl::SetTextColot(const TRgb& aColor,const TRgb& aSelColor)
{
	iFontColor = aColor;
	iSelColor = aSelColor;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyMenuControl::ActivateDeActivate(TBool aActive)
{
	MakeVisible(aActive);

	iCurrentItem = iFirstInList = 0;

	if(aActive)
	{
		if (!iFrameReadTimer)
		{
			iFrameReadTimer = CPeriodic::NewL(CActive::EPriorityHigh);
			iFrameReadTimer->Start(iAddInterval, iAddInterval, TCallBack(DoChangeAddImageL, this));
		}
		
		delete iCommands;
		iCommands = NULL;
		iCommands = new(ELeave)CArrayFixFlat<TInt>(5);
		delete iTexts;
		iTexts = NULL;
		iTexts = new(ELeave)CDesCArrayFlat(5);
   
    	iMenuProvided.AppendMenuL(*iCommands,*iTexts);
	}
	else
	{
		if (iFrameReadTimer)
        	iFrameReadTimer->Cancel();
    
    	delete iFrameReadTimer;
    	iFrameReadTimer = NULL;
	}
	
	DrawNow();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
TInt CMyMenuControl::DoChangeAddImageL(TAny* aPtr)
{
	CMyMenuControl* self = static_cast<CMyMenuControl*>(aPtr);

	self->iCurrentAdd++;
	
	if(self->iCurrentAdd >= self->iBackAdd.Count())
	{
		self->iCurrentAdd = 0;
	}
	
	self->DrawNow();
    
	return KErrNone;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CMyMenuControl::SizeChanged()
{
	TInt FontH = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont)->HeightInPixels();
	FontH = ((FontH * 3) / 2);
	
	TRect MyRect(Rect());
	MyRect.Shrink(4,4);
	
	iMaxItems = (MyRect.Height() / FontH);
	
	if(iFirstInList > iCurrentItem)
	{
		iFirstInList = iCurrentItem;
	}
	else if(iFirstInList < (iCurrentItem - (iMaxItems-1)))
	{
		iFirstInList = iCurrentItem - (iMaxItems-1);
	}


	TInt ScrollW = (Rect().Width() / 50);
	
	if(ScrollW < 8)
	{
		ScrollW = 8;
	}
	
	iScrollRect.iBr.iX = (Rect().iBr.iX - 2);
	iScrollRect.iBr.iY = (Rect().iBr.iY - 2);
	
	iScrollRect.iTl.iX = (Rect().iBr.iX - ScrollW);
	iScrollRect.iTl.iY = (Rect().iTl.iY + 2);
	
	TFileName ImgFileName;
	iMenuProvided.GetFileName(ImgFileName);

	if(ImgFileName.Length())
	{	
		delete iBackGround;
		iBackGround = NULL;
		iBackGround = AknIconUtils::CreateIconL(ImgFileName, EMbmYapp_a000312dBack_normal);
		AknIconUtils::SetSize(iBackGround,Rect().Size(),EAspectRatioNotPreserved);	
		
		iBackAdd.ResetAndDestroy();
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add,EMbmYapp_a000312dBack_add_mask,Rect().Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add2,EMbmYapp_a000312dBack_add2_mask,Rect().Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add3,EMbmYapp_a000312dBack_add3_mask,Rect().Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add4,EMbmYapp_a000312dBack_add4_mask,Rect().Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add5,EMbmYapp_a000312dBack_add5_mask,Rect().Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add6,EMbmYapp_a000312dBack_add6_mask,Rect().Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add7,EMbmYapp_a000312dBack_add7_mask,Rect().Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add8,EMbmYapp_a000312dBack_add8_mask,Rect().Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add9,EMbmYapp_a000312dBack_add9_mask,Rect().Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add10,EMbmYapp_a000312dBack_add10_mask,Rect().Size()));
 
		delete iBackMask;
		iBackMask = NULL;
		iBackMask = LoadImageL(ImgFileName,EMbmYapp_a000312dBg_mask,EMbmYapp_a000312dBg_mask_mask,Rect().Size(),EAspectRatioNotPreserved);

		if(iMaxItems > 0)
		{
			TRect DrawRect(Rect());
			DrawRect.Shrink(4,4);
			
			TSize SelSizzz(DrawRect.Width(),(DrawRect.Height() / iMaxItems));
		
			delete iListSel;
			iListSel = NULL;
			iListSel = LoadImageL(ImgFileName,EMbmYapp_a000312dListsel_mask,EMbmYapp_a000312dListsel_mask_mask,SelSizzz,EAspectRatioNotPreserved);
	
			TSize ImgSiz(((iScrollRect.Width() * 8) / 10),(iScrollRect.Height() /2));
				
			delete iScroller;
			iScroller = NULL;
			iScroller = LoadImageL(ImgFileName,EMbmYapp_a000312dScroller,EMbmYapp_a000312dScroller_mask,ImgSiz);

			delete iScrollBack;
			iScrollBack = NULL;
			iScrollBack = LoadImageL(ImgFileName,EMbmYapp_a000312dScroll,EMbmYapp_a000312dScroll_mask,iScrollRect.Size());
		}
	}		
} 
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TInt CMyMenuControl::SelectedCommand(void)
{
	TInt Ret(-1);
	if(iCommands)
	{
		if(iCurrentItem >=0 && iCurrentItem < iCommands->Count())
		{
			Ret = iCommands->At(iCurrentItem);
		}
	}
	
	return Ret;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyMenuControl::UpAndDown(TBool aUp)
{
	if(iTexts)
	{
		if(aUp)
			iCurrentItem--;
		else
			iCurrentItem++;
		
		if(iCurrentItem < 0)
			iCurrentItem = iTexts->MdcaCount() - 1;
		else if(iCurrentItem >= iTexts->MdcaCount())
			iCurrentItem = 0;
		
		if(iFirstInList > iCurrentItem)
		{
			iFirstInList = iCurrentItem;
		}
		else if(iFirstInList < (iCurrentItem - (iMaxItems-1)))
		{
			iFirstInList = iCurrentItem - (iMaxItems-1);
		}
	}
	
	DrawNow();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMyMenuControl::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, aImg, aMsk);

	AknIconUtils::SetSize(FrameImg,aSize,aScaleMode);  
	AknIconUtils::SetSize(FrameMsk,aSize,aScaleMode); 

	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMenuControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	if(iBackGround)
	{
		if(iBackGround->Handle())
		{
			gc.DrawBitmap(Rect(),iBackGround);
		}
	}


	CGulIcon* TmpDrawMe(NULL);
	if(iBackAdd.Count())
	{
		if(iCurrentAdd < iBackAdd.Count())
		{
			TmpDrawMe = iBackAdd[iCurrentAdd];
		}
		else
		{
			TmpDrawMe = iBackAdd[0];
		}
	}
		
	if(TmpDrawMe)
	{
		if(TmpDrawMe->Bitmap() && TmpDrawMe->Mask())
		{
			if(TmpDrawMe->Bitmap()->Handle() && TmpDrawMe->Mask()->Handle())
			{
				TSize ImgSiz(TmpDrawMe->Bitmap()->SizeInPixels());
				
				TInt LftMargin = Rect().iTl.iX + ((Rect().Width() - ImgSiz.iWidth) / 2);
				TInt RghMargin = Rect().iTl.iY + ((Rect().Height() - ImgSiz.iHeight) / 2);
				
				TRect DrwRect(LftMargin,RghMargin,(LftMargin + ImgSiz.iWidth),(ImgSiz.iHeight + RghMargin));
				
				gc.DrawBitmapMasked(DrwRect,TmpDrawMe->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),TmpDrawMe->Mask(),EFalse);
			}
		}
	}
	
	if(iBackMask)
	{
		if(iBackMask->Bitmap() && iBackMask->Mask())
		{
			if(iBackMask->Bitmap()->Handle() && iBackMask->Mask()->Handle())
			{
				TSize ImgSiz(iBackMask->Bitmap()->SizeInPixels());
				gc.DrawBitmapMasked(Rect(),iBackMask->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iBackMask->Mask(),EFalse);
			}
		}
	}
	
	TRect DrawRect(Rect());
	DrawRect.Shrink(2,2);
	
	gc.SetPenColor(iFontColor);
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY)); 

	DrawRect.Shrink(2,2);
	
	if(iMaxItems > 0 && iUseFont && iTexts)
	{
		gc.UseFont(iUseFont);

		TInt StartGap = ((DrawRect.Height() % iMaxItems) / 2);
		TInt HeighGap = (DrawRect.Height() / iMaxItems);

		TRect ItemRect(DrawRect);
		
		ItemRect.iTl.iY = (DrawRect.iTl.iY + StartGap);
		ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
		
		for(TInt i=0; i < iMaxItems; i++)
		{
			TInt CurrItem = (iFirstInList + i);
			if(CurrItem >= 0 && CurrItem < iTexts->MdcaCount())
			{
				TBool Selected(EFalse);
				
				if(iCurrentItem == CurrItem)
				{
					Selected = ETrue;
				}
				
				DrawItem(gc,ItemRect, iTexts->MdcaPoint(CurrItem),Selected);
			}
			
			ItemRect.iTl.iY = ItemRect.iBr.iY;
			ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
		}
	
		
		if(iMaxItems > 1 && iMaxItems < iTexts->MdcaCount())
		{
			if(iScrollBack)
			{
				if(iScrollBack->Bitmap() && iScrollBack->Mask())
				{
					if(iScrollBack->Bitmap()->Handle() && iScrollBack->Mask()->Handle())
					{
						TSize SSImgSiz(iScrollBack->Bitmap()->SizeInPixels());
						gc.DrawBitmapMasked(iScrollRect,iScrollBack->Bitmap(),TRect(0,0,SSImgSiz.iWidth,SSImgSiz.iHeight),iScrollBack->Mask(),EFalse);
					}
				}
			}
			
			if(iScroller)
			{
				if(iScroller->Bitmap() && iScroller->Mask())
				{
					if(iScroller->Bitmap()->Handle() && iScroller->Mask()->Handle())
					{
						TSize SRImgSiz(iScroller->Bitmap()->SizeInPixels());
						
						TSize SRSizeImg(iScrollRect.Width(), ((iScrollRect.Height() * iMaxItems) / iTexts->MdcaCount()));
										
						TInt ScroollHeight = (iScrollRect.Height() - SRImgSiz.iHeight);
						
						TInt TmpCunt((iTexts->MdcaCount() - 1));
						
						TInt StartSrollY = ((ScroollHeight * iCurrentItem) / TmpCunt);
								
						StartSrollY = StartSrollY + iScrollRect.iTl.iY;
						
						TRect ScrollerRect(iScrollRect.iTl.iX,StartSrollY,iScrollRect.iBr.iX,(StartSrollY + SRImgSiz.iHeight));

						// have the match in here..
						gc.DrawBitmapMasked(ScrollerRect,iScroller->Bitmap(),TRect(0,0,SRImgSiz.iWidth,SRImgSiz.iHeight),iScroller->Mask(),EFalse);
					}
				}
			}
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMenuControl::DrawItem(CWindowGc& aGc,const TRect& aRect,const TDesC& aText, TBool aSelected) const
{
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	
	if(iUseFont)
	{
		if(aSelected && iListSel)
		{
			if(iListSel->Bitmap() && iListSel->Mask())
			{
				if(iListSel->Bitmap()->Handle() && iListSel->Mask()->Handle())
				{
					TSize ImgSiz(iListSel->Bitmap()->SizeInPixels());
					aGc.DrawBitmapMasked(aRect,iListSel->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iListSel->Mask(),EFalse);
				}
			}
		}

		TInt HHGappp = ((aRect.Height() - iUseFont->HeightInPixels()) / 2);
		
		TRect DrawSec(aRect);
		if(HHGappp > 0)
		{
			DrawSec.iTl.iY = (DrawSec.iTl.iY + HHGappp);
			DrawSec.iTl.iX = (DrawSec.iTl.iX + (HHGappp * 2));
		}
		
		aGc.DrawText(aText,DrawSec,iUseFont->AscentInPixels(),CGraphicsContext::ELeft, 0);
	}
}

/*
-----------------------------------------------------------------------------
***************************** CMyMultiControl ******************************
-----------------------------------------------------------------------------
*/
CMyMultiControl::~CMyMultiControl()
{
	delete iBackGround;
	iBackGround = NULL;
	delete iBackMask;
	iBackMask = NULL;
	
	delete iTitle;
	iTitle = NULL;
	
	DeleteAllControls();
}  
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMultiControl::DeleteAllControls(void)
{
	delete iEditor;
	iEditor=NULL;
	delete iParaform;
	iParaform = NULL;
	delete iCharform;
	iCharform = NULL;
	delete iRichMessage;
	iRichMessage = NULL; 
	
	delete iDateEditor;
	iDateEditor = NULL;
	delete iTimeEditor;
	iTimeEditor = NULL;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMyMultiControl::CMyMultiControl(MMyMenuProvided& aMenuProvided,TRgb aFontColor)
:iMenuProvided(aMenuProvided),iFontColor(aFontColor)	
{

}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMultiControl::ConstructL(const TRect& aRect)
{
    CreateWindowL();
	  
    TRect MyRect(aRect);
    SetRect(MyRect);
    
	ActivateL();	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMultiControl::StartDateEditorL(TTime aTime,const TDesC& aTitle)
{
	DeleteAllControls();
	
	iSavedTime = aTime;
	iEdType = EDateTimeEd;
	delete iTitle;
	iTitle = NULL;
	iTitle = aTitle.AllocL();	
	
	CalculateRects();
	
	TTime MaxAlarm(TDateTime(2999,TMonth(11),30,23,59,59,59));
	TTime MinAlarm(TDateTime(2000,TMonth(0),0,0,0,0,0));
	
	iDateEditor = new(ELeave)CEikDateEditor();
	iDateEditor->SetContainerWindowL(*this);
	iDateEditor->ConstructL(MinAlarm,MaxAlarm,aTime,0);
	
	TSize MeSize = iDateEditor->MinimumSize();
	
	if(MeSize.iHeight <= 10)
	{
		MeSize.iHeight = ((iDateEditor->Font()->HeightInPixels() * 5) / 4);
	}
	
	TRect SmallerRect(iControlRect);
	SmallerRect.Shrink(2,2);
	
	TInt XStuff = ((SmallerRect.Width() - MeSize.iWidth) / 2);
	TInt YStuff = ((SmallerRect.Height()- MeSize.iHeight)/ 2);
	
	if(XStuff > 0)
	{
		SmallerRect.iTl.iX = (SmallerRect.iTl.iX + XStuff);
		SmallerRect.iBr.iX = (SmallerRect.iTl.iX + MeSize.iWidth);
	}
	
	if(YStuff > 0 && MeSize.iHeight > 10)
	{
		SmallerRect.iTl.iY = (SmallerRect.iTl.iY + YStuff);
		SmallerRect.iBr.iY = (SmallerRect.iTl.iY + MeSize.iHeight);
	}
	
	iDateEditor->SetRect(SmallerRect);
	iDateEditor->SetDate(aTime);
	iDateEditor->SetFocus(ETrue);
	iDateEditor->SetFocusing(ETrue);
	iDateEditor->ActivateL();
	
	SmallerRect.Grow(2,2);
	iControlRect = SmallerRect;

	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMultiControl::StartTimeEditorL(TTime aTime,const TDesC& aTitle)
{
	DeleteAllControls();
	
	iSavedTime = aTime;
	iEdType = EDateTimeEd;
	delete iTitle;
	iTitle = NULL;
	iTitle = aTitle.AllocL();
	
	CalculateRects();
	
	TTime MaxAlarm(TDateTime(2999,TMonth(11),30,23,59,59,59));
	TTime MinAlarm(TDateTime(2000,TMonth(0),0,0,0,0,0));
	
	iTimeEditor = new(ELeave)CEikTimeEditor();
	iTimeEditor->SetContainerWindowL(*this);
	iTimeEditor->ConstructL(MinAlarm,MaxAlarm,aTime,EEikTimeWithoutSecondsField);
	
	TSize MeSize = iTimeEditor->MinimumSize();
	
	if(MeSize.iHeight <= 10)
	{
		MeSize.iHeight = ((iTimeEditor->Font()->HeightInPixels() * 5) / 4);
	}
	
	TRect SmallerRect(iControlRect);
	SmallerRect.Shrink(2,2);
	
	TInt XStuff1 = ((SmallerRect.Width() - MeSize.iWidth) / 2);
	TInt YStuff1 = ((SmallerRect.Height()- MeSize.iHeight)/ 2);
	
	if(XStuff1 > 0)
	{
		SmallerRect.iTl.iX = (SmallerRect.iTl.iX + XStuff1);
		SmallerRect.iBr.iX = (SmallerRect.iTl.iX + MeSize.iWidth);
	}
	
	if(YStuff1 > 0 && MeSize.iHeight > 10)
	{
		SmallerRect.iTl.iY = (SmallerRect.iTl.iY + YStuff1);
		SmallerRect.iBr.iY = (SmallerRect.iTl.iY + MeSize.iHeight);
	}
	
	iTimeEditor->SetRect(SmallerRect);
	iTimeEditor->SetTime(aTime);
	iTimeEditor->SetFocus(ETrue);
	iTimeEditor->SetFocusing(ETrue);
	iTimeEditor->ActivateL();
	
	SmallerRect.Grow(2,2);
	
	iControlRect = SmallerRect;

	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMultiControl::StartEditorL(HBufC* aMessage,const TDesC& aTitle)
{
	DeleteAllControls();
	
	iEdType = EMessageEd;
	delete iTitle;
	iTitle = NULL;
	iTitle = aTitle.AllocL();
	
	CalculateRects();
	MakeNewsEditorL();
	
	if(aMessage)
	{
		AddTextToNewsEditorL(*aMessage);
	}
	
/*	MAknEditingStateIndicator *ei = CAknEnv::Static()->EditingStateIndicator();
	if( ei)
	{
		CAknIndicatorContainer *ic = ei->IndicatorContainer();
		if(ic)
		{
			ic->SetIndicatorObserver(MAknIndicatorObserver *aIndicatorObserver, TUid aIndicatorUid)
		
		//	const CAknPictographInterface* interf = ic->PictographInterface();
			
			//ic->IndicatorState (TUid aIndicatorId)
		}

		
	//	ic->SetIndicatorValueL(TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ), msgSize);
	//	ic->SetIndicatorState(TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),EAknIndicatorStateOn);


	}*/
			
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CMyMultiControl::MakeNewsEditorL()
{
	TCharFormat		charFormat;	
    TCharFormatMask charFormatMask;	
	
	TFontSpec MyeFontSpec = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont)->FontSpecInTwips();

	charFormat.iFontSpec = MyeFontSpec;	
   	charFormat.iFontPresentation.iTextColor = iFontColor;
   	
   	charFormatMask.SetAll();

	delete iParaform;
	iParaform = NULL;
	iParaform = CParaFormatLayer::NewL(); 
	
	delete iCharform;
	iCharform = NULL;
	iCharform = CCharFormatLayer::NewL(charFormat,charFormatMask); 
	
	delete iRichMessage;
	iRichMessage = NULL;
	iRichMessage  = CRichText::NewL(iParaform,iCharform);

    delete iEditor;
    iEditor = NULL;
	iEditor = new (ELeave) CEikRichTextEditor;
//	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	iEditor->SetContainerWindowL(*this);
	iEditor->SetAvkonWrap(ETrue);
	iEditor->ConstructL(this,10,0,EAknEditorFlagDefault,EGulFontControlAll,EGulAllFonts);
	iEditor->SetBorder(TGulBorder::ESingleBlack);
	
	TRect SmallerRect(iControlRect);
	SmallerRect.Shrink(2,2);
	iEditor->SetRect(SmallerRect);
	iEditor->SetDocumentContentL(*iRichMessage);
	iEditor->SetCursorPosL(0,EFalse);
	
	iEditor->SetFocus(ETrue);
	iEditor->SetFocusing(ETrue);
	iEditor->ClearSelectionL();	
	iEditor->UpdateScrollBarsL();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
		
void CMyMultiControl::AddTextToNewsEditorL(const TDesC& aText)
{
	if (iRichMessage && iEditor && aText.Length())
	{
		iRichMessage->Reset();
		iRichMessage->InsertL(0,aText);
		iEditor->SetDocumentContentL(*iRichMessage);
		iEditor->SetCursorPosL(0,ETrue);
		iEditor->ClearSelectionL();	
		iEditor->DrawNow();
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
TTime CMyMultiControl::GetTime(TTime& aOriginal)
{
	if(iDateEditor)
	{
		TTime RetAlarm(TDateTime(iDateEditor->Date().DateTime().Year(),iDateEditor->Date().DateTime().Month(),iDateEditor->Date().DateTime().Day(),aOriginal.DateTime().Hour(),aOriginal.DateTime().Minute(), 0,0));
		return RetAlarm;
	}
	else if(iTimeEditor)
	{
		TTime RetAlarm(TDateTime(aOriginal.DateTime().Year(),aOriginal.DateTime().Month(),aOriginal.DateTime().Day(),iTimeEditor->Time().DateTime().Hour(),iTimeEditor->Time().DateTime().Minute(), 0,0));
		return RetAlarm;
	}
	else
	{
		return iSavedTime;
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
HBufC* CMyMultiControl::GetText(void)
{
	HBufC* Ret(NULL);
	
	if(iEditor)
	{
		Ret = iEditor->GetTextInHBufL();
	}
	
	return Ret;	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CMyMultiControl::SetTextColot(const TRgb& aColor)
{
	iFontColor = aColor;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CMyMultiControl::SizeChanged()
{
	iUseFont= AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);

	TInt FontH = iUseFont->HeightInPixels();
	FontH = ((FontH * 3) / 2);
	
	TRect MyRect(Rect());
	MyRect.Shrink(4,4);
	
	TFileName ImgFileName;
	iMenuProvided.GetFileName(ImgFileName);

	if(ImgFileName.Length())
	{	
		delete iBackGround;
		iBackGround = NULL;
		iBackGround = AknIconUtils::CreateIconL(ImgFileName, EMbmYapp_a000312dBack_normal);
		AknIconUtils::SetSize(iBackGround,Rect().Size(),EAspectRatioNotPreserved);	
		
		delete iBackMask;
		iBackMask = NULL;
		iBackMask = LoadImageL(ImgFileName,EMbmYapp_a000312dBg_mask,EMbmYapp_a000312dBg_mask_mask,Rect().Size(),EAspectRatioNotPreserved);
	}		
	
	CalculateRects();
	
	if(iEditor)
	{
		HBufC* TmpBuffer = GetText();
		CleanupStack::PushL(TmpBuffer);
		
		MakeNewsEditorL();
		
		if(TmpBuffer)
		{
			AddTextToNewsEditorL(*TmpBuffer);
		}
		
		CleanupStack::PopAndDestroy(TmpBuffer);
	}
	else if(iDateEditor || iTimeEditor)
	{
		iSavedTime = GetTime(iSavedTime);
	
		TBuf<255> TmpTitle;
		if(iTitle)
		{
			if(iTitle->Des().Length() > 250)
				TmpTitle.Copy(iTitle->Des().Left(250));
			else
				TmpTitle.Copy(iTitle->Des());
		}
		
		if(iTimeEditor)
		{
			StartTimeEditorL(iSavedTime,TmpTitle);
		}
		else if(iDateEditor)
		{
			StartDateEditorL(iSavedTime,TmpTitle);
		}
	}
} 

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CMyMultiControl::CalculateRects(void)
{
	TRect MyRect(Rect());
	MyRect.Shrink(4,4);
	
	if(iUseFont && iTitle)
	{
		TInt FontH = iUseFont->HeightInPixels();
		TInt TitleHeight = ((FontH * 3) / 2);
	
		iControlRect = MyRect;
		iTitleRect = MyRect;
		
		iTitleRect.iBr.iY = (iTitleRect.iTl.iY + TitleHeight);
		iControlRect.iTl.iY = iTitleRect.iBr.iY;
	}
	else
	{
		iTitleRect.iTl.iX = iTitleRect.iTl.iY = iTitleRect.iBr.iX = iTitleRect.iBr.iY = 0;
		iControlRect = MyRect;
	}
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CMyMultiControl::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode)
{
	CFbsBitmap* FrameImg(NULL);
	CFbsBitmap* FrameMsk(NULL);

	AknIconUtils::CreateIconL(FrameImg, FrameMsk, aFileName, aImg, aMsk);

	AknIconUtils::SetSize(FrameImg,aSize,aScaleMode);  
	AknIconUtils::SetSize(FrameMsk,aSize,aScaleMode); 

	CGulIcon* Ret = CGulIcon::NewL(FrameImg,FrameMsk);

	return Ret;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMyMultiControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	
	
	if(iDateEditor)
	{
		Ret = iDateEditor->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iTimeEditor)
	{
		Ret = iTimeEditor->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iEditor)
	{		
		Ret = iEditor->OfferKeyEventL(aKeyEvent,aEventCode);
	
		MAknEditingStateIndicator *ei = CAknEnv::Static()->EditingStateIndicator();
		if( ei)
		{
			CAknIndicatorContainer *ic = ei->IndicatorContainer();
			if(ic)
			{
				TInt Tmp1 = ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorT9));
				TInt Tmp2 = ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorUpperCase));
				TInt Tmp3 = ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorLowerCase));
				TInt Tmp4 = ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorTextCase));
				TInt Tmp5 = ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorNumberCase));
				
				if(iState1 != Tmp1
				|| iState2 != Tmp2
				|| iState3 != Tmp3
				|| iState4 != Tmp4
				|| iState5 != Tmp5)
				{
					DrawNow();
				}
				
				iState1 = Tmp1; 
				iState2 = Tmp2;
				iState3 = Tmp3;
				iState4 = Tmp4;
				iState5 = Tmp5;
			}
		}
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	   		
	   		break;
		case EKeyUpArrow:
		case EKeyDownArrow:
			break;
		default:

			break;
		}	
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMyMultiControl::ComponentControl( TInt /*aIndex*/) const
{
	if(iDateEditor)
	{
		return iDateEditor;
	}
	else if(iTimeEditor)
	{
		return iTimeEditor;
	}
	else if(iEditor)
	{
		return iEditor;	
	}
	else
	{
		return NULL;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMyMultiControl::CountComponentControls() const
{	
	if(iDateEditor)
	{
		return 1;
	}
	else if(iTimeEditor)
	{
		return 1;
	}
	else if(iEditor)
	{
		return 1;	
	}
	else
	{
		return 0;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMyMultiControl::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();

	if(iBackGround)
	{
		if(iBackGround->Handle())
		{
			gc.DrawBitmap(Rect(),iBackGround);
		}
	}

	if(iBackMask)
	{
		if(iBackMask->Bitmap() && iBackMask->Mask())
		{
			if(iBackMask->Bitmap()->Handle() && iBackMask->Mask()->Handle())
			{
				TSize ImgSiz(iBackMask->Bitmap()->SizeInPixels());
				gc.DrawBitmapMasked(Rect(),iBackMask->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iBackMask->Mask(),EFalse);
			}
		}
	}
	
	TRect DrawRect(Rect());
	DrawRect.Shrink(2,2);
	
	gc.SetPenColor(iFontColor);
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iTl,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iTl.iX,DrawRect.iBr.iY));
	gc.DrawLine(DrawRect.iBr,TPoint(DrawRect.iBr.iX,DrawRect.iTl.iY)); 

	if(iTitle && iTitleRect.Height() && iUseFont)
	{
		gc.SetPenColor(iFontColor);
		gc.UseFont(iUseFont);
		
		TRect DrawTxtRect(iTitleRect);

		TInt DropMeRect = ((DrawTxtRect.Height() - iUseFont->HeightInPixels()) / 2);
		
		DrawTxtRect.iTl.iY = (DrawTxtRect.iTl.iY + DropMeRect);
		DrawTxtRect.iTl.iX = DrawTxtRect.iTl.iX + 4;

		gc.DrawText(*iTitle,DrawTxtRect,iUseFont->AscentInPixels(), CGraphicsContext::ELeft, 0);	
	
		gc.DrawLine(TPoint(iTitleRect.iTl.iX,iTitleRect.iBr.iY),iTitleRect.iBr);
	
		if(iEditor)
		{
			TBuf<20> Tmpp;
			
			MAknEditingStateIndicator *ei = CAknEnv::Static()->EditingStateIndicator();
			if( ei)
			{
				CAknIndicatorContainer *ic = ei->IndicatorContainer();
				if(ic)
				{
					//const CAknPictographInterface* interf = ic->PictographInterface();
					
					if(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorNumberCase)))
					{
						Tmpp.Copy(_L("123"));
					}
					else
					{
						if(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorT9)))
						{
							Tmpp.Copy(_L("T9: "));
						}
						
						if(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorTextCase)))
						{
							Tmpp.Append(_L("Abc"));
						}
						else if(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorUpperCase)))
						{
							Tmpp.Append(_L("ABC"));
						}
						else if(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorLowerCase)))
						{
							Tmpp.Append(_L("abc"));
						}
						else
						{
							Tmpp.Append(_L("Abc"));
						}
					}
					
			/*		//Tmpp.Num(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorT9)));
					Tmpp.Append(_L(":"));
					Tmpp.AppendNum(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorUpperCase)));
					Tmpp.Append(_L(":"));
					Tmpp.AppendNum(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorLowerCase)));
					Tmpp.Append(_L(":"));
					Tmpp.AppendNum(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorTextCase)));
					Tmpp.Append(_L(":"));
					Tmpp.AppendNum(ic->IndicatorState(TUid::Uid(EAknNaviPaneEditorIndicatorNumberCase)));
*/
				}
    
				
			//	ic->SetIndicatorValueL( EAknNaviPaneEditorIndicatorMessageLength ), msgSize);
			//	ic->SetIndicatorState(TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),EAknIndicatorStateOn);


			}
			
		
		/*	MCoeFepAwareTextEditor* FepEd = iEditor->InputCapabilities().FepAwareTextEditor();
		 	if(FepEd)
		 	{
			 	const CAknEdwinState* Statte = STATIC_CAST(CAknEdwinState*, FepEd->Extension1()->State(KNullUid));
    			if(Statte)
    			{
    				Tmpp.Num(Statte->CurrentCase());
    				Tmpp.Append(_L(":"));
    				Tmpp.AppendNum(Statte->CurrentInputMode()); 
    				Tmpp.Append(_L(":"));
    			}
		 	}

			Tmpp.AppendNum(iEditor->AknEditorCurrentInputMode());*/
			
			DrawTxtRect.iTl.iX = (DrawTxtRect.iBr.iX - 80);
			gc.DrawText(Tmpp,DrawTxtRect,iUseFont->AscentInPixels(), CGraphicsContext::ERight, 0);	
	
		}
	}
	

	gc.DrawLine(iControlRect.iTl,TPoint(iControlRect.iTl.iX,iControlRect.iBr.iY));
	gc.DrawLine(iControlRect.iTl,TPoint(iControlRect.iBr.iX,iControlRect.iTl.iY));
	gc.DrawLine(iControlRect.iBr,TPoint(iControlRect.iTl.iX,iControlRect.iBr.iY));
	gc.DrawLine(iControlRect.iBr,TPoint(iControlRect.iBr.iX,iControlRect.iTl.iY)); 
}
