/* Copyright (c) 2004, Nokia. All rights reserved */


#ifndef __SDP_ATTRIBUTE_PARSER_H__
#define __SDP_ATTRIBUTE_PARSER_H__

// INCLUDES
#include <btsdp.h>
#include "StaticArrayC.h"

// FORWARD DECLARATIONS
class MSdpAttributeNotifier;

// CLASS DECLARATIONS
/**
* TSdpAttributeParser
* An instance of TSdpAttributeParser is used to check an SDP 
* attribute value, and read selected parts
*/
class TSdpAttributeParser : public MSdpAttributeValueVisitor 
    {
    public:

        /**
        * TNodeCommand
        * The command to be carried out at a node
        * ECheckType. Check the type of the value
        * ECheckValue. Check the type and the value
        * ECheckEnd. Check that a list ends at this point
        * ESkip. Do not check this value - can not match a list end
        * EReadValue. Pass the value onto the observer
        * EFinished. Marks the end of the node list
        */
        enum TNodeCommand
            {
                ECheckType,     
                ECheckValue,  
                ECheckEnd,      
                ESkip,        
                EReadValue,    
                EFinished
            };

        /**
        * SSdpAttributeNode
        * An instance of SSdpAttributeNode is used to determine how
        * to parse an SDP attribute value data element
        */
        struct SSdpAttributeNode
        {
            /** iCommand the command for the node */
            TNodeCommand iCommand;

            /** iType the expected type */
            TSdpElementType iType;

            /** iValue the expected value for ECheckValue,the value of aKey
            * passed to the observer for EReadValue 
            */
            TInt iValue;    
        };

        typedef const TStaticArrayC< SSdpAttributeNode > TSdpAttributeList;

        /**
        * TSdpAttributeParser
        * Construct a TSdpAttributeParser
        * @param aNodeList the list of expected nodes 
        * @param aObserver an observer to read specified node values
        */
        TSdpAttributeParser( TSdpAttributeList& aNodeList, 
                             MSdpAttributeNotifier& aObserver );

        /**
        * HasFinished
        * Check if parsing processed the whole list
        * @return true is the index refers to the EFinished node
        */
        TBool HasFinished() const;

    public: // from MSdpAttributeValueVisitor

        /**
        * VisitAttributeValueL()
        * Process a data element
        * @param aValue the data element 
        * @param aType the type of the data element
        */
        void VisitAttributeValueL( CSdpAttrValue& aValue, 
                                   TSdpElementType aType );

        /**
        * StartListL()
        * Process the start of a data element list
        * @param aList the data element list 
        */
        void StartListL( CSdpAttrValueList& aList );

        /**
        * EndListL()
        * Process the end of a data element list
        */
        void EndListL();

    private:    // Functions from base classes

        /**
        * CheckTypeL()
        * Check the type of the current node is the same as 
        * the specified type.
        * @param aElementType the type of the current data element
        */
        void CheckTypeL( TSdpElementType aElementType ) const;

        /**
        * CheckValueL()
        * Check value of the current node is the same as the specified value
        * @param aValue the value of the current data element.
        */
        void CheckValueL( CSdpAttrValue& aValue ) const;

        /**
        * ReadValueL()
        * Pass the data element value to the observer
        * @param aValue the value of the current data element.
        */
        void ReadValueL( CSdpAttrValue& aValue ) const;

        /**
        * CurrentNode()
        * Get the current node
        * @return the current node
        */
        const SSdpAttributeNode& CurrentNode() const;

        /**
        * AdvanceL()
        * Advance to the next node. Leaves with KErrEof 
        * if at the finished node.
        */
        void AdvanceL();

    private:    // data

        /** iObserver the observer to read values */
        MSdpAttributeNotifier& iObserver;

        /** iNodeList a list defining the expected 
        * structure of the value 
        */
        TSdpAttributeList& iNodeList;

        /** iCurrentNodeIndex the index of the current node in iNodeList */
        TInt iCurrentNodeIndex;
    };

#endif // __SDP_ATTRIBUTE_PARSER_H__

// End of File
