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


#include "WorkDays_Containers.h"                     // own definitions

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CWorkDContainer::CWorkDContainer(MMyGrapProvider& aProvider)
:iProvider(aProvider),iCurrentItem(0),iFirstInList(0)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CWorkDContainer::~CWorkDContainer()
{  

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CWorkDContainer::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    
    TLocale locc;
    iWorkDays = locc.WorkDays();
    
	SetRect(aRect);	
	
	ActivateL();
	DrawNow();	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CWorkDContainer::SetTextColot(const TRgb& aColor,const TRgb& aSelColor)
{
	iItemTxtColor = aColor;
	iItemTxtColorSel = aSelColor;
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CWorkDContainer::SizeChanged()
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
	
	iItemCount = 7;// week days count;
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
void CWorkDContainer::Draw(const TRect& /*aRect*/) const
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
	
	if(iMaxItems > 0)
	{
		TInt StartGap = ((Rect().Height() % iMaxItems) / 2);
		TInt HeighGap = (Rect().Height() / iMaxItems);

		TRect ItemRect(Rect());
		
		ItemRect.iTl.iY = (Rect().iTl.iY + StartGap);
		ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
		
		for(TInt i=0; i < iMaxItems; i++)
		{
			TInt CurrItem = (iFirstInList + i);
			if(CurrItem >= 0 && CurrItem < iItemCount)
			{
				TBool Selected(EFalse);
				
				if(iCurrentItem == CurrItem)
				{
					Selected = ETrue;
				}
				
				DrawItem(gc,ItemRect,CurrItem,Selected);
			}
			
			ItemRect.iTl.iY = ItemRect.iBr.iY;
			ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
		}
		
		if(iMaxItems > 1 && iMaxItems < iItemCount)
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
						
						TSize SRSizeImg(iScrollRect.Width(), ((iScrollRect.Height() * iMaxItems) / iItemCount));
										
						TInt ScroollHeight = (iScrollRect.Height() - SRImgSiz.iHeight);
						
						TInt TmpCunt((iItemCount - 1));
						
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
void CWorkDContainer::DrawItem(CWindowGc& aGc,const TRect& aRect,TInt aItem, TBool aSelected) const
{
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	
	if(aSelected && iProvider.ListSel())
	{
		if(iProvider.ListSel()->Bitmap() && iProvider.ListSel()->Mask())
		{
			if(iProvider.ListSel()->Bitmap()->Handle() && iProvider.ListSel()->Mask()->Handle())
			{
				aGc.DrawBitmapMasked(aRect,iProvider.ListSel()->Bitmap(),TRect(0,0,aRect.Width(),aRect.Height()),iProvider.ListSel()->Mask(),EFalse);
			}
		}
	}

	const CFont* SecondLine = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
	
	TRect SmallerRect(aRect);
	
	if(iScrollRect.iTl.iX > 0)
	{
		SmallerRect.iBr.iX = iScrollRect.iTl.iX;
	}
	
	TRect IconRect((SmallerRect.iTl.iX + aRect.Height()),SmallerRect.iTl.iY,(SmallerRect.iTl.iX + (aRect.Height() * 2)),SmallerRect.iBr.iY);

	TRect SecnR((SmallerRect.iTl.iX + (aRect.Height() * 3)),SmallerRect.iTl.iY,SmallerRect.iBr.iX,SmallerRect.iBr.iY);
	
	TBool SelEcted(EFalse);
	TDay MyDDay(EMonday);
	switch(aItem)
	{
	case 0:
		if(iWorkDays & 0x01)
		{
			SelEcted = ETrue;
		}
		MyDDay = EMonday;
		break;
	case 1:
		if(iWorkDays & 0x02)
		{
			SelEcted = ETrue;
		}
		MyDDay = ETuesday;
		break;
	case 2:
		if(iWorkDays & 0x04)
		{
			SelEcted = ETrue;
		}
		MyDDay = EWednesday;
		break;
	case 3:
		if(iWorkDays & 0x08)
		{
			SelEcted = ETrue;
		}
		MyDDay = EThursday;
		break;
	case 4:
		if(iWorkDays & 0x10)
		{
			SelEcted = ETrue;
		}
		MyDDay = EFriday;
		break;
	case 5:
		if(iWorkDays & 0x20)
		{
			SelEcted = ETrue;
		}
		MyDDay = ESaturday;
		break;
	case 6:
		if(iWorkDays & 0x40)
		{
			SelEcted = ETrue;
		}
		MyDDay = ESunday;
		break;
	}
	
	TDayName ThisDay(MyDDay);
	
	if(aSelected)
		aGc.SetPenColor(iItemTxtColor);
	else
		aGc.SetPenColor(iItemTxtColorSel);
		
	if(SelEcted)
	{
		TInt RepIcon(14);
		if(RepIcon >= 0 && RepIcon < iProvider.Numbers().Count())
		{
			if(iProvider.Numbers()[RepIcon])
			{
				if(iProvider.Numbers()[RepIcon]->Bitmap() && iProvider.Numbers()[RepIcon]->Mask())
				{
					if(iProvider.Numbers()[RepIcon]->Bitmap()->Handle() && iProvider.Numbers()[RepIcon]->Mask()->Handle())
					{
						TSize ImgSiz(iProvider.Numbers()[RepIcon]->Bitmap()->SizeInPixels());
					
						TRect DbgRect(IconRect);
						
						DbgRect.iTl.iX = (DbgRect.iTl.iX + ((DbgRect.Width() - ImgSiz.iWidth) / 2));
						DbgRect.iBr.iX = (DbgRect.iTl.iX + ImgSiz.iWidth);
						
						DbgRect.iTl.iY = (DbgRect.iTl.iY + ((DbgRect.Height()- ImgSiz.iHeight)/ 2));
						DbgRect.iBr.iY = (DbgRect.iTl.iY + ImgSiz.iHeight);
						
						// have the match in here..
						aGc.DrawBitmapMasked(DbgRect,iProvider.Numbers()[RepIcon]->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),iProvider.Numbers()[RepIcon]->Mask(),EFalse);			
					}
				}
			}	
		}
	}
	
	TInt HHGappp = ((SecnR.Height() - SecondLine->HeightInPixels()) / 2);
	
	TRect DrawSec(SecnR);
	if(HHGappp > 0)
	{
		DrawSec.iTl.iY = (DrawSec.iTl.iY + HHGappp);
	}
	
	aGc.UseFont(SecondLine);
	aGc.DrawText(ThisDay,DrawSec,SecondLine->AscentInPixels(),CGraphicsContext::ELeft, 0);
	
	aGc.SetPenStyle(CGraphicsContext::EDottedPen);
	aGc.SetPenColor(iItemTxtColor);
	aGc.DrawLine(TPoint(aRect.iTl.iX,aRect.iBr.iY),aRect.iBr);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CWorkDContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aEventCode*/)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	


	switch (aKeyEvent.iCode)
    {
    case EKeyDevice3:
   		HandleViewCommandL(EWorkDChangeValue);
	   	break;
	case EKeyUpArrow:
	case EKeyDownArrow:
		{
			if(aKeyEvent.iCode == EKeyUpArrow)
				iCurrentItem--;
			else
				iCurrentItem++;
			
			if(iCurrentItem < 0)
				iCurrentItem = iItemCount - 1;
			else if(iCurrentItem >= iItemCount)
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
void CWorkDContainer::AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts)
{
	TBuf<60> hjelpper;
	StringLoader::Load(hjelpper,R_STR_CHANGEVAL);

	aCommands.AppendL(EWorkDChangeValue);
	aTexts.AppendL(hjelpper);
	
	StringLoader::Load(hjelpper,R_CMD_CANCEL);
	
	aCommands.AppendL(EWorkDaysCancel);
	aTexts.AppendL(hjelpper);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CWorkDContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EWorkDChangeValue:
		{
			switch(iCurrentItem)
			{
			case 0:
				if(iWorkDays & 0x01)
				{
					iWorkDays = iWorkDays ^ 0x01;
				}
				else
				{
					iWorkDays = iWorkDays | 0x01;
				}
				break;
			case 1:
				if(iWorkDays & 0x02)
				{
					iWorkDays = iWorkDays ^ 0x02;
				}
				else
				{
					iWorkDays = iWorkDays | 0x02;
				}
				break;
			case 2:
				if(iWorkDays & 0x04)
				{
					iWorkDays = iWorkDays ^ 0x04;
				}
				else
				{
					iWorkDays = iWorkDays | 0x04;
				}
				break;
			case 3:
				if(iWorkDays & 0x08)
				{
					iWorkDays = iWorkDays ^ 0x08;
				}
				else
				{
					iWorkDays = iWorkDays | 0x08;
				}
				break;
			case 4:
				if(iWorkDays & 0x10)
				{
					iWorkDays = iWorkDays ^ 0x10;
				}
				else
				{
					iWorkDays = iWorkDays | 0x10;
				}
				break;
			case 5:
				if(iWorkDays & 0x20)
				{
					iWorkDays = iWorkDays ^ 0x20;
				}
				else
				{
					iWorkDays = iWorkDays | 0x20;
				}
				break;
			case 6:
				if(iWorkDays & 0x40)
				{
					iWorkDays = iWorkDays ^ 0x40;
				}
				else
				{
					iWorkDays = iWorkDays | 0x40;
				}
				break;
			}
		}
		DrawNow();
		break;
	default:
		break;
	}	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CWorkDContainer::SetWorkDaysL(void)
{
	TLocale locc;
    locc.SetWorkDays(iWorkDays);
    locc.Set();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CWorkDContainer::ShowNoteL(const TDesC& aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
	TInt NoteId = dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	User::After(2000000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CWorkDContainer::SetMenuL(void)
{
	TBuf<60> hjelp1,hjelp2;
	StringLoader::Load(hjelp1,R_CMD_OPTIONS);
	StringLoader::Load(hjelp2,R_CMD_DONE);
	
	iProvider.SetCbaL(R_MY_WORKD_CBA,hjelp1,hjelp2);
}





