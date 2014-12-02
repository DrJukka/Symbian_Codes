/* 	
	Copyright (c) 2001 - 2006 , 
	J.P. Silvennoinen. 
	All rights reserved 
*/
#include "ImageView.h"                     // own definitions
#include "YToolsMBM_MIFUI.h"
#include "YToolsMBM_MIFUI.hrh"
#include "YToolsMBM_MIFUI_res.rsg"

#include <aknutils.h> 
#include <eikmenub.h> 
#include <bautils.h> 
#include <eikenv.h> 
#include <stringloader.h>

#include <eikappui.h>
#include <eikapp.h>


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CImageView* CImageView::NewL(MYBrowserFileHandlerUtils* aUtils, const TRect& aRect)
    {
    CImageView* self = CImageView::NewLC(aUtils,aRect);
    CleanupStack::Pop(self);
    return self;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CImageView* CImageView::NewLC(MYBrowserFileHandlerUtils* aUtils, const TRect& aRect)
    {
    CImageView* self = new (ELeave) CImageView(aUtils);
    CleanupStack::PushL(self);
	self->ConstructL(aRect);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CImageView::CImageView(MYBrowserFileHandlerUtils* aUtils)
:iUtils(aUtils),iBitmap(NULL),iOpener(NULL),iWaitDialog(NULL)
,iZoomSelect(NULL),iSlideTime(1000)
{
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CImageView::~CImageView()
{
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
			
	if(iWaitDialog)
	{
		iWaitDialog-> ProcessFinishedL();
		iWaitDialog = NULL;
	}

	if(iOpener)
	{
		iOpener->Cancel();
		iOpener->Close();
	}
	
	delete iMBM_Reader;
	iMBM_Reader = NULL;

	delete iOpener;
	delete iBitmap;
	delete iZoomSelect;
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CImageView::ConstructL(const TRect& aRect)
{
    CreateWindowL();

	iCurrPoint.iX = iCurrPoint.iY  = 0;
	
	iImageCount = 0;
	iZoomState = 100;

	SetRect(aRect);
	ActivateL();
	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::DialogDismissedL (TInt /*aButtonId*/)
{
	if(iOpener)
	{
		iOpener->Cancel();
		iOpener->Close();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFbsBitmap* CImageView::GetPicture(void)
{
	CFbsBitmap* ToBit = NULL;
	if(iBitmap)
	{
		if(iBitmap->Handle())
		{
			ToBit = new (ELeave) CFbsBitmap();
			if(ToBit)
			{
				CleanupStack::PushL(ToBit);
				
				TRect MyRect(Rect());
				TRect ImgRect;
				
				SetImageDrawRect(ImgRect,MyRect);
				
				if(!iFullScreen)
				{
					TSize BitmapSize = iBitmap->SizeInPixels();
				
					ImgRect.SetRect(0,0,BitmapSize.iWidth,BitmapSize.iHeight);
					MyRect.SetRect(0,0,ImgRect.Width(),ImgRect.Height());	
				}
			
				ToBit->Create(MyRect.Size(),EColor16M);
					
				CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(ToBit);
				CleanupStack::PushL(bitmapDevice);

				CFbsBitGc* graphicsContext = NULL;
				User::LeaveIfError(bitmapDevice->CreateContext(graphicsContext));
				CleanupStack::PushL(graphicsContext);

				graphicsContext->DrawBitmap(MyRect,iBitmap,ImgRect);

				CleanupStack::PopAndDestroy(2);//graphicsContext,bitmapDevice,
			
				CleanupStack::Pop();//ToBit
			}
		}
	}
	
	return ToBit;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::FlipImageL(TBool aVertical, TBool aHorizontal,TBool aRotate)
{
	if(iBitmap)
	{
		if(iBitmap->Handle())
		{
			TDisplayMode ImgDisplayMode = iBitmap->DisplayMode();

			if(ImgDisplayMode != ENone)
			{
				if(aRotate)
				{
					CFbsBitmap* TmpImg = FlipVertToHorL(iBitmap);
					if(TmpImg)
					{
						if(TmpImg->Handle())
						{
							delete iBitmap;
							iBitmap = NULL;
							iBitmap = TmpImg;
						}
						else
						{
							delete TmpImg;
							TmpImg = NULL;
						}
					}
				}
				
				TPoint ImgPoint1(0,0),ImgPoint2(0,0);	
				TInt ImgWi = iBitmap->SizeInPixels().iWidth;
				TInt ImgHi = iBitmap->SizeInPixels().iHeight;
			
			
				HBufC8* RevTmpBuf = HBufC8::NewLC(iBitmap->ScanLineLength(ImgWi,ImgDisplayMode));
				HBufC8* ImgBuf1 = HBufC8::NewLC(iBitmap->ScanLineLength(ImgWi,ImgDisplayMode));
				HBufC8* ImgBuf2 = HBufC8::NewLC(iBitmap->ScanLineLength(ImgWi,ImgDisplayMode));
				
				TPtr8 MyBufPoint1(ImgBuf1->Des());
				TPtr8 MyBufPoint2(ImgBuf2->Des());
				TPtr8 RevTmpPoint(RevTmpBuf->Des());
		
				TInt BitPerPixel(1);
				switch(ImgDisplayMode)
				{
				case EGray256:
				case EColor256:
					BitPerPixel = 1;
					break;
				case EColor4K:
				case EColor64K:
					BitPerPixel = 2;
					break;
				case EColor16M:
				case ERgb:
					BitPerPixel = 3;
					break;
				default:
					BitPerPixel = 1; // not really correct
					break;
				}
					
				for(TInt i=0; i < (ImgHi / 2); i++)
				{
					ImgPoint1.iY = i;
					ImgPoint2.iY = (ImgHi - i - 1);
					ImgBuf1->Des().Zero();
					ImgBuf2->Des().Zero();
					MyBufPoint1.Set(ImgBuf1->Des());
					MyBufPoint2.Set(ImgBuf2->Des());
					
					iBitmap->GetScanLine(MyBufPoint1,ImgPoint1,ImgWi,ImgDisplayMode);	
					iBitmap->GetScanLine(MyBufPoint2,ImgPoint2,ImgWi,ImgDisplayMode);	
					
					MyBufPoint1.Set(ImgBuf1->Des());
					MyBufPoint2.Set(ImgBuf2->Des());
					
					if(aVertical && RevTmpBuf)
					{
						RevTmpBuf->Des().Zero();
						RevTmpPoint.Set(RevTmpBuf->Des());
						RevercePixelBuffer(RevTmpPoint,MyBufPoint1,BitPerPixel,ImgWi);
						MyBufPoint1.Copy(RevTmpBuf->Des());
						
						RevTmpBuf->Des().Zero();
						RevTmpPoint.Set(RevTmpBuf->Des());
						RevercePixelBuffer(RevTmpPoint,MyBufPoint2,BitPerPixel,ImgWi);
						MyBufPoint2.Copy(RevTmpBuf->Des());
					}
								
					if(aHorizontal)
					{
						iBitmap->SetScanLine(MyBufPoint1,ImgPoint2.iY);
						iBitmap->SetScanLine(MyBufPoint2,ImgPoint1.iY);
					}
					else
					{
						iBitmap->SetScanLine(MyBufPoint1,ImgPoint1.iY);
						iBitmap->SetScanLine(MyBufPoint2,ImgPoint2.iY);
					}
				}

				CleanupStack::PopAndDestroy(3);//ImgBuf1,ImgBuf2,RevTmpBuf
			}
		}
	}
	
	CheckCurrentPoint();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::RevercePixelBuffer(TDes8& aTarget,const TDesC8& aBuffer,TInt aBitsPerPixel, TInt aLength)
{
	aTarget.Zero();
	TInt PixelBufLnght = (aBitsPerPixel * aLength);
	
	for(TInt i=PixelBufLnght; i >0; i = (i - aBitsPerPixel))
	{
		aTarget.Append(aBuffer.Mid((i - aBitsPerPixel),aBitsPerPixel));
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CFbsBitmap* CImageView::FlipVertToHorL(CFbsBitmap* aBitmap)
{
	CFbsBitmap* RetImg = NULL;
	
	if(aBitmap)
	{
		if(aBitmap->Handle())
		{
			TInt ImgWi = aBitmap->SizeInPixels().iWidth;
			TInt ImgHi = aBitmap->SizeInPixels().iHeight;
			TDisplayMode ImgDisplayMode = aBitmap->DisplayMode();
			
			RetImg = new (ELeave) CFbsBitmap();
			if(RetImg)
			{
				CleanupStack::PushL(RetImg);
				RetImg->Create(TSize(ImgHi,ImgWi),ImgDisplayMode);
			
				HBufC8* ImgBuf = HBufC8::NewLC(aBitmap->ScanLineLength(ImgHi,ImgDisplayMode));
				TPtr8 MyBufPoint(ImgBuf->Des());
				
				for(TInt i=0; i < ImgWi; i++)
				{
					MyBufPoint.Set(ImgBuf->Des());
					
					aBitmap->GetVerticalScanLine(MyBufPoint,i,ImgDisplayMode);
					
					MyBufPoint.Set(ImgBuf->Des());
					RetImg->SetScanLine(MyBufPoint,i);
				}
				
				CleanupStack::PopAndDestroy(ImgBuf);
				CleanupStack::Pop();//RetImg
			}
		}
	}
	
	return RetImg;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();	

	gc.SetPenColor(KRgbBlack);
	TRect MyRect(Rect());
	
	gc.Clear(MyRect);
	
	if(iBitmap)
	{
		if(iBitmap->Handle())
		{	
			TRect ImgRect;
			
			SetImageDrawRect(ImgRect,MyRect);	
			
			if(!iFullScreen)
			{	
				TSize BitmapSize = iBitmap->SizeInPixels();
				
				ImgRect.SetRect(0,0,BitmapSize.iWidth,BitmapSize.iHeight);
				
				TInt XPoint = ((MyRect.Width() - ImgRect.Width()) / 2);
				TInt YPoint = ((MyRect.Height() - ImgRect.Height()) / 2);
			
				MyRect.SetRect(XPoint,YPoint,(XPoint + ImgRect.Width()),(YPoint + ImgRect.Height()));	
			}
			
			gc.DrawBitmap(MyRect,iBitmap,ImgRect);
		}			
		else
		{
			MyRect.Shrink(4,4);
			gc.DrawRect(MyRect);
		}
    }
	else
	{
		MyRect.Shrink(4,4);
		gc.DrawRect(MyRect);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::CheckCurrentPoint(void)
{
	if(iBitmap)
	{
		if(iBitmap->Handle())
		{
			TInt Zoommm(iZoomState);
			
			if(iFullScreen)
			{
				if(Zoommm < 10)
					Zoommm = 10;
				else if(Zoommm > 100)
					Zoommm = 100;
			}
			else
			{
				Zoommm = 100;
			}
				
			TSize BitmapSize = iBitmap->SizeInPixels();
		
	 		TInt WWW = ((Rect().Width() * 100) / BitmapSize.iWidth);
			TInt HHH = ((Rect().Height()* 100) / BitmapSize.iHeight);
		
		 	if(WWW > HHH)
		 	{
		 		TInt Hjelp1 = ((Rect().Height() * BitmapSize.iWidth) / Rect().Width());
		 		Hjelp1 = ((Hjelp1 * Zoommm) / 100);
		 		
		 		TInt HWidth = ((BitmapSize.iWidth * Zoommm) / 100);
		 		
		 		if(iCurrPoint.iY + Hjelp1 >= BitmapSize.iHeight)
		 		{
		 			iCurrPoint.iY = (BitmapSize.iHeight - 1 - Hjelp1);
		 		}
		 		
		 		if(iCurrPoint.iX + HWidth >= BitmapSize.iWidth)
		 		{
		 			iCurrPoint.iX  = (BitmapSize.iWidth - 1 - HWidth);
		 		}
		 	}
		 	else //if(WWW > HHH)
		 	{
		 		TInt Hjelp2 = ((BitmapSize.iHeight * Rect().Width()) / Rect().Height());
		 		Hjelp2 = ((Hjelp2 * Zoommm) / 100);
		 		TInt HHeight = ((BitmapSize.iHeight * Zoommm) / 100);
		 		
		 		if(iCurrPoint.iX + Hjelp2 >= BitmapSize.iWidth)
		 		{
		 			iCurrPoint.iX = (BitmapSize.iWidth - 1 - Hjelp2);
		 		}

		 		if((iCurrPoint.iY + HHeight) >= BitmapSize.iHeight)
		 		{
		 			iCurrPoint.iY = (BitmapSize.iHeight - 1 - HHeight);
		 		}
		 	}
		}
	}
	
	if(iCurrPoint.iY < 0)
	{
		iCurrPoint.iY = 0;
	}
	
	if(iCurrPoint.iX < 0)
	{
		iCurrPoint.iX = 0;
	}	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CImageView::TimerExpired()
{
	ChangeFrameL(ETrue);
	iTimeOutTimer->After((iSlideTime * 1000));

	DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CImageView::HandleViewCommandL(TInt aCommand) 
{	
	switch(aCommand)
	{
	case EImgSlideShow:
		{
			iTimeOutTimer = CTimeOutTimer::NewL(0, *this);
			
			delete iZoomSelect;
			iZoomSelect = NULL;
			iZoomSelect = CZoomSelect::NewL(2000,100,iSlideTime,100,Rect());
			iZoomSelect->SetLabels(_L("0.1 s"),_L("2 sec"),_L("timeout"));
		}
		SetMenuL();
		DrawNow();
		break;
	case EImgSlideShowStop:
		{
			delete iTimeOutTimer;
			iTimeOutTimer = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EImgNormalSize:
		{
			iFullScreen = EFalse;		
		}
		DrawNow();
		break;
	case EImgFullScreen:
		{
			iZoomState = 100;
			iFullScreen = ETrue;
		}
		DrawNow();
		break;
	case EImgRotate90:
		if(Flip90or270Ok())
		{
			FlipImageL(ETrue,EFalse,ETrue);
		}
		else
		{
			//ShowNoteL(_L("Not enough memory for the operation"));	
		}	
		break;
	case EImgRotate180:
		FlipImageL(ETrue,ETrue,EFalse);
		break;
	case EImgRotate270:
		if(Flip90or270Ok())
		{
			FlipImageL(EFalse,ETrue,ETrue);
		}
		else
		{
			//ShowNoteL(_L("Not enough memory for the operation"));	
		}
		break;
	case EImgFlipHorizontal:
		FlipImageL(EFalse, ETrue,EFalse);
		break;
	case EImgFlipVertical:
		FlipImageL(ETrue,EFalse,EFalse);
		break;
	case EImgViewZoom:
		{
			delete iZoomSelect;
			iZoomSelect = NULL;
			iZoomSelect = CZoomSelect::NewL(100,10,iZoomState,5,Rect());
			iZoomSelect->SetLabels(_L("In"),_L("Out"),_L("Zoom"));
		}
		SetMenuL();
		DrawNow();
		break;
	case EImgViewZoomSelect:
		if(iZoomSelect)
		{
			if(iTimeOutTimer)
			{
				iSlideTime = iZoomSelect->GetValue();
				iTimeOutTimer->After((iSlideTime * 1000));
			}
			else
			{
				iZoomState = iZoomSelect->GetValue();
				CheckCurrentPoint();
			}
			
			delete iZoomSelect;
			iZoomSelect = NULL;
		}
		SetMenuL();
		DrawNow();
		break;
	case EImgViewZoomCancel:
		delete iZoomSelect;
		iZoomSelect = NULL;
		delete iTimeOutTimer;
		iTimeOutTimer = NULL;
		SetMenuL();
		DrawNow();
		break;	
	default:
		break;
	};
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TBool CImageView::Flip90or270Ok(void)
{
	TBool Ret = EFalse;
	if(iBitmap)
	{
		if(iBitmap->Handle())
		{
			TInt ImgWi = iBitmap->SizeInPixels().iWidth;
			TInt ImgHi = iBitmap->SizeInPixels().iHeight;
		
			TInt BitPerPixel(1);
			switch(iBitmap->DisplayMode())
			{
			case EColor4K:
			case EColor64K:
				BitPerPixel = 2;
				break;
			case EColor16M:
			case ERgb:
				BitPerPixel = 3;
				break;
			case EGray256:
			case EColor256:
			default:
				BitPerPixel = 1; // not really correct
				break;
			}
			
			TInt RequiredMeme = ImgWi * ImgHi * BitPerPixel;
			if(CheckHasEnoughMemory(RequiredMeme))
			{
				Ret = ETrue;
			}
		}
	}	
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CImageView::CountComponentControls() const
{
	if(iZoomSelect)
		return 1;
	else
		return 0;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CImageView::ComponentControl( TInt /*aIndex*/) const
{	
	if(iZoomSelect)
		return iZoomSelect;
	else
		return NULL;
}
/* 
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TKeyResponse CImageView::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	TBool HasBitMap = EFalse;

	if(iZoomSelect)
	{
		return iZoomSelect->OfferKeyEventL(aKeyEvent,aType);
	}
	else
	{
		if(iBitmap)
		{
			if(iBitmap->Handle())
			{
				HasBitMap = ETrue;
			}
		}

		switch (aKeyEvent.iCode)
		{
		case EKeyDevice3:
		   	CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		case EKeyRightArrow:
			if(!iFullScreen)
			{
				ChangeFrameL(ETrue);
			}
			else if(HasBitMap)
			{
				if(aKeyEvent.iRepeats)
					iCurrPoint.iX = iCurrPoint.iX + 100;
				else
					iCurrPoint.iX = iCurrPoint.iX + 10;

				CheckCurrentPoint();
			}
			DrawNow();
			break;	
		case EKeyLeftArrow:
			if(!iFullScreen)
			{
				ChangeFrameL(EFalse);
			}
			else if(HasBitMap)
			{
				if(aKeyEvent.iRepeats)
					iCurrPoint.iX = iCurrPoint.iX - 100;
				else
					iCurrPoint.iX = iCurrPoint.iX - 10;
			
				CheckCurrentPoint();
			}
			DrawNow();
			break;	
		case EKeyUpArrow:
			if(!iFullScreen)
			{
				
			}
			else if(HasBitMap)
			{
				if(aKeyEvent.iRepeats)
					iCurrPoint.iY = iCurrPoint.iY - 100;
				else
					iCurrPoint.iY = iCurrPoint.iY - 10;
			
				CheckCurrentPoint();
			}
			DrawNow();
			break;	
		case EKeyDownArrow:
			if(!iFullScreen)
			{
				
			}
			else if(HasBitMap)
			{
				if(aKeyEvent.iRepeats)
					iCurrPoint.iY = iCurrPoint.iY + 100;
				else
					iCurrPoint.iY = iCurrPoint.iY + 10;

				CheckCurrentPoint();
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
void CImageView::ChangeFrameL(TBool aNext)
{
	if(aNext)
	{
		iCurrentImage++;
		
		if(iCurrentImage > (iImageCount - 1))
		{
			iCurrentImage = 0;
		}			
	}
	else
	{			
		iCurrentImage--;
		
		if(iCurrentImage < 0)
		{
			iCurrentImage = (iImageCount - 1);
		}
	}
	
	if(iMBM_Reader)
	{
		delete iBitmap;
		iBitmap = NULL;
		iBitmap = iMBM_Reader->LoadImageL(iCurrentImage,Rect().Size());
	}
	

	TBuf<100> Navibuf(_L("img: "));

	Navibuf.AppendNum((iCurrentImage + 1));
	Navibuf.Append(_L(" / "));
	Navibuf.AppendNum(iImageCount);
	iUtils->GetFileUtils().SetNaviTextL(Navibuf);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::SetImageDrawRect(TRect& aImgRect,const TRect& aDrawRect) const
{
	aImgRect.SetRect(0,0,0,0);
	
	if(iBitmap)
	{
		if(iBitmap->Handle())
		{
			TInt Zoommm(iZoomState);
			
			if(iFullScreen)
			{
				if(Zoommm < 10)
					Zoommm = 10;
				else if(Zoommm > 100)
					Zoommm = 100;
			}
			else
			{
				Zoommm = 100;
			}
				
			TSize BitmapSize = iBitmap->SizeInPixels();
		
	 		TInt WWW = ((aDrawRect.Width() * 100) / BitmapSize.iWidth);
			TInt HHH = ((aDrawRect.Height()* 100) / BitmapSize.iHeight);
		
		 	if(WWW > HHH)
		 	{
		 		TInt Hjelp1 = ((aDrawRect.Height() * BitmapSize.iWidth) / aDrawRect.Width());
		 		
		 		Hjelp1 = ((Hjelp1 * Zoommm) / 100);
		 		TInt HWidth = ((BitmapSize.iWidth * Zoommm) / 100);
		 		
		 		aImgRect.iTl.iY = iCurrPoint.iY;
		 		aImgRect.iBr.iY = iCurrPoint.iY + Hjelp1;
		 		
		 		if(aImgRect.iBr.iY >= BitmapSize.iHeight)
		 		{
		 			aImgRect.iBr.iY = BitmapSize.iHeight - 1;
		 			aImgRect.iTl.iY = (aImgRect.iBr.iY - Hjelp1);
		 		}
		 		
		 		if(aImgRect.iTl.iY < 0)
		 		{
		 			aImgRect.iTl.iY = 0;
		 			aImgRect.iBr.iY = Hjelp1;	
		 		}
		 		
		 		aImgRect.iTl.iX = iCurrPoint.iX;
		 		aImgRect.iBr.iX = (iCurrPoint.iX + HWidth);
		 		
		 		if(aImgRect.iBr.iX >= BitmapSize.iWidth)
		 		{
		 			aImgRect.iBr.iX = (BitmapSize.iWidth - 1);
		 			aImgRect.iTl.iX = (aImgRect.iBr.iX - HWidth);
		 		}
		 		
		 		if(aImgRect.iTl.iX < 0)
		 		{
		 			aImgRect.iTl.iX = 0;
		 			aImgRect.iBr.iX = HWidth;
		 		}
		 		
		 	}
		 	else //if(WWW > HHH)
		 	{
		 		TInt Hjelp2 = ((BitmapSize.iHeight * aDrawRect.Width()) / aDrawRect.Height());
		 		Hjelp2 = ((Hjelp2 * Zoommm) / 100);
		 		TInt HHeight = ((BitmapSize.iHeight * Zoommm) / 100);
		 		
		 		aImgRect.iTl.iX = iCurrPoint.iX;
		 		aImgRect.iBr.iX = iCurrPoint.iX + Hjelp2;
		 		
		 		if(aImgRect.iBr.iX >= BitmapSize.iWidth)
		 		{
		 			aImgRect.iBr.iX = BitmapSize.iWidth - 1;
		 			aImgRect.iTl.iX = (aImgRect.iBr.iX - Hjelp2);
		 		}
		 		
		 		if(aImgRect.iTl.iX < 0)
		 		{
		 			aImgRect.iTl.iX = 0;
		 			aImgRect.iBr.iX = Hjelp2;	
		 		}
		 		
		 		aImgRect.iTl.iY = iCurrPoint.iY;
		 		aImgRect.iBr.iY = (iCurrPoint.iY + HHeight);
		 		
		 		if(aImgRect.iBr.iY >= BitmapSize.iHeight)
		 		{
		 			aImgRect.iBr.iY = (BitmapSize.iHeight - 1);
		 			aImgRect.iTl.iY = (aImgRect.iBr.iY - HHeight);
		 		}
		 		
		 		if(aImgRect.iTl.iY < 0)
		 		{
		 			aImgRect.iTl.iY = 0;
		 			aImgRect.iBr.iY = HHeight;
		 		}
		 	}
		}
	}	
}	
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CImageView::ReadDoneL(TInt aCount)	
{
	if(iWaitDialog)
	{
		iWaitDialog-> ProcessFinishedL();
		iWaitDialog = NULL;
	}
	
	iImageCount = aCount;
	iCurrentImage = -1;
	
	if(aCount > 0 && iMBM_Reader)
	{
		// will add one to the iCurrentImage
		ChangeFrameL(ETrue);
	}
	
	DrawNow();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CImageView::OpenMBMFileL(const TDesC& aFileName)
{	
	iZoomState = 100;
	iFullScreen = EFalse;

	if(iWaitDialog)
	{
		iWaitDialog-> ProcessFinishedL();
		iWaitDialog = NULL;
	}
	
	iWaitDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), ETrue);
	iWaitDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);
	iWaitDialog->SetCallback(this);
	iWaitDialog->SetTextL(_L("Opening Image"));
	
	iWaitDialog->RunLD();
	
	delete iBitmap;
	iBitmap = NULL;
	
	delete iMBM_Reader;
	iMBM_Reader = NULL;
	iMBM_Reader = CMBM_Reader::NewL(this,aFileName);

	DrawNow();
}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CImageView::ImageLoadedL(TInt aIndex,CFbsBitmap* aImage)
{
	if(iWaitDialog)
	{
		iWaitDialog-> ProcessFinishedL();
		iWaitDialog = NULL;
	}
	
	delete iBitmap;
	iBitmap = NULL;
	iBitmap = aImage;
	DrawNow();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CImageView::OpenFileL(const TDesC& aFileName)
{	
	iZoomState = 100;
	iFullScreen = EFalse;
	
	if(iWaitDialog)
	{
		iWaitDialog-> ProcessFinishedL();
		iWaitDialog = NULL;
	}

	if(iOpener)
	{
		iOpener->Cancel();
		iOpener->Close();
	}
	
	iWaitDialog = new (ELeave) CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), ETrue);
	iWaitDialog->PrepareLC(R_WAIT_NOTE_SOFTKEY_CANCEL);
	iWaitDialog->SetCallback(this);
	iWaitDialog->SetTextL(_L("Opening Image"));
	
	iWaitDialog->RunLD();

	delete iBitmap;
	iBitmap = NULL;
	
	delete iOpener;
	iOpener = NULL;
			
	iOpener = CMdaImageFileToBitmapUtility::NewL(*this);
	iOpener->OpenL(aFileName);

	DrawNow();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::MiuoOpenComplete(TInt aError)
{
	if (aError == KErrNone && iOpener)
	{	
		TFrameInfo frameInfo;
		iOpener->FrameInfo(0,frameInfo);	
	
		delete iBitmap;
		iBitmap = NULL;
		iBitmap = new(ELeave)CFbsBitmap();

		TDisplayMode MyDMode;

		if(frameInfo.iFlags & TFrameInfo::EColor )
		{
			switch(frameInfo.iBitsPerPixel)
			{
			case 1:
				MyDMode = EGray2;
				break;
			case 2:
				MyDMode = EGray4;
				break;
			case 4:
				MyDMode = EColor16;
				break;
			case 8:
				MyDMode = EColor256;
				break;
			case 12:
				MyDMode = EColor4K;
				break;
			case 16:
				MyDMode = EColor64K;
				break;
			default:
				MyDMode = EColor16M;
				break;
			}
		}
		else
		{
			switch(frameInfo.iBitsPerPixel)
			{		
			case 1:
				MyDMode = EGray2;
				break;
			case 2:
				MyDMode = EGray4;
				break;
			case 4:
				MyDMode = EGray16;
				break;
			default:
				MyDMode = EGray256;
				break;
			}
		}

		TSize BitmapSize(frameInfo.iOverallSizeInPixels);
	
		TInt ReuiredMem = (frameInfo.iBitsPerPixel / 8) * frameInfo.iOverallSizeInPixels.iHeight * frameInfo.iOverallSizeInPixels.iWidth;
		if(CheckHasEnoughMemory(ReuiredMem))
		{		
			iZoomState = 100;
			iCurrPoint.iX = iCurrPoint.iY  = 0;
			iBitmap->Create(BitmapSize,MyDMode);			
			iOpener->ConvertL(*iBitmap,0);
		}
		else
		{
			delete iBitmap;
			iBitmap = NULL;
			if(iWaitDialog)
			{
				iWaitDialog-> ProcessFinishedL();
				iWaitDialog = NULL;
			}
			
			//ShowNoteL(_L("Not enough memory for the operation"));		
		}
	}
	else
	{
		delete iBitmap;
		iBitmap = NULL;
		if(iWaitDialog)
		{
			iWaitDialog-> ProcessFinishedL();
			iWaitDialog = NULL;
		}
	}

	DrawNow();
}
	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::MiuoConvertComplete(TInt aError)
{
	if (aError == KErrNone) 
	{	
		CheckCurrentPoint();
		// We are Done
	}
	else // We have an error 
	{
		delete iBitmap;
		iBitmap = NULL;
	}
	
	if(iWaitDialog)
	{
		iWaitDialog-> ProcessFinishedL();
		iWaitDialog = NULL;
	}
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TBool CImageView::CheckHasEnoughMemory(TInt& aMemory)
{
	TBool Ret = EFalse;
	
	User::CompressAllHeaps();
	TMemoryInfoV1Buf   memInfoBuf;

	if(KErrNone == UserHal::MemoryInfo(memInfoBuf))
	{
		if(memInfoBuf().iFreeRamInBytes > aMemory)
		{
			Ret = ETrue;
		}
		
		aMemory = memInfoBuf().iFreeRamInBytes;
	}
		
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::SizeChanged()
{
	if(iZoomSelect)
	{
		iZoomSelect->SetNewRect(Rect());
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::MiuoCreateComplete(TInt aError)
{	
	if (aError == KErrNone)
    {
		// We shoudl never get to here
	}
	else
	{	
		delete iBitmap;
		iBitmap = NULL;
	}

	if(iWaitDialog)
	{
		iWaitDialog-> ProcessFinishedL();
		iWaitDialog = NULL;
	}

	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CImageView::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{
	if(R_IMGVIEW_MENU == aResourceId)
	{	
		TBool DimAll = ETrue;
		
		if(iBitmap)
		{
			if(iBitmap->Handle())
			{
				DimAll = EFalse;
			}
		}
	
		if(DimAll)
		{	
			aMenuPane->SetItemDimmed(EImgFlip,ETrue);	
			aMenuPane->SetItemDimmed(EImgRotate,ETrue);	
			aMenuPane->SetItemDimmed(EImgViewZoom,ETrue);		
		}	
		
		if(iFullScreen)
		{
			aMenuPane->SetItemDimmed(EImgFullScreen,ETrue);	
		}
		else
		{
			aMenuPane->SetItemDimmed(EImgNormalSize,ETrue);	
			aMenuPane->SetItemDimmed(EImgViewZoom,ETrue);
		}
	}	
}	

	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CImageView::SetMenuL(void)
{
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->StopDisplayingMenuBar();				
	
	if(iZoomSelect)
	{
		iUtils->GetCba().SetCommandSetL(R_ZOOM_CBA);
		iUtils->GetCba().DrawDeferred();
	}
	else if(iTimeOutTimer)
	{
		iUtils->GetCba().SetCommandSetL(R_SLIDE_CBA);
		iUtils->GetCba().DrawDeferred();
	}
	else
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetMenuTitleResourceId(R_IMGVIEW_MENUBAR);

		iUtils->GetCba().SetCommandSetL(R_MAIN_CBA);
		iUtils->GetCba().DrawDeferred();
	}
}

/*
-----------------------------------------------------------------------------
********************************  ********************************
-----------------------------------------------------------------------------
*/
CZoomSelect* CZoomSelect::NewL(TInt aMax,TInt aMin, TInt aCurrent,TInt aStep, const TRect aRect)
{
    CZoomSelect* self = CZoomSelect::NewLC(aMax,aMin,aCurrent,aStep,aRect);
    CleanupStack::Pop(self);
    return self;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CZoomSelect* CZoomSelect::NewLC(TInt aMax,TInt aMin, TInt aCurrent,TInt aStep, const TRect aRect)
    {
    CZoomSelect* self = new (ELeave)CZoomSelect(aMax,aMin,aCurrent,aStep);
    CleanupStack::PushL(self);
	self->ConstructL(aRect);
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CZoomSelect::CZoomSelect(TInt aMax,TInt aMin, TInt aCurrent,TInt aStep)
:iMax(aMax),iMin(aMin),iCurrent(aCurrent),iStep(aStep)
{
}
/*
-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
*/
CZoomSelect::~CZoomSelect()
{

}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CZoomSelect::ConstructL(const TRect aRect)
{
    CreateWindowL();
    
    TRect MyRect(aRect);
    
    TInt StartX = ((MyRect.Width()  - 134) / 2);
    TInt StartY = ((MyRect.Height() - 52)  / 2);
    
    MyRect.iTl.iX = StartX;
    MyRect.iBr.iX = MyRect.iTl.iX + 134;
    MyRect.iTl.iY = StartY;
    MyRect.iBr.iY = MyRect.iTl.iY + 52;

	SetRect(MyRect);

	ActivateL();
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CZoomSelect::SetLabels(const TDesC& aMin,const TDesC& aMax,const TDesC& aTitle)
{
	iMinLabel.Copy(aMin);
	iMaxLabel.Copy(aMax);
	iTitleLabel.Copy(aTitle);
	
	DrawNow();
}



/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CZoomSelect::SetNewRect(const TRect aRect)
{
    TRect MyRect(aRect);
    
    TInt StartX = ((MyRect.Width()  - 134) / 2);
    TInt StartY = ((MyRect.Height() - 52)  / 2);
    
    MyRect.iTl.iX = StartX;
    MyRect.iBr.iX = MyRect.iTl.iX + 134;
    MyRect.iTl.iY = StartY;
    MyRect.iBr.iY = MyRect.iTl.iY + 52;

	SetRect(MyRect);	
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CZoomSelect::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();	
	gc.SetBrushColor(KRgbWhite);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.SetPenColor(KRgbBlack);
	gc.DrawRect(Rect());
	
	gc.UseFont(LatinBold13());
	
	gc.DrawText(iTitleLabel,TRect(2,4,(Rect().iBr.iX - 2),19),LatinBold13()->AscentInPixels(), CGraphicsContext::ECenter, 0);
	gc.DrawLine(TPoint(1,20),TPoint(Rect().iBr.iX,20));
	
	gc.DrawText(iMinLabel,TRect(5,35,50,50),LatinBold13()->AscentInPixels(), CGraphicsContext::ELeft, 0);	
	 
	gc.DrawText(iMaxLabel,TRect((Rect().iBr.iX - 50),35,(Rect().iBr.iX - 2),50),LatinBold13()->AscentInPixels(), CGraphicsContext::ERight, 0);

	gc.DrawLine(TPoint(5,30),TPoint((Rect().iBr.iX - 5),30));
	
	TInt ZTotalVal = iMax - iMin;
	TInt ZCurrnVal = iCurrent - iMin;
	
	gc.SetBrushColor(KRgbRed);
	
	if(ZTotalVal > 0)
	{
		TInt LineVal = Rect().Width() - 10;
		
		if(ZCurrnVal < ZTotalVal)
		{
			LineVal = ((LineVal * ZCurrnVal) / ZTotalVal);
		}
			
		TRect MyRect;
		
		LineVal = LineVal + 5;
		
		MyRect.SetRect((LineVal - 3),27,(LineVal + 3),33);
				
		gc.DrawRect(MyRect);
	}
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CZoomSelect::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
{
	TKeyResponse Ret = EKeyWasNotConsumed;
	
	switch (aKeyEvent.iCode)
    {
    case EKeyDevice3:
		CEikonEnv::Static()->EikAppUi()->HandleCommandL(EImgViewZoomSelect);
		break;
	case EKeyRightArrow:
		{
			iCurrent = iCurrent + iStep;
		
			if(iCurrent > iMax)
				iCurrent = iMax;
		
			Ret = EKeyWasConsumed;
		}
		DrawNow();
		break;
	case EKeyLeftArrow:
		{
			iCurrent = iCurrent - iStep;
			
			if(iCurrent < iMin)
				iCurrent = iMin;
		
			Ret = EKeyWasConsumed;
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
TInt CZoomSelect::GetValue(void)
{
	TInt retVal(iCurrent);

	if(iCurrent < iMin)
		retVal = iMin;
	else if(iCurrent > iMax)
		retVal = iMax;
	else
		retVal = iCurrent;
		
	return retVal;
}
