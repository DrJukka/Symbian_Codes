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
* Description:  Battery strength indicator control.
*
*/


// INCLUDE FILES
#include "AknBatteryStrength.h"
#include "aknconsts.h"
#include "AknUtils.h"
#include "AknBitmapMirrorUtils.h"
#include "aknenv.h"
#include "AknIconUtils.h"
#include "AknStatuspaneUtils.h"
#include "aknappui.h"

#include <avkon.mbg>
#include <AknsDrawUtils.h>
#include <eikenv.h>
#include <eikspane.h>
#include "AknDef.h"
#include <layoutmetadata.cdl.h>

#include <AknTasHook.h>
const TInt KBatteryStrengthLevels = 7;
const TInt KScaleFactor           = 10000;


// ---------------------------------------------------------------------------
// CAknBatteryStrength::CAknBatteryStrength
// Default constructor.
// ---------------------------------------------------------------------------
//
CAknBatteryStrength::CAknBatteryStrength()
    {
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::~CAknBatteryStrength
// Destructor.
// ---------------------------------------------------------------------------
//
CAknBatteryStrength::~CAknBatteryStrength()
    {
    AKNTASHOOK_REMOVE();
    AknsUtils::DeregisterControlPosition( this );

    delete iDefaultBatteryIndicatorBitmap;
    delete iDefaultBatteryIndicatorBitmapMask;

    delete iFlatBatteryIndicatorBitmap;
    delete iFlatBatteryIndicatorBitmapMask;

    delete iStaconLeftSideBatteryIndicatorBitmap;
    delete iStaconLeftSideBatteryIndicatorBitmapMask;
    delete iStaconRightSideBatteryIndicatorBitmap;
    delete iStaconRightSideBatteryIndicatorBitmapMask;
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAknBatteryStrength* CAknBatteryStrength::NewL()
    {
    CAknBatteryStrength* self = CAknBatteryStrength::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAknBatteryStrength* CAknBatteryStrength::NewLC()
    {
    CAknBatteryStrength* self = new (ELeave) CAknBatteryStrength;
    CleanupStack::PushL( self );
    AKNTASHOOK_ADDL( self, "CAknBatteryStrength" );
    return self;
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::SizeChanged
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::SizeChanged()
    {
    TSize size( Size() );

    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        TRAP_IGNORE( LoadStaconBitmapsL() );
        if ( iStaconLeftSideBatteryIndicatorBitmap )
            {
            AknIconUtils::SetSize(
                iStaconLeftSideBatteryIndicatorBitmap,
                size,
                EAspectRatioPreservedAndUnusedSpaceRemoved );
            }

        if ( iStaconRightSideBatteryIndicatorBitmap )
            {
            AknIconUtils::SetSize(
                iStaconRightSideBatteryIndicatorBitmap,
                size,
                EAspectRatioPreservedAndUnusedSpaceRemoved );
            }

        iLastStaconSize = size;
        }
    else if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        TRAP_IGNORE( LoadFlatBitmapsL() );
        if ( iFlatBatteryIndicatorBitmap )
            {
            AknIconUtils::SetSize( iFlatBatteryIndicatorBitmap,
                                   size,
                                   EAspectRatioNotPreserved );
            }

        iLastFlatSize = size;
        }
    else if ( AknStatuspaneUtils::ExtendedLayoutActive() )
        {
        TRAP_IGNORE( LoadDefaultBitmapsL() );
        if ( iDefaultBatteryIndicatorBitmap )
            {
            AknIconUtils::SetSize( iDefaultBatteryIndicatorBitmap,
                                   size,
                                   EAspectRatioNotPreserved );
            }

        iLastDefaultSize = size;
        }
    else
        {
        TRAP_IGNORE( LoadDefaultBitmapsL() );
        if ( iDefaultBatteryIndicatorBitmap )
            {
            AknIconUtils::SetSize( iDefaultBatteryIndicatorBitmap, size );
            }
        iLastDefaultSize = size;
        }

    SetBatteryLevel( iBatteryLevel ); // refreshes battery level offsets
    AknsUtils::RegisterControlPosition( this );
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::PositionChanged
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::PositionChanged()
    {
    AknsUtils::RegisterControlPosition( this );
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        TRAP_IGNORE( LoadStaconBitmapsL() );
        }
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::Draw
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::Draw( const TRect& /*aRect*/ ) const
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


// ---------------------------------------------------------------------------
// CAknBatteryStrength::BatteryLevel
// ---------------------------------------------------------------------------
//
TInt CAknBatteryStrength::BatteryLevel()
    {
    return iBatteryLevel;
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::SetBatteryLevel
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::SetBatteryLevel( TInt aLevel )
    {
    TBool legalValue  =
        ( ( aLevel <= KBatteryLevelMax ) &&
          ( aLevel >= KBatteryLevelMin ) );

    if ( !legalValue )
        {
        // Battery level value is not changed if wrong value was given.
        return;
        }

    TSize size( Size() );

    iBatteryLevel = aLevel;

    if ( iBatteryLevel == KBatteryLevelMin )
        {
        iDefaultBatteryBitmapOffset = size.iHeight;
        iFlatBatteryBitmapOffset    = size.iHeight;
        iStaconBatteryBitmapOffset  = 0;
        }

    // Offsets are calculated from the battery strength area
    // because the battery strength level layouts return
    // wrong values in some cases.
    else if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        TInt stepSize =
            size.iHeight * KScaleFactor / KBatteryStrengthLevels;

        iFlatBatteryBitmapOffset =
            size.iHeight - ( stepSize * iBatteryLevel / KScaleFactor );
        }
    else if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        TInt stepSize = size.iHeight * KScaleFactor / KBatteryStrengthLevels;
        iStaconBatteryBitmapOffset = stepSize * iBatteryLevel / KScaleFactor;
        }
    else // normal statuspane
        {
        TInt stepSize =
            size.iHeight * KScaleFactor / KBatteryStrengthLevels;

        iDefaultBatteryBitmapOffset =
            size.iHeight - ( stepSize * iBatteryLevel / KScaleFactor );
        }
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::LoadDefaultBitmapsL
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::LoadDefaultBitmapsL()
    {
    if ( iDefaultBatteryIndicatorBitmap &&
         iDefaultBatteryIndicatorBitmapMask &&
         iLastDefaultSize == Size() )
        {
        return;
        }
    else
        {
        delete iDefaultBatteryIndicatorBitmap;
        iDefaultBatteryIndicatorBitmap = NULL;
        delete iDefaultBatteryIndicatorBitmapMask;
        iDefaultBatteryIndicatorBitmapMask = NULL;
        }

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    AknsUtils::CreateIconL( skin,
                            KAknsIIDQgnIndiBatteryStrength,
                            iDefaultBatteryIndicatorBitmap,
                            iDefaultBatteryIndicatorBitmapMask,
                            KAvkonBitmapFile,
                            EMbmAvkonQgn_indi_battery_strength,
                            EMbmAvkonQgn_indi_battery_strength_mask );

    if ( AknStatuspaneUtils::ExtendedLayoutActive() )
        {
        AknIconUtils::SetSize( iDefaultBatteryIndicatorBitmap,
                               Size(),
                               EAspectRatioNotPreserved );
        }
    else
        {
        AknIconUtils::SetSize( iDefaultBatteryIndicatorBitmap,
                               Size() );
        }

    CFbsBitmap* itemIsSkinned = AknsUtils::GetCachedBitmap(
            skin, KAknsIIDQgnIndiBatteryStrength );
    if ( AknLayoutUtils::LayoutMirrored() && !itemIsSkinned )
        {
        CFbsBitmap* tmpBitmap =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iDefaultBatteryIndicatorBitmap );

        CFbsBitmap* tmpBitmapMask =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iDefaultBatteryIndicatorBitmapMask );

        delete iDefaultBatteryIndicatorBitmap;
        iDefaultBatteryIndicatorBitmap = tmpBitmap;
        delete iDefaultBatteryIndicatorBitmapMask;
        iDefaultBatteryIndicatorBitmapMask = tmpBitmapMask;
        }
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::LoadStaconBitmapsL
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::LoadStaconBitmapsL()
    {
    // stacon bitmaps
    TBool iconsExistForActiveStaconLayout = EFalse;
    if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
        {
        if ( iStaconRightSideBatteryIndicatorBitmap &&
             iStaconRightSideBatteryIndicatorBitmapMask )
            {
            iconsExistForActiveStaconLayout = ETrue;
            }
        }
    else if ( AknStatuspaneUtils::StaconSoftKeysRight() )
        {
        if ( iStaconLeftSideBatteryIndicatorBitmap &&
             iStaconLeftSideBatteryIndicatorBitmapMask )
            {
            iconsExistForActiveStaconLayout = ETrue;
            }
        }


    if ( iconsExistForActiveStaconLayout &&
         iLastStaconSize == Size() )
        {
        // Bitmaps exist and size hasn't changed.
        return;
        }
    else
        {
        delete iStaconRightSideBatteryIndicatorBitmap;
        iStaconRightSideBatteryIndicatorBitmap = NULL;
        delete iStaconRightSideBatteryIndicatorBitmapMask;
        iStaconRightSideBatteryIndicatorBitmapMask = NULL;

        delete iStaconLeftSideBatteryIndicatorBitmap;
        iStaconLeftSideBatteryIndicatorBitmap = NULL;
        delete iStaconLeftSideBatteryIndicatorBitmapMask;
        iStaconLeftSideBatteryIndicatorBitmapMask = NULL;
        }

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TRAPD( err, AknsUtils::CreateIconL( skin,
                                        KAknsIIDQgnIndiBatteryStrengthLsc,
                                        iStaconLeftSideBatteryIndicatorBitmap,
                                        iStaconLeftSideBatteryIndicatorBitmapMask,
                                        KAvkonBitmapFile,
                                        EMbmAvkonQgn_indi_battery_strength_lsc,
                                        EMbmAvkonQgn_indi_battery_strength_lsc_mask ) );

    // Only icon for right sk situation exists, we mirror graphics here if needed.
    if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
        {
        if ( err == KErrNone )
            {
            AknIconUtils::SetSize( iStaconLeftSideBatteryIndicatorBitmap,
                                   Size(),
                                   EAspectRatioPreservedAndUnusedSpaceRemoved );

            CFbsBitmap* tmpBitmap =
                AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                    iStaconLeftSideBatteryIndicatorBitmap );

            CFbsBitmap* tmpBitmapMask =
                AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                    iStaconLeftSideBatteryIndicatorBitmapMask );

            delete iStaconRightSideBatteryIndicatorBitmap;
            iStaconRightSideBatteryIndicatorBitmap = tmpBitmap;
            delete iStaconRightSideBatteryIndicatorBitmapMask;
            iStaconRightSideBatteryIndicatorBitmapMask = tmpBitmapMask;
            }
        }

    // Delete unnecessary bitmaps to save RAM
    if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
        {
        delete iStaconLeftSideBatteryIndicatorBitmap;
        iStaconLeftSideBatteryIndicatorBitmap = NULL;
        delete iStaconLeftSideBatteryIndicatorBitmapMask;
        iStaconLeftSideBatteryIndicatorBitmapMask = NULL;
        }
    else if ( AknStatuspaneUtils::StaconSoftKeysRight() )
        {
        delete iStaconRightSideBatteryIndicatorBitmap;
        iStaconRightSideBatteryIndicatorBitmap = NULL;
        delete iStaconRightSideBatteryIndicatorBitmapMask;
        iStaconRightSideBatteryIndicatorBitmapMask = NULL;
        }
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::LoadFlatBitmapsL
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::LoadFlatBitmapsL()
    {
    if ( iFlatBatteryIndicatorBitmap &&
         iFlatBatteryIndicatorBitmapMask &&
         iLastFlatSize == Size() )
        {
        return;
        }
    else
        {
        delete iFlatBatteryIndicatorBitmap;
        iFlatBatteryIndicatorBitmap = NULL;
        delete iFlatBatteryIndicatorBitmapMask;
        iFlatBatteryIndicatorBitmapMask = NULL;
        }

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Flat bitmaps
    AknsUtils::CreateIconL( skin,
                            KAknsIIDQgnIndiBatteryStrength,
                            iFlatBatteryIndicatorBitmap,
                            iFlatBatteryIndicatorBitmapMask,
                            KAvkonBitmapFile,
                            EMbmAvkonQgn_indi_battery_strength,
                            EMbmAvkonQgn_indi_battery_strength_mask );

    AknIconUtils::SetSize( iFlatBatteryIndicatorBitmap,
                           Size(),
                           EAspectRatioNotPreserved );

    CFbsBitmap*  itemIsSkinned = AknsUtils::GetCachedBitmap(
            skin, KAknsIIDQgnIndiBatteryStrength );
    if ( AknLayoutUtils::LayoutMirrored() && !itemIsSkinned )
        {
        CFbsBitmap* tmpBitmap =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iFlatBatteryIndicatorBitmap );

        CFbsBitmap* tmpBitmapMask =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iFlatBatteryIndicatorBitmapMask );

        delete iFlatBatteryIndicatorBitmap;
        iFlatBatteryIndicatorBitmap = tmpBitmap;
        delete iFlatBatteryIndicatorBitmapMask;
        iFlatBatteryIndicatorBitmapMask = tmpBitmapMask;
        }
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::DeleteBitmaps
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::DeleteBitmaps()
    {
    delete iDefaultBatteryIndicatorBitmap;
    iDefaultBatteryIndicatorBitmap = NULL;

    delete iDefaultBatteryIndicatorBitmapMask;
    iDefaultBatteryIndicatorBitmapMask = NULL;

    delete iFlatBatteryIndicatorBitmap;
    iFlatBatteryIndicatorBitmap = NULL;

    delete iFlatBatteryIndicatorBitmapMask;
    iFlatBatteryIndicatorBitmapMask = NULL;

    delete iStaconLeftSideBatteryIndicatorBitmap;
    iStaconLeftSideBatteryIndicatorBitmap = NULL;

    delete iStaconLeftSideBatteryIndicatorBitmapMask;
    iStaconLeftSideBatteryIndicatorBitmapMask = NULL;

    delete iStaconRightSideBatteryIndicatorBitmap;
    iStaconRightSideBatteryIndicatorBitmap = NULL;

    delete iStaconRightSideBatteryIndicatorBitmapMask;
    iStaconRightSideBatteryIndicatorBitmapMask = NULL;
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::HandleResourceChange( TInt aType )
    {
    if ( aType==KEikDynamicLayoutVariantSwitch ||
         aType == KEikColorResourceChange ||
         aType == KAknsMessageSkinChange)
        {
        DeleteBitmaps(); // SizeChanged reloads needed bitmaps
        SizeChanged();

        if ( aType == KEikDynamicLayoutVariantSwitch )
            {
            SetBatteryLevel( iBatteryLevel ); // refreshes battery level offsets
            }

        DrawDeferred();
        }
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::DrawInNormalStatusPane
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::DrawInNormalStatusPane( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    TRect rect( Rect() );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Draws battery strength bar
    CAknsMaskedBitmapItemData* bitmap = NULL;

    if ( skin )
        {
        bitmap = static_cast<CAknsMaskedBitmapItemData*>(
            skin->GetCachedItemData(
                KAknsIIDQgnIndiBatteryStrength, EAknsITMaskedBitmap ) );
        if ( bitmap )
            {
            if ( bitmap->Bitmap() )
                {
                AknIconUtils::SetSize( bitmap->Bitmap(),
                                       Size() );
                }
            if ( bitmap->Mask() )
                {
                AknIconUtils::SetSize( bitmap->Mask(),
                                       Size() );
                }

            gc.BitBltMasked(
                TPoint( rect.iTl.iX, iDefaultBatteryBitmapOffset ),
                bitmap->Bitmap(),
                TRect( 0,
                       iDefaultBatteryBitmapOffset,
                       rect.Width(),
                       rect.Height() ),
                bitmap->Mask(),
                ETrue );
            }
        }

    if ( !bitmap )
        {
        if ( iDefaultBatteryIndicatorBitmap &&
             iDefaultBatteryIndicatorBitmapMask )
            {
            gc.BitBltMasked(
                TPoint( rect.iTl.iX, iDefaultBatteryBitmapOffset ),
                iDefaultBatteryIndicatorBitmap,
                TRect( 0,
                       iDefaultBatteryBitmapOffset,
                       rect.Width(),
                       rect.Height() ),
                iDefaultBatteryIndicatorBitmapMask,
                ETrue );
            }
        }

    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::DrawInStaconPane
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::DrawInStaconPane( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask   = NULL;

    TRect rect( Rect() );

    if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
        {
        bitmap = iStaconRightSideBatteryIndicatorBitmap;
        mask   = iStaconRightSideBatteryIndicatorBitmapMask;
        }
    else
        {
        bitmap = iStaconLeftSideBatteryIndicatorBitmap;
        mask   = iStaconLeftSideBatteryIndicatorBitmapMask;
        }

    TPoint point( rect.iTl );
    if ( AknStatuspaneUtils::StaconSoftKeysLeft() && bitmap )
        {
        point = TPoint( rect.iBr.iX - bitmap->SizeInPixels().iWidth,
                        rect.iTl.iY );
        }

    if ( bitmap && mask )
        {
        gc.BitBltMasked(
            point,
            bitmap,
            TRect( 0, 0, rect.Width(), iStaconBatteryBitmapOffset ),
            mask,
            ETrue );
        }

    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::DrawInFlatStatusPane
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::DrawInFlatStatusPane( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    TRect rect( Rect() );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Draws battery strength bar
    CAknsMaskedBitmapItemData* bitmapData = NULL;

    if ( skin )
        {
        bitmapData = static_cast<CAknsMaskedBitmapItemData*>(
            skin->GetCachedItemData(
                KAknsIIDQgnIndiBatteryStrength, EAknsITMaskedBitmap ) );
        if ( bitmapData )
            {
            CFbsBitmap* bitmap = bitmapData->Bitmap();

            if ( bitmap )
                AknIconUtils::SetSize( bitmap,
                                       TSize( rect.Width(),
                                              rect.Height() ),
                                              EAspectRatioNotPreserved );

            TPoint point( 0, iFlatBatteryBitmapOffset );
            if ( bitmap && !AknLayoutUtils::LayoutMirrored() )
                {
                point = TPoint( rect.iBr.iX - bitmap->SizeInPixels().iWidth,
                                iFlatBatteryBitmapOffset);
                }

            gc.BitBltMasked(
                point,
                bitmap, TRect(0,iFlatBatteryBitmapOffset,
                rect.Width(),
                rect.Height()),
                bitmapData->Mask(), ETrue );
            }
        }

    if ( !bitmapData )
        {
        if ( iFlatBatteryIndicatorBitmap && iFlatBatteryIndicatorBitmapMask )
            {
            TPoint point( 0, iFlatBatteryBitmapOffset );
            if ( !AknLayoutUtils::LayoutMirrored() )
                {
                point = TPoint(
                    rect.iBr.iX - iFlatBatteryIndicatorBitmap->SizeInPixels().iWidth,
                    iFlatBatteryBitmapOffset );
                }
            gc.BitBltMasked(
                point,
                iFlatBatteryIndicatorBitmap,
                TRect( 0,
                       iFlatBatteryBitmapOffset,
                       rect.Width(),
                       rect.Height() ),
                iFlatBatteryIndicatorBitmapMask,
                ETrue );
            }
        }

    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::SetRecharging
// ---------------------------------------------------------------------------
//
void CAknBatteryStrength::SetRecharging( TBool aRecharging )
    {
    iRecharging = aRecharging;
    }


// ---------------------------------------------------------------------------
// CAknBatteryStrength::Recharging
// ---------------------------------------------------------------------------
//
TBool CAknBatteryStrength::Recharging()
    {
    return iRecharging;
    }

//  End of File
