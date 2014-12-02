/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __OBJECTEXCHANGECLIENT_H__
#define __OBJECTEXCHANGECLIENT_H__

// INCLUDES
#include <e32base.h>
#include <obex.h>

// CONSTANTS
#include "ObjectExchangeProtocolConstants.h"

// FORWARD DECLARATIONS
class CObjectExchangeServiceSearcher;
class MLog;
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
class CObjectExchangeClient : public CActive
    {
    public: // Constructors and destructor.
        static CObjectExchangeClient* NewL( MLog& aLog );
        static CObjectExchangeClient* NewLC( MLog& aLog );
        virtual ~CObjectExchangeClient();
        void ConnectL();
        void DisconnectL();
        void SendObjectL(const TDesC& aName);
        void StopL();
        TBool IsConnected();
        TBool IsBusy();
    protected:    // from CActive
        void DoCancel();
        void RunL();
    private:    // Constructors
        CObjectExchangeClient( MLog& aLog );
        void ConstructL();
        void ConnectToServerL();
    private:    // data
        enum TState
            {
                EWaitingToGetDevice,
                EGettingDevice,
                EGettingService,
                EGettingConnection,
                EWaitingToSend,
                EDisconnecting
            };

        TState iState;
        CObjectExchangeServiceSearcher* iServiceSearcher;
        CObexClient* iClient;
        CBufFlat* iBuffer;
        CObexBaseObject* iCurrObject;
        MLog& iLog;
        TBool iRealSis;
    };

#endif // __OBJECTEXCHANGECLIENT_H__

// End of File
