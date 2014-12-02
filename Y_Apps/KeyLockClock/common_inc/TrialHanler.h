/* Copyright (c) 2001 - 2008 , Dr Jukka Silvennoinen. All rights reserved */


#ifndef __TRIALHAND_HEADER__
#define __TRIALHAND_HEADER__

#include <e32base.h>
#include <S32FILE.H>
#include "Common.h"


class CTrialHandler : public CBase
    {
public : 
    static void SetDateNowL();
    static TBool IsNowOkL(TBool& aFirstTime,TInt& aDaysLeft);
    };


	
#endif //__EXAMPLESNAMEDDB_HEADER__
