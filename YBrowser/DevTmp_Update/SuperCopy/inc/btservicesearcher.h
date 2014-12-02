/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __BTSERVICESEARCHER_H__
#define __BTSERVICESEARCHER_H__

// INCLUDES
#include <e32base.h>
#include <bttypes.h>
#include <btextnotifiers.h>

#include "SdpAttributeParser.h"
#include "SdpAttributeNotifier.h"
#include "Log.h"

// CONSTANTS
_LIT(KErrNRRCErr,"NRRC ERR");

/**
* CBTServiceSearcher
* Searches for a service on a remote machine
*/

class CBTServiceSearcher : public CBase,
                           public MSdpAgentNotifier,
                           public MSdpAttributeNotifier
    {
    public: // Constructors and destructor

        virtual ~CBTServiceSearcher();
        void SelectDeviceByDiscoveryL(TRequestStatus& aObserverRequestStatus );
        void FindServiceL( TRequestStatus& aObserverRequestStatus );
        const TBTDevAddr& BTDevAddr();
        const TBTDeviceResponseParams& ResponseParams();
    protected:
        CBTServiceSearcher( MLog& aLog );
        virtual void Finished( TInt aError = KErrNone );
        virtual TBool HasFinishedSearching() const;
        TBool HasFoundService() const;
    protected: // abstract methods
        virtual const TSdpAttributeParser::TSdpAttributeList& ProtocolList() const = 0;
        virtual const TUUID& ServiceClass() const = 0;
        virtual void FoundElementL( TInt aKey,const CSdpAttrValue& aValue ) = 0;
    public: // from MSdpAgentNotifier
        void NextRecordRequestComplete( TInt aError,TSdpServRecordHandle aHandle, TInt aTotalRecordsCount );
        void AttributeRequestResult( TSdpServRecordHandle aHandle,TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue );
        void AttributeRequestComplete( TSdpServRecordHandle aHandle,TInt aError );
    private:
        void NextRecordRequestCompleteL( TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount );
        void AttributeRequestResultL( TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue );
        void AttributeRequestCompleteL( TSdpServRecordHandle,TInt aError );
    private:    // data

        TRequestStatus* iStatusObserver;
        TBool iIsDeviceSelectorConnected;
        RNotifier iDeviceSelector;
        TBTDeviceResponseParamsPckg iResponse;
        CSdpAgent* iAgent;
        CSdpSearchPattern* iSdpSearchPattern;
        TBool iHasFoundService;
        MLog& iLog;
    };

#endif // __BTSERVICESEARCHER_H__

// End of File
