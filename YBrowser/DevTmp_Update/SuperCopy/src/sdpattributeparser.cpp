/* Copyright (c) 2004, Nokia. All rights reserved */


// INCLUDE FILES
#include <bt_sock.h>

#include "SdpAttributeParser.h"
#include "SdpAttributeNotifier.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeParser()
// Construct a TSdpAttributeParser.
// ----------------------------------------------------------------------------
//
TSdpAttributeParser::TSdpAttributeParser( TSdpAttributeList& aNodeList, 
                                          MSdpAttributeNotifier& aObserver ) 
:   iObserver( aObserver ),
    iNodeList( aNodeList ),
    iCurrentNodeIndex( 0 )
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::HasFinished()
// Check if parsing processed the whole list.
// ----------------------------------------------------------------------------
//
TBool TSdpAttributeParser::HasFinished() const
    {
    return CurrentNode().iCommand == EFinished;
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::VisitAttributeValueL()
// Process a data element.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::VisitAttributeValueL( CSdpAttrValue& aValue, 
                                                TSdpElementType aType )
    {
    switch ( CurrentNode().iCommand )
        {
        case ECheckType:
            CheckTypeL( aType );
            break;

        case ECheckValue:
            CheckTypeL( aType );
            CheckValueL( aValue );
            break;

        case ECheckEnd:
            User::Leave( KErrGeneral ); //list element contains too many items
            break;

        case ESkip:
            break;  // no checking required

        case EReadValue:
            CheckTypeL( aType );
            ReadValueL( aValue );
            break;

        case EFinished:
            User::Leave( KErrGeneral ); // element is after 
            return;                     // value should have ended

        default:
			break;
        };

    AdvanceL();
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::StartListL()
// Process the start of a data element list.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::StartListL( CSdpAttrValueList& /*aList*/ )
    {
    // no checks done here
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::EndListL()
// Process the end of a data element list.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::EndListL()
    {
    // check we are at the end of a list
    if ( CurrentNode().iCommand != ECheckEnd )
        {
        User::Leave( KErrGeneral );
        }

    AdvanceL();
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::CheckTypeL()
// Check the type of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::CheckTypeL( TSdpElementType aElementType ) const
    {
    if ( CurrentNode().iType != aElementType )
        {
        User::Leave( KErrGeneral );
        }
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::CheckValueL()
// Check the value of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::CheckValueL( CSdpAttrValue& aValue ) const
    {
    switch ( aValue.Type() )
        {
        case ETypeNil:
          //  Panic( ESdpAttributeParserNoValue );
            break;

        case ETypeUint:
            if ( aValue.Uint() != ( TUint )CurrentNode().iValue )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeInt:
            if ( aValue.Int() != CurrentNode().iValue )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeBoolean:
            if ( aValue.Bool() != CurrentNode().iValue )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeUUID:
            if ( aValue.UUID() != TUUID( CurrentNode().iValue ) )
                {
                User::Leave( KErrArgument );
                }
            break;

        // these are lists, so have to check contents
        case ETypeDES:
        case ETypeDEA:
           // Panic( ESdpAttributeParserValueIsList );
            break;

        // these aren't supported - use EReadValue and leave on error
        //case ETypeString:
        //case ETypeURL:
        //case ETypeEncoded:
        default:
         //   Panic( ESdpAttributeParserValueTypeUnsupported );
            break;
        }
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::ReadValueL()
// Pass the data element value to the observer.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::ReadValueL( CSdpAttrValue& aValue ) const
    {
    iObserver.FoundElementL( CurrentNode().iValue, aValue );
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::CurrentNode()
// Get the current node.
// ----------------------------------------------------------------------------
//
const TSdpAttributeParser::SSdpAttributeNode& TSdpAttributeParser
::CurrentNode() const
    {
    return  iNodeList[iCurrentNodeIndex];
    }

// ----------------------------------------------------------------------------
// TSdpAttributeParser::AdvanceL()
// Advance to the next node.
// ----------------------------------------------------------------------------
//
void TSdpAttributeParser::AdvanceL()
    {
    // check not at end
    if ( CurrentNode().iCommand == EFinished )
        {
        User::Leave( KErrEof );
        }

    // move to the next item
    ++iCurrentNodeIndex;
    }

// End of File
