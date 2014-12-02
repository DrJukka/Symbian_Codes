#ifndef __PHONEEXAMPLE_SIGNALOBS_H__
#define __PHONEEXAMPLE_SIGNALOBS_H__

#include <e32base.h>
#include <Etel3rdParty.h>


class MSignalObsCallBack
	{
	public:
		virtual void SignalStatus(TInt32 aStrength,TInt8 aBars) = 0;
	};
		
	
class CMySignalObserver : public CActive
  { 
public:// public constructors
  	static CMySignalObserver* NewLC(MSignalObsCallBack &aCallBack);
  	static CMySignalObserver* NewL(MSignalObsCallBack &aCallBack);
	~CMySignalObserver();
private:// from CActive
    void RunL();
    void DoCancel();
private:// private constructors
	CMySignalObserver(MSignalObsCallBack& aCallBack);
    void ConstructL(void);
private:
	MSignalObsCallBack& 				iCallBack;
    CTelephony* 						iTelephony;
    CTelephony::TSignalStrengthV1 		iSigStrengthV1;
    CTelephony::TSignalStrengthV1Pckg 	iSigStrengthV1Pckg;
    TBool								iMonitoring;
   };

#endif //__PHONEEXAMPLE_SIGNALOBS_H__

