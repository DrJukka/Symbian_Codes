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
#include <hal.h>

#include "Main_Containers.h"  

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


#include "Items_Container.h"                     // own definitions


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CItemsContainer::CItemsContainer(CEikButtonGroupContainer* aCba)
:iCba(aCba)
{		

}

	/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CItemsContainer::~CItemsContainer()
{  	
	delete iMyMenuControl;
	iMyMenuControl = NULL;
	
	delete iMainContainer;
	iMainContainer = NULL;
	
	delete iMyStatusControl;
	iMyStatusControl = NULL;
	
	delete iMyIconControl;
	iMyIconControl = NULL;
	
	delete iNaviPanel;
	iNaviPanel = NULL;
	
	delete iMyButtonControl1;
	iMyButtonControl1 = NULL;
	
	delete iMyButtonControl3;
	iMyButtonControl3 = NULL;
	
	delete iMyBatteryControl;
	iMyBatteryControl = NULL;
	
	delete iMySignalControl;
	iMySignalControl = NULL;
	
	delete iStatGround;
	iStatGround = NULL;
	
	delete iStatGround2;
	iStatGround2 = NULL;
	
	delete iListSel;
	iListSel = NULL;
		
	iBackAdd.ResetAndDestroy();
	
	delete iBackMask;
	iBackMask = NULL;
	
	delete iBackGround;
	iBackGround = NULL;
	
	delete iScroller;
	iScroller = NULL;
	delete iScrollBack;
	iScrollBack = NULL;
	
	iNumbers.ResetAndDestroy();

	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Close();
	}
	
	iDbgFile.Close();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CItemsContainer::ConstructL()
{
	CreateWindowL();	
	
/*	_LIT(KRecFilename			,"C:\\YAlarms.txt");
	CCoeEnv::Static()->FsSession().Delete(KRecFilename);		
	User::LeaveIfError(iDbgFile.Create(CCoeEnv::Static()->FsSession(),KRecFilename,EFileWrite|EFileShareAny));
*/

	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("Started: \n"));
	}
	TRect rect(0,0,0,0);

	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, rect);

	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("Set rect: \n"));
	}
	
	SetRect(rect); 
	
	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("ActivateL: \n"));
	}	
	
	ActivateL();
	
	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("SetMenuL: \n"));
	}	
	
	SetMenuL();
	
		if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("DrawNow: \n"));
	}	
	
	DrawNow();
	
	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("Started: \n"));
	}
	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CItemsContainer::ForegroundChangedL(TBool aForeground)
{
	if(aForeground)
	{
		SizeChanged();	
		
		if(iDbgFile.SubSessionHandle())
		{
	  		iDbgFile.Write(_L8("Freground gained: \n"));
		}
	}
	else
	{		
		if(iDbgFile.SubSessionHandle())
		{
	  		iDbgFile.Write(_L8("Freground Lost: \n"));
		}
	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::ShowNoteL(const TDesC& aMessage)
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
void CItemsContainer::SizeChanged()
{	
	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("SizeChnaged: \n"));
	}
	
	TBool NormalSBLayout(ETrue);
	
	SetRectanglesL(NormalSBLayout);

	if(iDbgFile.SubSessionHandle())
	{
		TBuf8<200> DbgBuffer;
		
		DbgBuffer.Copy(_L8("iBackRect: "));
		DbgBuffer.AppendNum(iBackRect.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iBackRect.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iBackRect.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iBackRect.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iStatRect: "));
		DbgBuffer.AppendNum(iStatRect.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iStatRect.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iStatRect.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iStatRect.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iStatLower: "));
		DbgBuffer.AppendNum(iStatLower.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iStatLower.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iStatLower.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iStatLower.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iStatUpper: "));
		DbgBuffer.AppendNum(iStatUpper.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iStatUpper.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iStatUpper.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iStatUpper.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iBattRect: "));
		DbgBuffer.AppendNum(iBattRect.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iBattRect.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iBattRect.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iBattRect.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iSignRect: "));
		DbgBuffer.AppendNum(iSignRect.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iSignRect.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iSignRect.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iSignRect.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iButRect1: "));
		DbgBuffer.AppendNum(iButRect1.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iButRect1.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iButRect1.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iButRect1.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iButRect3: "));
		DbgBuffer.AppendNum(iButRect3.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iButRect3.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iButRect3.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iButRect3.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iNaviRect: "));
		DbgBuffer.AppendNum(iNaviRect.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iNaviRect.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iNaviRect.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iNaviRect.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	  	
	  	DbgBuffer.Copy(_L8("iIconRect: "));
		DbgBuffer.AppendNum(iIconRect.iTl.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iIconRect.iTl.iY);
		DbgBuffer.Append(_L8(":"));
		DbgBuffer.AppendNum(iIconRect.iBr.iX);
		DbgBuffer.Append(_L8("x"));
		DbgBuffer.AppendNum(iIconRect.iBr.iY);
		DbgBuffer.Append(_L8("\n"));
	  	iDbgFile.Write(DbgBuffer);
	}
	
	TInt FontH = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->HeightInPixels();
	FontH = (FontH * 2);
	
	iMaxItems = (iBackRect.Height() / FontH);
	
	TFileName ImgFileName;
	GetFileName(ImgFileName);
	
	if(ImgFileName.Length())
	{			
		iButColor1 = KRgbWhite;
		iButColor3A = KRgbWhite;
		iButColor3B = KRgbWhite;
		
	#ifdef __YUCCA_VERSION__
		iNaviColor = KRgbWhite;
	#else
		iNaviColor = KRgbBlack;
	#endif
	
		iItemTxtColor= KRgbBlack;
		iItemTxtColorSel= KRgbBlack;
		
		delete iBackGround;
		iBackGround = NULL;
		iBackGround = AknIconUtils::CreateIconL(ImgFileName, EMbmYapp_a000312dBack_normal);
		AknIconUtils::SetSize(iBackGround,iBackRect.Size(),EAspectRatioNotPreserved);	
		
		iBackAdd.ResetAndDestroy();
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add,EMbmYapp_a000312dBack_add_mask,iBackRect.Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add2,EMbmYapp_a000312dBack_add2_mask,iBackRect.Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add3,EMbmYapp_a000312dBack_add3_mask,iBackRect.Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add4,EMbmYapp_a000312dBack_add4_mask,iBackRect.Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add5,EMbmYapp_a000312dBack_add5_mask,iBackRect.Size()));
 		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add6,EMbmYapp_a000312dBack_add6_mask,iBackRect.Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add7,EMbmYapp_a000312dBack_add7_mask,iBackRect.Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add8,EMbmYapp_a000312dBack_add8_mask,iBackRect.Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add9,EMbmYapp_a000312dBack_add9_mask,iBackRect.Size()));
		iBackAdd.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dBack_add10,EMbmYapp_a000312dBack_add10_mask,iBackRect.Size()));
 
		delete iBackMask;
		iBackMask = NULL;
		iBackMask = LoadImageL(ImgFileName,EMbmYapp_a000312dBg_mask,EMbmYapp_a000312dBg_mask_mask,iBackRect.Size(),EAspectRatioNotPreserved);

		delete iStatGround;
		iStatGround = NULL;
		iStatGround = AknIconUtils::CreateIconL(ImgFileName, EMbmYapp_a000312dBack_status);
		AknIconUtils::SetSize(iStatGround,iStatRect.Size(),EAspectRatioNotPreserved);	
		
		delete iStatGround2;
		iStatGround2 = NULL;
		iStatGround2 = AknIconUtils::CreateIconL(ImgFileName, EMbmYapp_a000312dBack_status2);
		AknIconUtils::SetSize(iStatGround2,iStatLower.Size(),EAspectRatioNotPreserved);	
	
		TInt FontH = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont)->HeightInPixels();

		TSize NmberSiz(FontH,FontH);

		iNumbers.ResetAndDestroy();
		
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dZero,EMbmYapp_a000312dZero_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dOne,EMbmYapp_a000312dOne_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dTwo,EMbmYapp_a000312dTwo_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dThree,EMbmYapp_a000312dThree_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dFour,EMbmYapp_a000312dFour_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dFive,EMbmYapp_a000312dFive_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dSix,EMbmYapp_a000312dSix_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dSeven,EMbmYapp_a000312dSeven_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dEight,EMbmYapp_a000312dEight_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dNine,EMbmYapp_a000312dNine_mask,NmberSiz));
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dColon,EMbmYapp_a000312dColon_mask,TSize((NmberSiz.iWidth / 2),NmberSiz.iHeight)));
	
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dCalendar_alarm,EMbmYapp_a000312dCalendar_alarm_mask,NmberSiz));//11
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dClock_alarm,EMbmYapp_a000312dClock_alarm_mask,NmberSiz));//12
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dRepeat,EMbmYapp_a000312dRepeat_mask,NmberSiz));	//13
		
		iNumbers.Append(LoadImageL(ImgFileName,EMbmYapp_a000312dMarked,EMbmYapp_a000312dMarked_mask,NmberSiz));	//14
		
		if(iMaxItems > 0)
		{
			TSize SelSizzz(iBackRect.Width(),(iBackRect.Height() / iMaxItems));
		
			delete iListSel;
			iListSel = NULL;
			iListSel = LoadImageL(ImgFileName,EMbmYapp_a000312dListsel_mask,EMbmYapp_a000312dListsel_mask_mask,SelSizzz,EAspectRatioNotPreserved);
		}
		
		TSize ImgSiz(((iBackRect.Width() * 8) / 10),(iBackRect.Height() /2));
		
		delete iScroller;
		iScroller = NULL;
		iScroller = LoadImageL(ImgFileName,EMbmYapp_a000312dScroller,EMbmYapp_a000312dScroller_mask,ImgSiz);

		delete iScrollBack;
		iScrollBack = NULL;
		iScrollBack = LoadImageL(ImgFileName,EMbmYapp_a000312dScroll,EMbmYapp_a000312dScroll_mask,iBackRect.Size());
	}	
	
	TRect MnuRect(iBackRect);
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
	
	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iMySignalControl: \n"));
	}
	
	if(iMySignalControl)
	{
		iMySignalControl->SetRectangle(iSignRect,NormalSBLayout);
	}
	else
	{
		iMySignalControl = CMySignalControl::NewL(*this,iSignRect,NormalSBLayout,*this);
	}
	
	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iMyBatteryControl: \n"));
	}
		
	if(iMyBatteryControl)
	{
		iMyBatteryControl->SetRectangle(iBattRect,NormalSBLayout);
	}
	else
	{
		iMyBatteryControl = CMyBatteryControl::NewL(*this,iBattRect,NormalSBLayout,*this);
	}


	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iMyIconControl: \n"));
	}
	
		
	if(iMyIconControl)
	{
		iMyIconControl->SetRect(iIconRect);
	}
	else
	{
		iMyIconControl = CMyIconControl::NewL(*this,iIconRect,*this);
	}


	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iMyStatusControl: \n"));
	}
		
	if(iMyStatusControl)
	{
		iMyStatusControl->SetRect(iStatUpper);
	}
	else
	{
		iMyStatusControl = CMyStatusControl::NewL(*this,iStatUpper,*this);
	}

	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iNaviPanel: \n"));
	}
	
	const CFont* NaviFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);

	if(iNaviPanel)
	{
		iNaviPanel->SetRect(iNaviRect);
		iNaviPanel->SetAlligment(CGraphicsContext::ECenter);
		iNaviPanel->SetTextColot(iNaviColor,NaviFont);
		iNaviPanel->SetText(_L(""));
	}
	else
	{
		iNaviPanel = CMyButtonControl::NewL(*this,iNaviRect,*this,NaviFont);
		iNaviPanel->SetAlligment(CGraphicsContext::ECenter);
		iNaviPanel->SetTextColot(iNaviColor,NaviFont);
		iNaviPanel->SetText(_L(""));
	}	

	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iMyButtonControl1: \n"));
	}
		
	const CFont* ButtFont = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont);

	if(iMyButtonControl1)
	{
		iMyButtonControl1->SetRect(iButRect1);
		iMyButtonControl1->SetAlligment(iAllig1);
		iMyButtonControl1->SetTextColot(iButColor1,ButtFont);
	}
	else
	{
		iMyButtonControl1 = CMyButtonControl::NewL(*this,iButRect1,*this,ButtFont);
		iMyButtonControl1->SetAlligment(iAllig1);
		iMyButtonControl1->SetTextColot(iButColor1,ButtFont);
	}

	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iMyButtonControl3: \n"));
	}
	
	if(iMyButtonControl3)
	{
		iMyButtonControl3->SetAlligment(iAllig3);
		iMyButtonControl3->SetRect(iButRect3);
		iMyButtonControl3->SetTextColot(iUseButColor3,ButtFont);
	}
	else
	{
		iMyButtonControl3 = CMyButtonControl::NewL(*this,iButRect3,*this,ButtFont);
		iMyButtonControl3->SetAlligment(iAllig3);
		iMyButtonControl3->SetTextColot(iUseButColor3,ButtFont);
	}

	if(iDbgFile.SubSessionHandle())
	{
	  	iDbgFile.Write(_L8("iMainContainer: \n"));
	}
				
	if(iMainContainer)
	{
		iMainContainer->SetRect(iBackRect);
		iMainContainer->SetTextColot(iItemTxtColor,iItemTxtColorSel);
	}
	else
	{
		iMainContainer = new(ELeave)CMainContainer(*this);
    	iMainContainer->ConstructL(*this,iBackRect);
    	iMainContainer->SetTextColot(iItemTxtColor,iItemTxtColorSel);
	}
} 
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CItemsContainer::GetFileName(TDes& aFileName)
{
	TFindFile privFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == privFile.FindByDir(KtxIconFileName2, KNullDesC))
	{			
			aFileName.Copy(privFile.File());
	}	
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CGulIcon* CItemsContainer::LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode)
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
void CItemsContainer::HandleResourceChange(TInt aType)
{
    if ( aType==KEikDynamicLayoutVariantSwitch )
    {  	
    	TRect rect(0,0,0,0);

		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EApplicationWindow, rect);
		
		
		SetRect(rect); 
 	}
 	
	CCoeControl::HandleResourceChange(aType);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFbsBitmap* CItemsContainer::BagImageL(const CCoeControl* aControl,TRect& aBgRect)
{	
	CFbsBitmap* RetBg(NULL);
	
	if(aControl == iMySignalControl)
	{
		aBgRect = TRect(TPoint(0,0),iSignRect.Size()); 
		RetBg = iStatGround;
	}
	else if(aControl == iMyBatteryControl)
	{
		if(iStatLower.iTl.iX == iBattRect.iTl.iX)
		{
			aBgRect = TRect(TPoint(0,0),iBattRect.Size()); 
			RetBg = iStatGround2;
		}
		else
		{
			aBgRect.iBr = iStatRect.iBr;
			aBgRect.iTl.iY  = iStatRect.iTl.iY;
			aBgRect.iTl.iX  = (aBgRect.iBr.iX - iBattRect.Width());
			
			RetBg = iStatGround;
		}
	}
	else if(aControl == iMyButtonControl1)
	{
		if(iStatLower.iTl.iY == iButRect1.iTl.iY)
		{
			TInt StartX = (iButRect1.iTl.iX - iStatLower.iTl.iX);
			TInt EndddX = (iButRect1.iBr.iX - iStatLower.iTl.iX);
			
			aBgRect = TRect(TPoint(StartX,0),TPoint(EndddX,iStatLower.Height())); 
			RetBg = iStatGround2;
		}
		else
		{
			TInt StartX = (iButRect1.iTl.iX - iStatRect.iTl.iX);
			TInt EndddX = (iButRect1.iBr.iX - iStatRect.iTl.iX);
			
			aBgRect = TRect(TPoint(StartX,0),TPoint(EndddX,iStatRect.Height())); 

			RetBg = iStatGround;
		}
	}
	else if(aControl == iMyButtonControl3)
	{
		if(iStatLower.iTl.iY == iButRect3.iTl.iY)
		{
			TInt StartX = (iButRect3.iTl.iX + iStatLower.iTl.iX);
			TInt EndddX = (iButRect3.iBr.iX - iStatLower.iTl.iX);
			
			aBgRect = TRect(TPoint(StartX,0),TPoint(EndddX,iStatLower.Height())); 
			RetBg = iStatGround2;
		}
		else
		{
			TInt StartX = (iButRect3.iTl.iX - iStatRect.iTl.iX);
			TInt EndddX = (iButRect3.iBr.iX - iStatRect.iTl.iX);
			
			aBgRect = TRect(TPoint(StartX,0),TPoint(EndddX,iStatRect.Height())); 

			RetBg = iStatGround;
		}
	}
	else if(aControl == iMyStatusControl)
	{
	
		TInt StartX = (iStatUpper.iTl.iX - iStatRect.iTl.iX);
		TInt EndddX = (iStatUpper.iBr.iX - iStatRect.iTl.iX);
			
		aBgRect = TRect(TPoint(StartX,0),TPoint(EndddX,iStatUpper.Height())); 

		RetBg = iStatGround;
	}
	else if(aControl == iNaviPanel)
	{
		if(iStatLower.iTl.iY == iNaviRect.iTl.iY)
		{
			TInt StartX = (iNaviRect.iTl.iX - iStatLower.iTl.iX);
			TInt EndddX = (iNaviRect.iBr.iX - iStatLower.iTl.iX);
			
			aBgRect = TRect(TPoint(StartX,0),TPoint(EndddX,iStatLower.Height())); 

			RetBg = iStatGround2;
		}
		else
		{
			TInt StartX = (iNaviRect.iTl.iX - iStatRect.iTl.iX);
			TInt EndddX = (iNaviRect.iBr.iX - iStatRect.iTl.iX);
			
			aBgRect = TRect(TPoint(StartX,(iStatRect.Height() - iNaviRect.Height())),TPoint(EndddX,iStatRect.Height())); 

			RetBg = iStatGround;	
		}
	}
	else if(iMyIconControl)
	{
		TInt StartX = (iIconRect.iTl.iX - iStatRect.iTl.iX);
		TInt EndddX = (iIconRect.iBr.iX - iStatRect.iTl.iX);
			
		aBgRect = TRect(TPoint(StartX,0),TPoint(EndddX,iStatRect.Height())); 			
		RetBg = iStatGround;	
	}
	
	
	
	return RetBg;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::SetRectanglesL(TBool& aNormalLayout)
{	
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane, iStatRect);
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EBatteryPane,iBattRect);
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::ESignalPane, iSignRect);

	iStatLower = TRect(0,0,0,0);
	iStatUpper = iStatRect;
	
	iStatRect.iBr.iX = Rect().iBr.iX;
		
	if(iBattRect.iTl.iY == 0
	&& iBattRect.iTl.iX == 0)
	{
		iBattRect.iBr    = iStatRect.iBr;
		iBattRect.iTl.iY = iStatRect.iTl.iY;
		iBattRect.iTl.iX = (iBattRect.iBr.iX - (iStatRect.Height() / 3));	
	
		iStatUpper.iBr.iX = iBattRect.iTl.iX;
	}
	
	if(iSignRect.Width() == 0
	&& iSignRect.Height()== 0)
	{
		iSignRect.iTl    = iStatRect.iTl;
		iSignRect.iBr.iY = iStatRect.iBr.iY;
		iSignRect.iBr.iX = (iSignRect.iTl.iX + (iStatRect.Height() / 3));
	}
		
	iIconRect.iTl.iX = iSignRect.iBr.iX;
	iIconRect.iTl.iY = iSignRect.iTl.iY;
	iIconRect.iBr.iY = iSignRect.iBr.iY;
	iIconRect.iBr.iX = (iIconRect.iTl.iX + (iStatUpper.Width()/3));
	
	const CFont* NaviFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
		
	iNaviRect.iTl.iX = iIconRect.iBr.iX;
	iNaviRect.iBr.iX = iBattRect.iTl.iX;
	iNaviRect.iBr.iY = iBattRect.iBr.iY;
	iNaviRect.iTl.iY = (iNaviRect.iBr.iY - ((NaviFont->HeightInPixels() * 4) / 3));
	
	if(iCba)
	{
		if(iCba->ButtonGroup())
		{
			iUseButColor3 = iButColor3A;
		
			iButRect1 = TRect(iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(0))->Rect());		
			iButRect3 = TRect (iCba->ControlOrNull(iCba->ButtonGroup()->CommandId(2))->Rect());
			
			TInt BurHeight = iButRect1.Height();
			if(iButRect1.Height() > iButRect3.Height())
			{
				 BurHeight = iButRect3.Height();
			}
			
			iAllig3 = CGraphicsContext::ERight;
			iAllig1 = CGraphicsContext::ELeft;
			
			switch(AknLayoutUtils::CbaLocation())
			{
			case AknLayoutUtils::EAknCbaLocationBottom: //landscape and portrait
				{
					iAllig3 = CGraphicsContext::ERight;
					iAllig1 = CGraphicsContext::ELeft;
					
					iButRect1.iBr.iY = Rect().iBr.iY;
					iButRect3.iBr.iY = Rect().iBr.iY;
					
					iButRect3.iTl.iY = (iButRect3.iBr.iY - BurHeight); 
					iButRect1.iTl.iY = (iButRect1.iBr.iY - BurHeight); 
				
					if(iButRect3.iTl.iX == iButRect1.iTl.iX)
					{
						iButRect3.iTl.iX = iButRect1.iBr.iX;
						iButRect3.iBr.iX = Rect().iBr.iX;
					}
					
					iStatLower.iTl.iX = 0;
					iStatLower.iTl.iY = iButRect1.iTl.iY;
					iStatLower.iBr = Rect().iBr;
					
					iStatUpper.iBr.iY = iNaviRect.iTl.iY;
					
					if(iDbgFile.SubSessionHandle())
					{
					  	iDbgFile.Write(_L8("EAknCbaLocationBottom: \n"));
					}
				}
				break;
			case AknLayoutUtils::EAknCbaLocationRight:  //only landscape
				{
					iUseButColor3 = iButColor3B;
					aNormalLayout = EFalse;
					
					iStatLower.iTl = iBattRect.iTl;
					iStatLower.iBr = Rect().iBr;
					
					iAllig3 = CGraphicsContext::ERight;
					iAllig1 = CGraphicsContext::ERight;
					
					BurHeight = iStatRect.Height();
					
					iButRect1.iTl.iY = (iButRect1.iBr.iY - BurHeight); 
					
					iButRect3.iTl.iY = iStatRect.iTl.iY;
					iButRect3.iBr.iY = (iButRect3.iTl.iY + BurHeight);
					
					
					
									
					if(iButRect1.iTl.iY < iButRect3.iTl.iY)
					{
						iNaviRect.iTl.iX = iBattRect.iBr.iX;
						iNaviRect.iBr.iX = iButRect3.iTl.iX;
						iNaviRect.iBr.iY = iButRect3.iBr.iY;
						iNaviRect.iTl.iY = iButRect3.iTl.iY;
					}
					else
					{
						iNaviRect.iTl.iX = iBattRect.iBr.iX;
						iNaviRect.iBr.iX = iButRect1.iTl.iX;
						iNaviRect.iBr.iY = iButRect1.iBr.iY;
						iNaviRect.iTl.iY = iButRect1.iTl.iY;
					}
					
					TBool REttt= IsNormalOrNot(iCba->ButtonGroup()->CommandId(2),iCba->ButtonGroup()->CommandId(0));
				
					TInt Machhhine(0);
					HAL::Get(HAL::EMachineUid,Machhhine);
					if(Machhhine == 0x20002496 //E90
					|| !REttt)
					{
						iButRect1.iTl.iY = Rect().iTl.iY;
						iButRect1.iBr.iY = (iButRect1.iTl.iY + BurHeight);
						
						iButRect3.iBr.iY = Rect().iBr.iY;
						iButRect3.iTl.iY = (iButRect3.iBr.iY - BurHeight);
					}
					
					if(iDbgFile.SubSessionHandle())
					{
					  	iDbgFile.Write(_L8("EAknCbaLocationRight: \n"));
					}
				}
				break;
			case AknLayoutUtils::EAknCbaLocationLeft:
				{
					iUseButColor3 = iButColor3B;
					aNormalLayout = EFalse;
					
					iStatLower.iTl = iBattRect.iTl;
					iStatLower.iBr = Rect().iBr;
					
					iAllig3 = CGraphicsContext::ELeft;
					iAllig1 = CGraphicsContext::ELeft;
					
					BurHeight = iStatRect.Height();
					
					iButRect1.iTl.iY = (iButRect1.iBr.iY - BurHeight); 
					
					iButRect3.iTl.iY = iStatRect.iTl.iY;
					iButRect3.iBr.iY = (iButRect3.iTl.iY + BurHeight);
						
					if(iButRect1.iTl.iY < iButRect3.iTl.iY)
					{
						iNaviRect.iBr.iX = iBattRect.iTl.iX;
						iNaviRect.iTl.iX = iButRect3.iBr.iX;
						iNaviRect.iBr.iY = iButRect3.iBr.iY;
						iNaviRect.iTl.iY = iButRect3.iTl.iY;
					}
					else
					{
						iNaviRect.iBr.iX = iBattRect.iTl.iX;
						iNaviRect.iTl.iX = iButRect1.iBr.iX;
						iNaviRect.iBr.iY = iButRect1.iBr.iY;
						iNaviRect.iTl.iY = iButRect1.iTl.iY;
					}
					
					if(iDbgFile.SubSessionHandle())
					{
					  	iDbgFile.Write(_L8("EAknCbaLocationLeft: \n"));
					}
				}
				break;
			}
		}
	}
	
	iStatUpper.iTl.iX = iIconRect.iBr.iX;
							
	iBackRect = Rect();
	
	iBackRect.iBr.iY = iStatLower.iTl.iY;
	iBackRect.iTl.iY = iStatRect.iBr.iY;
}

/*
------------------------------------------------------------------------
------------------------------------------------------------------------
*/
TBool CItemsContainer::IsNormalOrNot(TInt aTop,TInt aBottom)
{
	TBool Rettt(ETrue);
	
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	if(cba)
    {
		// Assumes that CBA has Options and Exit commands
		CCoeControl* top = cba->ControlOrNull(aTop);
		CCoeControl* bottom = cba->ControlOrNull(aBottom);

		if( top && bottom )
	    {	    	
	    	if( top->Position().iY > bottom->Position().iY )
	        {
	        	Rettt =  EFalse;
	        }
	    }
    }
    
	return Rettt;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	if(iBackGround)
	{
		if(iBackGround->Handle())
		{
			gc.DrawBitmap(iBackRect,iBackGround);
		}
	}
		
	if(iStatGround && iStatRect.Width() && iStatRect.Height())
	{
		if(iStatGround->Handle())
		{
			gc.DrawBitmap(iStatRect,iStatGround);
		}
	}
	
	if(iStatGround2 && iStatLower.Width() && iStatLower.Height())
	{
		if(iStatGround2->Handle())
		{
			gc.DrawBitmap(iStatLower,iStatGround2);
		}
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CItemsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iMyMenuControl && iShowMenu)
	{
		switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
	    	HandleViewCommandL(EMyOptionsSelect);
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
	else if(iMainContainer)
	{
		Ret = iMainContainer->OfferKeyEventL(aKeyEvent,aEventCode);	
	}
	
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts)
{
	if(iMainContainer)
	{
		iMainContainer->AppendMenuL(aCommands,aTexts);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CItemsContainer::IsShowingMenu(void)
{
	TBool Ret(EFalse);
	
	if(iMyMenuControl && iShowMenu)
	{
		Ret = ETrue;
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
	{
	case EMyOptionsCommand:
		{
			if(iMyMenuControl == NULL)
			{
				TRect MnuRect(iBackRect);
				MnuRect.Shrink(14,14);
				
				TInt MyHeight = (((AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont)->HeightInPixels() * 3) / 2) * 4); 
		    
		    	TInt Smaller = (MnuRect.Height() - MyHeight);
		    	
		    	if(Smaller > 0)
		    	{
		    		MnuRect.iTl.iY = (MnuRect.iTl.iY + Smaller);
		    	}
				MnuRect.Grow(4,4);
				iMyMenuControl = CMyMenuControl::NewL(MnuRect,*this,iItemTxtColor,iItemTxtColorSel,KFrameReadInterval);
			}
			
			iShowMenu = ETrue;
			SetMenuL();
			if(iMyMenuControl)
			{
				iMyMenuControl->ActivateDeActivate(iShowMenu);
			}
		}
		DrawNow();
		break;
	case EMyOptionsSelect:
		if(iMyMenuControl)
		{
			TInt Comand = iMyMenuControl->SelectedCommand();
			iShowMenu = EFalse;
			SetMenuL();
			iMyMenuControl->ActivateDeActivate(iShowMenu);
			
			if(Comand == EMyExitCmd){
				STATIC_CAST(CAknAppUi*,CEikonEnv::Static()->AppUi())->HandleCommandL(EEikCmdExit);
				break;
			}else{
				HandleMenuCommandL(Comand);
			}
		}
	case EMyOptionsBack:
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
	default:
		HandleMenuCommandL(aCommand);
		break;
	}	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::HandleMenuCommandL(TInt aCommand)
{
	if(iMainContainer)
	{
		iMainContainer->HandleViewCommandL(aCommand);
	}
}
		
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CItemsContainer::ComponentControl( TInt aIndex) const
{
	if(aIndex == 0)
	{
		return iMyButtonControl1;
	}
	else if(aIndex == 1)
	{
		return iMyButtonControl3;
	}
	else if(aIndex == 2)
	{
		return iMyBatteryControl;
	}
	else if(aIndex == 3)
	{
		return iMySignalControl;
	}
	else if(aIndex == 4)
	{
		return iMyStatusControl;
	}
	else if(aIndex == 5)
	{
		return iMyIconControl;
	}
	else if(aIndex == 6)
	{
		return iMainContainer;
	}
	else if(aIndex == 7 && iNaviPanel)
	{
		return iNaviPanel;
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
TInt CItemsContainer::CountComponentControls() const
{	
	if(iMyStatusControl 
	&& iMyButtonControl1 
	&& iMyButtonControl3 
	&& iMyBatteryControl 
	&& iMySignalControl
	&& iMyIconControl
	&& iMainContainer)
	{
		TInt RetVal(0);
		if(iNaviPanel)
			RetVal = 8;
		else
			RetVal = 7;
		
		if(iMyMenuControl && iShowMenu)
		{
			RetVal = RetVal + 1;
		}
		
		return RetVal;
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
void CItemsContainer::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();

	if(iMyMenuControl && iShowMenu)
	{
		TBuf<60> hjelp1,hjelp2;
		StringLoader::Load(hjelp1,R_CMD_SELECT);
		StringLoader::Load(hjelp2,R_CMD_CANCEL);
	
		SetCbaL(R_MY_OPTIONS_CMD,hjelp1,hjelp2);
	}
	else if(iMainContainer)
	{
		iMainContainer->SetMenuL();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::SetCbaL(TInt aResource,const TDesC& aLeft,const TDesC&  aRight)
{
	if(iCba)
	{
		iCba->SetCommandSetL(aResource);
		iCba->DrawDeferred();
	}
	
	if(iMyButtonControl1)
	{
		iMyButtonControl1->SetText(aLeft);
	}
	
	if(iMyButtonControl3)
	{
		iMyButtonControl3->SetText(aRight);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CItemsContainer::UpOneOnAdds(void)
{
	iCurrentAdd++;
	
	if(iCurrentAdd >= iBackAdd.Count())
	{
		iCurrentAdd = 0;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CItemsContainer::CurrnetAddIndex(void)
{
	return iCurrentAdd;
}



