/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen, All rights reserved */

#ifndef __SMSMM_SENDERRR_H__
#define __SMSMM_SENDERRR_H__


#include <e32base.h>
#include <msvapi.h>
#include <GSMUELEM.H>


class CSmsAppUi;
class CClientMtmRegistry;
class CSmsClientMtm;

class MMsvObserver
    {
public:
    enum TStatus
        {
        ECreated,
        EMovedToOutBox,
        EScheduledForSend,
        ESent,
        EDeleted,
        EScheduled,
        EGotReply
        };

    enum TError
        {
        EScheduleFailed,
        ESendFailed,
        ENoServiceCentre,
        EFatalServerError
        };
    virtual void HandleStatusChange(MMsvObserver::TStatus aStatus) = 0;
    virtual void HandleError(MMsvObserver::TError aError) = 0;
};

//CSMSSender, SendMsgL(HBufC* aMessage,HBufC* aRecipients,TBool aUnicodeEnabled);

class CSMSSender : public CActive, public MMsvSessionObserver
    {
    public: // Constructors and destructor
        static CSMSSender* NewL(MMsvObserver& aObserver);
        static CSMSSender* NewLC(MMsvObserver& aObserver);
        virtual ~CSMSSender();
    public: // New functions
       TBool SendMsgL(HBufC* aMessage,HBufC* aRecipients,TBool aUnicodeEnabled);
    public: // Functions from base classes
        void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1,TAny* aArg2, TAny* aArg3 );
    protected: // Functions from base classes
        void DoCancel();
        void RunL();
    private: // Constructors
    	CSMSSender(MMsvObserver& aObserver);
        void ConstructL();
    private: // New functions
	    TBool CreateMsgL();
        void ScheduleL();
        TBool ValidateL();
    private: // Enumeration
        enum TState
            {
            EWaitingForMoving = 1,
            EWaitingForScheduling
            };

    private: // Data
    	MMsvObserver& 		iObserver;
        TState iState;
        CMsvSession* 		iSession;
        CClientMtmRegistry* iMtmRegistry;
        CSmsClientMtm* 		iSmsMtm;
        CMsvOperation* 		iOperation;
		TBuf<100> 			iRecipientNumber;
		TBuf<200> 			iMessageText;
		TBool 				iUnicodeEnabled;
		TSmsDataCodingScheme::TSmsAlphabet 	iCharSet;
    };
	

#endif // __SMSMM_SENDERRR_H__
