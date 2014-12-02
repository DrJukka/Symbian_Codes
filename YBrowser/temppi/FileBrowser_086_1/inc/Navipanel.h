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
		static CYuccaNavi* NewL();
		~CYuccaNavi();
		void SetTextL(const TDesC& aText);
		void GetTextL(TDes& aText);
	//	void ReFresh(void);
	private:
		void TimerExpired();
		CYuccaNavi();
		void ConstructL(void);
	private:
		CAknNavigationControlContainer* iNaviPane;
        CAknNavigationDecorator* 	iNaviDecorator;
        HBufC* 						iFullText;
        CTimeOutTimer*				iTimeOutTimer;
		TBool						iGoingRight;
		TInt						iCurrentCut,iExtraCutCount;
    };

#endif // YUCCA_NAVIPANEL_H

// End of file
