/* Copyright (c) 2001 - 2009, Dr. Jukka Silvennoinen, All rights reserved */

#ifndef __SMSMM_SENDERRR_H__
#define __SMSMM_SENDERRR_H__


#include <e32base.h>
#include <msvapi.h>
#include <GSMUELEM.H>

class CClientMtmRegistry;

class MMsvObserver
    {
public:
    enum TStatus
        {
        ECreated = 15,
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
    virtual void HandleError(const TInt& aError) = 0;
};





class CSMSSender : public CActive, public MMsvSessionObserver
    {
public:
    static CSMSSender* NewL(MMsvObserver& aObserver);
    static CSMSSender* NewLC(MMsvObserver& aObserver);
    ~CSMSSender();
	void SendMsgL(const TDesC& ,const TDesC& aRecipients,TBool aUnicodeEnabled,TBool aFlashSMS);
public: // from MMsvSessionObserver
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
protected: // from CActive
    void RunL();
    void DoCancel();
protected:
    CSMSSender(MMsvObserver& aObservers);
private:
	void DeleteEntryL(TMsvEntry& aMsvEntry);
	void SetMtmEntryL(TMsvId aEntryId);
    void ConstructL();
    void CreateNewMessageL();
    TBool SetupSmsHeaderL();
    void PopulateMessageL(TMsvEntry& aMsvEntry,TDes& aDescription,TDes& aDetails);
    TBool InitializeMessageL();
    TBool MoveMessageEntryL(TMsvId aTarget);
    void SetScheduledSendingStateL(CMsvEntrySelection& aSelection);
    void HandleChangedEntryL(TMsvId aEntryId);
private:
    enum TPhase
        {
        EIdle,
        EWaitingForCreate,
        EWaitingForMove,
        EWaitingForScheduled,
        EWaitingForSent,
        EWaitingForDeleted
        };

    // Member variables
    TPhase                          	iPhase;
    CMsvOperation*  					iOperation;
    CMsvSession*        				iSession;
    CBaseMtm*           				iMtm;
    CClientMtmRegistry* 				iMtmRegistry;
    MMsvObserver&       				iObserver;
    HBufC*								iDetails;
    HBufC*								iDescription;
	TBuf<160> 							iMessage,iRecipients;
	TBool 								iUnicodeEnabled,iFlashSMS;
	TSmsDataCodingScheme::TSmsAlphabet 	iCharSet;
	TBool								iMessageMade,iMessageSent,iMessageReceived;
    };
	

#endif // __SMSMM_SENDERRR_H__
