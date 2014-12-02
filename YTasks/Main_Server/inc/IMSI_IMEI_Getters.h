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

/*
-----------------------------------------------------------------------------
*****************************  CImsiReader 	*********************************
-----------------------------------------------------------------------------
*/
	class MImsiObserver
	{
	public:	//	New methods
		virtual void GotIMSIL(const TDesC& aIMSI, TInt aError) = 0;
	};

	class CImsiReader : public CActive
	    {
	public:
		static CImsiReader* NewL(MImsiObserver* aObserver);
		static CImsiReader* NewLC(MImsiObserver* aObserver);
		~CImsiReader();
	protected:	
		void DoCancel();
		void RunL();
	private:
		CImsiReader(MImsiObserver* aObserver);
	    void ConstructL(void);
	private:
		MImsiObserver* 					iObserver;
		CTelephony* 					iTelephony;
		CTelephony::TSubscriberIdV1 	iImsiV1;    
    	CTelephony::TSubscriberIdV1Pckg iImsiV1Pkg;
	    };
/*
-----------------------------------------------------------------------------
*****************************  CCellIDCheck *********************************
-----------------------------------------------------------------------------
*/
class MCellIdObserver
	{
	public:
		virtual void CellIDL(const TDesC& aCountryCode,const TDesC& aNwId,TUint aAreaCode,TUint aCellId) = 0;
	};
			
class CCellIDCheck : public CActive
  { 
public:
    CCellIDCheck(MCellIdObserver& aObserver);
    void ConstructL(void);
	~CCellIDCheck();
private:
    void RunL();
    void DoCancel();
private:
	MCellIdObserver& 				iObserver;
    CTelephony* 					iTelephony;
    CTelephony::TNetworkInfoV1  	iIdV1;    
    CTelephony::TNetworkInfoV1Pckg 	iIdV1Pkg;
   };   

#endif
