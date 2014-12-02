/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __OBJECTEXCHANGESERVICESEARCHER_H__
#define __OBJECTEXCHANGESERVICESEARCHER_H__

// INCLUDES
#include <e32base.h>

#include "BTServiceSearcher.h"
#include "ObjectExchangeProtocolConstants.h"
#include "Log.h"

// CLASS DECLARATIONS
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
class CObjectExchangeServiceSearcher : public CBTServiceSearcher
    {
    public: // Constructors and destructor.
        static CObjectExchangeServiceSearcher* NewL( MLog& aLog );
        static CObjectExchangeServiceSearcher* NewLC( MLog& aLog );
        virtual ~CObjectExchangeServiceSearcher();
        TInt Port();
    protected:  // Functions from base classes.
        const TUUID& ServiceClass() const;
        const TSdpAttributeParser::TSdpAttributeList& ProtocolList() const;
        virtual void FoundElementL( TInt aKey, const CSdpAttrValue& aValue );
    private:    // Constructors
        CObjectExchangeServiceSearcher( MLog& aLog );
        void ConstructL();
    private:    // data
        TUUID iServiceClass;
        TInt iPort;
    };

#endif // __OBJECTEXCHANGESERVICESEARCHER_H__

// End of File
