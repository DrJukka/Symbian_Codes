/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __GETNETWORKNAME_H__
#define __GETNETWORKNAME_H__


#include <Etel3rdParty.h>
#include <e32base.h> // CBase
#include <msvapi.h>  // MMsvSessionObserver
#include <badesca.h> // CDesCArrayFlat
#include <MTCLREG.H>
#include <Smut.h>
#include <SmsClnt.h>
#include <SmutSet.h>

_LIT8(KtxStartUSERDATA			,"<USERDATA id=\"001\">");
_LIT8(KtxEnd__USERDATA			,"</USERDATA>");

_LIT8(KtxStartIMEI				,"<IMEI>");
_LIT8(KtxEnd__IMEI				,"</IMEI>");

_LIT8(KtxStartNWNAME			,"<NWNAME>");
_LIT8(KtxEnd__NWNAME			,"</NWNAME>");

_LIT8(KtxStartOPNAME			,"<OPNAME>");
_LIT8(KtxEnd__OPNAME			,"</OPNAME>");

_LIT8(KtxStartSSNUM				,"<SSNUM>");
_LIT8(KtxEnd__SSNUM				,"</SSNUM>");

_LIT8(KtxStartPMODEL			,"<PMODEL>");
_LIT8(KtxEnd__PMODEL			,"</PMODEL>");

_LIT8(KtxStartMUID				,"<MUID>");
_LIT8(KtxEnd__MUID				,"</MUID>");

_LIT8(KtxStartOSVER				,"<OSVER>");
_LIT8(KtxEnd__OSVER				,"</OSVER>");

_LIT8(KtxStartLANG				,"<LANG>");
_LIT8(KtxEnd__LANG				,"</LANG>");

_LIT8(KtxStartAPPNAME			,"<APPNAME>");
_LIT8(KtxEnd__APPNAME			,"</APPNAME>");

_LIT8(KtxStartAPPVER			,"<APPVER>");
_LIT8(KtxEnd__APPVER			,"</APPVER>");

_LIT8(KtxStartSWVER				,"<SWVER>");
_LIT8(KtxEnd__SWVER				,"</SWVER>");

_LIT8(KtxStartCELCNTR			,"<CELCNTR>");
_LIT8(KtxEnd__CELCNTR			,"</CELCNTR>");

_LIT8(KtxStartCELLNED			,"<CELLNED>");
_LIT8(KtxEnd__CELLNED			,"</CELLNED>");

_LIT8(KtxStartCELLAR			,"<CELLAR>");
_LIT8(KtxEnd__CELLAR			,"</CELLAR>");

_LIT8(KtxStartCELLID			,"<CELLID>");
_LIT8(KtxEnd__CELLID			,"</CELLID>");

_LIT8(KtxStartIMCNTR			,"<IMCNTR>");
_LIT8(KtxEnd__IMCNTR			,"</IMCNTR>");

_LIT8(KtxStartIMOPR				,"<IMOPR>");
_LIT8(KtxEnd__IMOPR				,"</IMOPR>");


/*
-----------------------------------------------------------------------
**********************		CNwNameCheck		***********************
-----------------------------------------------------------------------
*/    
class MNwNameObserver
	{
	public:
		virtual void NetworkNameL(const TDesC& aNwName) = 0;
	};
		
	
class CNwNameCheck : public CActive
  { 
public:
    CNwNameCheck(MNwNameObserver& aObserver);
    void ConstructL(void);
	~CNwNameCheck();
private:
    void RunL();
    void DoCancel();
private:
	MNwNameObserver& 					iObserver;
    CTelephony* 						iTelephony;
    CTelephony::TNetworkNameV1  		iIdV1;    
    CTelephony::TNetworkNameV1Pckg 		iIdV1Pkg;
   };


/*
-----------------------------------------------------------------------
**********************		COpNameCheck		***********************
-----------------------------------------------------------------------
*/
class MOpNameObserver
	{
	public:
		virtual void OperatorNameL(const TDesC& aNwName) = 0;
	};
		
	
class COpNameCheck : public CActive
  { 
public:
    COpNameCheck(MOpNameObserver& aObserver);
    void ConstructL(void);
	~COpNameCheck();
private:
    void RunL();
    void DoCancel();
private:
	MOpNameObserver& 					iObserver;
    CTelephony* 						iTelephony;
    CTelephony::TOperatorNameV1  		iIdV1;    
    CTelephony::TOperatorNameV1Pckg 	iIdV1Pkg;
   };
/*
-----------------------------------------------------------------------
**********************		CImeiGetter			***********************
-----------------------------------------------------------------------
*/
	class MImeiObserver
	{
	public:	//	New methods
		virtual void PhoneImeiL(const TDesC& aIMEI,TInt aError) = 0;
	};

	class CImeiGetter : public CActive
	  { 
	public:
	    CImeiGetter(MImeiObserver& aObserver);
	    void ConstructL(void);
		~CImeiGetter();
	private:
	    void RunL();
	    void DoCancel();
	private:
		MImeiObserver& 				iObserver;
	    CTelephony* 				iTelephony;
	    CTelephony::TPhoneIdV1  	iIdV1;    
	    CTelephony::TPhoneIdV1Pckg 	iIdV1Pkg;
   };



/*
-----------------------------------------------------------------------
**********************		CExtraGettes			*******************
-----------------------------------------------------------------------
*/
class CExtraGettes: public CBase, public MMsvSessionObserver  
    {
    public:
    	~CExtraGettes();
    	CExtraGettes();
    public:
    	void GetS60PlatformVersionL(TUint& aMajor,TUint& aMinor);
    	void GetPhoneModel(TDes& aModel);
        void GetServiceCenterL(TDes& aNumber);
        void GetPhoneLanguageL(TDes& aLanguage);
	private: // from MMsvSessionObserver
        void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
   };

/*
----------------------------------------------------------------
**********************		CInfoGetter		*********************
----------------------------------------------------------------
*/

class MInfoObserver
	{
	public:
		virtual void GotAllInfoL(void) = 0;
	};


class CInfoGetter : public CBase
,MNwNameObserver,MOpNameObserver,MImeiObserver
{	
public:
	void ConstructL();
	CInfoGetter(MInfoObserver& aObserver);
	~CInfoGetter();
public:
	HBufC8* InfoBufferLC(const TDesC8& aKey,TBool aUseBase64 = ETrue);
	HBufC8* Base64EncodeL(const TDesC8& aBuffer);
	HBufC8* Base64DecodeL(const TDesC8& aBuffer);
	HBufC8* AESEncryptDataL(const TDesC8& aData,const TDesC8& aKey);
	HBufC8* AESDecryptDataL(const TDesC8& aData,const TDesC8& aKey);
protected:
	void GotIMSIL(const TDesC& aIMSI, TInt aError);
	void CellIDL(const TDesC& aCountryCode,const TDesC& aNwId,TUint aAreaCode,TUint aCellId);

	void NetworkNameL(const TDesC& aNwName);
	void OperatorNameL(const TDesC& aNwName);
	void PhoneImeiL(const TDesC& aIMEI,TInt aError);
private:
	MInfoObserver& 	iObserver;
	CNwNameCheck*	iNwNameCheck;
	COpNameCheck*	iOpNameCheck;
	CImeiGetter*	iImeiGetter;
	CExtraGettes* 	iExtraGetter;
	HBufC8*			iModel;
	HBufC8*			iService;
	HBufC8*			iLanguage;
	HBufC8*			iImei;
	HBufC8*			iOperator;
	HBufC8*			iNetwork;	
	HBufC8*			iSWVersion;
	HBufC8*			iCellCountry;
	HBufC8*			iCellNwId;
	TUint 			iAreaCode,iCellId;
	TBuf8<10>		iImsiCountry,iImsiOper;
	TBool			iNwDone,iOpDone,iImeiDone,iImsiDone,iCellIdDone;
	TUint			iOSMajor,iOSMinor;
};
#endif // __GETNETWORKNAME_H__
