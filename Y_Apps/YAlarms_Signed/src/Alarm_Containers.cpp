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


#include "Alarm_Containers.h"                     // own definitions

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAlarmContainer::CAlarmContainer(MMyGrapProvider& aProvider)
:iProvider(aProvider),iCurrentItem(0),iFirstInList(0)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAlarmContainer::~CAlarmContainer()
{  
	delete iMyAlarmItem;
	iMyAlarmItem = NULL;
	
	delete iMyMenuControl;
	iMyMenuControl = NULL;
	
	delete iMyMultiControl;
	iMyMultiControl = NULL;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAlarmContainer::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    
	SetRect(aRect);	
	
	ActivateL();
	DrawNow();	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAlarmContainer::SetAlarmDataL(CMyAlarmItem* aData)
{
	delete iMyAlarmItem;
	iMyAlarmItem = NULL;

	iMyAlarmItem = new(ELeave)CMyAlarmItem();
	if(aData)
	{
		if(aData->iMessage)
		{
			iMyAlarmItem->iMessage = aData->iMessage->Des().AllocL();
		}
		
		iMyAlarmItem->iTime = aData->iTime;
		iMyAlarmItem->iClockAlarm = aData->iClockAlarm;
		iMyAlarmItem->iStatus = aData->iStatus;
		iMyAlarmItem->iRepeat = aData->iRepeat;
		iMyAlarmItem->iAlarmId= aData->iAlarmId;
	}
	else
	{
		iMyAlarmItem->iMessage = NULL;
		iMyAlarmItem->iTime.HomeTime();
		iMyAlarmItem->iClockAlarm = ETrue;
		iMyAlarmItem->iStatus = EAlarmStatusEnabled;
		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatNext24Hours;
		iMyAlarmItem->iAlarmId = -1;
	}
	
	CheckLinesValues();
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAlarmContainer::SetTextColot(const TRgb& aColor,const TRgb& aSelColor)
{
	iItemTxtColor = aColor;
	iItemTxtColorSel = aSelColor;
	
	if(iMyMenuControl)
	{
		iMyMenuControl->SetTextColot(aColor,aSelColor);
	}
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAlarmContainer::SizeChanged()
{
	TRect MnuRect(Rect());
	MnuRect.Shrink(14,14);

	TInt MyHeight = (((AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont)->HeightInPixels() * 3) / 2) * 4); 

	TInt Smaller = (MnuRect.Height() - MyHeight);

	if(Smaller > 0)
	{
		MnuRect.iTl.iY = (MnuRect.iTl.iY + Smaller);
	}
	
	MnuRect.Grow(4,4);
    	
	if(iMyMenuControl)
	{	
		iMyMenuControl->SetRect(MnuRect);
	}
	else
	{
		iMyMenuControl = CMyMenuControl::NewL(MnuRect,*this,iItemTxtColor,iItemTxtColorSel,KFrameReadInterval);
	}
	
	if(iMyMultiControl)
	{
		iMyMultiControl->SetRect(MnuRect);
	}
	
	TInt ScrollW = (Rect().Width() / 50);
	
	if(ScrollW < 8)
	{
		ScrollW = 8;
	}
		
	iScrollRect.iBr.iX = (Rect().iBr.iX - 2);
	iScrollRect.iBr.iY = (Rect().iBr.iY - 2);	
	iScrollRect.iTl.iX = (iScrollRect.iBr.iX - ScrollW);
	iScrollRect.iTl.iY = (Rect().iTl.iY + 2);

	CheckLinesValues();		
} 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAlarmContainer::CheckLinesValues()
{	
	if(iMyAlarmItem)
	{
		switch(iMyAlarmItem->iRepeat)
		{
		case EAlarmRepeatDefintionRepeatDaily:
		case EAlarmRepeatDefintionRepeatWeekly:
		case EAlarmRepeatDefintionRepeatWorkday:
		case EAlarmRepeatDefintionRepeatNext24Hours:
			{
				iItemCount = 4;
				// not showing day
			}
			break;
		default:
			{
				iItemCount = 5;
				// showing the day as well.
			}
			break;
		}
		
		TInt FontH = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->HeightInPixels();
		FontH = (FontH * 2);
		
		iMaxItems = (Rect().Height() / FontH);
		
//		if(iItemCount > iMaxItems)
//		{
//			iItemCount = iMaxItems;
//		}
		
		if(iFirstInList > iCurrentItem)
		{
			iFirstInList = iCurrentItem;
		}
		else if(iFirstInList < (iCurrentItem - (iItemCount-1)))
		{
			iFirstInList = iCurrentItem - (iItemCount-1);
		}
	}

}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAlarmContainer::Draw(const TRect& /*aRect*/) const
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

		
		for(TInt i=0; i < iItemCount; i++)
		{
			TInt CurrItem = (iFirstInList + i);
			
			if(iMyAlarmItem)
			{
				TBool Selected(EFalse);
						
				if(iCurrentItem == CurrItem)
				{
					Selected = ETrue;
				}
					
				if(CurrItem > 1)
				{
					switch(iMyAlarmItem->iRepeat)
					{
					case EAlarmRepeatDefintionRepeatDaily:
					case EAlarmRepeatDefintionRepeatWeekly:
					case EAlarmRepeatDefintionRepeatWorkday:
					case EAlarmRepeatDefintionRepeatNext24Hours:
						{
							// not showing day
							CurrItem = CurrItem + 1;
						}
						break;
					default:
						{
							// showing the day as well.
						}
						break;
					}
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
	
	if(iProvider.IsShowingMenu()
	|| IsShowingMenu())
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
void CAlarmContainer::DrawItem(CWindowGc& aGc,const TRect& aRect,TInt aItem, TBool aSelected) const
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
	const CFont* SecondLine = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
	
	TInt FHeight = FirstLine->HeightInPixels();
	
	TRect SmallerRect(aRect);
	
	if(iScrollRect.iTl.iX > 0)
	{
		SmallerRect.iBr.iX = iScrollRect.iTl.iX;
	}
	
	TInt XXGappp = FirstLine->HeightInPixels();

	
	TRect FirsR((SmallerRect.iTl.iX + XXGappp),SmallerRect.iTl.iY,SmallerRect.iBr.iX,(SmallerRect.iTl.iY + FHeight));
	TRect SecnR(SmallerRect.iTl.iX,FirsR.iBr.iY,SmallerRect.iBr.iX,SmallerRect.iBr.iY);
	
	TBuf<100> FirstTxt;
	TBuf<100> SecondTxt;

	if(iMyAlarmItem)
	{
		if(aItem == 0)	// repeat
		{
			StringLoader::Load(FirstTxt,R_STR_REPEAT);
			
			switch(iMyAlarmItem->iRepeat)
			{
			case EAlarmRepeatDefintionRepeatDaily:
				{
					StringLoader::Load(SecondTxt,R_STR_DAILY);
				}
				break;
			case EAlarmRepeatDefintionRepeatWeekly:
				{
					SecondTxt.Copy(TDayName(iMyAlarmItem->iTime.DayNoInWeek()));	
				}
				break;
			case EAlarmRepeatDefintionRepeatWorkday:
				{
					StringLoader::Load(SecondTxt,R_STR_WORKDAYS);
				}
				break;
			case EAlarmRepeatDefintionRepeatNext24Hours:
				{
					StringLoader::Load(SecondTxt,R_STR_INNEXTT24);
				}
				break;
			case EAlarmRepeatDefintionRepeatOnce:
				{
					StringLoader::Load(SecondTxt,R_STR_ONCE);
				}
				break;
			default:
				break;
			}	
		}
		else if(aItem == 1)	// Time
		{
			StringLoader::Load(FirstTxt,R_STR_TIME);

			SecondTxt.Zero();
				
			if(iMyAlarmItem->iTime.DateTime().Hour() < 10)
			{
				SecondTxt.AppendNum(0);
			}
			
			SecondTxt.AppendNum(iMyAlarmItem->iTime.DateTime().Hour());
			
			SecondTxt.Append(_L(":"));
			
			if(iMyAlarmItem->iTime.DateTime().Minute() < 10)
			{
				SecondTxt.AppendNum(0);
			}
			SecondTxt.AppendNum(iMyAlarmItem->iTime.DateTime().Minute());
			
		}
		else if(aItem == 2)	// Date
		{
			StringLoader::Load(FirstTxt,R_STR_DATE);
			
			TDayNameAbb ThisDay(iMyAlarmItem->iTime.DayNoInWeek());
			SecondTxt.Copy(ThisDay);
			SecondTxt.Append(_L(" "));
			SecondTxt.AppendNum(iMyAlarmItem->iTime.DateTime().Day() + 1);
			SecondTxt.Append(_L("/"));
			TMonthNameAbb ThisMonth(iMyAlarmItem->iTime.DateTime().Month());
			SecondTxt.Append(ThisMonth);
			SecondTxt.Append(_L("/"));
			SecondTxt.AppendNum(iMyAlarmItem->iTime.DateTime().Year());
		}
		else if(aItem == 3)	// Type
		{
			StringLoader::Load(FirstTxt,R_STR_TYPE);
			
			if(iMyAlarmItem->iClockAlarm)
				StringLoader::Load(SecondTxt,R_STR_CLOCKALRM);
			else
				StringLoader::Load(SecondTxt,R_STR_CALALARM);
		}
		else if(aItem == 4)	// Message
		{
			StringLoader::Load(FirstTxt,R_STR_MESSAGE);

			if(iMyAlarmItem->iMessage)
			{
				if(iMyAlarmItem->iMessage->Des().Length() > 100)
				{
					SecondTxt.Copy(iMyAlarmItem->iMessage->Des().Left(100));
				}
				else
				{
					SecondTxt.Copy(iMyAlarmItem->iMessage->Des());
				}
			}
		}
	}

	
	if(aSelected)
		aGc.SetPenColor(iItemTxtColor);
	else
		aGc.SetPenColor(iItemTxtColorSel);
	
	aGc.UseFont(FirstLine);
	aGc.DrawText(FirstTxt,FirsR,FirstLine->AscentInPixels(),CGraphicsContext::ELeft, 0);
	
	TInt HHGappp = ((SecnR.Height() - SecondLine->HeightInPixels()) / 2);
	
	TRect DrawSec(SecnR);
	if(HHGappp > 0)
	{
		DrawSec.iTl.iY = (DrawSec.iTl.iY + HHGappp);
	}
	
	aGc.UseFont(SecondLine);
	aGc.DrawText(SecondTxt,DrawSec,SecondLine->AscentInPixels(),CGraphicsContext::ECenter, 0);
	
	aGc.SetPenStyle(CGraphicsContext::EDottedPen);
	aGc.SetPenColor(iItemTxtColor);
	aGc.DrawLine(TPoint(aRect.iTl.iX,aRect.iBr.iY),aRect.iBr);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CAlarmContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iMyMultiControl)
	{
		Ret = iMyMultiControl->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iMyMenuControl && iShowMenu)
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	    	HandleViewCommandL(EMyOptionsSelect2);
	   		break;
		case EKeyUpArrow:
			{
				iMyMenuControl->UpAndDown(ETrue);
				Ret = EKeyWasConsumed;
			}
			break;
		case EKeyDownArrow:
			{
				iMyMenuControl->UpAndDown(EFalse);
				Ret = EKeyWasConsumed;
			}
			break;
		default:
			break;
		}	
		
	}
	else 
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	   		HandleViewCommandL(EAlarmChangeValue);
	   		break;
		case EKeyUpArrow:
		case EKeyDownArrow:
			if(iMyAlarmItem)
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
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CAlarmContainer::GetFileName(TDes& aFileName)
{
	iProvider.GetFileName(aFileName);
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CAlarmContainer::AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts)
{
	TBuf<100> hjelp1;

	if(IsShowingMenu())
	{
		StringLoader::Load(hjelp1,R_STR_INNEXTT24);
		aCommands.AppendL(ENext24Rep);
		aTexts.AppendL(hjelp1);
		
		StringLoader::Load(hjelp1,R_STR_ONCE);
		aCommands.AppendL(EOnceRep);
		aTexts.AppendL(hjelp1);
		
		StringLoader::Load(hjelp1,R_STR_DAILY);
		aCommands.AppendL(EDailyRep);
		aTexts.AppendL(hjelp1);
		
		StringLoader::Load(hjelp1,R_STR_WORKDAYS);
		aCommands.AppendL(EWorkDaysRep);
		aTexts.AppendL(hjelp1);
		
		aCommands.AppendL(EMondaysRep);
		aTexts.AppendL(TDayName(EMonday));
		
		aCommands.AppendL(ETusdaysRep);
		aTexts.AppendL(TDayName(ETuesday));
		
		aCommands.AppendL(EWednesdaysRep);
		aTexts.AppendL(TDayName(EWednesday));
		
		aCommands.AppendL(EThursdaysRep);
		aTexts.AppendL(TDayName(EThursday));
		
		aCommands.AppendL(EFridaysRep);
		aTexts.AppendL(TDayName(EFriday));
		
		aCommands.AppendL(ESaturdaysRep);
		aTexts.AppendL(TDayName(ESaturday));
		
		aCommands.AppendL(ESundaysRep);
		aTexts.AppendL(TDayName(ESunday));    	
	}
	else
	{	
		StringLoader::Load(hjelp1,R_STR_CHANGEVAL);
		aCommands.AppendL(EAlarmChangeValue);
		aTexts.AppendL(hjelp1);
		
		StringLoader::Load(hjelp1,R_CMD_CANCEL);
		
		aCommands.AppendL(EAlarmBack);
		aTexts.AppendL(hjelp1);
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CAlarmContainer::ComponentControl( TInt /*aIndex*/) const
{
	if(iMyMultiControl)
	{
		return iMyMultiControl;
	}
	else
	{
		return iMyMenuControl;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CAlarmContainer::CountComponentControls() const
{	
	TInt RetVal(0);
	
	if(iMyMultiControl)
	{
		RetVal = 1;
	}
	else if(iMyMenuControl && iShowMenu)
	{
		RetVal = RetVal + 1;
	}
	
	return RetVal;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAlarmContainer::HandleViewCommandL(TInt aCommand)
{
	TBuf<200> hjelp1;

	switch(aCommand)
	{
	case EMyOptionsSelect3:
		if(iMyMultiControl && iMyAlarmItem)
		{
			switch(iMyMultiControl->GetType())
			{
			case CMyMultiControl::EDateTimeEd:
				{
					iMyAlarmItem->iTime = iMyMultiControl->GetTime(iMyAlarmItem->iTime);
				}
				break;
			case CMyMultiControl::EMessageEd:
				{
					delete iMyAlarmItem->iMessage;
					iMyAlarmItem->iMessage = NULL;
					
					iMyAlarmItem->iMessage = iMyMultiControl->GetText();		
				}
				break;
			}
		}
	case EMyOptionsBack3:
		{
			delete iMyMultiControl;
			iMyMultiControl = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EMyOptionsSelect2:
		if(iMyMenuControl && iMyAlarmItem)
		{
			TInt Comand = iMyMenuControl->SelectedCommand();
			iShowMenu = EFalse;
			SetMenuL();
			iMyMenuControl->ActivateDeActivate(iShowMenu);
			
			TDay AlDayy(EMonday);
			 
			switch(Comand)
			{
			case ENext24Rep:
				iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatNext24Hours;
				break;
	    	case EOnceRep:
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatOnce;
	    		break;
	    	case EDailyRep:
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatDaily;
	    		break;
	    	case EWorkDaysRep:
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWorkday;
	    		break;
	    	case EMondaysRep:
	    		AlDayy = EMonday;
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWeekly;
	    		break;
	    	case ETusdaysRep:
	    		AlDayy = ETuesday;
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWeekly;
	    		break;
	    	case EWednesdaysRep:
	    		AlDayy = EWednesday;
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWeekly;
	    		break;
	    	case EThursdaysRep:
	    		AlDayy = EThursday;
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWeekly;
	    		break;
	    	case EFridaysRep:
	    		AlDayy = EFriday;
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWeekly;
	    		break;
	    	case ESaturdaysRep:	
	    		AlDayy = ESaturday;
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWeekly;
	    		break;
	    	case ESundaysRep:
	    		AlDayy = ESunday;
	    		iMyAlarmItem->iRepeat = EAlarmRepeatDefintionRepeatWeekly;
	    		break;
			}
	
			TTime NowTim;
			NowTim.HomeTime();
			
			TTime RetAlarm(TDateTime(NowTim.DateTime().Year(),NowTim.DateTime().Month(),NowTim.DateTime().Day(),iMyAlarmItem->iTime.DateTime().Hour(),iMyAlarmItem->iTime.DateTime().Minute(), 0,0));
			
			if(iMyAlarmItem->iRepeat == EAlarmRepeatDefintionRepeatWeekly)
			{
				if(iMyAlarmItem->iTime.DayNoInWeek() != AlDayy)
				{
					if(AlDayy != RetAlarm.DayNoInWeek())
					{
						TInt Diff = (AlDayy - RetAlarm.DayNoInWeek());
					
						if(Diff < 0)
						{
							Diff = (7 + Diff);
						}
						
						RetAlarm = (NowTim + TTimeIntervalDays(Diff));
					}
					
					iMyAlarmItem->iTime = RetAlarm;	
				}
			
			}
		
			CheckLinesValues();
		}
	case EMyOptionsBack2:
		{
			iShowMenu = EFalse;
			SetMenuL();	
			if(iMyMenuControl)
			{
				iMyMenuControl->ActivateDeActivate(iShowMenu);
			}	
		}
		SetMenuL();
		DrawNow();
		break;
	case EAlarmChangeValue:
		if(iMyAlarmItem)
		{
		
			delete iMyMultiControl;
			iMyMultiControl = NULL;
				
			TRect MnuRect(Rect());
			MnuRect.Shrink(14,14);

			TInt MyHeight = (((AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont)->HeightInPixels() * 3) / 2) * 4); 

			TInt Smaller = (MnuRect.Height() - MyHeight);

			if(Smaller > 0)
			{
				MnuRect.iTl.iY = (MnuRect.iTl.iY + Smaller);
			}
			
			MnuRect.Grow(4,4);
	
			TInt CurrItem = iCurrentItem;
			if(CurrItem > 1)
			{
				switch(iMyAlarmItem->iRepeat)
				{
				case EAlarmRepeatDefintionRepeatDaily:
				case EAlarmRepeatDefintionRepeatWeekly:
				case EAlarmRepeatDefintionRepeatWorkday:
				case EAlarmRepeatDefintionRepeatNext24Hours:
					{
						// not showing day
						CurrItem = CurrItem + 1;
					}
					break;
				default:
					{
						// showing the day as well.
					}
					break;
				}
			}
			
			if(CurrItem == 0)	// repeat
			{
				if(iMyMenuControl)
				{
					iShowMenu = ETrue;
					SetMenuL();
					
					iMyMenuControl->ActivateDeActivate(iShowMenu);
				}
			}
			else if(CurrItem == 1)	// Time
			{
				iMyMultiControl = new(ELeave)CMyMultiControl(*this,iItemTxtColor);
				iMyMultiControl->ConstructL(MnuRect);
				StringLoader::Load(hjelp1,R_STR_SETTIME);
				iMyMultiControl->StartTimeEditorL(iMyAlarmItem->iTime, hjelp1);	
			}
			else if(CurrItem == 2)	// Date
			{
				iMyMultiControl = new(ELeave)CMyMultiControl(*this,iItemTxtColor);
				iMyMultiControl->ConstructL(MnuRect);
				StringLoader::Load(hjelp1,R_STR_SETDATE);
				iMyMultiControl->StartDateEditorL(iMyAlarmItem->iTime,hjelp1);
			}
			else if(CurrItem == 3)	// Type
			{
				if(iMyAlarmItem->iClockAlarm)
					iMyAlarmItem->iClockAlarm = EFalse;
				else
					iMyAlarmItem->iClockAlarm = ETrue;	
			}
			else if(CurrItem == 4)	// Message
			{
				iMyMultiControl = new(ELeave)CMyMultiControl(*this,iItemTxtColor);
				iMyMultiControl->ConstructL(MnuRect);
				StringLoader::Load(hjelp1,R_STR_SETMSG);
				iMyMultiControl->StartEditorL(iMyAlarmItem->iMessage,hjelp1);
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
-----------------------------------------------------------------------------
*/
TBool CAlarmContainer::IsShowingMenu(void)  const
{
	TBool Ret(EFalse);
	
	if(iMyMultiControl)
	{
		Ret = ETrue;
	}
	else if(iMyMenuControl && iShowMenu)
	{
		Ret = ETrue;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CAlarmContainer::SetAlarmL(TTime& aAlarmTime)
{
	TBool Ret(EFalse);
	
	if(iMyAlarmItem)
	{
		RASCliSession AlarmSession;
		User::LeaveIfError(AlarmSession.Connect());
		CleanupClosePushL(AlarmSession);
			
		TASShdAlarm AlarmData;

		if(iMyAlarmItem->iAlarmId >= 0)
		{
			AlarmSession.GetAlarmDetails(iMyAlarmItem->iAlarmId,AlarmData);
			//AlTime = AlarmData.NextDueTime();
		}
		
		// should validate iMyAlarmItem->iTime in here..
		

		if(iMyAlarmItem->iClockAlarm)
		{
			AlarmData.Category().iUid = 0x101F793A;// clock alarm
		}
		else 
		{
			AlarmData.Category().iUid = 0x00000000;
		}
		
		AlarmData.RepeatDefinition() = iMyAlarmItem->iRepeat; 
							
		AlarmData.OriginalExpiryTime() = iMyAlarmItem->iTime;
		AlarmData.NextDueTime() = iMyAlarmItem->iTime;
 	
 		if(iMyAlarmItem->iMessage)
 		{
			AlarmData.Message().Copy(iMyAlarmItem->iMessage->Des());				
 		}
 		
 		const TInt KWakeupAlarmFlagIndex = 0;
 		AlarmData.ClientFlags().Set( KWakeupAlarmFlagIndex );
 		 
		AlarmSession.AlarmAdd(AlarmData);		
		AlarmSession.SetAlarmStatus(AlarmData.Id(),EAlarmStatusEnabled);
		
		if(iMyAlarmItem->iAlarmId >= 0)
	    {
	    	AlarmSession.AlarmDelete(iMyAlarmItem->iAlarmId);
	    }
	    
	    aAlarmTime = AlarmData.NextDueTime();

		CleanupStack::PopAndDestroy();//AlarmSession.Close();
	
		Ret = ETrue;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAlarmContainer::ShowNoteL(const TDesC& aMessage)
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
void CAlarmContainer::SetMenuL(void)
{
	TBuf<60> hjelp1,hjelp2;
	
	if(iMyMultiControl)
	{
		StringLoader::Load(hjelp1,R_CMD_OK);
		StringLoader::Load(hjelp2,R_CMD_CANCEL);	
		iProvider.SetCbaL(R_MY_CONTROL_CBA,hjelp1,hjelp2);
	}
	else if(iMyMenuControl && iShowMenu)
	{
		StringLoader::Load(hjelp1,R_CMD_SELECT);
		StringLoader::Load(hjelp2,R_CMD_CANCEL);	
		iProvider.SetCbaL(R_MY_REPEAT_CBA,hjelp1,hjelp2);
	}
	else 
	{
		StringLoader::Load(hjelp1,R_CMD_OPTIONS);
		StringLoader::Load(hjelp2,R_CMD_DONE);
		iProvider.SetCbaL(R_MY_ALARM_CBA,hjelp1,hjelp2);
	}
}





