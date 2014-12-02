/* 
	Copyright (c) 2006-07, 
	Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __CRASHMONITOR_H__
#define __CRASHMONITOR_H__

#include<e32const.h>
#include<e32cmn.h>
#include<e32base.h>

	class MExitMonitorCallBack
	{
	public:
		virtual void HandleExit(const TDesC& aName,const TExitType&  aExitType,const TInt& aExitReason,const TDesC& aExitCategory,const TDesC& aProcess,const TTime& aTime) = 0;
	};

	class CExitMonitor : public CActive
	{
	public:
	    static CExitMonitor* NewL(MExitMonitorCallBack* aCallBack);
	    static CExitMonitor* NewLC(MExitMonitorCallBack* aCallBack);
	    ~CExitMonitor();
	    void StartTheReadL(void);
	protected:
		void DoCancel();
		void RunL();
	private:
	    CExitMonitor(MExitMonitorCallBack* aCallBack);
	    void ConstructL(void);
	private:
		MExitMonitorCallBack* 	iCallBack;
		RUndertaker 			iUndertaker;
		TInt 					iHandle;
	};



#endif // __CRASHMONITOR_H__
