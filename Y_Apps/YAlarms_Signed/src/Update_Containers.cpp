/* 	
	Copyright (c) 2001 - 2007, 
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
#include <aknglobalnote.h> 
#include <eikclbd.h>
#include <EIKFUTIL.H>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <aknmessagequerydialog.h>
#include <GDI.H>

#include "YApp_E8876001.h"
#include "YApp_E8876001.hrh"
#include "Definitions.h"

#ifdef SONE_VERSION
	#include <YApp_2002DD36.rsg>
#else

	#ifdef LAL_VERSION

	#else
		#include <YApp_E8876001.rsg>
	#endif
#endif


#include "Update_Containers.h"                     // own definitions


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CUpdateContainer::CUpdateContainer(MMyGrapProvider& aProvider)
:iProvider(aProvider),iCurrentItem(0),iFirstInList(0)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CUpdateContainer::~CUpdateContainer()
{  
	delete iTextArray;
	iTextArray = NULL;
	delete iTextBuffer;
	iTextBuffer = NULL;

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CUpdateContainer::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    
    HBufC* hejpl1 = StringLoader::LoadLC(R_STR_HJELP1);
    HBufC* hejpl2 = StringLoader::LoadLC(R_STR_HJELP2);
    HBufC* hejpl3 = StringLoader::LoadLC(R_STR_HJELP3);
    HBufC* hejpl4 = StringLoader::LoadLC(R_STR_HJELP4);
    HBufC* hejpl5 = StringLoader::LoadLC(R_STR_HJELP5);
    
    TInt length(0);
    
    if(hejpl1){
		length = (length + hejpl1->Des().Length() + 2);
    }
    if(hejpl2){
   		length = (length + hejpl2->Des().Length() + 2);
	}
    if(hejpl3){
   		length = (length + hejpl3->Des().Length() + 2);
	}
    if(hejpl4){
   		length = (length + hejpl4->Des().Length() + 2);
	}
    if(hejpl5){
   		length = (length + hejpl5->Des().Length() + 2);
	}
    
    delete iTextBuffer;
    iTextBuffer = NULL;
    if(length > 0){
		iTextBuffer = HBufC::NewL(length);
	    if(hejpl1){
			iTextBuffer->Des().Append(hejpl1->Des());
			iTextBuffer->Des().Append(_L("\n\n"));
	    }
	    if(hejpl2){
			iTextBuffer->Des().Append(hejpl2->Des());
			iTextBuffer->Des().Append(_L("\n\n"));
		}
	    if(hejpl3){
			iTextBuffer->Des().Append(hejpl3->Des());
			iTextBuffer->Des().Append(_L("\n\n"));
		}
	    if(hejpl4){
			iTextBuffer->Des().Append(hejpl4->Des());
			iTextBuffer->Des().Append(_L("\n\n"));
		}
	    if(hejpl5){
			iTextBuffer->Des().Append(hejpl5->Des());
			iTextBuffer->Des().Append(_L("\n\n"));
		}	
    }
    
    CleanupStack::PopAndDestroy(5);
    
	SetRect(aRect);	
	
	ActivateL();
	DrawNow();	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CUpdateContainer::SetTextColot(const TRgb& aColor,const TRgb& aSelColor)
{
	iItemTxtColor = aColor;
	iItemTxtColorSel = aSelColor;
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::SizeChanged()
{
	TInt ScrollW = (Rect().Width() / 50);
	
	if(ScrollW < 8)
	{
		ScrollW = 8;
	}
		
	iScrollRect.iBr.iX = (Rect().iBr.iX - 2);
	iScrollRect.iBr.iY = (Rect().iBr.iY - 2);	
	iScrollRect.iTl.iX = (iScrollRect.iBr.iX - ScrollW);
	iScrollRect.iTl.iY = (Rect().iTl.iY + 2);

	TInt FontH = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->HeightInPixels();
	
	delete iTextArray;
	iTextArray = NULL;
	iTextArray = new (ELeave)CArrayFixFlat<TPtrC>(15);;
	
	TInt widthText = Rect().Width();
	
	if(iProvider.ScrollBack())
	{
		if(iProvider.ScrollBack()->Bitmap() && iProvider.ScrollBack()->Mask())
		{
			if(iProvider.ScrollBack()->Bitmap()->Handle() && iProvider.ScrollBack()->Mask()->Handle())
			{
				widthText = (widthText - iProvider.ScrollBack()->Bitmap()->SizeInPixels().iWidth);
			}
		}
	}
	if(iTextBuffer){
		AknTextUtils::WrapToArrayL(*iTextBuffer,widthText, *AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont), *iTextArray);
	}
	iMaxItems = (Rect().Height() / FontH);
	
	if(iFirstInList > iCurrentItem)
	{
		iFirstInList = iCurrentItem;
	}
	else if(iFirstInList < (iCurrentItem - (iMaxItems-1)))
	{
		iFirstInList = iCurrentItem - (iMaxItems-1);
	}		
				
} 

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	if(iProvider.BackGround())
	{
		if(iProvider.BackGround()->Handle())
		{
			gc.DrawBitmap(Rect(),iProvider.BackGround());
		}
	}
	
	CGulIcon* TmpDrawMe(NULL);
	if(iProvider.BackAdd().Count())
	{
		if(iProvider.CurrnetAddIndex() < iProvider.BackAdd().Count())
		{
			TmpDrawMe = iProvider.BackAdd()[iProvider.CurrnetAddIndex()];
		}
		else
		{
			TmpDrawMe = iProvider.BackAdd()[0];
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
	
	if(iProvider.BackMask())
	{
		if(iProvider.BackMask()->Bitmap() && iProvider.BackMask()->Mask())
		{
			if(iProvider.BackMask()->Bitmap()->Handle() && iProvider.BackMask()->Mask()->Handle())
			{
				TSize ImgSiz(iProvider.BackMask()->Bitmap()->SizeInPixels());
				gc.DrawBitmapMasked(Rect(),iProvider.BackMask()->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iProvider.BackMask()->Mask(),EFalse);
			}
		}
	}	
	
	if(iMaxItems > 0 && iTextArray)
	{
		TInt StartGap = ((Rect().Height() % iMaxItems) / 2);
		TInt HeighGap = (Rect().Height() / iMaxItems);

		TRect ItemRect(Rect());
		
		ItemRect.iTl.iY = (Rect().iTl.iY + StartGap);
		ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
		
		for(TInt i=0; i < iMaxItems; i++)
		{
			TInt CurrItem = (iFirstInList + i);
			if(CurrItem >= 0 && CurrItem < iTextArray->Count())
			{
				TBool Selected(EFalse);
				
				if(iCurrentItem == CurrItem)
				{
					Selected = ETrue;
				}
				
				DrawItem(gc,ItemRect, iTextArray->At(CurrItem),Selected);
			}
			
			ItemRect.iTl.iY = ItemRect.iBr.iY;
			ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
		}
		
		if(iMaxItems > 1 && iMaxItems < iTextArray->Count())
		{
			if(iProvider.ScrollBack())
			{
				if(iProvider.ScrollBack()->Bitmap() && iProvider.ScrollBack()->Mask())
				{
					if(iProvider.ScrollBack()->Bitmap()->Handle() && iProvider.ScrollBack()->Mask()->Handle())
					{
						TSize SSImgSiz(iProvider.ScrollBack()->Bitmap()->SizeInPixels());
						gc.DrawBitmapMasked(iScrollRect,iProvider.ScrollBack()->Bitmap(),TRect(0,0,SSImgSiz.iWidth,SSImgSiz.iHeight),iProvider.ScrollBack()->Mask(),EFalse);
					}
				}
			}
			
			if(iProvider.Scroller())
			{
				if(iProvider.Scroller()->Bitmap() && iProvider.Scroller()->Mask())
				{
					if(iProvider.Scroller()->Bitmap()->Handle() && iProvider.Scroller()->Mask()->Handle())
					{
						TSize SRImgSiz(iProvider.Scroller()->Bitmap()->SizeInPixels());
						
						TSize SRSizeImg(iScrollRect.Width(), ((iScrollRect.Height() * iMaxItems) / iTextArray->Count()));
										
						TInt ScroollHeight = (iScrollRect.Height() - SRImgSiz.iHeight);
						
						TInt TmpCunt((iTextArray->Count() - 1));
						
						TInt StartSrollY = ((ScroollHeight * iCurrentItem) / TmpCunt);
								
						StartSrollY = StartSrollY + iScrollRect.iTl.iY;
						
						TRect ScrollerRect(iScrollRect.iTl.iX,StartSrollY,iScrollRect.iBr.iX,(StartSrollY + SRImgSiz.iHeight));

						// have the match in here..
						gc.DrawBitmapMasked(ScrollerRect,iProvider.Scroller()->Bitmap(),TRect(0,0,SRImgSiz.iWidth,SRImgSiz.iHeight),iProvider.Scroller()->Mask(),EFalse);
					}
				}
			}
		}
	}
	
	if(iProvider.IsShowingMenu())
	{
		if(iProvider.BackMask())
		{
			if(iProvider.BackMask()->Bitmap() && iProvider.BackMask()->Mask())
			{
				if(iProvider.BackMask()->Bitmap()->Handle() && iProvider.BackMask()->Mask()->Handle())
				{
					TSize ImgSiz(iProvider.BackMask()->Bitmap()->SizeInPixels());
					gc.DrawBitmapMasked(Rect(),iProvider.BackMask()->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iProvider.BackMask()->Mask(),EFalse);
				}
			}
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::DrawItem(CWindowGc& aGc,const TRect& aRect,const TDesC& aText, TBool aSelected) const
{
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	
	if(aSelected && iProvider.ListSel())
	{
		if(iProvider.ListSel()->Bitmap() && iProvider.ListSel()->Mask())
		{
			if(iProvider.ListSel()->Bitmap()->Handle() && iProvider.ListSel()->Mask()->Handle())
			{
				TSize ImgSiz(iProvider.ListSel()->Bitmap()->SizeInPixels());
				aGc.DrawBitmapMasked(aRect,iProvider.ListSel()->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iProvider.ListSel()->Mask(),EFalse);
			}
		}
	}

	const CFont* FirstLine = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);
	
	if(aSelected)
		aGc.SetPenColor(iItemTxtColor);
	else
		aGc.SetPenColor(iItemTxtColorSel);
	
	aGc.UseFont(FirstLine);
	aGc.DrawText(aText,aRect,FirstLine->AscentInPixels(),CGraphicsContext::ELeft, 0);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CUpdateContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aEventCode*/)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	


	switch (aKeyEvent.iCode)
    {
    case EKeyDevice3:
   		STATIC_CAST(CEikAppUi*,CEikonEnv::Static()->AppUi())->HandleCommandL(EMyOptionsCommand);
		break;
	case EKeyUpArrow:
	case EKeyDownArrow:
		if(iTextArray)
		{
			if(aKeyEvent.iCode == EKeyUpArrow)
				iCurrentItem--;
			else
				iCurrentItem++;
			
			if(iCurrentItem < 0)
				iCurrentItem = 0;
			else if(iCurrentItem >= iTextArray->Count())
				iCurrentItem = iTextArray->Count() - 1;
			
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
		break;
	default:

		break;
	}	
	
	return Ret;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CUpdateContainer::AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts)
{

	
	aCommands.AppendL(0);
	aTexts.AppendL(_L("None"));
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CUpdateContainer::HandleViewCommandL(TInt /*aCommand*/)
{
/*	switch(aCommand)
	{
	case EShowInfo:
		break;
	default:
		break;
	}	*/
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CUpdateContainer::SetMenuL(void)
{
	TBuf<60> hjelp2;
	StringLoader::Load(hjelp2,R_CMD_BACK);
	
	iProvider.SetCbaL(R_MY_UPDATE_CBA,KNullDesC,hjelp2);
}





