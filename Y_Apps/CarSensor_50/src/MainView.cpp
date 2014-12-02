/*
 ============================================================================

 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include <AknUtils.h>
#include <e32math.h>
#include <aknglobalnote.h> 
#include "MainView.h"
#include "YApp_E887600D.h"
#include "YApp_E887600D.hrh"
#include "Help_Container.h"

#ifdef SONE_VERSION
	#include <YApp_20028857.rsg>
#else

	#ifdef LAL_VERSION
		#include <YApp_E887600D.rsg>
	#else
		#include <YApp_E887600D.rsg>
	#endif
#endif

const TInt Kacc1 = 0x10273024; 
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallAppView* CMovingBallAppView::NewL( const TRect& aRect,CEikButtonGroupContainer* aCba)
	{
	CMovingBallAppView* self = CMovingBallAppView::NewLC ( aRect,aCba);
	CleanupStack::Pop ( self);
	return self;
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallAppView* CMovingBallAppView::NewLC( const TRect& aRect,CEikButtonGroupContainer* aCba)
	{
	CMovingBallAppView* self = new ( ELeave ) CMovingBallAppView(aCba);
	CleanupStack::PushL ( self);
	self->ConstructL ( aRect);
	return self;
	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallAppView::ConstructL( const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL ();

	iBgContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,TRect(0,0,1,1), ETrue);
    
	// Set the windows size
	SetRect ( aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL ();

	CSensrvChannelFinder* SensrvChannelFinder = CSensrvChannelFinder::NewLC();
	RSensrvChannelInfoList ChannelInfoList;
	CleanupClosePushL( ChannelInfoList );
					
	TSensrvChannelInfo mySearchConditions; // none, so matches all.
	SensrvChannelFinder->FindChannelsL(ChannelInfoList,mySearchConditions);
					
	for ( TInt i = 0; i < ChannelInfoList.Count() ; i++ )
	{
		if(ChannelInfoList[i].iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData) //KSensrvChannelTypeIdRotationData )
		 {
			iSensrvSensorChannel = CSensrvChannel::NewL(ChannelInfoList[i]);
			iSensrvSensorChannel->OpenChannelL();
			iSensrvSensorChannel->StartDataListeningL( this, 1,1,0);
			iDbgBuf.Copy(_L("Ok"));
			break;
		}
	}
					
	ChannelInfoList.Close();
	CleanupStack::Pop( &ChannelInfoList );
	CleanupStack::PopAndDestroy( SensrvChannelFinder );
	
	if(!iSensrvSensorChannel)
	{
		ShowNoteL(_L("Rotation sensor not found"));
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMovingBallAppView::DataReceived( CSensrvChannel& aChannel, TInt aCount, TInt aDataLost )
{
	if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdAccelerometerXYZAxisData )
    {
		TInt aZAngle(iAngle2); 
		TInt aAngle(iAngle1);
	
		TSensrvAccelerometerAxisData data;
        
        TRAPD(erErr,
        for( TInt i = 0; i < aCount; i++ )
    	{
        	TPckgBuf<TSensrvAccelerometerAxisData> dataBuf;
           	aChannel.GetData(dataBuf);
           	data = dataBuf();
    	}
    	);
        
        if(erErr == KErrNone && HandleAccelerationL(data.iAxisX,data.iAxisY,data.iAxisZ,aZAngle,aAngle))
        {
			if(iLightOn)
			{
				User::ResetInactivityTime();
			}
					
			if(iResetNow)
			{
				iResetAngle1 = aAngle;
				iResetAngle2 = aZAngle;
				iResetNow = EFalse;
			}
					
			TBool doDraw(EFalse);
			aZAngle = aZAngle - iResetAngle2;
			aAngle = aAngle - iResetAngle1;
			
			if(aAngle < 0)
			{
				aAngle = (360 + aAngle);
			}
			
			if(aZAngle < 0)
			{
				aZAngle = (360 + aZAngle);
			}
					
			if(iCarImg->Handle() && iCarMsk->Handle()
			&& (iAngle2 != aZAngle))
			{
				iAngle2 = aZAngle;
				
				TSize CarSizz(iCarImg->SizeInPixels());
				AknIconUtils::SetSizeAndRotation(iCarImg,CarSizz,EAspectRatioPreserved,aZAngle);
				AknIconUtils::SetSizeAndRotation(iCarMsk,CarSizz,EAspectRatioPreserved,aZAngle);
				doDraw = ETrue;
			}
					
			if(iFrameImg->Handle() && iFrameMsk->Handle()
			&& iAngle1 != aAngle)
			{
				iAngle1 = aAngle;
						
				TSize ImgSizz(iFrameImg->SizeInPixels());
				AknIconUtils::SetSizeAndRotation(iFrameImg,ImgSizz,EAspectRatioPreserved,aAngle);
				AknIconUtils::SetSizeAndRotation(iFrameMsk,ImgSizz,EAspectRatioPreserved,aAngle);
				doDraw = ETrue;
			}
				
			if(doDraw)
			{
				DrawNow();
			}
        }
	}
}

/* 
---------------------------------------------------------
---------------------------------------------------------
*/

TBool CMovingBallAppView::HandleAccelerationL( TInt aX, TInt aY, TInt aZ, TInt& aZAngle, TInt& aAngle)
{    
	iXAvg = iXAvg + aX;
	iYAvg = iYAvg + aY;
	iZAvg = iZAvg + aZ;
	
	iXAvg2 = iXAvg2 + aX;
	iYAvg2 = iYAvg2 + aY;
	iZAvg2 = iZAvg2 + aZ;
	
	iXAvg3 = iXAvg3 + aX;
	iYAvg3 = iYAvg3 + aY;
	iZAvg3 = iZAvg3 + aZ;
	
	iXAvg4 = iXAvg4 + aX;
	iYAvg4 = iYAvg4 + aY;
	iZAvg4 = iZAvg4 + aZ;
	
	TBool DoNow(EFalse);
	
	TInt64 UseXAvg(0);
	TInt64 UseYAvg(0);
	TInt64 UseZAvg(0);
		
	iCount++;
	iCount2++;
	iCount3++;
	iCount4++;
	
	if(iCount >= 10)
	{
		DoNow = ETrue;
		UseXAvg = (iXAvg / iCount);
		UseYAvg = (iYAvg / iCount);
		UseZAvg = (iZAvg / iCount);
		iXAvg = iYAvg = iZAvg = 0;
		iCount = 0;
	}
	
	if(iCount2 >= 10)
	{
		DoNow = ETrue;
		UseXAvg = (iXAvg2 / iCount2);
		UseYAvg = (iYAvg2 / iCount2);
		UseZAvg = (iZAvg2 / iCount2);
		iXAvg2 = iYAvg2 = iZAvg2 = 0;
		iCount2 = 0;
	}
	
	if(iCount3 >= 10)
	{
		DoNow = ETrue;
		UseXAvg = (iXAvg3 / iCount3);
		UseYAvg = (iYAvg3 / iCount3);
		UseZAvg = (iZAvg3 / iCount3);
		iXAvg3 = iYAvg3 = iZAvg3 = 0;
		iCount3 = 0;
	}
	
	if(iCount4 >= 10)
	{
		DoNow = ETrue;
		UseXAvg = (iXAvg4 / iCount4);
		UseYAvg = (iYAvg4 / iCount4);
		UseZAvg = (iZAvg4 / iCount4);
		iXAvg4 = iYAvg4 = iZAvg4 = 0;
		iCount4 = 0;
	}
	
	if(DoNow)
	{
	
		if(UseZAvg > 60)
		{
			UseZAvg = 60;
		}
		
		if(UseZAvg < -60)
		{
			UseZAvg = -60;
		}
		
		if(UseXAvg > 60)
		{
			UseXAvg = 60;
		}
		
		if(UseXAvg < -60)
		{
			UseXAvg = -60;
		}
		
		if(UseYAvg > 60)
		{
			UseYAvg = 60;
		}
		
		if(UseYAvg < -60)
		{
			UseYAvg = -60;
		}
		iDbgBuf.Num(UseYAvg);
		iDbgBuf.Append(_L(","));
		iDbgBuf.AppendNum(UseXAvg);
		
		if(UseYAvg > 0) //270 - 90
		{
			if(UseXAvg > 0)  // 270 - 360
			{
				aAngle = 360 - ((90 * UseXAvg) / 60);
			}
			else // 0 - 90
			{
				aAngle = 90 - ((90 * UseYAvg) / 60);
			}
		}
		else // 0- 180
		{
			if(UseXAvg > 0) // 180 - 270
			{
				aAngle = 180 + ((90 * UseXAvg) / 60);	
			}
			else // 90 - 180
			{
				aAngle = 180 + ((90 * UseXAvg) / 60);	
			}
		}

		if(UseZAvg > 0)  //0 - 180
		{
			if(UseYAvg > 0) // 0 - 90
			{
				aZAngle = (( 90 * UseZAvg) / 60);
			}
			else // 90 - 180
			{
				aZAngle = 180 - ((90 * UseZAvg) / 60);
			}
		}
		else // 180 - 360
		{
			if(UseYAvg > 0) // 270 - 360
			{
				aZAngle = 360 + ((90 * UseZAvg) / 60); 
			}
			else//// 180 - 270
			{
				aZAngle = 180 + (( 90 * (-1 * UseZAvg)) / 60);
			}
		}
	}
	
	return DoNow;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMovingBallAppView::DataError( CSensrvChannel& /*aChannel*/, TSensrvErrorSeverity /*aError */)
	{
	
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMovingBallAppView::ShowNoteL(const TDesC&  aMessage)
{
	CAknGlobalNote* dialog = CAknGlobalNote::NewLC();
		
	TInt NoteId= dialog->ShowNoteL(EAknGlobalInformationNote,aMessage);
	
	User::After(1200000);
	dialog->CancelNoteL(NoteId);
	CleanupStack::PopAndDestroy(dialog);
}
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallAppView::CMovingBallAppView(CEikButtonGroupContainer* aCba):iCba(aCba)
	{

	}

/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
CMovingBallAppView::~CMovingBallAppView()
{
	if(iSensrvSensorChannel)
	{
		iSensrvSensorChannel->StopDataListening();
		iSensrvSensorChannel->CloseChannel();
	}

	delete iSensrvSensorChannel;
    
	delete iMyHelpContainer;
	iMyHelpContainer = NULL;
		
	delete iCarImg;
    delete iCarMsk;
    
    delete iFrameImg;
    delete iFrameMsk;
    
	delete iBgContext;
	iBgContext = NULL;
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CMovingBallAppView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	

	if(iMyHelpContainer)
	{
		Ret = iMyHelpContainer->OfferKeyEventL(aKeyEvent,aEventCode);
	}
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMovingBallAppView::HandleViewCommandL(TInt aCommand)
{
	switch(aCommand)
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
			iMyHelpContainer = CMyHelpContainer::NewL();
		}
		SetMenuL();
		DrawNow();    		
		break;	
	};
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CMovingBallAppView::ComponentControl( TInt /*aIndex*/) const
{
	CCoeControl* RetCont(NULL);

	if(iMyHelpContainer)
	{
		RetCont = iMyHelpContainer;		
	}
	
	return RetCont;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CMovingBallAppView::CountComponentControls() const
{
	TInt RetCount(0);
	
	if(iMyHelpContainer)
	{
		RetCount = 1;
	}

	return RetCount;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMovingBallAppView::SizeChanged()
{
	if ( iBgContext )
	{
		iBgContext->SetRect(Rect());

		if ( &Window() )
		{
			iBgContext->SetParentPos( PositionRelativeToScreen() );
		}
	}
	
	delete iCarImg;
    delete iCarMsk;
    delete iFrameImg;
    delete iFrameMsk;
    
	TFindFile ImgFile(CCoeEnv::Static()->FsSession());
	if(KErrNone == ImgFile.FindByDir(KtxImageFileName, KNullDesC))// finds the drive
	{			
		TSize NewSiz((Rect().Width() / 2),(Rect().Height() / 2));
	
		TInt CarIndex(0), CarId(1);

		AknIconUtils::ValidateLogicalAppIconId(ImgFile.File(), CarIndex, CarId);	
		AknIconUtils::CreateIconL(iCarImg,iCarMsk,ImgFile.File(),CarIndex,CarId);

		AknIconUtils::SetSize(iCarImg,NewSiz,EAspectRatioPreserved); 
		AknIconUtils::SetSize(iCarMsk,NewSiz,EAspectRatioPreserved); 

		TInt UseIndex(2), MaskId(3);

		AknIconUtils::ValidateLogicalAppIconId(ImgFile.File(), UseIndex, MaskId);	
		AknIconUtils::CreateIconL(iFrameImg,iFrameMsk,ImgFile.File(),UseIndex,MaskId);

		AknIconUtils::SetSize(iFrameImg,NewSiz,EAspectRatioPreserved); 
		AknIconUtils::SetSize(iFrameMsk,NewSiz,EAspectRatioPreserved); 
	}	
	
	if(iMyHelpContainer)
	{
		iMyHelpContainer->SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect()); // Sets rectangle of lstbox.
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CMovingBallAppView::SetMenuL(void)
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
	else
	{
		TInt MenuRes(R_MENUBAR);
		TInt ButtomRes(R_MYOPTEXIT_CBA);
	
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(MenuRes);
	
		if(iCba)
		{
			iCba->SetCommandSetL(ButtomRes);
			iCba->DrawDeferred();
		}
	}
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
TTypeUid::Ptr CMovingBallAppView::MopSupplyObject(TTypeUid aId)
{	
	if (iBgContext)
	{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext );
	}
	
	return CCoeControl::MopSupplyObject(aId);
}
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallAppView::Draw( const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc ();

	// Gets the control's extent
	TRect drawRect( Rect ());

  	// draw background skin first.
  	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	AknsDrawUtils::Background( skin, iBgContext, this, gc, drawRect);
    
    TInt YPosition(0);
    
	if(iCarImg && iCarMsk && iFrameImg && iFrameMsk)
	{
		if(iCarImg->Handle() && iCarMsk->Handle() && iFrameImg->Handle() && iFrameMsk->Handle())
		{
			TSize ImgSiz1(iCarImg->SizeInPixels());
			TSize ImgSiz2(iFrameImg->SizeInPixels());
			
			TInt GapX = ((drawRect.Width() - (ImgSiz1.iWidth + ImgSiz2.iWidth))/3);
			TInt GapY1 = ((drawRect.Height() - (ImgSiz1.iHeight))/2);
			TInt GapY2 = ((drawRect.Height() - (ImgSiz2.iHeight))/2);
			
			TRect Rgnht1(0,0,0,0);
			Rgnht1.iTl.iX = (drawRect.iTl.iX + GapX);
			Rgnht1.iTl.iY = (drawRect.iTl.iY + GapY1);
			
			Rgnht1.iBr.iX = (Rgnht1.iTl.iX + ImgSiz1.iWidth);
			Rgnht1.iBr.iY = (Rgnht1.iTl.iY + ImgSiz1.iHeight);
		
			YPosition = Rgnht1.iBr.iY;
		
			gc.DrawBitmapMasked(Rgnht1,iCarImg,TRect(0,0,ImgSiz1.iWidth,ImgSiz1.iHeight),iCarMsk,ETrue);
			
			TRect Rgnht2(0,0,0,0);
			Rgnht2.iTl.iX = (Rgnht1.iBr.iX + GapX);
			Rgnht2.iTl.iY = (drawRect.iTl.iY + GapY2);
			
			Rgnht2.iBr.iX = (Rgnht2.iTl.iX + ImgSiz2.iWidth);
			Rgnht2.iBr.iY = (Rgnht2.iTl.iY + ImgSiz2.iHeight);
			
			if(YPosition < Rgnht2.iBr.iY)
			{
				YPosition = Rgnht2.iBr.iY;
			}
				
			gc.DrawBitmapMasked(Rgnht2,iFrameImg,TRect(0,0,ImgSiz2.iWidth,ImgSiz2.iHeight),iFrameMsk,ETrue);
		}
	}	

	TInt Ang1(iAngle1),Ang2(iAngle2);
	
	if(iAngle1 > 180)
	{
		Ang1 = (iAngle1 - 360);
	}

	if(iAngle2 > 180)
	{
		Ang2 = (iAngle2 - 360);
	}
	
	TBuf<30> AngLeBuf;
	TRgb textcol;
	AknsUtils::GetCachedColor(skin,textcol,KAknsIIDQsnTextColors,EAknsCIQsnTextColorsCG6);
		
	const CFont* UseFontMe = AknLayoutUtils::FontFromId(EAknLogicalFontDigitalFont);
	gc.UseFont(UseFontMe);
	gc.SetPenColor(TLogicalRgb(textcol));

	TInt Gapp = ((drawRect.Height() - YPosition - ((UseFontMe->HeightInPixels()* 3)/ 2)) / 2);
	if(Gapp < 0)
	{
		Gapp = 0;
	}
	
	TRect MyRRR(0,0,(drawRect.Width() / 2),(drawRect.Height() - Gapp));
	
	MyRRR.iTl.iY = (MyRRR.iBr.iY - UseFontMe->HeightInPixels()  - 5);
	
	AngLeBuf.Num(Ang2);
//	gc.DrawText(iDbgBuf,MyRRR,(UseFontMe->AscentInPixels() + 4), CGraphicsContext::ECenter, 0);
	
	gc.DrawText(AngLeBuf,MyRRR,(UseFontMe->AscentInPixels() + 4), CGraphicsContext::ECenter, 0);
	
	MyRRR.iTl.iX = MyRRR.iBr.iX;
	MyRRR.iBr.iX = drawRect.Width();

	AngLeBuf.Num(Ang1);
//	gc.DrawText(iDbgBuf,MyRRR,(UseFontMe->AscentInPixels() + 4), CGraphicsContext::ECenter, 0);

	gc.DrawText(AngLeBuf,MyRRR,(UseFontMe->AscentInPixels() + 4), CGraphicsContext::ECenter, 0);
}
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallAppView::Reset(void)
{
	iResetNow = ETrue;
}
/*
------------------------------------------------------------------------------
------------------------------------------------------------------------------
*/
void CMovingBallAppView::SetLights(TBool aOn)
{
	iLightOn = aOn;
}

// End of File
