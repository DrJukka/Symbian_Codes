/* Copyright (c) 2001-2006,Jukka Silvennoinen, All rights reserved */


#include <eikedwin.h>
#include <aknenv.h>
#include <stringloader.h>
#include <AknUtils.h>
#include <BARSREAD.H>
#include <TXTRICH.H>
#include <eikappui.h>
#include <APGCLI.H>

#include "ProcessesAndTasks.hrh"
#include "Definitions.h"
#ifdef SONE_VERSION
	#include "YTools_2002B0A0_res.rsg"
#else
	#ifdef LAL_VERSION
		#include "YTools_A000257C_res.rsg"
	#else
		#include "YTools_A000257C_res.rsg"
	#endif
#endif



#include "InfoContainer.h"


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CYuccaInfo* CYuccaInfo::NewL(const TRect& aRect)
	{
	CYuccaInfo* self = new(ELeave)CYuccaInfo();
	self->ConstructL(aRect);
	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CYuccaInfo::~CYuccaInfo()
{
	delete iEditor;
	iEditor=NULL;
	delete iParaformMe;
	delete iCharformMe;
	delete iRichMessageMe; 
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CYuccaInfo::CYuccaInfo()
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CYuccaInfo::ConstructL(const TRect& aRect)
{
    CreateWindowL();
	SetRect(aRect);
	
//	MakeEditorL();
	
	ActivateL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/			
void CYuccaInfo::MakeEditorL(void)
{
	delete iEditor;
	iEditor = NULL;	
	iEditor = new (ELeave) CEikRichTextEditor;
	
//	iEditor->SetMopParent(this);
	iEditor->SetContainerWindowL(*this);
	iEditor->SetAvkonWrap(ETrue);
	
	iEditor->ConstructL(this,0,0,0);
	//iEditor->ConstructL(this,10,0,EAknEditorFlagDefault |EAknEditorFlagEnableScrollBars,EGulFontControlAll,EGulAllFonts);
	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	
//	iEditor->InitFindL(iModel,iFindList,iReplaceList);
	
	iEditor->SetRect(Rect());
//	iEditor->SetMaxLength(250);

	TCharFormat		charFormat;	
    TCharFormatMask charFormatMask;	

	_LIT(KFontArial,"Arial");
		
	TFontSpec MyeFontSpec (KFontArial, 12*10); // 12 points = 1/6 inch
		
	MyeFontSpec.iTypeface.SetIsProportional(ETrue); // Arial is proportional 
	charFormat.iFontSpec = MyeFontSpec;

    charFormatMask.SetAll();
	
	delete iParaformMe;
	iParaformMe = NULL;
	iParaformMe = CParaFormatLayer::NewL(); 
	
	delete iCharformMe;
	iCharformMe = NULL;
	iCharformMe = CCharFormatLayer::NewL(charFormat,charFormatMask); 
	
	delete iRichMessageMe;
	iRichMessageMe = NULL;
	iRichMessageMe  = CRichText::NewL(iParaformMe,iCharformMe);

	iEditor->SetAknEditorFlags(EAknEditorFlagEnableScrollBars);
	TRect rect = Rect();
	
	iEditor->CreateScrollBarFrameL();
//	iEditor->CreatePreAllocatedScrollBarFrameL();
	iEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	
	CEikScrollBarFrame* hhh = iEditor->ScrollBarFrame();
	if(hhh)
	{	
		CEikScrollBar* cccc = hhh->VerticalScrollBar();
		if(cccc)
		{
			TRect ScrollBarRect = cccc->Rect();
			iEditor->SetExtent(TPoint(0,0), TSize(rect.Width()-ScrollBarRect.Width(), rect.Height()));
		}
	}
	iEditor->SetFocus(ETrue);
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaInfo::SizeChanged()
{
	MakeEditorL();
}
	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CYuccaInfo::CountComponentControls() const
    {
	if(iEditor)
		return 1;
	else
		return 0;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CYuccaInfo::Draw(const TRect& aRect) const
{    
	TRect rect(aRect);
	rect.SetHeight(22);
	rect.Shrink(1,1);
	rect.SetWidth(rect.Width()-2);
    CWindowGc& gc = SystemGc();
	gc.Clear();
	
    gc.SetBrushColor(KRgbGreen);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.DrawRect(rect);

#ifdef __SERIES60_3X__
	const CFont* UseMeFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);;
#else	
	const CFont* UseMeFont = MyeFontSpec = LatinBold17();
#endif

	gc.UseFont(UseMeFont);
	
	TRect TxtRect(2,2,(Rect().Size().iWidth - 2),UseMeFont->HeightInPixels());
	
	gc.DrawText(iProcessName,TxtRect,UseMeFont->AscentInPixels(), CGraphicsContext::ELeft, 0);
	gc.DiscardFont();	
}
   
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CCoeControl* CYuccaInfo::ComponentControl(TInt /*aIndex*/) const
    {
       return iEditor;
    }

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

TKeyResponse CYuccaInfo::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
{
	TKeyResponse  MyRet = EKeyWasNotConsumed;

	
	switch (aKeyEvent.iCode)
    {
	case EKeyUpArrow:
		if(iEditor)
		{
			iEditor->MoveCursorL(TCursorPosition::EFPageUp, EFalse);
	        iEditor->UpdateScrollBarsL();
			iEditor->DrawNow();
			MyRet = EKeyWasConsumed;
		}
		DrawNow();
		break;
	case EKeyDownArrow:
		if(iEditor)
		{
			iEditor->MoveCursorL(TCursorPosition::EFPageDown,EFalse);
			iEditor->UpdateScrollBarsL();
			iEditor->DrawNow();
			MyRet = EKeyWasConsumed;
		}
		DrawNow();
		break;
	default:
		break;
	}        

	return MyRet;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

void CYuccaInfo::AddProcessDetailsL(TFindProcess find) 
{
 	if (iRichMessageMe)
	{
		TFileName Hlepper;
	  	RProcess ph;
	  	ph.Open(find);
	
	  	iProcessName.Copy(ph.FileName());
		
		Hlepper.Copy(_L("Priority: "));
		Hlepper.AppendNum(ph.Priority());
  		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

#ifdef __SERIES60_3X__
		Hlepper.Copy(_L("ProcessID: "));
		Hlepper.AppendNum(ph.Id().Id(), EHex);
  		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		
		AddApplicationDetailsL(ph.SecureId());	
		Hlepper.Copy(_L("SecureID: "));
		Hlepper.AppendNum(ph.SecureId(), EHex);
  		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

		Hlepper.Copy(_L("VendorID: "));
		Hlepper.AppendNum(ph.VendorId(), EHex);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			
		TModuleMemoryInfo aInfo;
		if(KErrNone == ph.GetMemoryInfo(aInfo))
		{
			Hlepper.Copy(_L("CodeSize: "));
			Hlepper.AppendNum(aInfo.iCodeSize);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

			Hlepper.Copy(_L("ConstData: "));
			Hlepper.AppendNum(aInfo.iConstDataSize);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
	
			Hlepper.Copy(_L("InitData: "));
			Hlepper.AppendNum(aInfo.iInitialisedDataSize);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
	
			Hlepper.Copy(_L("UnInitData: "));
			Hlepper.AppendNum(aInfo.iUninitialisedDataSize);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		
			TInt Caps(0);
			if(ph.HasCapability(ECapabilityTCB,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: TCB"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityCommDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: CommDD"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Hlepper.Copy(_L("Capability: PowerMgmt"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityMultimediaDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: MultimediaDD"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityReadDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: ReadDeviceData"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityWriteDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: WriteDeviceData"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityDRM,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: DRM"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityTrustedUI,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: TrustedUI"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityProtServ,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: ProtServ"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityDiskAdmin,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: DiskAdmin"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityNetworkControl,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: NetworkControl"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityAllFiles,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: AllFiles"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilitySwEvent,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: SwEvent"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityNetworkServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: NetworkServices"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityLocalServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: LocalServices"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityReadUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: ReadUserData"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityWriteUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: WriteUserData"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityLocation,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: Location"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilitySurroundingsDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: SurroundingsDD"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			if(ph.HasCapability(ECapabilityUserEnvironment,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
			{
				Caps++;
				Hlepper.Copy(_L("Capability: UserEnvironment"));
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
				iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			}
			
			Hlepper.Copy(_L("Capabilities count: "));
			Hlepper.AppendNum(Caps);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
#else
	
#endif
	  	ph.Close();

		if(iEditor)
		{
			iEditor->SetDocumentContentL(*iRichMessageMe);
			iEditor->SetDocumentContentL(*iRichMessageMe);
			iEditor->SetCursorPosL(0,EFalse);
			
			iEditor->SetFocus(EFalse);
			iEditor->SetFocusing(ETrue);
			iEditor->ClearSelectionL();
				
			iEditor->UpdateScrollBarsL();
			iEditor->DrawNow();
		}
	}
}
 /*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/ 	
void CYuccaInfo::AddThreadDetailsL(TFindThread find) 
{	
	if (iRichMessageMe)
	{
	  	RThread ph;
	  	ph.Open(find);
	  	
	  	TFileName Hlepper;
	  	RProcess aProcess;
	  	
	  	if(KErrNone == ph.Process(aProcess))
	  	{
	  		iProcessName.Copy(aProcess.FileName());
	  	}

  		Hlepper.Copy(_L("Priority: "));
		Hlepper.AppendNum(ph.Priority());
  		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

#ifdef __SERIES60_3X__

		Hlepper.Copy(_L("ProcessID: "));
		Hlepper.AppendNum(ph.Id().Id(), EHex);
  		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
	
		AddApplicationDetailsL(ph.SecureId());
		
		Hlepper.Copy(_L("SecureID: "));
		Hlepper.AppendNum(ph.SecureId(), EHex);
  		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

		Hlepper.Copy(_L("VendorID: "));
		Hlepper.AppendNum(ph.VendorId(), EHex);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

		TThreadStackInfo aInfo;
		if(KErrNone == ph.StackInfo(aInfo))
		{
			Hlepper.Copy(_L("Stack size: "));
			Hlepper.AppendNum(aInfo.iBase -  aInfo.iLimit);	
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
#else
	
#endif
		TTimeIntervalMicroSeconds CpuTime(0);
		
		ph.GetCpuTime(CpuTime);
		Hlepper.Copy(_L("CpuTime: "));
		Hlepper.AppendNum(CpuTime.Int64());
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

#ifdef __SERIES60_3X__

		TInt Caps(0);
		
		if(ph.HasCapability(ECapabilityTCB,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: TCB"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityCommDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: CommDD"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: PowerMgmt"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityMultimediaDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: MultimediaDD"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityReadDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: ReadDeviceData"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityWriteDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: WriteDeviceData"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityDRM,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: DRM"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityTrustedUI,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: TrustedUI"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityProtServ,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: ProtServ"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityDiskAdmin,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: DiskAdmin"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityNetworkControl,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: NetworkControl"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityAllFiles,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: AllFiles"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilitySwEvent,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: SwEvent"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityNetworkServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: NetworkServices"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityLocalServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: LocalServices"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityReadUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: ReadUserData"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityWriteUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: WriteUserData"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityLocation,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: Location"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilitySurroundingsDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: SurroundingsDD"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		if(ph.HasCapability(ECapabilityUserEnvironment,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			Caps++;
			Hlepper.Copy(_L("Capability: UserEnvironment"));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
		}
		
		Hlepper.Copy(_L("Capabilities count: "));
		Hlepper.AppendNum(Caps);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
		iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
#else
	
#endif			
	  	ph.Close();
		
		if(iEditor)
		{
			iEditor->SetDocumentContentL(*iRichMessageMe);
			iEditor->SetDocumentContentL(*iRichMessageMe);
			iEditor->SetCursorPosL(0,EFalse);
					
			iEditor->SetFocus(EFalse);
			iEditor->SetFocusing(ETrue);
			iEditor->ClearSelectionL();
						
			iEditor->UpdateScrollBarsL();
			iEditor->DrawNow();
		}
	}

}
 /*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/ 	
void CYuccaInfo::AddApplicationDetailsL(TInt aUidVal) 
{	
	RApaLsSession appArcSession;
	if(KErrNone == appArcSession.Connect()) // connect to AppArc server
	{
		TApaAppInfo aInfo;
		if(KErrNone == appArcSession.GetAppInfo(aInfo, TUid::Uid(aUidVal)))
		{
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),_L("Caption: "));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),aInfo.iCaption);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
			
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),_L("Short aption: "));
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),aInfo.iShortCaption);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);
	
			TFileName Hlepper;
			Hlepper.Copy(_L("AppUid: "));
			Hlepper.AppendNum((TUint32)aInfo.iUid.iUid, EHex);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(),Hlepper);
			iRichMessageMe->InsertL(iRichMessageMe->DocumentLength(), CEditableText::ELineBreak);

		}
		
		appArcSession.Close();
	}
}
