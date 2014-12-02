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


#include "Main_Containers.h"                     // own definitions
#include "WorkDays_Containers.h"
#include "About_Containers.h"
#include "Update_Containers.h"

#include "Sounds_Containers.h"

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::CMainContainer(MMyGrapProvider& aProvider)
:iProvider(aProvider),iCurrentItem(0),iFirstInList(0)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CMainContainer::~CMainContainer()
{  
    if (iFrameReadTimer)
        iFrameReadTimer->Cancel();
    
    delete iFrameReadTimer;
    
    delete iWorkDContainer;
    iWorkDContainer = NULL;
    
    delete iAboutContainer;
    iAboutContainer = NULL;
	
	delete iUpdateContainer;
	iUpdateContainer = NULL;
	
	delete iAlarmContainer;
	iAlarmContainer = NULL;
	
	delete iSoundsContainer;
	iSoundsContainer = NULL;
	
	iAlarmsArray.ResetAndDestroy();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    
	GetAlarmsArrayL();

	SetRect(aRect);	
	
	ActivateL();
	DrawNow();	
	
	iFrameReadTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iFrameReadTimer->Start(KFrameReadInterval, KFrameReadInterval, TCallBack(DoChangeAddImageL, this));
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CMainContainer::SetTextColot(const TRgb& aColor,const TRgb& aSelColor)
{
	iItemTxtColor = aColor;
	iItemTxtColorSel = aSelColor;
	
	DrawNow();
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMainContainer::GetAlarmsArrayL(void)
{
	iAlarmsArray.ResetAndDestroy();
	
	RASCliSession AlarmSession;
	User::LeaveIfError(AlarmSession.Connect());
	CleanupClosePushL(AlarmSession);

	RArray<TAlarmId> AlarmIds;
	AlarmSession.GetAlarmIdListL(AlarmIds);
	
	for(TInt ii=0;ii < AlarmIds.Count();ii++)
	{	
		TASShdAlarm AlarmData;
	
		if(KErrNone == AlarmSession.GetAlarmDetails(AlarmIds[ii],AlarmData))
		{	
			if(AlarmData.Category().iUid != 0x101F793A
			&& AlarmData.Category().iUid != 0x00000000)
			{
				// ignore standard calender alarms..
			}
			else if(AlarmData.State() == EAlarmStateNotified)
			{
				AlarmSession.AlarmDelete(AlarmIds[ii]);
			}
			else
			{
				CMyAlarmItem* NewIttem = new(ELeave)CMyAlarmItem();
				CleanupStack::PushL(NewIttem);
				
				NewIttem->iTime = AlarmData.NextDueTime();
		
				if(AlarmData.Category().iUid == 0x101F793A)
				{
					NewIttem->iClockAlarm = ETrue;					
				}
				else
				{
					NewIttem->iClockAlarm = EFalse;
				}
				
				if(AlarmData.Message().Length())
				{
					NewIttem->iMessage = AlarmData.Message().AllocL();
				}
				
				NewIttem->iStatus = AlarmData.Status();
				NewIttem->iRepeat = AlarmData.RepeatDefinition();
				NewIttem->iAlarmId= AlarmIds[ii];
				
				CleanupStack::Pop(NewIttem);
				iAlarmsArray.Append(NewIttem);
			}
		}			
	}
	
	AlarmIds.Reset();
	CleanupStack::PopAndDestroy();//AlarmSession
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::SizeChanged()
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
	FontH = (FontH * 2);
	
	iMaxItems = (Rect().Height() / FontH);
	
	if(iFirstInList > iCurrentItem)
	{
		iFirstInList = iCurrentItem;
	}
	else if(iFirstInList < (iCurrentItem - (iMaxItems-1)))
	{
		iFirstInList = iCurrentItem - (iMaxItems-1);
	}		
				
	if(iWorkDContainer
	|| iAboutContainer
	|| iUpdateContainer
	|| iAlarmContainer
	||iSoundsContainer)
	{
		// do not update navi this time
	}
	else if(iProvider.NaviPanel())
	{	
		TBuf<100> AlarmTime;
	
		if(iCurrentItem >= 0 && iCurrentItem < iAlarmsArray.Count())
		{
			if(iAlarmsArray[iCurrentItem])
			{
				GetAlarmDueInL(iAlarmsArray[iCurrentItem]->iTime,AlarmTime);
			}
		}
	
		iProvider.NaviPanel()->SetText(AlarmTime);
	}
	
	if(iWorkDContainer)
	{
		iWorkDContainer->SetRect(Rect());
	}
	
	if(iAboutContainer)
	{
		iAboutContainer->SetRect(Rect());
	}
	
	if(iUpdateContainer)
	{
		iUpdateContainer->SetRect(Rect());
	}
	
	if(iAlarmContainer)
	{
		iAlarmContainer->SetRect(Rect());
	}

	if(iSoundsContainer)
	{
		iSoundsContainer->SetRect(Rect());
	}	
} 

/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
void CMainContainer::GetAlarmDueInL(const TTime& aTime,TDes& aBuffer)
{
	TBuf<60> hjelp1;
	TTime AlarmTime(aTime);

	TTime ShowTime;
	ShowTime.HomeTime();
	TTimeIntervalMinutes MinutesTo(0);
	TTimeIntervalHours 	 HoursTo(0);


	TTimeIntervalDays DaysTo = AlarmTime.DaysFrom(ShowTime);
	if(DaysTo.Int() > 0)
	{
		StringLoader::Load(hjelp1,R_STR_JUSTD);
		
		aBuffer.AppendNum(DaysTo.Int());
		aBuffer.Append(hjelp1);
		AlarmTime = AlarmTime - DaysTo;
	}

	AlarmTime.HoursFrom(ShowTime,HoursTo);

	if(HoursTo.Int() > 0)
	{
		StringLoader::Load(hjelp1,R_STR_JUSTH);
	
		aBuffer.AppendNum(HoursTo.Int()); 
		aBuffer.Append(hjelp1);
		
		AlarmTime = AlarmTime - HoursTo;
	}

	AlarmTime.MinutesFrom(ShowTime,MinutesTo);

	if(MinutesTo.Int() > 0)
	{
		StringLoader::Load(hjelp1,R_STR_JUSTMIN);
	
		aBuffer.AppendNum(MinutesTo.Int());
		aBuffer.Append(hjelp1);
	}
	else if(HoursTo.Int() <= 0 &&DaysTo.Int() <= 0)
	{
		//Alarm already expired
	}
}



/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/	
TInt CMainContainer::DoChangeAddImageL(TAny* aPtr)
{
	CMainContainer* self = static_cast<CMainContainer*>(aPtr);

	self->iProvider.UpOneOnAdds();
	self->DrawNow();
    
	return KErrNone;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMainContainer::ComponentControl( TInt /*aIndex*/) const
{
	if(iWorkDContainer)
	{
		return iWorkDContainer;
	}
	else if(iAboutContainer)
	{
		return iAboutContainer;
	}
	else if(iUpdateContainer)
	{
		return iUpdateContainer;
	}
	else if(iAlarmContainer)
	{
		return iAlarmContainer;
	}
	else if(iSoundsContainer)
	{
		return iSoundsContainer;
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
TInt CMainContainer::CountComponentControls() const
{	
	if(iWorkDContainer)
	{
		return 1;
	}
	else if(iAboutContainer)
	{
		return 1;
	}
	else if(iUpdateContainer)
	{
		return 1;
	}
	else if(iAlarmContainer)
	{
		return 1;
	}
	else if(iSoundsContainer)
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
void CMainContainer::Draw(const TRect& /*aRect*/) const
{
	if(iWorkDContainer
	|| iAboutContainer
	|| iUpdateContainer
	|| iAlarmContainer
	|| iSoundsContainer)
	{
		
	}
	else
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
				if(CurrItem >= 0 && CurrItem < iAlarmsArray.Count())
				{
					TBool Selected(EFalse);
					
					if(iCurrentItem == CurrItem)
					{
						Selected = ETrue;
					}
					
					DrawItem(gc,ItemRect, iAlarmsArray[CurrItem],Selected);
				}
				
				ItemRect.iTl.iY = ItemRect.iBr.iY;
				ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
			}
			
			if(iMaxItems > 1 && iMaxItems < iAlarmsArray.Count())
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
							
							TSize SRSizeImg(iScrollRect.Width(), ((iScrollRect.Height() * iMaxItems) / iAlarmsArray.Count()));
											
							TInt ScroollHeight = (iScrollRect.Height() - SRImgSiz.iHeight);
							
							TInt TmpCunt((iAlarmsArray.Count() - 1));
							
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
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::DrawItem(CWindowGc& aGc,const TRect& aRect, CMyAlarmItem* aItem, TBool aSelected) const
{
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	
	if(aItem)
	{
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
		
		TRect TimeR(SmallerRect.iTl.iX,SmallerRect.iTl.iY,(SmallerRect.iTl.iX + ((SmallerRect.Height() * 3) / 2)),SmallerRect.iBr.iY);
		TRect StatR((SmallerRect.iBr.iX - SmallerRect.Height()),SmallerRect.iTl.iY,SmallerRect.iBr.iX,SmallerRect.iBr.iY);
		TRect FirsR(TimeR.iBr.iX,SmallerRect.iTl.iY,StatR.iTl.iX,(SmallerRect.iTl.iY + FHeight));
		TRect SecnR(TimeR.iBr.iX,FirsR.iBr.iY,StatR.iTl.iX,SmallerRect.iBr.iY);
		
		DrawTime(TimeR,aGc,aItem->iTime);
		
		TBool IsRepeat(ETrue);
		
		TBuf<100> FirstTxt;
		switch(aItem->iRepeat)
		{
		case EAlarmRepeatDefintionRepeatDaily:
			{
				StringLoader::Load(FirstTxt,R_STR_DAILY);
			}
			break;
		case EAlarmRepeatDefintionRepeatWeekly:
			{
			//	TDayNameAbb ThisDay(aItem->iTime.DayNoInWeek());
				TDayName ThisDay(aItem->iTime.DayNoInWeek());
				FirstTxt.Copy(ThisDay);
			//	FirstTxt.Append(_L(" / Weekly"));
			}
			break;
		case EAlarmRepeatDefintionRepeatWorkday:
			{
				StringLoader::Load(FirstTxt,R_STR_WORKDAYS);
			}
			break;
		default:
			{
				IsRepeat = EFalse;
				
				TDayNameAbb ThisDay(aItem->iTime.DayNoInWeek());
				FirstTxt.Copy(ThisDay);
				FirstTxt.Append(_L(" "));
				FirstTxt.AppendNum(aItem->iTime.DateTime().Day() + 1);
				FirstTxt.Append(_L("/"));
				TMonthNameAbb ThisMonth(aItem->iTime.DateTime().Month());
				FirstTxt.Append(ThisMonth);
			}
			break;
		}	
		
		if(aSelected)
			aGc.SetPenColor(iItemTxtColor);
		else
			aGc.SetPenColor(iItemTxtColorSel);
		
		aGc.UseFont(FirstLine);
		aGc.DrawText(FirstTxt,FirsR,FirstLine->AscentInPixels(),CGraphicsContext::ELeft, 0);

		if(aItem->iMessage)
		{
			TInt HHGappp = ((SecnR.Height() - SecondLine->HeightInPixels()) / 2);
			
			TRect DrawSec(SecnR);
			if(HHGappp > 0)
			{
				DrawSec.iTl.iY = (DrawSec.iTl.iY + HHGappp);
				DrawSec.iTl.iX = (DrawSec.iTl.iX + (HHGappp * 2));
			}
			
			aGc.UseFont(SecondLine);
			aGc.DrawText(*aItem->iMessage,DrawSec,SecondLine->AscentInPixels(),CGraphicsContext::ELeft, 0);
		}
		
		if(aItem->iStatus == EAlarmStatusEnabled)
		{
			TInt AlarmIcon(11);
			
			if(aItem->iClockAlarm)
			{
				AlarmIcon = 12;
			}
			
			if(AlarmIcon >= 0 && AlarmIcon < iProvider.Numbers().Count())
			{
				TRect AlIccon(StatR.iTl.iX,StatR.iTl.iY,(StatR.iTl.iX + (StatR.Width()/2)),StatR.iBr.iY);
				DrawNumber(AlIccon,aGc,iProvider.Numbers()[AlarmIcon]);	
			}
		}
		
		if(IsRepeat)
		{
			TInt RepIcon(13);
			if(RepIcon >= 0 && RepIcon < iProvider.Numbers().Count())
			{
				TRect StIccon((StatR.iBr.iX - (StatR.Width()/2)),StatR.iTl.iY,StatR.iBr.iX,StatR.iBr.iY);
				DrawNumber(StIccon,aGc,iProvider.Numbers()[RepIcon]);	
			}
		}
	}
	
	aGc.SetPenStyle(CGraphicsContext::EDottedPen);
	aGc.SetPenColor(iItemTxtColor);
	aGc.DrawLine(TPoint(aRect.iTl.iX,aRect.iBr.iY),aRect.iBr);
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::DrawTime(const TRect& aRect,CWindowGc& aGc,TTime aTime) const
{
	if(iProvider.Numbers().Count())
	{
		if(iProvider.Numbers()[0])
		{
			if(iProvider.Numbers()[0]->Bitmap())
			{
				if(iProvider.Numbers()[0]->Bitmap()->Handle())
				{
					TSize NumSiz(iProvider.Numbers()[0]->Bitmap()->SizeInPixels());
					
					TInt HourTen = (aTime.DateTime().Hour() / 10);
					TInt Hours = (aTime.DateTime().Hour() % 10);
					TInt MinTen= (aTime.DateTime().Minute() / 10);
					TInt Min = (aTime.DateTime().Minute() % 10);
					
					TInt OneWidth = (NumSiz.iWidth / 2);
					
					TInt GappNum = ((aRect.Width() - (NumSiz.iWidth * 4) - (NumSiz.iWidth / 2)) / 2);
					
					TRect TimRect(aRect);
					
					if(GappNum > 0)
					{
						TimRect.iTl.iX = (TimRect.iTl.iX + GappNum);
					}
					
					TRect FirstTime(TimRect.iTl.iX,TimRect.iTl.iY,(TimRect.iTl.iX + (2 * OneWidth)),TimRect.iBr.iY);
					TRect SeconTime(FirstTime.iBr.iX,TimRect.iTl.iY,(FirstTime.iBr.iX + (2 * OneWidth)),TimRect.iBr.iY);
					TRect ColonMark(SeconTime.iBr.iX,TimRect.iTl.iY,(SeconTime.iBr.iX + (2 * OneWidth)),TimRect.iBr.iY);
					TRect ThirdTime(ColonMark.iBr.iX,TimRect.iTl.iY,(ColonMark.iBr.iX + (2 * OneWidth)),TimRect.iBr.iY);
					TRect FourtTime(ThirdTime.iBr.iX,TimRect.iTl.iY,(ThirdTime.iBr.iX + (2 * OneWidth)),TimRect.iBr.iY);
					
					if(iProvider.Numbers().Count() > HourTen)
					{
						DrawNumber(FirstTime,aGc,iProvider.Numbers()[HourTen]);
					}
					
					if(iProvider.Numbers().Count() > Hours)
					{
						DrawNumber(SeconTime,aGc,iProvider.Numbers()[Hours]);
					}
					
					if(iProvider.Numbers().Count() > 10)
					{
						DrawNumber(ColonMark,aGc,iProvider.Numbers()[10]);
					}
					
					if(iProvider.Numbers().Count() > MinTen)
					{
						DrawNumber(ThirdTime,aGc,iProvider.Numbers()[MinTen]);
					}
					
					if(iProvider.Numbers().Count() > Min)
					{
						DrawNumber(FourtTime,aGc,iProvider.Numbers()[Min]);
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
void CMainContainer::DrawNumber(const TRect& aRect,CWindowGc& aGc,CGulIcon* aIcon) const
{
	if(aIcon)
	{
		if(aIcon->Bitmap() && aIcon->Mask())
		{
			if(aIcon->Bitmap()->Handle() && aIcon->Mask()->Handle())
			{
				TSize ImgSiz(aIcon->Bitmap()->SizeInPixels());
			
				TRect DbgRect(aRect);
				
				DbgRect.iTl.iX = (DbgRect.iTl.iX + ((DbgRect.Width() - ImgSiz.iWidth) / 2));
				DbgRect.iBr.iX = (DbgRect.iTl.iX + ImgSiz.iWidth);
				
				DbgRect.iTl.iY = (DbgRect.iTl.iY + ((DbgRect.Height()- ImgSiz.iHeight)/ 2));
				DbgRect.iBr.iY = (DbgRect.iTl.iY + ImgSiz.iHeight);
				
				// have the match in here..
				aGc.DrawBitmapMasked(DbgRect,aIcon->Bitmap(),TRect(0,0,ImgSiz.iWidth,ImgSiz.iHeight),aIcon->Mask(),EFalse);			
			}
		}
	}	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMainContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iWorkDContainer)
	{
		Ret = iWorkDContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iSoundsContainer)
	{
		Ret = iSoundsContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iAlarmContainer)
	{
		Ret = iAlarmContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iUpdateContainer)
	{
		Ret = iUpdateContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else if(iAboutContainer)
	{
		Ret = iAboutContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	else
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	    	STATIC_CAST(CEikAppUi*,CEikonEnv::Static()->AppUi())->HandleCommandL(EMyOptionsCommand);
	   		break;
		case EKeyUpArrow:
		case EKeyDownArrow:
			{
				if(aKeyEvent.iCode == EKeyUpArrow)
					iCurrentItem--;
				else
					iCurrentItem++;
				
				if(iCurrentItem < 0)
					iCurrentItem = iAlarmsArray.Count() - 1;
				else if(iCurrentItem >= iAlarmsArray.Count())
					iCurrentItem = 0;
				
				if(iFirstInList > iCurrentItem)
				{
					iFirstInList = iCurrentItem;
				}
				else if(iFirstInList < (iCurrentItem - (iMaxItems-1)))
				{
					iFirstInList = iCurrentItem - (iMaxItems-1);
				}
				
				if(iProvider.NaviPanel())
				{
					TBuf<100> AlarmTime;
				
					if(iCurrentItem >= 0 && iCurrentItem < iAlarmsArray.Count())
					{
						if(iAlarmsArray[iCurrentItem])
						{
							GetAlarmDueInL(iAlarmsArray[iCurrentItem]->iTime,AlarmTime);
						}
					}
			
					iProvider.NaviPanel()->SetText(AlarmTime);
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
void CMainContainer::AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts)
{
	TBuf<100> hejelpp;
	
	if(iWorkDContainer)
	{
		iWorkDContainer->AppendMenuL(aCommands,aTexts);
	}
	else if(iSoundsContainer)
	{
		iSoundsContainer->AppendMenuL(aCommands,aTexts);
	}
	else if(iAlarmContainer)
	{
		iAlarmContainer->AppendMenuL(aCommands,aTexts);
	}
	else if(iUpdateContainer)
	{
		iUpdateContainer->AppendMenuL(aCommands,aTexts);
	}
	else if(iAboutContainer)
	{
		iAboutContainer->AppendMenuL(aCommands,aTexts);
	}
	else
	{
		StringLoader::Load(hejelpp,R_CMD_NEW);
		aCommands.AppendL(ENewAlarm);
		aTexts.AppendL(hejelpp);
		
		if(iCurrentItem >= 0 && iCurrentItem < iAlarmsArray.Count())
		{
			if(iAlarmsArray[iCurrentItem])
			{
				StringLoader::Load(hejelpp,R_CMD_RESET);
				aCommands.AppendL(EResetAlarm);
				aTexts.AppendL(hejelpp);
				
				StringLoader::Load(hejelpp,R_CMD_REMOVE);
				aCommands.AppendL(ERemoveAlarm);
				aTexts.AppendL(hejelpp);
				
				if(iAlarmsArray[iCurrentItem]->iStatus != EAlarmStatusEnabled)
				{
					StringLoader::Load(hejelpp,R_CMD_ENABLE);
					aCommands.AppendL(EActivateAlarm);
					aTexts.AppendL(hejelpp);
				}
				else
				{
					StringLoader::Load(hejelpp,R_CMD_DISABLE);
					aCommands.AppendL(EDeactivateAlarm);
					aTexts.AppendL(hejelpp);
				}
			}
		}
		
		StringLoader::Load(hejelpp,R_CMD_SETALSUND);
		aCommands.AppendL(ESetAlarmSounds);
		aTexts.AppendL(hejelpp);
		
		StringLoader::Load(hejelpp,R_CMD_SETWRKDAYS);
		aCommands.AppendL(ESetWorkDays);
		aTexts.AppendL(hejelpp);

		StringLoader::Load(hejelpp,R_CMD_HELP);		
		aCommands.AppendL(EUpdateList);
		aTexts.AppendL(hejelpp);
		
		StringLoader::Load(hejelpp,R_CMD_ABOUT);
		aCommands.AppendL(EShowInfo);
		aTexts.AppendL(hejelpp);

		StringLoader::Load(hejelpp,R_CMD_EXIT);
		aCommands.AppendL(EMyExitCmd);
		aTexts.AppendL(hejelpp);		
		
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CMainContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case ESetWorkDays:
		{
			if(iProvider.NaviPanel())
			{
				iProvider.NaviPanel()->SetText(_L(""));
			}
		
			delete iWorkDContainer;
			iWorkDContainer = NULL;
			iWorkDContainer = new(ELeave)CWorkDContainer(iProvider);
    		iWorkDContainer->ConstructL(*this,Rect());
    		iWorkDContainer->SetTextColot(iItemTxtColor,iItemTxtColorSel);
		}
		SetMenuL();
		DrawNow();
		break;
	case EWorkDSaveValues:
		if(iWorkDContainer)
		{
			iWorkDContainer->SetWorkDaysL();
		}
	case EWorkDaysCancel:
		delete iWorkDContainer;
		iWorkDContainer = NULL;
		SetMenuL();
		DrawNow();
		break;
	case EShowInfo:
		{	
			if(iProvider.NaviPanel())
			{
				iProvider.NaviPanel()->SetText(_L(""));
			}
			
			delete iAboutContainer;
			iAboutContainer = NULL;
			iAboutContainer = new(ELeave)CAboutContainer(iProvider);
    		iAboutContainer->ConstructL(*this,Rect());
    		iAboutContainer->SetTextColot(iItemTxtColor,iItemTxtColorSel);
		}
		SetMenuL();
		DrawNow();
		break;
	case EInfoBack:
		delete iAboutContainer;
		iAboutContainer = NULL;
		SetMenuL();
		DrawNow();
		break;
	case EUpdateList:
		{	
			if(iProvider.NaviPanel())
			{
				iProvider.NaviPanel()->SetText(_L(""));
			}
			
			delete iUpdateContainer;
			iUpdateContainer = NULL;
			iUpdateContainer = new(ELeave)CUpdateContainer(iProvider);
    		iUpdateContainer->ConstructL(*this,Rect());
    		iUpdateContainer->SetTextColot(iItemTxtColor,iItemTxtColorSel);
		}
		SetMenuL();
		DrawNow();
		break;
	case EUpdateBack:
		delete iUpdateContainer;
		iUpdateContainer = NULL;
		SetMenuL();
		DrawNow();
		break;
	case ENewAlarm:
	case EResetAlarm:
		{	
			if(iProvider.NaviPanel())
			{
				iProvider.NaviPanel()->SetText(_L(""));
			}
			
			delete iAlarmContainer;
			iAlarmContainer = NULL;
			iAlarmContainer = new(ELeave)CAlarmContainer(iProvider);
    		iAlarmContainer->ConstructL(*this,Rect());
    		iAlarmContainer->SetTextColot(iItemTxtColor,iItemTxtColorSel);
    		
    		CMyAlarmItem* ModMe(NULL);
    		
    		if(aCommand == EResetAlarm)
    		{
    			if(iCurrentItem >= 0 && iCurrentItem < iAlarmsArray.Count())
				{
					ModMe = iAlarmsArray[iCurrentItem];
				}
			}

			iAlarmContainer->SetAlarmDataL(ModMe);
		}
		SetMenuL();
		DrawNow();
		break;
	case EAlarmSave:
		if(iAlarmContainer)
		{
			TTime NowTim;
			NowTim.HomeTime();
			if(iAlarmContainer->SetAlarmL(NowTim))
			{
				TBuf<255> ShowMeNote;
				StringLoader::Load(ShowMeNote,R_CMD_ALDUEIN);
			
				GetAlarmDueInL(NowTim,ShowMeNote);
			
				ShowNoteL(ShowMeNote);
				
				GetAlarmsArrayL();
			}
		}
	case EAlarmBack:
		delete iAlarmContainer;
		iAlarmContainer = NULL;
		SetMenuL();
		DrawNow();
		break;
	case ESetAlarmSounds:
		{	
			if(iProvider.NaviPanel())
			{
				iProvider.NaviPanel()->SetText(_L(""));
			}
			
			delete iSoundsContainer;
			iSoundsContainer = NULL;
			iSoundsContainer = new(ELeave)CSoundsContainer(iProvider);
    		iSoundsContainer->ConstructL(*this,Rect());
    		iSoundsContainer->SetTextColot(iItemTxtColor,iItemTxtColorSel);
		}
		SetMenuL();
		DrawNow();
		break;
	case EStopSoundFile:
		if(iSoundsContainer)
		{
			iSoundsContainer->StopSound();
		}
	case EStopSoundFile2:
		break;
	case EPlaySoundFile:
		if(iSoundsContainer)
		{
			iSoundsContainer->PlaySoundL();
		}
		break;
	case ESetSoundBack:
		delete iSoundsContainer;
		iSoundsContainer = NULL;
		SetMenuL();
		DrawNow();
		break;
	case ERemoveAlarm:
	case EActivateAlarm:
	case EDeactivateAlarm:
		if(iCurrentItem >= 0 && iCurrentItem < iAlarmsArray.Count())
		{
			if(iAlarmsArray[iCurrentItem])
			{
				RASCliSession AlarmSession;
				User::LeaveIfError(AlarmSession.Connect());
				CleanupClosePushL(AlarmSession);
				
				if(aCommand == ERemoveAlarm)
				{
					User::LeaveIfError(AlarmSession.AlarmDelete(iAlarmsArray[iCurrentItem]->iAlarmId));
				}
				else
				{
					TAlarmStatus AlrStat(EAlarmStatusEnabled);
		   		
			   		if(aCommand == EDeactivateAlarm)
					{
						AlrStat = EAlarmStatusDisabled;
					}
				
					User::LeaveIfError(AlarmSession.SetAlarmStatus(iAlarmsArray[iCurrentItem]->iAlarmId,AlrStat));
				}
				
				CleanupStack::PopAndDestroy();//AlarmSession

				GetAlarmsArrayL();
			}
		}
		DrawNow();
		break;
	default:
		if(iWorkDContainer)
		{
			iWorkDContainer->HandleViewCommandL(aCommand);
		}
		else if(iAboutContainer)
		{
			iAboutContainer->HandleViewCommandL(aCommand);
		}
		else if(iUpdateContainer)
		{
			iUpdateContainer->HandleViewCommandL(aCommand);
		}
		else if(iAlarmContainer)
		{
			iAlarmContainer->HandleViewCommandL(aCommand);
		}
		else if(iSoundsContainer)
		{
			iSoundsContainer->HandleViewCommandL(aCommand);
		}
		break;
	}	
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMainContainer::ShowNoteL(const TDesC& aMessage)
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
void CMainContainer::SetMenuL(void)
{
	if(iWorkDContainer)
	{
		iWorkDContainer->SetMenuL();
	}
	else if(iSoundsContainer)
	{
		iSoundsContainer->SetMenuL();
	}
	else if(iAlarmContainer)
	{
		iAlarmContainer->SetMenuL();
	}
	else if(iUpdateContainer)
	{
		iUpdateContainer->SetMenuL();
	}
	else if(iAboutContainer)
	{
		iAboutContainer->SetMenuL();
	}
	else
	{
		TBuf<100> AlarmTime;
		
		if(iCurrentItem >= 0 && iCurrentItem < iAlarmsArray.Count())
		{
			if(iAlarmsArray[iCurrentItem])
			{
				GetAlarmDueInL(iAlarmsArray[iCurrentItem]->iTime,AlarmTime);
			}
		}
	
		TBuf<60> hjelp1,hjelp2;
		StringLoader::Load(hjelp1,R_CMD_OPTIONS);
		StringLoader::Load(hjelp2,R_CMD_EXIT);
		iProvider.SetCbaL(R_MY_OPTIONS_EXIT,hjelp1,hjelp2);
	
		if(iProvider.NaviPanel())
		{
			iProvider.NaviPanel()->SetText(AlarmTime);
		}
	}
}





