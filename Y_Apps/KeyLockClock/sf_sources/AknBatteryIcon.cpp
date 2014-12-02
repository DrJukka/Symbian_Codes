/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Control to show signal icon indicator on the
*                status pane's battery pane.
*
*/


// INCLUDE FILES
#include "AknBatteryIcon.h"
#include "aknconsts.h"
#include "AknUtils.h"
#include "aknenv.h"
#include "AknIconUtils.h"
#include "AknStatuspaneUtils.h"
#include "aknappui.h"

#include <eikspane.h>
#include <AknsDrawUtils.h>
#include <eikenv.h>
#include <avkon.mbg>
#include <AknsDrawUtils.h>
#include <akniconconfig.h>

#include <AknTasHook.h>
CAknBatteryIcon::CAknBatteryIcon() :
    iIconState( EAknBatteryIndicatorNormal )
	{
	}

CAknBatteryIcon::~CAknBatteryIcon()
	{
	AKNTASHOOK_REMOVE();
    AknsUtils::DeregisterControlPosition( this );
    delete iBatteryIcon;
    delete iBatteryIconMask;
	}

CAknBatteryIcon* CAknBatteryIcon::NewL()
	{
	CAknBatteryIcon* self=CAknBatteryIcon::NewLC();
	CleanupStack::Pop();
	return self;
	}


CAknBatteryIcon* CAknBatteryIcon::NewLC()
	{
	CAknBatteryIcon* self=new(ELeave) CAknBatteryIcon;
	CleanupStack::PushL(self);
	self->ConstructL();
	AKNTASHOOK_ADDL( self, "CAknBatteryIcon" );
	return self;
	}

void CAknBatteryIcon::ConstructL()
	{
	UpdateColorBitmapL( EAknsCIQsnIconColorsCG6 );
	}

void CAknBatteryIcon::SizeChanged()
    {
    AknsUtils::RegisterControlPosition( this );

    TInt colorIndex = 0;
    if ( AknStatuspaneUtils::StaconPaneActive() ||
         AknStatuspaneUtils::FlatLayoutActive() )
        {
        colorIndex = EAknsCIQsnIconColorsCG24;
        }
    else
        {
        colorIndex = EAknsCIQsnIconColorsCG6;
        }

	if ( iColorIndex != colorIndex )
	    {
    	TRAP_IGNORE( UpdateColorBitmapL( colorIndex ) );
	    }

    AknIconUtils::SetSize( iBatteryIcon, Size() );
    }

void CAknBatteryIcon::PositionChanged()
    {
    AknsUtils::RegisterControlPosition( this );
    }

void CAknBatteryIcon::Draw( const TRect& /*aRect*/ ) const
    {        
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        DrawInStaconPane( Rect() );
        }
    else if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        DrawInFlatStatusPane( Rect() );
        }
    else
        {
        DrawInNormalStatusPane( Rect() );
        }
    }

void CAknBatteryIcon::UpdateColorBitmapL( TInt aColorIndex )
    {
    iColorIndex = aColorIndex;
	SetBatteryIconL( iIconState );
    }

void CAknBatteryIcon::HandleResourceChange( TInt aType ) 
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikColorResourceChange || aType == KAknsMessageSkinChange )
        {
        iColorIndex = 0;
        // SizeChanged updates bitmap which is used for color skinning the icons.
		SizeChanged();
        }
    }


void CAknBatteryIcon::SetBatteryIconL( TInt aIconState )
    {
    if ( aIconState >= EAknBatteryIndicatorNormal &&
         aIconState <= EAknBatteryIndicatorPowerSave )
        {
        delete iBatteryIcon;
    	iBatteryIcon = NULL;
    	delete iBatteryIconMask;
    	iBatteryIconMask = NULL;

        TInt iconID;
        TInt maskID;
        TAknsItemID skinItemID;
        skinItemID.Set( 0, 0 );
        
        switch ( aIconState )
            {
            case EAknBatteryIndicatorNormal:
            default:
                iconID     = EMbmAvkonQgn_prop_battery_icon;
                maskID     = EMbmAvkonQgn_prop_battery_icon_mask;
                skinItemID = KAknsIIDQgnPropBatteryIcon;
                break;
            case EAknBatteryIndicatorPowerSave:
                iconID = EMbmAvkonQgn_prop_battery_ps_activate;
                maskID = EMbmAvkonQgn_prop_battery_ps_activate_mask;
                break;
            }

        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsUtils::CreateColorIconL(
            skin,
            skinItemID,
            KAknsIIDQsnIconColors,
            iColorIndex,
            iBatteryIcon,
            iBatteryIconMask,
            KAvkonBitmapFile,
            iconID,
            maskID,
            KRgbGray );
            
        iIconState = aIconState;
        
        AknIconUtils::SetSize( iBatteryIcon, Size() );
        }
    }


TInt CAknBatteryIcon::IconState()
    {
    return iIconState;
    }


void CAknBatteryIcon::DrawInNormalStatusPane( const TRect& /*aRect*/ ) const
    {
	CWindowGc& gc = SystemGc();
	TRect rect( Rect() );
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.SetBrushColor( AKN_LAF_COLOR( KStatusPaneBackgroundColor ) );

    // Draws battery icon
    gc.BitBltMasked( 
        rect.iTl,
        iBatteryIcon,
        TRect( rect.Size() ),
        iBatteryIconMask,
        ETrue );
    }


void CAknBatteryIcon::DrawInStaconPane( const TRect& /*aRect*/ ) const
    {
	CWindowGc& gc = SystemGc();
	TRect rect( Rect() );

    gc.SetBrushStyle( CGraphicsContext::ENullBrush );

    // Draws battery icon
    gc.BitBltMasked( 
        rect.iTl,
        iBatteryIcon, 
        TRect( rect.Size() ), 
        iBatteryIconMask,
        ETrue );    
    }


void CAknBatteryIcon::DrawInFlatStatusPane( const TRect& /*aRect*/ ) const
    {
	CWindowGc& gc = SystemGc();
	TRect rect( Rect() );
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.SetBrushColor( AKN_LAF_COLOR( KStatusPaneBackgroundColor ) );

    // Draws battery icon
    gc.BitBltMasked( 
        rect.iTl,
        iBatteryIcon,
        TRect( rect.Size() ),
        iBatteryIconMask,
        ETrue );
    }

//  End of File  
