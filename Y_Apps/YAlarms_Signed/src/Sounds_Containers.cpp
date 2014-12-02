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
#include <centralrepository.h>
#include <mgfetch.h> 


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


#include "Sounds_Containers.h"                     // own definitions

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSoundsContainer::CSoundsContainer(MMyGrapProvider& aProvider)
:iProvider(aProvider),iCurrentItem(0),iFirstInList(0)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSoundsContainer::~CSoundsContainer()
{  
	if(iDrmPlayerUtility)
	{		
		iDrmPlayerUtility->Stop();
		iDrmPlayerUtility->Close();

		delete iDrmPlayerUtility;
		iDrmPlayerUtility = NULL;
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
{
    SetContainerWindowL(aContainer);
    
	SetRect(aRect);	
	
	TRAPD(Err,
	CheckFileL();
	CheckFile2L();
	);
	
	ActivateL();
	DrawNow();	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::CheckFileL(void)
{
	iClockFile.Zero();
	
 	CRepository* cr = CRepository::NewLC( KUidClockAlarmSound );
	cr->Get(0,iClockFile);
	cr->Get(2,iClockSnoz);
 	CleanupStack::PopAndDestroy( cr );
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::SetFileL(const TDesC& aFileName)
{	
 	CRepository* cr = CRepository::NewLC( KUidClockAlarmSound );
	cr->Set(0,aFileName);
 	CleanupStack::PopAndDestroy( cr );
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::SetSnoozeL(const TInt& aVal)
{	
 	CRepository* cr = CRepository::NewLC( KUidClockAlarmSound );
	cr->Set(2,aVal);
 	CleanupStack::PopAndDestroy( cr );
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::CheckFile2L(void)
{
	iCalenderFile.Zero();
	
 	CRepository* cr = CRepository::NewLC( KUidCalenAlarmSound );
	cr->Get(4,iCalenderFile);
	cr->Get(7,iCalenderSnoz);
 	CleanupStack::PopAndDestroy( cr );
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::SetFile2L(const TDesC& aFileName)
{
 	CRepository* cr = CRepository::NewLC( KUidCalenAlarmSound );
	cr->Set(4,aFileName);
 	CleanupStack::PopAndDestroy( cr );
	
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::SetSnooze2L(const TInt& aVal)
{	
 	CRepository* cr = CRepository::NewLC( KUidClockAlarmSound );
	cr->Set(7,aVal);
 	CleanupStack::PopAndDestroy( cr );
}	
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSoundsContainer::SetTextColot(const TRgb& aColor,const TRgb& aSelColor)
{
	iItemTxtColor = aColor;
	iItemTxtColorSel = aSelColor;
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSoundsContainer::SizeChanged()
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
	
	iItemCount = 4;
	iMaxItems = (Rect().Height() / FontH);
	
	if(iFirstInList > iCurrentItem)
	{
		iFirstInList = iCurrentItem;
	}
	else if(iFirstInList < (iCurrentItem - (iItemCount-1)))
	{
		iFirstInList = iCurrentItem - (iItemCount-1);
	}		
				
} 

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSoundsContainer::Draw(const TRect& /*aRect*/) const
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
			TBool Selected(EFalse);
			
			if(iCurrentItem == CurrItem)
			{
				Selected = ETrue;
			}
			
			DrawItem(gc,ItemRect, CurrItem,Selected);
			
			ItemRect.iTl.iY = ItemRect.iBr.iY;
			ItemRect.iBr.iY = (ItemRect.iTl.iY + HeighGap);
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
void CSoundsContainer::DrawItem(CWindowGc& aGc,const TRect& aRect,TInt aItem, TBool aSelected) const
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

	if(aItem == 0)	// Clock
	{
		StringLoader::Load(FirstTxt,R_STR_CLOCKALRM);
		
		if(iClockFile.Length())
		{
			TParsePtrC Hjelpp(iClockFile);
			
			if(Hjelpp.NameAndExt().Length() > 100)
				SecondTxt.Copy(Hjelpp.NameAndExt().Left(100));
			else
				SecondTxt.Copy(Hjelpp.NameAndExt());
		}
	
	}
	else if(aItem == 1)	// clock snooze
	{
		StringLoader::Load(FirstTxt,R_STR_SNOOZE);
		SecondTxt.Num(iClockSnoz);
	}
	else if(aItem == 2)	// Calender
	{
		StringLoader::Load(FirstTxt,R_STR_CALALARM);
		
		if(iCalenderFile.Length())
		{
			TParsePtrC Hjelpp(iCalenderFile);
			
			if(Hjelpp.NameAndExt().Length() > 100)
				SecondTxt.Copy(Hjelpp.NameAndExt().Left(100));
			else
				SecondTxt.Copy(Hjelpp.NameAndExt());
		}
	}
	else if(aItem == 3)	// Calendersnooze
	{
		StringLoader::Load(FirstTxt,R_STR_SNOOZE);
		SecondTxt.Num(iCalenderSnoz);
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
TKeyResponse CSoundsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aEventCode*/)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	


	switch (aKeyEvent.iCode)
    {
    case EKeyDevice3:
   		HandleViewCommandL(ESetSoundFile);
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
			else if(iFirstInList < (iCurrentItem - (iItemCount-1)))
			{
				iFirstInList = iCurrentItem - (iItemCount-1);
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
void CSoundsContainer::AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts)
{
	TBuf<60> hjelpper;
	
	StringLoader::Load(hjelpper,R_CMD_CHANGE);
	aCommands.AppendL(ESetSoundFile);
	aTexts.AppendL(hjelpper);
	
	TBool showPlay(EFalse);
	
	if(iCurrentItem == 0)// clock sound
	{
		if(iClockFile.Length())
		{
			showPlay = ETrue;
		}
	}
	else if(iCurrentItem == 2)// calendar sound
	{
		if(iCalenderFile.Length())
		{
			showPlay = ETrue;
		}
	}

	if(showPlay)
	{
		StringLoader::Load(hjelpper,R_CMD_PLAY);
		aCommands.AppendL(EPlaySoundFile);
		aTexts.AppendL(hjelpper);
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CSoundsContainer::HandleViewCommandL(TInt aCommand)
{
	TBuf<60> hjelpper;

	switch(aCommand)
	{
	case ESetSoundFile:
		{
			if(iCurrentItem == 1)// clock snooze
			{
				StringLoader::Load(hjelpper,R_STR_TIME);
			
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iClockSnoz,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_TIMME_DIALOG);
				Dialog->SetPromptL(hjelpper);
				if(Dialog->RunLD())
				{
					SetSnoozeL(iClockSnoz);
				}
			}
			else if(iCurrentItem == 3)// calendar snooze
			{
				StringLoader::Load(hjelpper,R_STR_TIME);
			
				CAknNumberQueryDialog* Dialog = CAknNumberQueryDialog::NewL(iCalenderSnoz,CAknQueryDialog::ENoTone);
				Dialog->PrepareLC(R_TIMME_DIALOG);
				Dialog->SetPromptL(hjelpper);
				if(Dialog->RunLD())
				{
					SetSnooze2L(iCalenderSnoz);
				}
			}
			else
			{
				CDesCArrayFlat* SelectedFiles = new(ELeave)CDesCArrayFlat(1);
				CleanupStack::PushL(SelectedFiles);
					
				if(MGFetch::RunL(*SelectedFiles,EAudioFile, EFalse)) 
				{
					if(SelectedFiles->Count())
					{	
						if(iCurrentItem == 0)// clock sound
						{
							SetFileL(SelectedFiles->MdcaPoint(0));
							CheckFileL();
						}
						else if(iCurrentItem == 2)// calendar sound
						{
							SetFile2L(SelectedFiles->MdcaPoint(0));
							CheckFile2L();
						}
					}
				}
			
				CleanupStack::PopAndDestroy(SelectedFiles);
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
void CSoundsContainer::MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
	if(iDrmPlayerUtility && aError == KErrNone)
	{
		TInt MaxVol= iDrmPlayerUtility->MaxVolume();
		
		MaxVol = ((MaxVol * 5) / 10);
		
		iDrmPlayerUtility->SetVolume(MaxVol);
		iDrmPlayerUtility->Play();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSoundsContainer::MdapcPlayComplete(TInt /*aError*/)
{
	delete iDrmPlayerUtility;
	iDrmPlayerUtility = NULL;
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSoundsContainer::PlaySoundL(void)
{
	if(iCurrentItem == 0)// clock
	{
		iDrmPlayerUtility = CDrmPlayerUtility::NewFilePlayerL(iClockFile,*this,EMdaPriorityNormal,EMdaPriorityPreferenceTimeAndQuality);	
	}
	else if(iCurrentItem == 1)// clock snooze
	{
		
	}
	else if(iCurrentItem == 2)// calendar
	{
		iDrmPlayerUtility = CDrmPlayerUtility::NewFilePlayerL(iCalenderFile,*this,EMdaPriorityNormal,EMdaPriorityPreferenceTimeAndQuality);	
	}
	//else if(iCurrentItem == 3)// calendar snooze
	
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSoundsContainer::StopSound(void)
{
	delete iDrmPlayerUtility;
	iDrmPlayerUtility = NULL;
	SetMenuL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CSoundsContainer::ShowNoteL(const TDesC& aMessage)
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
void CSoundsContainer::SetMenuL(void)
{
	TBuf<60> hjelp1,hjelp2;
	
	if(!iDrmPlayerUtility){
		StringLoader::Load(hjelp1,R_CMD_OPTIONS);
		StringLoader::Load(hjelp2,R_CMD_DONE);
		
		iProvider.SetCbaL(R_MY_SOUNDS_CBA,hjelp1,hjelp2);
	}else{

		StringLoader::Load(hjelp2,R_CMD_STOP);
		
		iProvider.SetCbaL(R_MY_SOUNDS2_CBA,KNullDesC,hjelp2);
	}
}








