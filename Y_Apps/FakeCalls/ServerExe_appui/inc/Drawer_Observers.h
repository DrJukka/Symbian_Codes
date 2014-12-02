

#ifndef __DRAWER_OBSERVER_H__
#define __DRAWER_OBSERVER_H__


#include <e32std.h>
#include <w32std.h>
#include <coedef.h>
#include <apgwgnam.h>
#include <etel.h>
#include <mmsconst.h>
#include <coecntrl.h>
#include <cntdb.h>
#include <aknwseventobserver.h> 


class MKeyCallBack 
{
public:
	virtual TBool KeyCaptured(TWsEvent aEvent) = 0;
};

class CKeyObserver : public CActive 
{
public:
	static CKeyObserver* NewL(MKeyCallBack* aObserver,const TBool& aNoOn,const TBool& aYesOn,const TBool& aOffOn,const TBool& aCameraOn);
	static CKeyObserver* NewLC(MKeyCallBack* aObserver,const TBool& aiNoOn,const TBool& aiYesOn,const TBool& aOffOn,const TBool& aCameraOn);
	virtual ~CKeyObserver();
	void CancelCaptureKeys(void);
	void CaptureKeys(void);
private:
	CKeyObserver(MKeyCallBack* aObserver,const TBool& aiNoOn,const TBool& aiYesOn,const TBool& aOffOn,const TBool& aCameraOn);
	void ConstructL();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void Listen();
private:
	MKeyCallBack* 	iObserver;
	RWsSession      iWsSession;
	RWindowGroup    iWg; 
	TInt 			iHandle1,iHandle2,iHandle3,iHandle4;
	TBool			iNoOn,iYesOn,iOffOn,iCameraOn;
};

class MForegroundCallBack 
{
public:
	virtual TBool ForegroundChanged(TUid AppUid) = 0;
};




class CForegroundObserver : public CActive 
{
public:
	static CForegroundObserver* NewL(MForegroundCallBack* aObserver);
	static CForegroundObserver* NewLC(MForegroundCallBack* aObserver);
	virtual ~CForegroundObserver();
private:
	CForegroundObserver(MForegroundCallBack* aObserver);
	void ConstructL();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void Listen();
private:
	MForegroundCallBack* 	iObserver;
	RWsSession	     		iWsSession;
	RWindowGroup    		iWg; 
};




#endif //__DRAWER_OBSERVER_H__
