/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __SDP_ATTRIBUTE_NOTIFIER_H__
#define __SDP_ATTRIBUTE_NOTIFIER_H__

// FORWARD DECLARATIONS
class CSdpAttrValue;

// CLASS DECLARATIONS

/**
* MSdpAttributeNotifier
* An instance of MSdpAttributeNotifier is used to read selected SDP Attribute
* elements from an attribute value.
*/
class MSdpAttributeNotifier
    {
    public:     // Member function
        /**
        * FoundElementL()
        * Read the data element
        * @param aKey a key that identifies the element
        * @param aValue the data element
        */
        virtual void FoundElementL( TInt aKey, const CSdpAttrValue& aValue ) = 0;
    };

#endif // __SDP_ATTRIBUTE_NOTIFIER_H__

// End of File
