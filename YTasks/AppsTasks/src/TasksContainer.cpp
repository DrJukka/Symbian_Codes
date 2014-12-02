
#include "TasksContainer.h"

#include<AknIconArray.h>
#include <eikclbd.h>
#include <APGWGNAM.H>
#include <APGCLI.H>
#include <AknQueryDialog.h>

#include "MainServerSession.h"
#include "YTools_A0000F60.hrh"

#include "Common.h"

#ifdef SONE_VERSION
	#include <YTools_2002B0A7_res.rsg>
#else
	#ifdef LAL_VERSION
		#include <YTools_A0000F60_res.rsg>
	#else
		#include <YTools_A0000F60_res.rsg>
	#endif
#endif
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CTasksContainer::CTasksContainer(void)
{		

}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CTasksContainer::~CTasksContainer()
{  	
	delete iAppUIDs;
    delete iMyListBox;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTasksContainer::ConstructL()
{
	CreateWindowL();	
	SetRect(CEikonEnv::Static()->EikAppUi()->ClientRect());
		
	ActivateL();
	DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTasksContainer::SizeChanged()
{
	MakeListBoxL();	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTasksContainer::MakeListBoxL()
{
	TInt MySetIndex(0);
	
	if(iMyListBox)
	{
		MySetIndex = iMyListBox->CurrentItemIndex();
	}
    
	delete iMyListBox;
	iMyListBox = NULL;

    iMyListBox   = new( ELeave ) CAknDoubleLargeStyleListBox();
	iMyListBox->ConstructL(this,EAknListBoxSelectionList);

	CArrayPtr<CGulIcon>* icons =new( ELeave ) CAknIconArray(1);
	CleanupStack::PushL(icons);
	
	iMyListBox->Model()->SetItemTextArray(GetArrayL(icons));
   	iMyListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    
    CleanupStack::Pop(icons);
	iMyListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
	
	iMyListBox->CreateScrollBarFrameL( ETrue );
    iMyListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iMyListBox->SetRect(Rect());
	
	iMyListBox->View()->SetListEmptyTextL(KtxNoData);

	iMyListBox->ActivateL();

	TInt ItemsCount = iMyListBox->Model()->ItemTextArray()->MdcaCount();
	
	if(ItemsCount > MySetIndex && MySetIndex >= 0)
		iMyListBox->SetCurrentItemIndex(MySetIndex);
	else if(ItemsCount > 0)
		iMyListBox->SetCurrentItemIndex(0);
		
	UpdateScrollBar(iMyListBox);
	iMyListBox->DrawNow();
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

CDesCArray* CTasksContainer::GetArrayL(CArrayPtr<CGulIcon>* aIcon)
{	
	delete iAppUIDs;
	iAppUIDs = NULL;
	iAppUIDs = new(ELeave)CArrayFixFlat<TUid>(10);
	
	CDesCArrayFlat* MyArray = new(ELeave)CDesCArrayFlat(1);
	CleanupStack::PushL(MyArray);
	
	RWsSession& wsSession = CEikonEnv::Static()->WsSession();
	
	CApaWindowGroupName* wgName=CApaWindowGroupName::NewL(wsSession);
	CleanupStack::PushL(wgName);
	
	CArrayFixFlat<TInt>* WindowGroupIds = new(ELeave)CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(WindowGroupIds);

	User::LeaveIfError(wsSession.WindowGroupList(0,WindowGroupIds));
	const TInt count = WindowGroupIds->Count();

	TFileName HelpFn;
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	
	for (TInt ii=0; ii<count; ii++)
	{
		TInt wgId = WindowGroupIds->At(ii);
		wgName->ConstructFromWgIdL(wgId);
		
	TRAPD(Err,
		CFbsBitmap*	AppIcon(NULL);
		CFbsBitmap*	AppIconMsk(NULL);
	    AknsUtils::CreateAppIconLC(skin,wgName->AppUid(),  EAknsAppIconTypeContext,AppIcon,AppIconMsk);
	
		if(AppIcon && AppIconMsk)
		{
			HelpFn.Num(aIcon->Count());
			HelpFn.Append(_L("\t"));
			HelpFn.Append(wgName->Caption());
			HelpFn.Append(_L("\t"));
			if(wgName->Hidden())
			{
				HelpFn.Append(_L("Hidden "));
			}
			
			if(wgName->IsSystem())
			{
				HelpFn.Append(_L("System "));
			}
			
			HelpFn.TrimAll();
			
			iAppUIDs->AppendL(wgName->AppUid());
			MyArray->AppendL(HelpFn);
			
			aIcon->AppendL(CGulIcon::NewL(AppIcon,AppIconMsk));
			CleanupStack::Pop(2);	
		});			
	}

	CleanupStack::PopAndDestroy(2);//WindowGroupIds, wgName
	
	
	// Append Text string to MyArray in here as "1\tMytext", where 1 is the zero based icon index
	// also remember to load and add icons in here... listbox will automatically re-size icons
	// thus no need to set any size for them.
	
	CleanupStack::Pop(MyArray);
	return MyArray;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTasksContainer::UpdateScrollBar(CEikListBox* aListBox)
    {
    if (aListBox)
        {   
        TInt pos(aListBox->View()->CurrentItemIndex());
        if (aListBox->ScrollBarFrame())
            {
            aListBox->ScrollBarFrame()->MoveVertThumbTo(pos);
            }
        }
    }
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTasksContainer::Draw(const TRect& /*aRect*/) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(Rect());
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CTasksContainer::HandleViewCommandL(TInt aCommand)
{
    switch(aCommand)
        {
        case ETasksReFresh:
        	{
        		MakeListBoxL();
        	}
        	break;
		case EBringToFrg:
			if(iMyListBox && iAppUIDs)
			{
				TInt Curr = iMyListBox->CurrentItemIndex();	
				if(Curr < iAppUIDs->Count() && Curr >= 0)
				{
					RWsSession& wsSession = CEikonEnv::Static()->WsSession();
	
					TApaTaskList taskList( wsSession );
					TApaTask task = taskList.FindApp(iAppUIDs->At(Curr));

					if( task.Exists() ) 
					{
						task.BringToForeground();
					}
				}
			}
			break;
		case ECloseTask:
			if(iMyListBox && iAppUIDs)
			{
				TInt Curr = iMyListBox->CurrentItemIndex();	
				if(Curr < iAppUIDs->Count() && Curr >= 0)
				{
					CAknQueryDialog* dlg = CAknQueryDialog::NewL();
					if(dlg->ExecuteLD(R_QUERY,_L("Close Task")))
					{
						RWsSession& wsSession = CEikonEnv::Static()->WsSession();
		
						TApaTaskList taskList( wsSession );
						TApaTask task = taskList.FindApp(iAppUIDs->At(Curr));

						if( task.Exists() ) 
						{
							task.EndTask();
						}
						
						MakeListBoxL();
					}
				}
			}
			break;
		case EKillTask:
			if(iMyListBox && iAppUIDs)
			{
				TInt Curr = iMyListBox->CurrentItemIndex();	
				if(Curr < iAppUIDs->Count() && Curr >= 0)
				{
					CAknQueryDialog* dlg = CAknQueryDialog::NewL();
					if(dlg->ExecuteLD(R_QUERY,_L("Kill Task")))
					{
						RMainServerClient MainServerClient;
						if(MainServerClient.Connect() == KErrNone)
						{
							TUid Hjelpper(iAppUIDs->At(Curr));
							MainServerClient.KillTaskL(Hjelpper);
							MainServerClient.Close();
						}
					
						MakeListBoxL();
					}
				}
			}
			break;
        default:
            break;
        }	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC* CTasksContainer::GetTaskInfoL(TDes& aTitle)
{
	HBufC* Ret(NULL);
	if(iMyListBox && iAppUIDs)
	{
		TInt Curr = iMyListBox->CurrentItemIndex();	
		if(Curr < iAppUIDs->Count() && Curr >= 0)
		{	
			Ret = HBufC::NewLC(4000);

			TPtr Hjep(Ret->Des());
			AddApplicationDetailsL(iAppUIDs->At(Curr),Hjep,aTitle);
			
			CleanupStack::Pop(Ret);
		}
	}
	
	return Ret;
}
 /*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/ 	
void CTasksContainer::AddProcessDetailsL(RProcess& ph,TDes& aBuffer) 
{
		aBuffer.Append(_L("\nPriority: "));
		aBuffer.AppendNum(ph.Priority());

#ifdef __SERIES60_3X__
		aBuffer.Append(_L("\nProcessID: "));
		aBuffer.AppendNum(ph.Id().Id(), EHex);
	
		aBuffer.Append(_L("\nSecureID: "));
		aBuffer.AppendNum(ph.SecureId(), EHex);
  
		aBuffer.Append(_L("\nVendorID: "));
		aBuffer.AppendNum(ph.VendorId(), EHex);
	
		TModuleMemoryInfo aInfo;
		if(KErrNone == ph.GetMemoryInfo(aInfo))
		{
			aBuffer.Append(_L("\nCodeSize: "));
			aBuffer.AppendNum(aInfo.iCodeSize);

			aBuffer.Append(_L("\nConstData: "));
			aBuffer.AppendNum(aInfo.iConstDataSize);

			aBuffer.Append(_L("\nInitData: "));
			aBuffer.AppendNum(aInfo.iInitialisedDataSize);
	
			aBuffer.Append(_L("\nUnInitData: "));
			aBuffer.AppendNum(aInfo.iUninitialisedDataSize);
		}
		
		if(ph.HasCapability(ECapabilityTCB,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: TCB"));
		}
		
		if(ph.HasCapability(ECapabilityCommDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: CommDD"));
		}
		
		if(ph.HasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: PowerMgmt"));
		}
		
		if(ph.HasCapability(ECapabilityMultimediaDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: MultimediaDD"));
		}
		
		if(ph.HasCapability(ECapabilityReadDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: ReadDeviceData"));
		}
		
		if(ph.HasCapability(ECapabilityWriteDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: WriteDeviceData"));
		}
		
		if(ph.HasCapability(ECapabilityDRM,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: DRM"));
		}
		
		if(ph.HasCapability(ECapabilityTrustedUI,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: TrustedUI"));
		}
		
		if(ph.HasCapability(ECapabilityProtServ,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: ProtServ"));
		}
		
		if(ph.HasCapability(ECapabilityDiskAdmin,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: DiskAdmin"));
		}
		
		if(ph.HasCapability(ECapabilityNetworkControl,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: NetworkControl"));
		}
		
		if(ph.HasCapability(ECapabilityAllFiles,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: AllFiles"));
		}
		
		if(ph.HasCapability(ECapabilitySwEvent,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: SwEvent"));
		}
		
		if(ph.HasCapability(ECapabilityNetworkServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: NetworkServices"));
		}
		
		if(ph.HasCapability(ECapabilityLocalServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: LocalServices"));
		}
		
		if(ph.HasCapability(ECapabilityReadUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: ReadUserData"));
		}
		
		if(ph.HasCapability(ECapabilityWriteUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: WriteUserData"));
			}
		
		if(ph.HasCapability(ECapabilityLocation,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: Location"));
		}
		
		if(ph.HasCapability(ECapabilitySurroundingsDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: SurroundingsDD"));
		}
		
		if(ph.HasCapability(ECapabilityUserEnvironment,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
		{
			aBuffer.Append(_L("\nCapability: UserEnvironment"));
		}
#else
	
#endif

}
 /*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/ 	
void CTasksContainer::AddThreadDetailsL(RThread aThread,TDes& aBuffer) 
{	
  	aBuffer.Append(_L("\nPriority: "));
	aBuffer.AppendNum(aThread.Priority());

#ifdef __SERIES60_3X__

	aBuffer.Append(_L("\nProcessID: "));
	aBuffer.AppendNum(aThread.Id().Id(), EHex);

	aBuffer.Append(_L("\nSecureID: "));
	aBuffer.AppendNum(aThread.SecureId(), EHex);

	aBuffer.Append(_L("\nVendorID: "));
	aBuffer.AppendNum(aThread.VendorId(), EHex);

	TThreadStackInfo aInfo;
	if(KErrNone == aThread.StackInfo(aInfo))
	{
		aBuffer.Append(_L("\nStack size: "));
		aBuffer.AppendNum(aInfo.iBase -  aInfo.iLimit);	
	}
#else

#endif
	TTimeIntervalMicroSeconds CpuTime(0);
	
	aThread.GetCpuTime(CpuTime);
	aBuffer.Append(_L("\nCpuTime: "));
	aBuffer.AppendNum(CpuTime.Int64());

#ifdef __SERIES60_3X__

	if(aThread.HasCapability(ECapabilityTCB,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: TCB"));
	}
	
	if(aThread.HasCapability(ECapabilityCommDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: CommDD"));
	}
	
	if(aThread.HasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: PowerMgmt"));
	}
	
	if(aThread.HasCapability(ECapabilityMultimediaDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: MultimediaDD"));
	}
	
	if(aThread.HasCapability(ECapabilityReadDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: ReadDeviceData"));
	}
	
	if(aThread.HasCapability(ECapabilityWriteDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: WriteDeviceData"));
	}
	
	if(aThread.HasCapability(ECapabilityDRM,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: DRM"));
	}
	
	if(aThread.HasCapability(ECapabilityTrustedUI,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: TrustedUI"));
	}
	
	if(aThread.HasCapability(ECapabilityProtServ,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: ProtServ"));
	}
	
	if(aThread.HasCapability(ECapabilityDiskAdmin,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: DiskAdmin"));
	}
	
	if(aThread.HasCapability(ECapabilityNetworkControl,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: NetworkControl"));
	}
	
	if(aThread.HasCapability(ECapabilityAllFiles,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: AllFiles"));
	}
	
	if(aThread.HasCapability(ECapabilitySwEvent,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: SwEvent"));
	}
	
	if(aThread.HasCapability(ECapabilityNetworkServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: NetworkServices"));
	}
	
	if(aThread.HasCapability(ECapabilityLocalServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: LocalServices"));
	}
	
	if(aThread.HasCapability(ECapabilityReadUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: ReadUserData"));
	}
	
	if(aThread.HasCapability(ECapabilityWriteUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: WriteUserData"));
	}
	
	if(aThread.HasCapability(ECapabilityLocation,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: Location"));
	}
	
	if(aThread.HasCapability(ECapabilitySurroundingsDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: SurroundingsDD"));
	}
	
	if(aThread.HasCapability(ECapabilityUserEnvironment,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
	{
		aBuffer.Append(_L("\nCapability: UserEnvironment"));
	}
#else

#endif			
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/ 	
void CTasksContainer::AddApplicationDetailsL(TUid aUid, TDes& aBuffer, TDes& aName) 
{	
	RWsSession& wsSession = CEikonEnv::Static()->WsSession();
	
	CApaWindowGroupName* wgName=CApaWindowGroupName::NewL(wsSession);
	CleanupStack::PushL(wgName);
	
	CArrayFixFlat<TInt>* WindowGroupIds = new(ELeave)CArrayFixFlat<TInt>(1);
	CleanupStack::PushL(WindowGroupIds);

	User::LeaveIfError(wsSession.WindowGroupList(0,WindowGroupIds));
	const TInt count = WindowGroupIds->Count();

	for (TInt ii=0; ii<count; ii++)
	{
		TInt wgId = WindowGroupIds->At(ii);
		wgName->ConstructFromWgIdL(wgId);
		if((TUint32)wgName->AppUid().iUid == (TUint32)aUid.iUid)
		{
			aBuffer.Copy(_L("Caption:\n\t"));
			aBuffer.Append(wgName->Caption());
				
			if(wgName->DocName().Length())
			{
				aBuffer.Append(_L("\nDoc name:\n\t"));
				aBuffer.Append(wgName->DocName());
			}
			
			if(wgName->WindowGroupName().Length())
			{
				aBuffer.Append(_L("\nWindowgroup name:\n\t"));
				aBuffer.Append(wgName->WindowGroupName());	
			}
			
			aBuffer.Append(_L("\nAppUid: "));
			aBuffer.AppendNum((TUint32)aUid.iUid, EHex);
	
			TThreadId ThreadId;
			if(wsSession.GetWindowGroupClientThreadId(wgId,ThreadId) == KErrNone)
			{
				TFileName res;
				TFindThread find;
				while(find.Next(res) == KErrNone)
				{
					RThread ph;
			      	if(ph.Open(find) == KErrNone)
			      	{
				      	if(ph.Id() == ThreadId)
				      	{
				      		RProcess Process;
						  	if(KErrNone == ph.Process(Process))
						  	{
						  		TParsePtrC Hjelppper(Process.FileName());
					  			aName.Copy(Hjelppper.NameAndExt());
						  		Process.Close();
						  	}

					  	 	aBuffer.Append(_L("\nPriority: "));
							aBuffer.AppendNum(ph.Priority());

						#ifdef __SERIES60_3X__

							aBuffer.Append(_L("\nProcessID: "));
							aBuffer.AppendNum(ph.Id().Id(), EHex);

							aBuffer.Append(_L("\nSecureID: "));
							aBuffer.AppendNum(ph.SecureId(), EHex);

							aBuffer.Append(_L("\nVendorID: "));
							aBuffer.AppendNum(ph.VendorId(), EHex);

							TThreadStackInfo aInfo;
							if(KErrNone == ph.StackInfo(aInfo))
							{
								aBuffer.Append(_L("\nStack size: "));
								aBuffer.AppendNum(aInfo.iBase -  aInfo.iLimit);	
							}
						#else

						#endif
							TTimeIntervalMicroSeconds CpuTime(0);
							
							ph.GetCpuTime(CpuTime);
							aBuffer.Append(_L("\nCpuTime: "));
							aBuffer.AppendNum(CpuTime.Int64());

						#ifdef __SERIES60_3X__

							if(ph.HasCapability(ECapabilityTCB,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: TCB"));
							}
							
							if(ph.HasCapability(ECapabilityCommDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: CommDD"));
							}
							
							if(ph.HasCapability(ECapabilityPowerMgmt,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: PowerMgmt"));
							}
							
							if(ph.HasCapability(ECapabilityMultimediaDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: MultimediaDD"));
							}
							
							if(ph.HasCapability(ECapabilityReadDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: ReadDeviceData"));
							}
							
							if(ph.HasCapability(ECapabilityWriteDeviceData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: WriteDeviceData"));
							}
							
							if(ph.HasCapability(ECapabilityDRM,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: DRM"));
							}
							
							if(ph.HasCapability(ECapabilityTrustedUI,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: TrustedUI"));
							}
							
							if(ph.HasCapability(ECapabilityProtServ,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: ProtServ"));
							}
							
							if(ph.HasCapability(ECapabilityDiskAdmin,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: DiskAdmin"));
							}
							
							if(ph.HasCapability(ECapabilityNetworkControl,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: NetworkControl"));
							}
							
							if(ph.HasCapability(ECapabilityAllFiles,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: AllFiles"));
							}
							
							if(ph.HasCapability(ECapabilitySwEvent,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: SwEvent"));
							}
							
							if(ph.HasCapability(ECapabilityNetworkServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: NetworkServices"));
							}
							
							if(ph.HasCapability(ECapabilityLocalServices,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: LocalServices"));
							}
							
							if(ph.HasCapability(ECapabilityReadUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: ReadUserData"));
							}
							
							if(ph.HasCapability(ECapabilityWriteUserData,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: WriteUserData"));
							}
							
							if(ph.HasCapability(ECapabilityLocation,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: Location"));
							}
							
							if(ph.HasCapability(ECapabilitySurroundingsDD,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: SurroundingsDD"));
							}
							
							if(ph.HasCapability(ECapabilityUserEnvironment,__PLATSEC_DIAGNOSTIC_STRING(KSuppressPlatSecDiagnostic)))
							{
								aBuffer.Append(_L("\nCapability: UserEnvironment"));
							}
						#else

						#endif	
						  	
						  	
						  	break;
				      	}
			      	
			      		ph.Close();
			      	}
				}
			}
		}
	}

	CleanupStack::PopAndDestroy(2);//WindowGroupIds, wgName
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TKeyResponse CTasksContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode)
{
	TKeyResponse Ret = EKeyWasNotConsumed;	
	
	switch (aKeyEvent.iCode)
	    {
	    case EKeyDevice3:
			CEikonEnv::Static()->AppUiFactory()->MenuBar()->TryDisplayMenuBarL();
			break;
		default:
			if(iMyListBox)
			{
				Ret = iMyListBox->OfferKeyEventL(aKeyEvent,aEventCode);
			}
			break;
	    };
	
	return Ret;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
CCoeControl* CTasksContainer::ComponentControl( TInt /*aIndex*/) const
{
	return iMyListBox;
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
TInt CTasksContainer::CountComponentControls() const
{	
	if(iMyListBox)
		return 1;
	else
		return 0;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CTasksContainer::InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane)
{	
	if(R_MAIN_MENU == aResourceId)
	{
		TBool DimSel(ETrue);
		if(iMyListBox)
		{
			if(iMyListBox->CurrentItemIndex() >= 0)
			{
				DimSel= EFalse;
			}
		}
		
		if(DimSel)
		{
			aMenuPane->SetItemDimmed(ECloseTask,ETrue);
			aMenuPane->SetItemDimmed(EShowInfo,ETrue);
			aMenuPane->SetItemDimmed(EKillTask,ETrue);
		}
	}
}

