/* 
	Copyright (c) 2006, 
	Jukka Silvennoinen
	All rights reserved 
*/

#ifndef YUCCA_NAVIPANEL_H
#define YUCCA_NAVIPANEL_H

#include <e32base.h>
#include <aknnavilabel.h>

#include "CTimeOutTimer.h"

class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CEikonEnv;
class CAknIndicatorContainer;


class CYuccaNavi : public CCoeControl,MTimeOutNotify
    {
	public:
		static CYuccaNavi* NewL(const TBool& aScrroll);
		~CYuccaNavi();
		void SetTextL(const TDesC& aText);
		void GetTextL(TDes& aText);
		void SetScroll(const TBool& aScrroll);
	private:
		void TimerExpired();
		CYuccaNavi(const TBool& aScrroll);
		void ConstructL(void);
	private:
		CAknNavigationControlContainer* iNaviPane;
        CAknNavigationDecorator* 	iNaviDecorator;
        HBufC* 						iFullText;
        CTimeOutTimer*				iTimeOutTimer;
		TBool						iGoingRight,iScrroll;
		TInt						iCurrentCut,iExtraCutCount;
    };

#endif // YUCCA_NAVIPANEL_H

// End of file
