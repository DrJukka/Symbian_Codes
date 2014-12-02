/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#include <akninputlanguageinfo.h> 
#include <IMCVCODC.H>
#include <HAL.H>
#include <HAL_data.H>
#include <sysutil.h> 

#include "Rijandel_AES.h"
#include "YTools_A000257B.h"
#include "InformationGetters.h"

#include "MainServerSession.h"



/*
-----------------------------------------------------------------------------
****************************		CInfoGetter		*************************
-----------------------------------------------------------------------------
*/
CInfoGetter::CInfoGetter(MInfoObserver& aObserver):iObserver(aObserver)
{		

}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CInfoGetter::~CInfoGetter()
{  		
	delete iNwNameCheck;
	iNwNameCheck = NULL;
	
	delete iOpNameCheck;
	iOpNameCheck = NULL;
	
	delete iImeiGetter;
	iImeiGetter = NULL;

	delete iExtraGetter;
	iExtraGetter = NULL;
	
	delete iModel;
	delete iService;
	delete iLanguage;
	delete iImei;
	delete iOperator;
	delete iNetwork;
	delete iSWVersion;
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoGetter::ConstructL()
{	
#ifdef __WINS__	
	iImsiDone = iCellIdDone = iNwDone = iOpDone = iImeiDone = ETrue;
#else
	iImsiDone = iCellIdDone = iNwDone = iOpDone = iImeiDone = EFalse;
	iNwNameCheck = new(ELeave)CNwNameCheck(*this);
	iNwNameCheck->ConstructL();
	
	iOpNameCheck = new(ELeave)COpNameCheck(*this);
	iOpNameCheck->ConstructL();
	
	iImeiGetter = new(ELeave)CImeiGetter(*this);
	iImeiGetter->ConstructL();
#endif	
	iExtraGetter = new(ELeave)CExtraGettes();

	
	TBuf<255> Hjellper;
	iExtraGetter->GetServiceCenterL(Hjellper);
	Hjellper.TrimAll();
	
	if(Hjellper.Length())
	{
		iService = HBufC8::NewL(Hjellper.Length());
		iService->Des().Copy(Hjellper);
	}
	
	Hjellper.Zero();
	iExtraGetter->GetPhoneModel(Hjellper);
	Hjellper.TrimAll();
	
	if(Hjellper.Length())
	{
		iModel = HBufC8::NewL(Hjellper.Length());
		iModel->Des().Copy(Hjellper);
	}
	
	iExtraGetter->GetS60PlatformVersionL(iOSMajor,iOSMinor);

	Hjellper.Zero();
	iExtraGetter->GetPhoneLanguageL(Hjellper);
	Hjellper.TrimAll();
	
	if(Hjellper.Length())
	{
		iLanguage = HBufC8::NewL(Hjellper.Length());
		iLanguage->Des().Copy(Hjellper);
	}
	
	Hjellper.Zero();
	SysUtil::GetSWVersion(Hjellper);

	if(Hjellper.Length())
	{
		iSWVersion= HBufC8::NewL(Hjellper.Length());
		iSWVersion->Des().Copy(Hjellper);
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC8* CInfoGetter::AESEncryptDataL(const TDesC8& aData,const TDesC8& aKey)
{
	HBufC8* RetBuf = HBufC8::NewLC(aData.Length() + 16);

	CMyAESCrypter* MyAESCrypter = CMyAESCrypter::NewL(aKey);

	TPtr8 HjelppPoint(RetBuf->Des());
	MyAESCrypter->Encrypt(aData,HjelppPoint);
	
	delete MyAESCrypter;
	MyAESCrypter = NULL;
	
	CleanupStack::Pop(RetBuf);
	return RetBuf;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC8* CInfoGetter::AESDecryptDataL(const TDesC8& aData,const TDesC8& aKey)
{
	HBufC8* RetBuf = HBufC8::NewLC(aData.Length() + 16);
	
	CMyAESCrypter* MyAESCrypter = CMyAESCrypter::NewL(aKey);

	TPtr8 HjelppPoint(RetBuf->Des());
	MyAESCrypter->Decrypt(aData,HjelppPoint);
	
	delete MyAESCrypter;
	MyAESCrypter = NULL;

	CleanupStack::Pop(RetBuf);
	return RetBuf;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

HBufC8* CInfoGetter::InfoBufferLC(const TDesC8& aKey,TBool aUseBase64)
{
	RMainServerClient MainServerClient;
	
	if(MainServerClient.Connect() == KErrNone)
	{
		TExtraInfoItem ItemBuffer;
		MainServerClient.GetExtraInfoL(ItemBuffer);
		MainServerClient.Close();
	#ifdef __WINS__	
	#else
		GotIMSIL(ItemBuffer.iImsi, KErrNone);
		CellIDL(ItemBuffer.iCountryCode,ItemBuffer.iNwId,ItemBuffer.iAreaCode,ItemBuffer.iCellId);
	#endif
	}

	HBufC8* TmpBuffer = HBufC8::NewLC(3000);
	
	if(!aUseBase64)
	{
		TmpBuffer->Des().Copy(KtxStartUSERDATA);
	}
	else
	{
		TmpBuffer->Des().Copy(KtxStartUSERDATA);
	}
	
	TmpBuffer->Des().Append(KtxStartIMEI);
	if(iImei)
	{
		TmpBuffer->Des().Append(*iImei);
	}
	TmpBuffer->Des().Append(KtxEnd__IMEI);
	
	TmpBuffer->Des().Append(KtxStartNWNAME);
	if(iNetwork)
	{
		TmpBuffer->Des().Append(*iNetwork);
	}
	TmpBuffer->Des().Append(KtxEnd__NWNAME);
	
	TmpBuffer->Des().Append(KtxStartOPNAME);
	if(iOperator)
	{
		TmpBuffer->Des().Append(*iOperator);
	}
	TmpBuffer->Des().Append(KtxEnd__OPNAME);
	
	TmpBuffer->Des().Append(KtxStartSSNUM);
	if(iService)
	{
		TmpBuffer->Des().Append(*iService);
	}
	TmpBuffer->Des().Append(KtxEnd__SSNUM);
	
	TmpBuffer->Des().Append(KtxStartPMODEL);
	if(iModel)
	{
		TmpBuffer->Des().Append(*iModel);
	}
	TmpBuffer->Des().Append(KtxEnd__PMODEL);

	TmpBuffer->Des().Append(KtxStartMUID);
	
	TInt revision = 0;
	HAL::Get( HALData::EMachineUid, revision );
	TmpBuffer->Des().AppendNum(revision,EHex);
	
	TmpBuffer->Des().Append(KtxEnd__MUID);
	
	TmpBuffer->Des().Append(KtxStartOSVER);
	TmpBuffer->Des().AppendNum(iOSMajor);
	TmpBuffer->Des().Append(_L("::"));
	TmpBuffer->Des().AppendNum(iOSMinor);
	TmpBuffer->Des().Append(KtxEnd__OSVER);
	
	TmpBuffer->Des().Append(KtxStartSWVER);
	if(iSWVersion)
	{
		TmpBuffer->Des().Append(*iSWVersion);
	}
	TmpBuffer->Des().Append(KtxEnd__SWVER);
		
	TmpBuffer->Des().Append(KtxStartLANG);
	if(iLanguage)
	{
		TmpBuffer->Des().Append(*iLanguage);
	}
	TmpBuffer->Des().Append(KtxEnd__LANG);

	TmpBuffer->Des().Append(KtxStartAPPNAME);
	TmpBuffer->Des().AppendNum((TUint32)KUidTOneViewer.iUid,EHex);
	TmpBuffer->Des().Append(KtxEnd__APPNAME);
	
	TmpBuffer->Des().Append(KtxStartAPPVER);
	TmpBuffer->Des().AppendNum(0);
	TmpBuffer->Des().Append(KtxEnd__APPVER);	
	
	TmpBuffer->Des().Append(KtxStartCELCNTR);
	if(iCellCountry)
	{
		TmpBuffer->Des().Append(*iCellCountry);
	}
	TmpBuffer->Des().Append(KtxEnd__CELCNTR);
		
	TmpBuffer->Des().Append(KtxStartCELLNED);
	if(iCellNwId)
	{
		TmpBuffer->Des().Append(*iCellNwId);
	}
	TmpBuffer->Des().Append(KtxEnd__CELLNED);

	TmpBuffer->Des().Append(KtxStartCELLAR);
	TmpBuffer->Des().AppendNum(iAreaCode,EHex);
	TmpBuffer->Des().Append(KtxEnd__CELLAR);	

	TmpBuffer->Des().Append(KtxStartCELLID);
	TmpBuffer->Des().AppendNum(iCellId,EHex);
	TmpBuffer->Des().Append(KtxEnd__CELLID);	
	
	TmpBuffer->Des().Append(KtxStartIMCNTR);
	TmpBuffer->Des().Append(iImsiCountry);
	TmpBuffer->Des().Append(KtxEnd__IMCNTR);	

	TmpBuffer->Des().Append(KtxStartIMOPR);
	TmpBuffer->Des().Append(iImsiOper);
	TmpBuffer->Des().Append(KtxEnd__IMOPR);

	TmpBuffer->Des().Append(KtxEnd__USERDATA);
	

	if(aKey.Length())
	{
		HBufC8* HjelpTmp3 = AESEncryptDataL(*TmpBuffer,aKey);
		
		TmpBuffer->Des().Zero();
		TmpBuffer->Des().Copy(*HjelpTmp3);
	
		delete HjelpTmp3;
	}
	
	if(aUseBase64)
	{
		HBufC8* HjelpTmp = Base64EncodeL(*TmpBuffer);
		
		TmpBuffer->Des().Zero();
		TmpBuffer->Des().Copy(*HjelpTmp);
	
		delete HjelpTmp;
	}
	
	return TmpBuffer;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC8* CInfoGetter::Base64DecodeL(const TDesC8& aBuffer)
{
	HBufC8* Ret = HBufC8::NewLC(aBuffer.Length());
	
	TImCodecB64 MyEnc;
	MyEnc.Initialise();

	TPtr8 AddPoint(Ret->Des());
	MyEnc.Decode(aBuffer,AddPoint);
	
	CleanupStack::Pop(Ret);
	return Ret;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
HBufC8* CInfoGetter::Base64EncodeL(const TDesC8& aBuffer)
{
	HBufC8* Ret = HBufC8::NewLC((aBuffer.Length() * 2));
	
	TImCodecB64 MyEnc;
	MyEnc.Initialise();

	TPtr8 AddPoint(Ret->Des());
	MyEnc.Encode(aBuffer,AddPoint);
	
	CleanupStack::Pop(Ret);
	return Ret;
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoGetter::CellIDL(const TDesC& aCountryCode,const TDesC& aNwId,TUint aAreaCode,TUint aCellId)
{
	delete iCellCountry;
	iCellCountry = NULL;
	
	delete iCellNwId;
	iCellNwId = NULL;
	
	if(aCountryCode.Length())
	{
		iCellCountry = HBufC8::NewL(aCountryCode.Length());
		iCellCountry->Des().Copy(aCountryCode);
	}

	if(aNwId.Length())
	{
		iCellNwId = HBufC8::NewL(aNwId.Length());
		iCellNwId->Des().Copy(aNwId);		
	}
	
	iAreaCode= aAreaCode;
	iCellId  = aCellId;
	
	iCellIdDone = ETrue;
	
	if(iNwDone && iOpDone && iImeiDone && iImsiDone)
	{
		iObserver.GotAllInfoL();
	}	
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoGetter::GotIMSIL(const TDesC& aIMSI, TInt aError)
{
	if(aIMSI.Length() >= 5)
	{
		iImsiCountry.Copy(aIMSI.Left(3));
		iImsiOper.Copy(aIMSI.Mid(2,2));
	}
	else
	{
		iImsiCountry.Copy(aIMSI);
		iImsiOper.Num(aError);
	}

	iImsiDone = ETrue;
	
	if(iNwDone && iOpDone && iImeiDone && iCellIdDone)
	{
		iObserver.GotAllInfoL();
	}	
}
	
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoGetter::NetworkNameL(const TDesC& aNwName)
{
	iNwDone = ETrue;
	delete iNetwork;
	iNetwork = NULL;
	
	if(aNwName.Length())
	{
		iNetwork = HBufC8::NewL(aNwName.Length());
		iNetwork->Des().Copy(aNwName);
	}
	
	if(iNwDone && iOpDone && iImeiDone)
	{
		iObserver.GotAllInfoL();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoGetter::PhoneImeiL(const TDesC& aIMEI,TInt /*aError*/)
{
	iImeiDone  = ETrue;
	delete iImei;
	iImei = NULL;
	
	if(aIMEI.Length())
	{
		iImei = HBufC8::NewL(aIMEI.Length());
		iImei->Des().Copy(aIMEI);
	}
	
	if(iNwDone && iOpDone && iImeiDone)
	{
		iObserver.GotAllInfoL();
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CInfoGetter::OperatorNameL(const TDesC& aNwName)
{
	iOpDone  = ETrue;
	delete iOperator;
	iOperator = NULL;
	
	if(aNwName.Length())
	{
		iOperator = HBufC8::NewL(aNwName.Length());
		iOperator->Des().Copy(aNwName);
	}
	
	if(iNwDone && iOpDone && iImeiDone)
	{
		iObserver.GotAllInfoL();
	}
}
/*
-----------------------------------------------------------------------
***********************		CNwNameCheck		***********************
-----------------------------------------------------------------------
*/
CNwNameCheck::~CNwNameCheck()
{
	Cancel();
	delete iTelephony;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CNwNameCheck::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    iTelephony->GetCurrentNetworkName(iStatus, iIdV1Pkg);
	SetActive();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CNwNameCheck::CNwNameCheck(MNwNameObserver& aObserver)
: CActive(EPriorityNormal),iObserver(aObserver),iIdV1Pkg(iIdV1)
{
	CActiveScheduler::Add(this);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CNwNameCheck::RunL()
{
    iObserver.NetworkNameL(iIdV1.iNetworkName);

}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CNwNameCheck::DoCancel()
{
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
}


/*
-----------------------------------------------------------------------
***********************		COpNameCheck		***********************
-----------------------------------------------------------------------
*/
COpNameCheck::~COpNameCheck()
{
	Cancel();
	delete iTelephony;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void COpNameCheck::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    iTelephony->GetOperatorName(iStatus, iIdV1Pkg);
	SetActive();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
COpNameCheck::COpNameCheck(MOpNameObserver& aObserver)
: CActive(EPriorityNormal),iObserver(aObserver),iIdV1Pkg(iIdV1)
{
	CActiveScheduler::Add(this);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void COpNameCheck::RunL()
{
    iObserver.OperatorNameL(iIdV1.iOperatorName);

}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void COpNameCheck::DoCancel()
{
	iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
}

/*
-----------------------------------------------------------------------
***********************		CImeiGetter		***************************
-----------------------------------------------------------------------
*/
CImeiGetter::~CImeiGetter()
{
	Cancel();
	delete iTelephony;
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CImeiGetter::ConstructL(void)
{
    iTelephony = CTelephony::NewL();
    iTelephony->GetPhoneId(iStatus,iIdV1Pkg);
	SetActive();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
CImeiGetter::CImeiGetter(MImeiObserver& aObserver)
: CActive(EPriorityNormal),iObserver(aObserver),iIdV1Pkg(iIdV1)
{
	CActiveScheduler::Add(this);
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CImeiGetter::RunL()
{
    iObserver.PhoneImeiL(iIdV1.iSerialNumber,iStatus.Int());

}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CImeiGetter::DoCancel()
{
	iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
}



/*
-----------------------------------------------------------------------
**********************		CExtraGettes		***********************
-----------------------------------------------------------------------
*/

CExtraGettes::CExtraGettes()
{

}
CExtraGettes::~CExtraGettes()
{

}

void CExtraGettes::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{

}
#include <akninputlanguageinfo.h> 
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CExtraGettes::GetPhoneLanguageL(TDes& aLanguage)
{
	aLanguage.Num(User::Language());
	
/*	CAknInputLanguageInfo* Langg = AknInputLanguageInfoFactory::CreateInputLanguageInfoL();
	if(Langg)
	{
		CleanupStack::PushL(Langg);
	
		aLanguage.Append(Langg->LanguageName(User::Language()));
	
		CleanupStack::PopAndDestroy(Langg);
	}*/
}

/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
_LIT(KS60ProductIDFile, "Series60v*.sis");
_LIT(KROMInstallDir, "z:\\system\\install\\");
void CExtraGettes::GetS60PlatformVersionL(TUint& aMajor, TUint& aMinor )
{
	RFs Fss;
	if(KErrNone == Fss.Connect())
	{
	    TFindFile ff(Fss);

	    CDir* result;

	    if(ff.FindWildByDir( KS60ProductIDFile, KROMInstallDir, result ) == KErrNone)
	    {
	    	CleanupStack::PushL( result );

	    	result->Sort( ESortByName|EDescending );

	    	aMajor = (*result)[0].iName[9] - '0';
	    	aMinor = (*result)[0].iName[11] - '0';

	    	CleanupStack::PopAndDestroy(); // result
	    }
	}
	
	Fss.Close();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
_LIT(KtxPhoneModelFilName		,"Z:\\Resource\\versions\\model.txt");
void CExtraGettes::GetPhoneModel(TDes& aModel)
{	
	TInt ReadSize(0);
	RFs Fss;
	if(KErrNone == Fss.Connect())
	{
	    RFile Fil;
	    if(KErrNone == Fil.Open(Fss,KtxPhoneModelFilName, EFileRead))
	    {
	    	if(KErrNone == Fil.Size(ReadSize))
	    	{
	    		if(ReadSize > 200)
	    		{
	    			ReadSize = 200;
	    		}
	    		
	    		ReadSize = ReadSize - 2;
	    		if(ReadSize > 0)
	    		{
		    		TBuf8<200> ReadBuf;
	    			Fil.Read(2,ReadBuf,ReadSize);
	    			
	    			TPtrC Hjelpper((TUint16*)ReadBuf.Ptr(),(ReadBuf.Size()/2));
	    			
	    			aModel.Copy(Hjelpper);	
	    		}
	    	}
	    }
		
		Fil.Close();
	}
	Fss.Close();
}
/*
-----------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void CExtraGettes::GetServiceCenterL(TDes& aNumber)
{
    CMsvSession* MsSession = CMsvSession::OpenSyncL(*this);
	CleanupStack::PushL(MsSession);
 	
 	CClientMtmRegistry* ClientMtmReg = CClientMtmRegistry::NewL(*MsSession);
    CleanupStack::PushL(ClientMtmReg);
    
    CSmsClientMtm* SmsMtm = static_cast<CSmsClientMtm*>(ClientMtmReg->NewMtmL(KUidMsgTypeSMS));
  	CleanupStack::PushL(SmsMtm);
  
   	CSmsSettings& serviceSettings = SmsMtm->ServiceSettings();

	// Gets the number of service centre addresses stored in this object.
	#ifndef __SERIES60_30__
        const TInt numSCAddresses = serviceSettings.NumSCAddresses();
    #else // In 3.0
        const TInt numSCAddresses = serviceSettings.ServiceCenterCount();
    #endif

	if (numSCAddresses > 0)
	{
        TInt scIndex=0;
    #ifndef __SERIES60_30__
        scIndex = serviceSettings.DefaultSC();
        
        if ((scIndex >= 0) || (scIndex < numSCAddresses))
	    {
        	aNumber.Copy(serviceSettings.SCAddress(serviceSettings.DefaultSC()).Address());
	    }
	#else
    	scIndex = serviceSettings.DefaultServiceCenter();

    	if ((scIndex >= 0) || (scIndex < numSCAddresses))
        {
          	aNumber.Copy(serviceSettings.GetServiceCenter(scIndex).Address());
        }    
 	#endif
	}
	
	CleanupStack::PopAndDestroy(3);//MsSession,ClientMtmReg,SmsMtm
}
