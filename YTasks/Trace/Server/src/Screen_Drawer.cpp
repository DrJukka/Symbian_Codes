
#include "Screen_Drawer.h"
#include <BAUTILS.H>


/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CScreenDrawer* CScreenDrawer::NewL(const TSettingsItem& aSettings)
	{
	CScreenDrawer* self = CScreenDrawer::NewLC(aSettings);
	CleanupStack::Pop(self);
	return self;
	}

/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
CScreenDrawer* CScreenDrawer::NewLC(const TSettingsItem& aSettings)
	{
	CScreenDrawer* self = new (ELeave) CScreenDrawer(aSettings);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CScreenDrawer::CScreenDrawer(const TSettingsItem& aSettings)
:CActive(EPriorityStandard),iSettings(aSettings)
	{
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CScreenDrawer::~CScreenDrawer()
	{
	Cancel();

	if(iMyFont)
		{
		iScreen->ReleaseFont(iMyFont);
		iMyFont = NULL;
		}
		
	delete iGc;
	delete iScreen;

	iMyWindow.Close();	
	iWg.Close();
	
	iWsSession.Close();
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScreenDrawer::ConstructL(void)
	{	
	User::LeaveIfError(iWsSession.Connect());

	iScreen=new (ELeave) CWsScreenDevice(iWsSession);
	User::LeaveIfError(iScreen->Construct());
	User::LeaveIfError(iScreen->CreateContext(iGc));

	iMyFont = NULL;
	_LIT(KFontArial,"Latin");
		
	TFontSpec myeFontSpec (KFontArial, iSettings.iFontSize);
			
	myeFontSpec.iTypeface.SetIsProportional(ETrue); 
	User::LeaveIfError(iScreen->GetNearestFontInTwips(iMyFont,myeFontSpec));

	iWg=RWindowGroup(iWsSession);
	User::LeaveIfError(iWg.Construct((TUint32)&iWg, EFalse));
	iWg.SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront + 100);
	iWg.EnableReceiptOfFocus(EFalse);

	CApaWindowGroupName* wn=CApaWindowGroupName::NewLC(iWsSession);
	wn->SetHidden(ETrue);
	wn->SetWindowGroupName(iWg);
	CleanupStack::PopAndDestroy();

	iMyWindow=RWindow(iWsSession);
	User::LeaveIfError(iMyWindow.Construct(iWg, (TUint32)&iMyWindow));
	
	TSize ScreenSize(100,100);
	
	_LIT(KMaxText,"000%");
	
	if(iSettings.iDrawStyle == 1)
	{
		ScreenSize.iWidth = (iMyFont->TextWidthInPixels(KMaxText) + 4);
		ScreenSize.iHeight = ((iMyFont->HeightInPixels() * 2) + 4);
	}
	else if(iSettings.iDrawStyle == 2)	 
	{
		_LIT(KMaxText2,"0000000 Kb");
		
		ScreenSize.iWidth = (iMyFont->TextWidthInPixels(KMaxText2) + 4);
		ScreenSize.iHeight = ((iMyFont->HeightInPixels() * 2) + 4);
	}
	else //if(iSettings.iDrawStyle == 0)
	{	
		ScreenSize.iHeight= (iMyFont->TextWidthInPixels(KMaxText) + 4); 
		ScreenSize.iWidth = ((iMyFont->TextWidthInPixels(KMaxText) * 2) + 4);
	}
	
	TPixelsTwipsAndRotation sizeAndRotation;
	iScreen->GetDefaultScreenSizeAndRotation(sizeAndRotation);
	
	TPoint ScreenPoint(0,0);
	ScreenPoint.iX = (((sizeAndRotation.iPixelSize.iWidth - ScreenSize.iWidth) * iSettings.iHorSlider) / 100);
	ScreenPoint.iY = (((sizeAndRotation.iPixelSize.iHeight -ScreenSize.iHeight)* iSettings.iVerSlider) / 100);
	
	iScreenRect = TRect(ScreenPoint,ScreenSize);
	
	iMyWindow.Activate();
	iMyWindow.SetExtent(iScreenRect.iTl,iScreenRect.Size());
	iMyWindow.SetBackgroundColor(KRgbWhite);
	iMyWindow.SetOrdinalPosition(0,ECoeWinPriorityAlwaysAtFront + 100);
	iMyWindow.SetNonFading(ETrue);	
	iMyWindow.SetVisible(ETrue);		
		
	Draw();

	TInt wgid=iWsSession.GetFocusWindowGroup();
	CApaWindowGroupName* gn;
	gn=CApaWindowGroupName::NewLC(iWsSession, wgid);
	
	gn->SetWindowGroupNameL(KTxMyWGName);

	CleanupStack::PopAndDestroy();

	CActiveScheduler::Add(this);
		
	Listen();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScreenDrawer::Listen()
	{
	iStatus=KRequestPending;
	iWsSession.RedrawReady(&iStatus);
	SetActive();
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScreenDrawer::SetValues(const TInt& aMemory,const TInt& aTotalRamInBytes,const TInt& aCpuLoad,const TInt& aGetMaxCPU)
{
	if(iSettings.iDrawStyle == 2)
	{
		iCpuVal = (100 - ((aCpuLoad * 100) / aGetMaxCPU));
		iMemVal = aMemory;
	}
	else //if(iSettings.iDrawStyle == 0 || 1)
	{	
		iCpuVal = (100 - ((aCpuLoad * 100) / aGetMaxCPU));
		iMemVal = ((aMemory * 100) / aTotalRamInBytes);
	}

	Draw();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScreenDrawer::RunL()
	{
	TWsRedrawEvent e;
	iWsSession.GetRedraw(e);
	
	Draw();
	if (iStatus != KErrCancel) 
		{
		Listen();
		}
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScreenDrawer::Draw(void)
	{
	iGc->Activate(iMyWindow);

	TRect drawRect(TPoint(0,0),iScreenRect.Size());
	
	iMyWindow.Invalidate(drawRect);
	iMyWindow.BeginRedraw();
	
	iGc->Clear(drawRect);
	
	TRgb myBackColor(KRgbWhite);
	TRgb myPenColor(KRgbBlack);

	iGc->SetBrushColor(myBackColor);
	iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	
	iGc->SetPenColor(myPenColor);

	if(iMyFont)
	{
		TInt txctxHeight = iMyFont->HeightInPixels();
		
		iGc->SetPenStyle(CGraphicsContext::ESolidPen);
		iGc->UseFont(iMyFont);
		
		
		drawRect.Shrink(2,2);
		
		iGc->DrawRect(drawRect);
		
		drawRect.Shrink(2,2);
		
		TBuf<25> hejlpper;
		
		if(iSettings.iDrawStyle == 1)
		{	
			hejlpper.Num(iCpuVal);
			hejlpper.Append(_L("%"));
			iGc->DrawText(hejlpper,drawRect,iMyFont->AscentInPixels(), CGraphicsContext::ECenter, 0);
			
			drawRect.iTl.iY = (drawRect.iTl.iY + txctxHeight);
			
			hejlpper.Num(iMemVal);
			hejlpper.Append(_L("%"));
			iGc->DrawText(hejlpper,drawRect,iMyFont->AscentInPixels(), CGraphicsContext::ECenter, 0);
		}
		else if(iSettings.iDrawStyle == 2)	
		{
			hejlpper.Num(iCpuVal);
			hejlpper.Append(_L("%"));
			iGc->DrawText(hejlpper,drawRect,iMyFont->AscentInPixels(), CGraphicsContext::ECenter, 0);
					
			drawRect.iTl.iY = (drawRect.iTl.iY + txctxHeight);
					
			hejlpper.Num(iMemVal);
			hejlpper.Append(_L(" Kb"));
			iGc->DrawText(hejlpper,drawRect,iMyFont->AscentInPixels(), CGraphicsContext::ECenter, 0);
		}
		else //if(iSettings.iDrawStyle == 0)	
		{
			TSize hjelpSiz(drawRect.Size());
			hjelpSiz.iWidth = (hjelpSiz.iWidth / 2);
						
			PieDrawer(drawRect.iTl, hjelpSiz.iWidth,iCpuVal,ETrue);
							
			drawRect.iTl.iX = (drawRect.iTl.iX + hjelpSiz.iWidth);
			PieDrawer(drawRect.iTl, hjelpSiz.iWidth,iMemVal,EFalse);
		}
	}

	iMyWindow.EndRedraw();
	
	iGc->Deactivate();
	iWsSession.Flush();
	}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScreenDrawer::PieDrawer(const TPoint& aLeftTop,const TInt& aSize, const TInt& aProsentages, TBool aInvertColors)
{
	if(iGc)
	{
		TBuf<50> Buffer;
		Buffer.Num(aProsentages);
		Buffer.Append(_L("%"));
	
		iGc->SetPenColor(KRgbBlack);
		
		if(aInvertColors)
			iGc->SetBrushColor(KRgbGreen);
		else
			iGc->SetBrushColor(KRgbRed);
		
		iGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		iGc->DrawEllipse(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize));
	
		if(aProsentages < 0)
		{
			Buffer.Copy(_L("N/A"));
			
			if(aInvertColors)
				iGc->SetBrushColor(KRgbGreen);
			else
				iGc->SetBrushColor(KRgbRed);
		}
		else
		{
			TInt HelpPie = 0;
	
			if(aInvertColors)
				iGc->SetBrushColor(KRgbRed);
			else
				iGc->SetBrushColor(KRgbGreen);
	
			TInt Angle =((aProsentages * 360) / 100);
	
			if(Angle == 360)
			{
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),aLeftTop,aLeftTop);
			}
			else if(Angle == 45)
			{
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX,aLeftTop.iY));
			}
			else if(Angle == 135)
			{
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX,aLeftTop.iY + aSize));
			}
			else if(Angle == 225)
			{
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX + aSize,aLeftTop.iY + aSize));
			}
			else if(Angle == 315)
			{
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX + aSize,aLeftTop.iY));
			}
			else if(Angle > 45 && Angle < 135)
			{				
				HelpPie = ((((Angle - 45) * aSize) / 90) + aLeftTop.iY);
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX,HelpPie));
			}
			else if(Angle > 135 && Angle < 225)
			{
				HelpPie = ((((Angle - 135) * aSize) / 90) + aLeftTop.iX);
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(HelpPie,aLeftTop.iY + aSize));
			}
			else if(Angle > 225 && Angle < 315)
			{
				HelpPie = (((aLeftTop.iY + aSize) - ((Angle - 225) * aSize) / 90));
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(aLeftTop.iX + aSize,HelpPie));
			}
			else if(Angle > 0 && Angle < 45)
			{
				HelpPie = (((aLeftTop.iX + (aSize/2)) - ((Angle) * (aSize/2)) / 45));
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(HelpPie,aLeftTop.iY));
			}
			else if(Angle > 315 && Angle < 360)
			{
				HelpPie = (((aLeftTop.iX + aSize) - ((Angle - 315) * (aSize/2)) / 45));
				iGc->DrawPie(TRect(aLeftTop.iX,aLeftTop.iY,aLeftTop.iX + aSize,aLeftTop.iY + aSize),TPoint(aLeftTop.iX + (aSize/2),aLeftTop.iY),TPoint(HelpPie,aLeftTop.iY));
			}
	
			if(Angle > 180)
				iGc->SetBrushColor(KRgbGreen);
			else
				iGc->SetBrushColor(KRgbRed);
		}
	
		TInt TextHeight= iMyFont->HeightInPixels();
		TInt TextWidth = iMyFont->TextWidthInPixels(Buffer);
	
		TInt StartX = (aLeftTop.iX + ((aSize - TextWidth) / 2));
		TInt StartY = (aLeftTop.iY + ((aSize - TextHeight) / 2));
	
		iGc->UseFont(iMyFont);
		iGc->SetPenColor(KRgbBlack);
		iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
		
		iGc->DrawText(Buffer,TRect(StartX,StartY,StartX + TextWidth,StartY + TextHeight),iMyFont->AscentInPixels(), CGraphicsContext::ELeft, 0);			
	
		iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CScreenDrawer::RunError(TInt /*aError*/)
	{
	return KErrNone;//aError;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CScreenDrawer::DoCancel()
	{
	iWsSession.RedrawReadyCancel();
	}
