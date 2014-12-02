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



#include "About_Containers.h"                     // own definitions


_LIT(KtxAppName			,"Y-Alarms");
_LIT(KtxAppVersion		,"ver: 1.10(2), Jan. 29th 2011");
_LIT(KtxRightReserved1	,"Copyright:");
_LIT(KtxRightReserved2	,"Dr. Jukka Silvennoinen, 2008-11");
_LIT(KtxRightReserved3	,"All rights reserved");
_LIT(KtxDrJukka			,"www.DrJukka.com");	


	
_LIT(KtxOpenWebAddress1	,"4  http://m.DrJukka.com");
_LIT(KtxOpenWebAddress2	,"4  http://m.DrJukka.com");
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAboutContainer::CAboutContainer(MMyGrapProvider& aProvider)
:iProvider(aProvider)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CAboutContainer::~CAboutContainer()
{  

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CAboutContainer::ConstructL(const CCoeControl& aContainer,const TRect& aRect)
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
void CAboutContainer::SetTextColot(const TRgb& aColor,const TRgb& /*aSelColor*/)
{
	iItemTxtColor = aColor;
	
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAboutContainer::SizeChanged()
{
	TInt FontH = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->HeightInPixels();
	FontH = (FontH * 2);
	
	iMaxItems = (Rect().Height() / FontH);				
} 

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAboutContainer::Draw(const TRect& /*aRect*/) const
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
		
		for(TInt i=0; i < 4; i++)
		{
			DrawItem(gc,ItemRect,i,EFalse);
			
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
void CAboutContainer::DrawItem(CWindowGc& aGc,const TRect& aRect,TInt aItem, TBool aSelected) const
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
	
	TBuf<100> FirstTxt;
	TBuf<100> SecondTxt;

	if(aItem == 0)	// repeat
	{
		FirstTxt.Copy(KtxAppName);
		SecondTxt.Copy(KtxAppVersion);
	}
	else if(aItem == 1)	// Time
	{
	//	FirstLine = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
		FirstTxt.Copy(KtxRightReserved1);
		SecondTxt.Copy(KtxRightReserved2);
	}
	else if(aItem == 2)	// Date
	{
		FirstLine = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);
		FirstTxt.Copy(KtxRightReserved3);
		SecondTxt.Copy(KtxDrJukka);
	}

	TRect FirsR(aRect.iTl.iX,aRect.iTl.iY,aRect.iBr.iX,(aRect.iTl.iY + FHeight));
	TRect SecnR(aRect.iTl.iX,FirsR.iBr.iY,aRect.iBr.iX,aRect.iBr.iY);
	
	
	aGc.SetPenColor(iItemTxtColor);
	
	aGc.UseFont(FirstLine);
	
	TRect DrawFirs(FirsR);
	TInt FFGappp = ((FirsR.Height() - FirstLine->HeightInPixels()) / 2);
	
	if(FFGappp > 0)
	{
		DrawFirs.iTl.iY = (DrawFirs.iTl.iY + FFGappp);
	}
	
	aGc.DrawText(FirstTxt,DrawFirs,FirstLine->AscentInPixels(),CGraphicsContext::ECenter, 0);
	
	TInt HHGappp = ((SecnR.Height() - SecondLine->HeightInPixels()) / 2);
	
	TRect DrawSec(SecnR);
	if(HHGappp > 0)
	{
		DrawSec.iTl.iY = (DrawSec.iTl.iY + HHGappp);
	}
	
	aGc.UseFont(SecondLine);
	aGc.DrawText(SecondTxt,DrawSec,SecondLine->AscentInPixels(),CGraphicsContext::ECenter, 0);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAboutContainer::AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts)
{
	TBuf<150> helpp;
	StringLoader::Load(helpp,R_STR_GOJUKKA);

	aCommands.AppendL(EAboutGoFirst);
	aTexts.AppendL(helpp);
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAboutContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EAboutGoFirst:
		OpenBrowserL(KtxOpenWebAddress1);
		break;
	case WAboutGoSecond:
		OpenBrowserL(KtxOpenWebAddress2);
		break;
	default:
		break;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
//const TInt KWmlBrowserUid = 0x10008D39;
const TInt KWmlBrowserUid = 0x10008D39;

void CAboutContainer::OpenBrowserL(const TDesC& aAddr )
{		
	TUid id( TUid::Uid( KWmlBrowserUid ) );
	TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
	TApaTask task = taskList.FindApp( id );
	if ( task.Exists() )
	{
		HBufC8* param8 = HBufC8::NewLC(aAddr.Length() );
		param8->Des().Append(aAddr);
		task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
		CleanupStack::PopAndDestroy();
	}
	else
	{
		RApaLsSession appArcSession;
		User::LeaveIfError(appArcSession.Connect()); // connect to AppArc server
		TThreadId id;
		appArcSession.StartDocument(aAddr, TUid::Uid( KWmlBrowserUid ),id);
		appArcSession.Close();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CAboutContainer::SetMenuL(void)
{
	TBuf<60> hjelp1,hjelp2;
	StringLoader::Load(hjelp2,R_CMD_BACK);

#ifdef SONE_VERSION
	iProvider.SetCbaL(R_MY_INFO2_CBA,hjelp1,hjelp2);
#else
    StringLoader::Load(hjelp1,R_CMD_OPTIONS);
	iProvider.SetCbaL(R_MY_INFO_CBA,hjelp1,hjelp2);
#endif
	
	
	
}





