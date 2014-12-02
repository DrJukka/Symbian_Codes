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
	CTimeOutTimer*		iTimeOutTimer;
	TBool				iStop;
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
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CStrrrrterView::ConstructL()
{
	iStop = EFalse;
	iTimeOutTimer = CTimeOutTimer::NewL(CActive::EPriorityStandard,*this);
	iTimeOutTimer->After(5000000);
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CStrrrrterView::TimerExpired() 
{	
	if(iStop)
	{
		CActiveScheduler::Stop();  	
	}
	else
	{
		RExampleServerClient Serrrverrr;
		Serrrverrr.Connect();
		Serrrverrr.ReReadValuesNow();
		Serrrverrr.Close();
		
		iStop = ETrue;
		iTimeOutTimer->After(30000000);
	}
}
/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

static void DoMainL() 
{
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	
	CStrrrrterView* view = new(ELeave)CStrrrrterView();
	CleanupStack::PushL(view);
	view->ConstructL();

	
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2);  //s & view
}

/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

GLDEF_C TInt E32Main() 
{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TRAPD(error, DoMainL());

    delete cleanup;
    __UHEAP_MARKEND;
    return 0;
}
