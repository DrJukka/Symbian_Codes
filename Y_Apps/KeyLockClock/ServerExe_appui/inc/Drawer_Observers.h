

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


class MLayoutChnageCallBack 
{
public:
	virtual void DisableScreen(TInt aDelay) = 0;
	virtual TBool IsKeyBoardLocked() = 0;
};

class MKeyCallBack 
{
public:
	virtual TBool KeyCaptured(TWsEvent aEvent) = 0;
};

class CKeyObserver : public CActive 
{
public:
	static CKeyObserver* NewL(MKeyCallBack* aObserver);
	static CKeyObserver* NewLC(MKeyCallBack* aObserver);
	virtual ~CKeyObserver();
	void CancelCaptureKeys(void);
	void CaptureKeys(void);
private:
	CKeyObserver(MKeyCallBack* aObserver);
	void ConstructL();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void Listen();
private:
	MKeyCallBack* 	iObserver;
	RWsSession      iWsSession;
	RWindowGroup    iWg; 
	TInt 			iHandle1,iHandle2,iHandle3,iHandle4,iHandle5;
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
