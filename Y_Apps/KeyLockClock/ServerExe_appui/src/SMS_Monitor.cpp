
#include "SMS_Monitor.h"

#include <COEMAIN.H>
#include <BAUTILS.H>
#include <apmrec.h> 
#include <apgcli.h>
#include <smut.h> 
#include <MmsConst.h>


/*
-------------------------------------------------------------------------------
****************************  CSMSMonitor   *************************************
-------------------------------------------------------------------------------
*/  

CSMSMonitor* CSMSMonitor::NewL(MInboxCallBack* aObserver)
	{
	CSMSMonitor* self = new(ELeave)CSMSMonitor(aObserver);
	self->ConstructL();
	return self;
	}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/  
CSMSMonitor::CSMSMonitor(MInboxCallBack* aObserver):iObserver(aObserver)
{
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CSMSMonitor::~CSMSMonitor()
{
	delete iMtmRegistry;
    iMtmRegistry = NULL;
    
	delete iSession;    
    iSession = NULL;
}
/*
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
*/
void CSMSMonitor::ConstructL(void)
{	
	iSession = CMsvSession::OpenSyncL(*this);
	iMtmRegistry = CClientMtmRegistry::NewL(*iSession);
	
	CheckInboxL();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CSMSMonitor::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* aArg2, TAny* /*aArg3*/)
    {
    switch (aEvent)
        {
        case EMsvEntriesChanged:
        case EMsvEntriesCreated:        // A new entry has been created in the message server
		case EMsvEntriesMoved:      // this event is given when message entries are moved
			{// We are interested in messages that are created in Inbox
	            TMsvId* entryId;
	            entryId = static_cast<TMsvId*>(aArg2);          // entry id from the session event
	            
	          	if ( *entryId == KMsvGlobalInBoxIndexEntryId )  // new entry has been created in Inbox folder
	            { 				
					CheckInboxL();
	            }
			}
			break;
        case EMsvCloseSession:
        case EMsvServerTerminated:	
            if(iSession)
            {
				delete iMtmRegistry;
                iMtmRegistry = NULL;
                
				iSession->CloseMessageServer();
				
				delete iSession;    
				iSession = NULL;
            }
            break;
        default:
            // All other events are ignored
            break;
        }
    }


/*
-----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void CSMSMonitor::CheckInboxL(void)
{	
	TInt smsCount(0),mmsCount(0);

	CMsvEntry* Entrie = iSession->GetEntryL(KMsvGlobalInBoxIndexEntryId);
	if(Entrie)
	{
		CleanupStack::PushL(Entrie);
		
		if(Entrie->Count())
		{
			TMsvSelectionOrdering MySortType;
			
			MySortType.SetSorting(EMsvSortByDate);
			
			Entrie->SetSortTypeL(MySortType);
			
			CMsvEntrySelection* entries = Entrie->ChildrenL();
			if(entries)
			{
				CleanupStack::PushL(entries);
		 
				for(TInt i = (entries->Count() - 1); i >= 0; i--)
				{
					if(Entrie->ChildDataL(entries->At(i)).Unread())
					{	
						if(Entrie->ChildDataL(entries->At(i)).iMtm == KUidMsgTypeSMS)
						{
							smsCount++;
						}
						else if(Entrie->ChildDataL(entries->At(i)).iMtm == KUidMsgTypeMultimedia)
						{
							mmsCount++;
						}
					}
				} 
			
				CleanupStack::PopAndDestroy(entries);
			}
		}
		
		CleanupStack::PopAndDestroy(1);//Entrie
	}
	
	if(iObserver){
		iObserver->InboxChangedL(smsCount,mmsCount);
	}
}


