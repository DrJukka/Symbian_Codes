/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Signal pane icon control.
*
*/


// INCLUDE FILES

#include <AknsDrawUtils.h>
#include <eikenv.h>
#include <akniconconfig.h>
#include <avkon.mbg>

#include <AknTasHook.h>
#include <AvkonInternalCRKeys.h>
#include <centralrepository.h> 

#include "AknSignalIcon.h"
#include "aknconsts.h"
#include "AknUtils.h"
#include "AknStatuspaneUtils.h"
#include "aknappui.h"


// ---------------------------------------------------------------------------
// CAknSignalIcon::~CAknSignalIcon
// Destructor
// ---------------------------------------------------------------------------
//
CAknSignalIcon::~CAknSignalIcon()
	{
	AKNTASHOOK_REMOVE();
    AknsUtils::DeregisterControlPosition( this );
    delete iSignalIcon;
    delete iSignalIconMask;
	}


// ---------------------------------------------------------------------------
// CAknSignalIcon::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAknSignalIcon* CAknSignalIcon::NewL()
	{
	CAknSignalIcon* self = CAknSignalIcon::NewLC();
	CleanupStack::Pop( self );
	return self;
	}


// ---------------------------------------------------------------------------
// CAknSignalIcon::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAknSignalIcon* CAknSignalIcon::NewLC()
	{
	CAknSignalIcon* self = new (ELeave) CAknSignalIcon;
	CleanupStack::PushL( self );
	self->ConstructL();
	AKNTASHOOK_ADDL( self, "CAknSignalIcon" );
	return self;
	}


// ---------------------------------------------------------------------------
// CAknSignalIcon::SetSignalIcon
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::SetSignalIcon( CFbsBitmap* aIcon )
    {
    iSignalIcon = aIcon;
    AknIconUtils::SetSize( iSignalIcon, Size() );
    }


// ---------------------------------------------------------------------------
// CAknSignalIcon::SetSignalIconMask
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::SetSignalIconMask( CFbsBitmap* aMask )
    {
    iSignalIconMask = aMask;
    }


// ---------------------------------------------------------------------------
// CAknSignalIcon::SetDrawBlank
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::SetDrawBlank( TBool aDrawBlank )
    {
    iDrawBlank = aDrawBlank;
    }


// ---------------------------------------------------------------------------
// CAknSignalIcon::DrawBlank
// ---------------------------------------------------------------------------
//
TBool CAknSignalIcon::DrawBlank()
    {
    return iDrawBlank;
    }


// ---------------------------------------------------------------------------
// CAknSignalIcon::SetOffLine
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::SetOffLine( TBool aOffLine )
    {
    TBool changedToOnline( !aOffLine && iOffLine );
    
    iOffLine = aOffLine;
    
    if ( changedToOnline )
        {
        // In offline mode the icon contains offline icon,
        // so a correct connection state icon must be re-loaded here.
        TRAP_IGNORE( LoadIconL( iIconState, iColorIndex, ETrue ) );
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalIcon::OffLine
// ---------------------------------------------------------------------------
//
TBool CAknSignalIcon::OffLine()
	{
	return iOffLine;	
	}


// ---------------------------------------------------------------------------
// CAknSignalIcon::SetColorIndex
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::SetColorIndex( TInt aColorIndex )
	{
	iColorIndex = aColorIndex;	
	}


// ---------------------------------------------------------------------------
// CAknSignalIcon::ColorIndex
// ---------------------------------------------------------------------------
//
TInt CAknSignalIcon::ColorIndex()
	{
	return iColorIndex;	
	}

void CAknSignalIcon::LoadATTIconL(TInt aIconState, 
                                  TInt aIconColorIndex )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CFbsBitmap* bitmap      = NULL;
    CFbsBitmap* mask        = NULL;
    
    switch ( aIconState )
        {
        case EAknSignalHsdpaIndicatorAvailable:
            AknsUtils::CreateColorIconL( skin,
                                         KAknsIIDQgnIndiSignalWcdmaIcon,
                                         KAknsIIDQsnIconColors,
                                         aIconColorIndex,
                                         bitmap,
                                         mask,
                                         AknIconUtils::AvkonIconFileName(), 
                                         EMbmAvkonQgn_indi_signal_wcdma_icon,
                                         EMbmAvkonQgn_indi_signal_wcdma_icon_mask,
                                         KRgbGray );
            break;
        case EAknSignalHsdpaIndicatorAttached:
        case EAknSignalHsdpaIndicatorEstablishingContext: 
            AknsUtils::CreateColorIconL( skin,
                                         KAknsIIDQgnIndiSignalWcdmaAttach,
                                         KAknsIIDQsnIconColors,
                                         aIconColorIndex,
                                         bitmap,
                                         mask, 
                                         AknIconUtils::AvkonIconFileName(), 
                                         EMbmAvkonQgn_indi_signal_wcdma_attach,
                                         EMbmAvkonQgn_indi_signal_wcdma_attach_mask,
                                         KRgbGray );
            break;
        case EAknSignalHsdpaIndicatorContext:
            AknsUtils::CreateColorIconL( skin,
                                         KAknsIIDQgnIndiSignalWcdmaContext,
                                         KAknsIIDQsnIconColors,
                                         aIconColorIndex,
                                         bitmap,
                                         mask, 
                                         AknIconUtils::AvkonIconFileName(), 
                                         EMbmAvkonQgn_indi_signal_wcdma_context,
                                         EMbmAvkonQgn_indi_signal_wcdma_context_mask,
                                         KRgbGray );
            break;
        case EAknSignalHsdpaIndicatorMultipdp:
            AknsUtils::CreateColorIconL( skin,
                                         KAknsIIDQgnIndiSignalWcdmaMultipdp,
                                         KAknsIIDQsnIconColors,
                                         aIconColorIndex,
                                         bitmap,
                                         mask,
                                         AknIconUtils::AvkonIconFileName(), 
                                         EMbmAvkonQgn_indi_signal_wcdma_multipdp,
                                         EMbmAvkonQgn_indi_signal_wcdma_multipdp_mask,
                                         KRgbGray );
            break;
        case EAknSignalHsdpaIndicatorSuspended:
            AknsUtils::CreateColorIconL( skin,
                                         KAknsIIDQgnIndiSignalWcdmaSuspended,
                                         KAknsIIDQsnIconColors,
                                         aIconColorIndex,
                                         bitmap,
                                         mask, 
                                         AknIconUtils::AvkonIconFileName(), 
                                         EMbmAvkonQgn_indi_signal_wcdma_suspended,
                                         EMbmAvkonQgn_indi_signal_wcdma_suspended_mask,
                                         KRgbGray ); 
            break;
        }
    if ( bitmap )
        {
        delete iSignalIcon;
        iSignalIcon = bitmap;
        AknIconUtils::SetSize( iSignalIcon, Size() );
        }

    if ( mask )
        {
        delete iSignalIconMask;
        iSignalIconMask = mask;
        }

    iIconState  = aIconState;
    iColorIndex = aIconColorIndex;
    }
// ---------------------------------------------------------------------------
// CAknSignalIcon::LoadIconL
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::LoadIconL( TInt aIconState,
                                TInt aIconColorIndex,
                                TBool aForceLoad )
    {
    if ( iIconState == aIconState &&
         aIconColorIndex == iColorIndex &&
         !aForceLoad )
        {
        // Icon already loaded.
        return;
        }
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CFbsBitmap* bitmap      = NULL;
    CFbsBitmap* mask        = NULL;

    TBool isHsdpa = ( aIconState >= EAknSignalHsdpaIndicatorAvailable
                    && aIconState <= EAknSignalHsdpaIndicatorMultipdp );
    if ( iOffLine )
        {
        // Offline mode is not an actual signal state in the signal icon
        // control, but it needs to be represented by an offline signal
        // icon in the universal indicator popup.
        AknsUtils::CreateColorIconL( skin,
                                     KAknsIIDNone,
                                     KAknsIIDQsnIconColors,
                                     aIconColorIndex,
                                     bitmap,
                                     mask, 
                                     AknIconUtils::AvkonIconFileName(), 
                                     EMbmAvkonQgn_indi_signal_offline,
                                     EMbmAvkonQgn_indi_signal_offline_mask,
                                     KRgbGray ); 
        }
    else
        {
        if( iATTEnable && isHsdpa)
            {
            LoadATTIconL( aIconState, aIconColorIndex );
            return;
            }
        switch( aIconState )
            {
            // Old legacy GPRS icons.
            case EAknSignalGprsIndicatorOff:
            case EAknSignalGprsIndicatorAvailable: // fallthrough
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnPropSignalIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(),
                                             EMbmAvkonQgn_prop_signal_icon,
                                             EMbmAvkonQgn_prop_signal_icon_mask,
                                             KRgbGray );
                break;
            case EAknSignalGprsIndicatorAttached:
            case EAknSignalGprsIndicatorEstablishingContext: // fallthrough
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalGprsContext,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(),
                                             EMbmAvkonQgn_indi_signal_gprs_context,
                                             EMbmAvkonQgn_indi_signal_gprs_context_mask,
                                             KRgbGray  );
                break;
            case EAknSignalGprsIndicatorContext:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalGprsContext,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(),
                                             EMbmAvkonQgn_indi_signal_gprs_context,
                                             EMbmAvkonQgn_indi_signal_gprs_context_mask,
                                             KRgbGray  );
                break;
            case EAknSignalGprsIndicatorSuspended:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalGprsSuspended,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(),
                                             EMbmAvkonQgn_indi_signal_gprs_suspended,
                                             EMbmAvkonQgn_indi_signal_gprs_suspended_mask,
                                             KRgbGray  );       
                break;
            case EAknSignalGprsIndicatorMultipdp:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalGprsMultipdp,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(),
                                             EMbmAvkonQgn_indi_signal_gprs_multipdp,
                                             EMbmAvkonQgn_indi_signal_gprs_multipdp_mask,
                                             KRgbGray );
                break;          
                
            // Create common packet data indicator bitmaps.
            case EAknSignalCommonPacketDataIndicatorOff:
            case EAknSignalCommonPacketDataIndicatorAvailable: // fallthrough
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnPropSignalIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_prop_signal_icon,
                                             EMbmAvkonQgn_prop_signal_icon_mask,
                                             KRgbGray );     
                break;
            case EAknSignalCommonPacketDataIndicatorAttached:
            case EAknSignalCommonPacketDataIndicatorEstablishingContext: // fallthrough
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalPdAttach,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_pd_attach,
                                             EMbmAvkonQgn_indi_signal_pd_attach_mask,
                                             KRgbGray );     
                break;
            case EAknSignalCommonPacketDataIndicatorContext:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalPdContext,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_pd_context,
                                             EMbmAvkonQgn_indi_signal_pd_context_mask,
                                             KRgbGray );     
                break;
            case EAknSignalCommonPacketDataIndicatorSuspended:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalPdSuspended,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_pd_suspended,
                                             EMbmAvkonQgn_indi_signal_pd_suspended_mask,
                                             KRgbGray );     
                break;
            case EAknSignalCommonPacketDataIndicatorMultipdp:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalPdMultipdp,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_pd_multipdp,
                                             EMbmAvkonQgn_indi_signal_pd_multipdp_mask,
                                             KRgbGray );          
                break;
                
            // Create EDGE packet data indicator bitmaps.
            case EAknSignalEdgeIndicatorOff:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnPropSignalIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_prop_signal_icon,
                                             EMbmAvkonQgn_prop_signal_icon_mask,
                                             KRgbGray );     
                break;
            case EAknSignalEdgeIndicatorAvailable:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnPropSignalIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_egprs_icon,
                                             EMbmAvkonQgn_indi_signal_egprs_icon_mask,
                                             KRgbGray );     
                break;
            case EAknSignalEdgeIndicatorAttached:
            case EAknSignalEdgeIndicatorEstablishingContext: // fallthrough
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalEgprsAttach,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_egprs_attach,
                                             EMbmAvkonQgn_indi_signal_egprs_attach_mask,
                                             KRgbGray );     
                break;
            case EAknSignalEdgeIndicatorContext:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalEgprsContext,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_egprs_context,
                                             EMbmAvkonQgn_indi_signal_egprs_context_mask,
                                             KRgbGray );     
                break;
            case EAknSignalEdgeIndicatorSuspended:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalEgprsSuspended,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_egprs_suspended,
                                             EMbmAvkonQgn_indi_signal_egprs_suspended_mask,
                                             KRgbGray );     
                break;
            case EAknSignalEdgeIndicatorMultipdp:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalEgprsMultipdp,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_egprs_multipdp,
                                             EMbmAvkonQgn_indi_signal_egprs_multipdp_mask,
                                             KRgbGray );          
                break;
            
            // Create WCDMA indicator bitmaps.
            case EAknSignalWcdmaIndicatorOff:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnPropSignalIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_prop_signal_icon,
                                             EMbmAvkonQgn_prop_signal_icon_mask,
                                             KRgbGray );         
                break;
            case EAknSignalWcdmaIndicatorAvailable:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalWcdmaIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_wcdma_icon,
                                             EMbmAvkonQgn_indi_signal_wcdma_icon_mask,
                                             KRgbGray );
                break;
            case EAknSignalWcdmaIndicatorAttached:
            case EAknSignalWcdmaIndicatorEstablishingContext: // fallthrough
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalWcdmaAttach,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_wcdma_attach,
                                             EMbmAvkonQgn_indi_signal_wcdma_attach_mask,
                                             KRgbGray );
                break;
            case EAknSignalWcdmaIndicatorContext:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalWcdmaContext,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_wcdma_context,
                                             EMbmAvkonQgn_indi_signal_wcdma_context_mask,
                                             KRgbGray );
                break;
            case EAknSignalWcdmaIndicatorMultipdp:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalWcdmaMultipdp,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_wcdma_multipdp,
                                             EMbmAvkonQgn_indi_signal_wcdma_multipdp_mask,
                                             KRgbGray );
                break;
            case EAknSignalWcdmaIndicatorSuspended:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalWcdmaSuspended,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_wcdma_suspended,
                                             EMbmAvkonQgn_indi_signal_wcdma_suspended_mask,
                                             KRgbGray ); 
                break;
            
            // Create HSDPA indicator bitmaps.
            case EAknSignalHsdpaIndicatorOff:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnPropSignalIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_prop_signal_icon,
                                             EMbmAvkonQgn_prop_signal_icon_mask,
                                             KRgbGray );         
                break;
            case EAknSignalHsdpaIndicatorAvailable:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalHsdpaIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(),
                                             EMbmAvkonQgn_indi_signal_hsdpa_icon,
                                             EMbmAvkonQgn_indi_signal_hsdpa_icon_mask,
                                             KRgbGray );         
                break;
            case EAknSignalHsdpaIndicatorAttached:
            case EAknSignalHsdpaIndicatorEstablishingContext: // fallthrough
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalHsdpaAttach,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_hsdpa_attach,
                                             EMbmAvkonQgn_indi_signal_hsdpa_attach_mask,
                                             KRgbGray );     
                break;
            case EAknSignalHsdpaIndicatorContext:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalHsdpaContext,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_hsdpa_context,
                                             EMbmAvkonQgn_indi_signal_hsdpa_context_mask,
                                             KRgbGray );     
                break;
            case EAknSignalHsdpaIndicatorSuspended:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalHsdpaSuspended,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_hsdpa_suspended,
                                             EMbmAvkonQgn_indi_signal_hsdpa_suspended_mask,
                                             KRgbGray );     
                break;
            case EAknSignalHsdpaIndicatorMultipdp:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnIndiSignalHsdpaMultipdp,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask, 
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_indi_signal_hsdpa_multipdp,
                                             EMbmAvkonQgn_indi_signal_hsdpa_multipdp_mask,
                                             KRgbGray );          
                break;        
    
            case EAknSignalUmaIndicatorOff:
                // default icon
                AknsUtils::CreateColorIconL(skin, KAknsIIDQgnPropSignalIcon,
                        KAknsIIDQsnIconColors, aIconColorIndex, bitmap, mask,
                        AknIconUtils::AvkonIconFileName(),
                        EMbmAvkonQgn_prop_signal_icon,
                        EMbmAvkonQgn_prop_signal_icon_mask, KRgbGray );
                break;

            case EAknSignalUmaIndicatorAvailable:
                AknsUtils::CreateColorIconL( skin,
                        KAknsIIDQgnIndiSignalUmaIcon,
                        KAknsIIDQsnIconColors, aIconColorIndex, bitmap, mask,
                        AknIconUtils::AvkonIconFileName(),
                        EMbmAvkonQgn_indi_signal_uma_icon,
                        EMbmAvkonQgn_indi_signal_uma_icon_mask,
                        KRgbGray );                         
                break;
                
            case EAknSignalUmaIndicatorAttached:
            case EAknSignalUmaIndicatorEstablishingContext:
                AknsUtils::CreateColorIconL(skin,
                        KAknsIIDQgnIndiSignalUmaAttach,
                        KAknsIIDQsnIconColors, aIconColorIndex, bitmap, mask,
                        AknIconUtils::AvkonIconFileName(),
                        EMbmAvkonQgn_indi_signal_uma_attach,
                        EMbmAvkonQgn_indi_signal_uma_attach_mask, KRgbGray );                  
                break;
                
            case EAknSignalUmaIndicatorContext:
                AknsUtils::CreateColorIconL(skin,
                        KAknsIIDQgnIndiSignalUmaContext,
                        KAknsIIDQsnIconColors, aIconColorIndex, bitmap, mask,
                        AknIconUtils::AvkonIconFileName(),
                        EMbmAvkonQgn_indi_signal_uma_context,
                        EMbmAvkonQgn_indi_signal_uma_context_mask, KRgbGray );                   
                break;
                
            case EAknSignalUmaIndicatorSuspended:
                AknsUtils::CreateColorIconL(skin,
                        KAknsIIDQgnIndiSignalUmaSuspended,
                        KAknsIIDQsnIconColors, aIconColorIndex, bitmap, mask,
                        AknIconUtils::AvkonIconFileName(),
                        EMbmAvkonQgn_indi_signal_uma_suspended,
                        EMbmAvkonQgn_indi_signal_uma_suspended_mask, KRgbGray );                                   
                break;
                
            case EAknSignalUmaIndicatorMultipdp:
                AknsUtils::CreateColorIconL(skin,
                        KAknsIIDQgnIndiSignalUmaMultipdp,
                        KAknsIIDQsnIconColors, aIconColorIndex, bitmap, mask,
                        AknIconUtils::AvkonIconFileName(),
                        EMbmAvkonQgn_indi_signal_uma_multipdp,
                        EMbmAvkonQgn_indi_signal_uma_multipdp_mask, KRgbGray );                      
                        break;
                        
            // Default in all situations
            default:
                AknsUtils::CreateColorIconL( skin,
                                             KAknsIIDQgnPropSignalIcon,
                                             KAknsIIDQsnIconColors,
                                             aIconColorIndex,
                                             bitmap,
                                             mask,
                                             AknIconUtils::AvkonIconFileName(), 
                                             EMbmAvkonQgn_prop_signal_icon,
                                             EMbmAvkonQgn_prop_signal_icon_mask,
                                             KRgbGray );
            }
        }
                
    if ( bitmap )
        {
        delete iSignalIcon;
        iSignalIcon = bitmap;
        AknIconUtils::SetSize( iSignalIcon, Size() );
        }

    if ( mask )
        {
        delete iSignalIconMask;
        iSignalIconMask = mask;
        }

    iIconState  = aIconState;
    iColorIndex = aIconColorIndex;
    }


// ---------------------------------------------------------------------------
// CAknSignalIcon::SetOffLineIconDraw
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::SetOffLineIconDraw( TBool aOn )
    {
    iShowOffLineIcon = aOn;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CAknSignalIcon::SizeChanged
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::SizeChanged()
    {
    AknsUtils::RegisterControlPosition( this );
 
    if ( iSignalIcon )
        {
        AknIconUtils::SetSize( iSignalIcon, Size() );
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CAknSignalIcon::PositionChanged
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::PositionChanged()
    {
    AknsUtils::RegisterControlPosition( this );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CAknSignalIcon::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::HandleResourceChange( TInt aType ) 
    {
    CCoeControl::HandleResourceChange( aType );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CAknSignalIcon::Draw
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    TRect rect( Rect() );
    
    if ( !iDrawBlank && ( !iOffLine || iShowOffLineIcon ) )
        {
        if ( iSignalIcon && iSignalIconMask )
        	{
	        gc.BitBltMasked(
	            rect.iTl, 
	            iSignalIcon,
	            TRect( rect.Size() ), 
	            iSignalIconMask,
	            ETrue );        		
        	}
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalIcon::CAknSignalIcon
// Default constructor.
// ---------------------------------------------------------------------------
//
CAknSignalIcon::CAknSignalIcon() 
	{
	}


// ---------------------------------------------------------------------------
// CAknSignalIcon::ConstructL
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CAknSignalIcon::ConstructL()
	{
	CRepository* repo = CRepository::NewL( KCRUidAvkon );

    TInt aTTEnabled = EFalse;
    iATTEnable = EFalse;
    TInt crErr = repo->Get( KAknATTSignalIconEnable, aTTEnabled );
    if ( crErr == KErrNone )
        {
        iATTEnable = aTTEnabled;
        }
    delete repo;
    repo = NULL;
	}

//  End of File  
