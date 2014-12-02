
#ifndef __PHONECALL_CALLOBSERVER_H__
#define __PHONECALL_CALLOBSERVER_H__

#include <e32base.h>
#include <Etel3rdParty.h> 


class MCallObsCallBack
{
public:
	virtual void NotifyChangeInCallStatusL(CTelephony::TCallStatus& aStatus, TInt aError)=0;
};

class CMyCallObserver : public CActive
  {
public: // public constructors & destructor
  	static CMyCallObserver* NewLC(MCallObsCallBack &aCallBack);
  	static CMyCallObserver* NewL(MCallObsCallBack &aCallBack);
  	~CMyCallObserver();
protected:// from CActive
	void DoCancel();
  	void RunL();
private: // private constructors
  	void ConstructL();
  	CMyCallObserver(MCallObsCallBack &aCallBack);
	// private internal functions
	void StartListeningForEvents();
private:
	MCallObsCallBack&				iCallBack;
  	CTelephony* 					iTelephony;
  	CTelephony::TCallStatusV1 		iCallStatus;
 	CTelephony::TCallStatusV1Pckg 	iCallStatusPckg;
};

#endif //__PHONECALL_CALLOBSERVER_H__

