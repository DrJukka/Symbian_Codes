/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen. All rights reserved */

#include <e32base.h>
#include <bitdev.h>
#include <f32file.h>
#include <w32std.h>
#include <coeutils.h>
#include <coemain.h>
#include <bautils.h>
#include <aknnotewrappers.h>
#include <apgcli.h> 
#include <e32cons.h>

#include "common.h"
#include "CTimeOutTimer.h"

#include "clientservercommon.h"
#include "ExampleServerSession.h"

class CStrrrrterView : public CBase, MTimeOutNotify
{
public:
	~CStrrrrterView();
	CStrrrrterView();
	void ConstructL();
protected:
	void TimerExpired();
private:
	CTimeOutTimer*					iTimeOutTimer;
	RFs		iFsSession;
	RFile	iFile;
};

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CStrrrrterView::CStrrrrterView()
{
}
/*
-----------------------------------------------------------------------------	
-----------------------------------------------------------------------------
*/
CStrrrrterView::~CStrrrrterView()
{
	if(iTimeOutTimer)
	{
		iTimeOutTimer->Cancel();
	}
	
	delete iTimeOutTimer;
	iTimeOutTimer = NULL;
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("Bye, bye, "));	
		iFile.Close();
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CStrrrrterView::ConstructL()
{
/*	iFsSession.Connect();
		
		_LIT(KRecFilename			,"C:\\FakeStarter.txt");
		iFsSession.Delete(KRecFilename);
		
		iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny);	
		
		if(iFile.SubSessionHandle())
		{	
			iFile.Write(_L8("start, \n\rs"));
		}*/
		
	iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
	iTimeOutTimer->After(5000000);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CStrrrrterView::TimerExpired() 
{	
	if(iFile.SubSessionHandle())
	{
		iFile.Write(_L8("timer done, starting\n\r"));	
	}
	
	RExampleServerClient Serrrverrr;
	Serrrverrr.Connect();
	Serrrverrr.ReReadValuesNow();
	Serrrverrr.Close();
		
	CActiveScheduler::Stop();  
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
static void MainL() 
{		
	// create and install the active scheduler we need
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	CStrrrrterView* apppp = new(ELeave)CStrrrrterView();
	CleanupStack::PushL(apppp);
	apppp->ConstructL();
	
	RProcess::Rendezvous(KErrNone);

	// Ready to run
	CActiveScheduler::Start();
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);	
	
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

GLDEF_C TInt E32Main() 
{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD(error, MainL());

    delete cleanup;
    __UHEAP_MARKEND;
    return 0;
}
