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
* Description:  Signal strength indicator control.
*
*/


// INCLUDE FILES
#include "AknSignalStrength.h"
#include "aknconsts.h"
#include "AknUtils.h"
#include "AknIconUtils.h"
#include "AknBitmapMirrorUtils.h"
#include "AknStatuspaneUtils.h"
#include "aknappui.h"

#include <avkon.mbg>
#include <AknsDrawUtils.h>
#include <eikenv.h>
#include <eikspane.h>
#include "AknDef.h"
#include <layoutmetadata.cdl.h>
#include <akniconconfig.h>

#include <AknTasHook.h> // for testability hooks
const TInt KSignalStrengthLevels = 7;
const TInt KScaleFactor          = 10000;


// ---------------------------------------------------------------------------
// CAknSignalStrength::CAknSignalStrength
// Default contructor.
// ---------------------------------------------------------------------------
//
CAknSignalStrength::CAknSignalStrength()
    {
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::~CAknSignalStrength
// Destructor.
// ---------------------------------------------------------------------------
//
CAknSignalStrength::~CAknSignalStrength()
    {
    AKNTASHOOK_REMOVE();
    AknsUtils::DeregisterControlPosition( this );

    delete iDefaultSignalIndicatorBitmap;
    delete iDefaultSignalIndicatorBitmapMask;
    delete iDefaultNoSignalIndicatorBitmap;
    delete iDefaultNoSignalIndicatorBitmapMask;

    delete iFlatSignalIndicatorBitmap;
    delete iFlatSignalIndicatorBitmapMask;
    delete iFlatNoSignalIndicatorBitmap;
    delete iFlatNoSignalIndicatorBitmapMask;

    delete iStaconLeftSideSignalIndicatorBitmap;
    delete iStaconLeftSideSignalIndicatorBitmapMask;
    delete iStaconRightSideSignalIndicatorBitmap;
    delete iStaconRightSideSignalIndicatorBitmapMask;
    delete iStaconNoSignalIndicatorBitmap;
    delete iStaconNoSignalIndicatorBitmapMask;
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAknSignalStrength* CAknSignalStrength::NewL()
    {
    CAknSignalStrength* self = CAknSignalStrength::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CAknSignalStrength* CAknSignalStrength::NewLC()
    {
    CAknSignalStrength* self = new (ELeave) CAknSignalStrength;
    CleanupStack::PushL( self );
    AKNTASHOOK_ADDL( self, "CAknSignalStrength" );
    return self;
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::SizeChanged
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::SizeChanged()
    {
    TSize size( Size() );

    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        TRAP_IGNORE( LoadStaconBitmapsL() );
        if ( iStaconRightSideSignalIndicatorBitmap )
            {
            AknIconUtils::SetSize( iStaconRightSideSignalIndicatorBitmap,
                                   size,
                                   EAspectRatioPreservedAndUnusedSpaceRemoved );
            }
        if ( iStaconLeftSideSignalIndicatorBitmap )
            {
            AknIconUtils::SetSize( iStaconLeftSideSignalIndicatorBitmap,
                                   size,
                                   EAspectRatioPreservedAndUnusedSpaceRemoved );
            }
        if ( iStaconNoSignalIndicatorBitmap )
            {
            AknIconUtils::SetSize( iStaconNoSignalIndicatorBitmap,
                                   size );
            }

        iLastStaconSize = size;
        }
    else if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        TRAP_IGNORE( LoadFlatBitmapsL() );
        if ( iFlatSignalIndicatorBitmap && iFlatNoSignalIndicatorBitmap )
            {
            AknIconUtils::SetSize( iFlatSignalIndicatorBitmap,
                                   size,
                                   EAspectRatioNotPreserved );
            AknIconUtils::SetSize( iFlatNoSignalIndicatorBitmap, size );
            }

        iLastFlatSize = size;
        }
    else if ( AknStatuspaneUtils::ExtendedLayoutActive() )
        {
        TRAP_IGNORE( LoadDefaultBitmapsL() );
        if ( iDefaultSignalIndicatorBitmap && iDefaultNoSignalIndicatorBitmap )
            {
            AknIconUtils::SetSize( iDefaultSignalIndicatorBitmap,
                                   size,
                                   EAspectRatioNotPreserved );
            AknIconUtils::SetSize( iDefaultNoSignalIndicatorBitmap, size );
            }

        iLastDefaultSize = size;
        }
    else
        {
        TRAP_IGNORE( LoadDefaultBitmapsL() );
        if ( iDefaultSignalIndicatorBitmap && iDefaultNoSignalIndicatorBitmap )
            {
            AknIconUtils::SetSize( iDefaultSignalIndicatorBitmap, size );
            AknIconUtils::SetSize( iDefaultNoSignalIndicatorBitmap, size );
            }

        iLastDefaultSize = size;
        }

    SetSignalLevel( iSignalLevel ); // refreshes level offsets
    AknsUtils::RegisterControlPosition( this );
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::PositionChanged
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::PositionChanged()
    {
    AknsUtils::RegisterControlPosition( this );
    if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        TRAP_IGNORE( LoadStaconBitmapsL() );
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::Draw
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::Draw( const TRect& /*aRect*/ ) const
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
// CAknSignalStrength::SetSignalLevel
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::SetSignalLevel( TInt aLevel )
    {
    if ( aLevel == KAknSignalOffLineMode )
        {
        // Off-line mode indicator is shown.
        iSignalLevel = aLevel;
        return;
        }

    TBool legalValue  =
        ( ( aLevel <= KSignalLevelMax ) &&
          ( aLevel >= KSignalLevelMin ) );

    if ( !legalValue )
        {
        // Signal value is not changed if wrong value was given.
        return;
        }

    iSignalLevel = aLevel;

    TSize size( Size() );

    if ( aLevel == KSignalLevelMin )
        {
        iDefaultSignalBitmapOffset = size.iHeight;
        iStaconSignalBitmapOffset  = size.iHeight;
        iFlatSignalBitmapOffset    = size.iHeight;
        }

    // Offsets are calculated from the signal strength area
    // because the signal strength level layouts return
    // wrong values in some cases.
    else if ( AknStatuspaneUtils::FlatLayoutActive() )
        {
        TInt stepSize =
            size.iHeight * KScaleFactor / KSignalStrengthLevels;

        iFlatSignalBitmapOffset = size.iHeight - stepSize * iSignalLevel / KScaleFactor;
        }
    else if ( AknStatuspaneUtils::StaconPaneActive() )
        {
        TInt stepSize = size.iHeight * KScaleFactor / KSignalStrengthLevels;

        iStaconSignalBitmapOffset =
            size.iHeight - stepSize * iSignalLevel / KScaleFactor;
        }
    else // normal statuspane
        {
        TInt stepSize =
            size.iHeight * KScaleFactor / KSignalStrengthLevels;

        iDefaultSignalBitmapOffset =
            size.iHeight - stepSize * iSignalLevel / KScaleFactor;
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::LoadDefaultBitmapsL
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::LoadDefaultBitmapsL()
    {
    if ( iDefaultSignalIndicatorBitmap &&
         iDefaultSignalIndicatorBitmapMask &&
         iDefaultNoSignalIndicatorBitmap &&
         iDefaultNoSignalIndicatorBitmapMask &&
         iLastDefaultSize == Size() )
        {
        return;
        }
    else
        {
        delete iDefaultSignalIndicatorBitmap;
        iDefaultSignalIndicatorBitmap = NULL;
        delete iDefaultSignalIndicatorBitmapMask;
        iDefaultSignalIndicatorBitmapMask = NULL;
        delete iDefaultNoSignalIndicatorBitmap;
        iDefaultNoSignalIndicatorBitmap = NULL;
        delete iDefaultNoSignalIndicatorBitmapMask;
        iDefaultNoSignalIndicatorBitmapMask = NULL;
        }

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    AknsUtils::CreateIconL( skin,
                            KAknsIIDQgnIndiSignalStrength,
                            iDefaultSignalIndicatorBitmap,
                            iDefaultSignalIndicatorBitmapMask,
                            KAvkonBitmapFile,
                            EMbmAvkonQgn_indi_signal_strength,
                            EMbmAvkonQgn_indi_signal_strength_mask );

    AknsUtils::CreateColorIconL( skin,
                                 KAknsIIDQgnIndiNoSignal,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG3,
                                 iDefaultNoSignalIndicatorBitmap,
                                 iDefaultNoSignalIndicatorBitmapMask,
                                 KAvkonBitmapFile,
                                 EMbmAvkonQgn_indi_no_signal,
                                 EMbmAvkonQgn_indi_no_signal_mask,
                                 KRgbGray );

    if ( AknStatuspaneUtils::ExtendedLayoutActive() )
        {
        AknIconUtils::SetSize( iDefaultSignalIndicatorBitmap,
                               Size(),
                               EAspectRatioNotPreserved );
        AknIconUtils::SetSize( iDefaultNoSignalIndicatorBitmap,
                               Size(),
                               EAspectRatioNotPreserved );
        }
    else
        {
        AknIconUtils::SetSize( iDefaultSignalIndicatorBitmap, Size() );
        AknIconUtils::SetSize( iDefaultNoSignalIndicatorBitmap, Size() );
        }

    CFbsBitmap* itemIsSkinned = AknsUtils::GetCachedBitmap(
            skin, KAknsIIDQgnIndiSignalStrength );
    if ( AknLayoutUtils::LayoutMirrored() && !itemIsSkinned )
        {
        CFbsBitmap* tmpBitmap =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iDefaultSignalIndicatorBitmap );

        CFbsBitmap* tmpBitmapMask =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iDefaultSignalIndicatorBitmapMask );

        delete iDefaultSignalIndicatorBitmap;
        iDefaultSignalIndicatorBitmap = tmpBitmap;
        delete iDefaultSignalIndicatorBitmapMask;
        iDefaultSignalIndicatorBitmapMask = tmpBitmapMask;
        }

    CFbsBitmap* itemIsSkinned2 = AknsUtils::GetCachedBitmap(
            skin, KAknsIIDQgnIndiNoSignal );
    if ( AknLayoutUtils::LayoutMirrored() && !itemIsSkinned2 )
        {
        CFbsBitmap* tmpBitmapNoSignal =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iDefaultNoSignalIndicatorBitmap );

        CFbsBitmap* tmpBitmapNoSignalMask =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iDefaultNoSignalIndicatorBitmapMask );

        delete iDefaultNoSignalIndicatorBitmap;
        iDefaultNoSignalIndicatorBitmap = tmpBitmapNoSignal;
        delete iDefaultNoSignalIndicatorBitmapMask;
        iDefaultNoSignalIndicatorBitmapMask = tmpBitmapNoSignalMask;
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::LoadStaconBitmapsL
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::LoadStaconBitmapsL()
    {
    TBool iconsExistForActiveStaconLayout = EFalse;
    if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
        {
        if ( iStaconRightSideSignalIndicatorBitmap &&
             iStaconRightSideSignalIndicatorBitmapMask &&
             iStaconNoSignalIndicatorBitmap &&
             iStaconNoSignalIndicatorBitmapMask )
            {
            iconsExistForActiveStaconLayout = ETrue;
            }
        }
    else if ( AknStatuspaneUtils::StaconSoftKeysRight() )
        {
        if ( iStaconLeftSideSignalIndicatorBitmap &&
             iStaconLeftSideSignalIndicatorBitmapMask &&
             iStaconNoSignalIndicatorBitmap &&
             iStaconNoSignalIndicatorBitmapMask )
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
        delete iStaconRightSideSignalIndicatorBitmap;
        iStaconRightSideSignalIndicatorBitmap = NULL;
        delete iStaconRightSideSignalIndicatorBitmapMask;
        iStaconRightSideSignalIndicatorBitmapMask = NULL;
        delete iStaconLeftSideSignalIndicatorBitmap;
        iStaconLeftSideSignalIndicatorBitmap = NULL;
        delete iStaconLeftSideSignalIndicatorBitmapMask;
        iStaconLeftSideSignalIndicatorBitmapMask = NULL;
        delete iStaconNoSignalIndicatorBitmap;
        iStaconNoSignalIndicatorBitmap = NULL;
        delete iStaconNoSignalIndicatorBitmapMask;
        iStaconNoSignalIndicatorBitmapMask = NULL;
        }


    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    AknsUtils::CreateColorIconL( skin,
                                 KAknsIIDQgnIndiNoSignal,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG21,
                                 iStaconNoSignalIndicatorBitmap,
                                 iStaconNoSignalIndicatorBitmapMask,
                                 KAvkonBitmapFile,
                                 EMbmAvkonQgn_indi_no_signal,
                                 EMbmAvkonQgn_indi_no_signal_mask,
                                 KRgbGray );

    AknIconUtils::SetSize( iStaconNoSignalIndicatorBitmap, Size() );

    TRAPD( err, AknsUtils::CreateIconL( skin,
                                        KAknsIIDQgnIndiSignalStrengthLsc,
                                        iStaconLeftSideSignalIndicatorBitmap,
                                        iStaconLeftSideSignalIndicatorBitmapMask,
                                        KAvkonBitmapFile,
                                        EMbmAvkonQgn_indi_signal_strength_lsc,
                                        EMbmAvkonQgn_indi_signal_strength_lsc_mask ) );

    if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
        {
        if ( err == KErrNone )
            {
            TSize size = Size();

            AknIconUtils::SetSize( iStaconLeftSideSignalIndicatorBitmap,
                                   size,
                                   EAspectRatioPreservedAndUnusedSpaceRemoved );

            CFbsBitmap* tmpBitmap =
                AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                    iStaconLeftSideSignalIndicatorBitmap );

            CFbsBitmap* tmpBitmapMask =
                AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                    iStaconLeftSideSignalIndicatorBitmapMask );


            delete iStaconRightSideSignalIndicatorBitmap;
            iStaconRightSideSignalIndicatorBitmap = tmpBitmap;
            delete iStaconRightSideSignalIndicatorBitmapMask;
            iStaconRightSideSignalIndicatorBitmapMask = tmpBitmapMask;
            }
        }

    // Delete unnecessary bitmaps to save RAM
    if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
        {
        delete iStaconLeftSideSignalIndicatorBitmap;
        iStaconLeftSideSignalIndicatorBitmap = NULL;
        delete iStaconLeftSideSignalIndicatorBitmapMask;
        iStaconLeftSideSignalIndicatorBitmapMask = NULL;
        }
    else if ( AknStatuspaneUtils::StaconSoftKeysRight() )
        {
        delete iStaconRightSideSignalIndicatorBitmap;
        iStaconRightSideSignalIndicatorBitmap = NULL;
        delete iStaconRightSideSignalIndicatorBitmapMask;
        iStaconRightSideSignalIndicatorBitmapMask = NULL;
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::LoadFlatBitmapsL
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::LoadFlatBitmapsL()
    {
    if ( iFlatSignalIndicatorBitmap &&
         iFlatSignalIndicatorBitmapMask &&
         iFlatNoSignalIndicatorBitmap &&
         iFlatNoSignalIndicatorBitmapMask &&
         iLastFlatSize == Size() )
        {
        return;
        }
    else
        {
        delete iFlatSignalIndicatorBitmap;
        iFlatSignalIndicatorBitmap = NULL;
        delete iFlatSignalIndicatorBitmapMask;
        iFlatSignalIndicatorBitmapMask = NULL;
        delete iFlatNoSignalIndicatorBitmap;
        iFlatNoSignalIndicatorBitmap = NULL;
        delete iFlatNoSignalIndicatorBitmapMask;
        iFlatNoSignalIndicatorBitmapMask = NULL;
        }


    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    AknsUtils::CreateIconL( skin,
                            KAknsIIDQgnIndiSignalStrength,
                            iFlatSignalIndicatorBitmap,
                            iFlatSignalIndicatorBitmapMask,
                            KAvkonBitmapFile,
                            EMbmAvkonQgn_indi_signal_strength,
                            EMbmAvkonQgn_indi_signal_strength_mask );

    AknsUtils::CreateColorIconL( skin,
                                 KAknsIIDQgnIndiNoSignal,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG3,
                                 iFlatNoSignalIndicatorBitmap,
                                 iFlatNoSignalIndicatorBitmapMask,
                                 KAvkonBitmapFile,
                                 EMbmAvkonQgn_indi_no_signal,
                                 EMbmAvkonQgn_indi_no_signal_mask,
                                 KRgbGray );

    AknIconUtils::SetSize( iFlatSignalIndicatorBitmap,
                           Size(),
                           EAspectRatioNotPreserved);
    AknIconUtils::SetSize( iFlatNoSignalIndicatorBitmap,
                           Size(),
                           EAspectRatioNotPreserved );

    CFbsBitmap* itemIsSkinned = AknsUtils::GetCachedBitmap(
            skin, KAknsIIDQgnIndiSignalStrength );
    if ( AknLayoutUtils::LayoutMirrored() && !itemIsSkinned )
        {
        CFbsBitmap* tmpBitmap =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iFlatSignalIndicatorBitmap );

        CFbsBitmap* tmpBitmapMask =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iFlatSignalIndicatorBitmapMask );

        delete iFlatSignalIndicatorBitmap;
        iFlatSignalIndicatorBitmap = tmpBitmap;
        delete iFlatSignalIndicatorBitmapMask;
        iFlatSignalIndicatorBitmapMask = tmpBitmapMask;
        }

    CFbsBitmap* itemIsSkinned2 = AknsUtils::GetCachedBitmap(
            skin, KAknsIIDQgnIndiNoSignal );
    if ( AknLayoutUtils::LayoutMirrored() && !itemIsSkinned2 )
        {
        CFbsBitmap* tmpBitmapNoSignal =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iFlatNoSignalIndicatorBitmap );

        CFbsBitmap* tmpBitmapNoSignalMask =
            AknBitmapMirrorUtils::HorizontallyMirrorBitmapL(
                iFlatNoSignalIndicatorBitmapMask );

        delete iFlatNoSignalIndicatorBitmap;
        iFlatNoSignalIndicatorBitmap = tmpBitmapNoSignal;
        delete iFlatNoSignalIndicatorBitmapMask;
        iFlatNoSignalIndicatorBitmapMask = tmpBitmapNoSignalMask;
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::DeleteBitmaps
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::DeleteBitmaps()
    {
    // default bitmaps
    delete iDefaultSignalIndicatorBitmap;
    iDefaultSignalIndicatorBitmap = NULL;
    delete iDefaultSignalIndicatorBitmapMask;
    iDefaultSignalIndicatorBitmapMask = NULL;
    delete iDefaultNoSignalIndicatorBitmap;
    iDefaultNoSignalIndicatorBitmap = NULL;
    delete iDefaultNoSignalIndicatorBitmapMask;
    iDefaultNoSignalIndicatorBitmapMask = NULL;

    // flat bitmaps
    delete iFlatSignalIndicatorBitmap;
    iFlatSignalIndicatorBitmap = NULL;
    delete iFlatSignalIndicatorBitmapMask;
    iFlatSignalIndicatorBitmapMask = NULL;
    delete iFlatNoSignalIndicatorBitmap;
    iFlatNoSignalIndicatorBitmap = NULL;
    delete iFlatNoSignalIndicatorBitmapMask;
    iFlatNoSignalIndicatorBitmapMask = NULL;

    // stacon bitmaps
    delete iStaconLeftSideSignalIndicatorBitmap;
    iStaconLeftSideSignalIndicatorBitmap = NULL;
    delete iStaconLeftSideSignalIndicatorBitmapMask;
    iStaconLeftSideSignalIndicatorBitmapMask = NULL;

    delete iStaconRightSideSignalIndicatorBitmap;
    iStaconRightSideSignalIndicatorBitmap = NULL;
    delete iStaconRightSideSignalIndicatorBitmapMask;
    iStaconRightSideSignalIndicatorBitmapMask = NULL;

    delete iStaconNoSignalIndicatorBitmap;
    iStaconNoSignalIndicatorBitmap = NULL;
    delete iStaconNoSignalIndicatorBitmapMask;
    iStaconNoSignalIndicatorBitmapMask = NULL;
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    if ( aType == KEikDynamicLayoutVariantSwitch ||
         aType == KEikColorResourceChange ||
         aType == KAknsMessageSkinChange )
        {
        DeleteBitmaps(); // SizeChanged loads bitmaps again
        SizeChanged();
        SetSignalLevel( iSignalLevel ); // Refreshes signal level offsets
        DrawDeferred();
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::DrawInNormalStatusPane
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::DrawInNormalStatusPane( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRect rect( Rect() );

    // Signal strength bar.
    CAknsMaskedBitmapItemData* bitmap;

    if( iSignalLevel == KAknSignalOffLineMode )
        {
        // Off-line indicator.
        if ( iDefaultNoSignalIndicatorBitmap &&
             iDefaultNoSignalIndicatorBitmapMask )
            {
            gc.BitBltMasked(
                rect.iTl,
                iDefaultNoSignalIndicatorBitmap,
                TRect( 0, 0, rect.Width(), rect.Height() ),
                iDefaultNoSignalIndicatorBitmapMask,
                ETrue );
            }
        }
    else
        {
        TBool legacydraw = EFalse;

        if ( skin )
            {
            bitmap = static_cast<CAknsMaskedBitmapItemData*>(
                skin->GetCachedItemData( KAknsIIDQgnIndiSignalStrength,
                                         EAknsITMaskedBitmap ) );
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
                    TPoint( rect.iTl.iX, iDefaultSignalBitmapOffset ),
                    bitmap->Bitmap(),
                    TRect( 0,
                           iDefaultSignalBitmapOffset,
                           rect.Width(),
                           rect.Height() ),
                    bitmap->Mask(),
                    ETrue );
                }
            else
                {
                legacydraw = ETrue;
                }
            }

        if ( legacydraw )
            {
            if ( iDefaultSignalIndicatorBitmap &&
                 iDefaultSignalIndicatorBitmapMask )
                {
                gc.BitBltMasked(
                    TPoint( rect.iTl.iX, iDefaultSignalBitmapOffset ),
                    iDefaultSignalIndicatorBitmap,
                    TRect( 0,
                           iDefaultSignalBitmapOffset,
                           rect.Width(),
                           rect.Height() ),
                    iDefaultSignalIndicatorBitmapMask,
                    ETrue );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::DrawInStaconPane
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::DrawInStaconPane( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    TRect rect( Rect() );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask   = NULL;

    if ( iSignalLevel == KAknSignalOffLineMode )
        {
        // Off-line indicator.
        if ( iStaconNoSignalIndicatorBitmap &&
             iStaconNoSignalIndicatorBitmapMask )
            {
            gc.BitBltMasked(
                rect.iTl,
                iStaconNoSignalIndicatorBitmap,
                TRect( 0, 0, rect.Width(), rect.Height() ),
                iStaconNoSignalIndicatorBitmapMask,
                ETrue );
            }
        }
    else
        {
        TPoint point( rect.iTl.iX,
                      rect.iTl.iY + iStaconSignalBitmapOffset );

        if ( AknStatuspaneUtils::StaconSoftKeysLeft() )
            {
            bitmap = iStaconRightSideSignalIndicatorBitmap;
            mask   = iStaconRightSideSignalIndicatorBitmapMask;

            if ( bitmap )
                {
                point = TPoint( rect.iBr.iX - bitmap->SizeInPixels().iWidth,
                                rect.iTl.iY + iStaconSignalBitmapOffset );
                }

            }
        else
            {
            bitmap = iStaconLeftSideSignalIndicatorBitmap;
            mask   = iStaconLeftSideSignalIndicatorBitmapMask;
            }

        if ( bitmap && mask )
            {
            gc.BitBltMasked(
                point,
                bitmap,
                TRect( 0,
                       iStaconSignalBitmapOffset,
                       rect.Width(),
                       rect.Height() ),
                mask,
                ETrue );
            }
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::DrawInFlatStatusPane
// ---------------------------------------------------------------------------
//
void CAknSignalStrength::DrawInFlatStatusPane( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRect rect( Rect() );

    // Signal strength bar.
    CAknsMaskedBitmapItemData* bitmapData;

    if ( iSignalLevel == KAknSignalOffLineMode )
        {
        // Off-line indicator.
        if ( iFlatNoSignalIndicatorBitmap && iFlatNoSignalIndicatorBitmapMask )
            {
            gc.BitBltMasked(
                rect.iTl,
                iFlatNoSignalIndicatorBitmap,
                TRect( 0, 0, rect.Width(), rect.Height() ),
                iFlatNoSignalIndicatorBitmapMask,
                ETrue );
            }
        }
    else
        {
        TBool legacydraw = EFalse;

        if ( skin )
            {
            bitmapData = static_cast<CAknsMaskedBitmapItemData*>(
                skin->GetCachedItemData(
                    KAknsIIDQgnIndiSignalStrength, EAknsITMaskedBitmap ) );
            if ( bitmapData )
                {
                TPoint point( rect.iTl.iX, iFlatSignalBitmapOffset );

                CFbsBitmap* bitmap = bitmapData->Bitmap();

                if ( bitmap )
                    {
                    AknIconUtils::SetSize(
                        bitmap,
                        TSize( rect.Width(),
                               rect.Height() ),
                               EAspectRatioNotPreserved );

                    if ( AknLayoutUtils::LayoutMirrored() )
                        {
                        point = TPoint(
                            rect.iBr.iX - bitmap->SizeInPixels().iWidth,
                            iFlatSignalBitmapOffset );
                        }
                    }

                gc.BitBltMasked(
                    point,
                    bitmap,
                    TRect( 0,
                           iFlatSignalBitmapOffset,
                           rect.Width(),
                           rect.Height() ),
                    bitmapData->Mask(),
                    ETrue );
                }
            else
                {
                legacydraw = ETrue;
                }
            }

        if ( legacydraw )
            {
            if ( iFlatSignalIndicatorBitmap && iFlatSignalIndicatorBitmapMask )
                {
                TPoint point( rect.iTl.iX, iFlatSignalBitmapOffset );

                if ( AknLayoutUtils::LayoutMirrored() )
                        {
                        point = TPoint(
                            rect.iBr.iX - iFlatSignalIndicatorBitmap->SizeInPixels().iWidth,
                            iFlatSignalBitmapOffset );
                        }

                gc.BitBltMasked(
                    point,
                    iFlatSignalIndicatorBitmap,
                    TRect( 0,
                           iFlatSignalBitmapOffset,
                           rect.Width(),
                           rect.Height() ),
                    iFlatSignalIndicatorBitmapMask,
                    ETrue );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CAknSignalStrength::SignalLevel
// ---------------------------------------------------------------------------
//
TInt CAknSignalStrength::SignalLevel()
    {
    return iSignalLevel;
    }

//  End of File
