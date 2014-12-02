/* 	Copyright (c) 2006, 
	Jukka Silvennoinen,
 	All rights reserved 
*/

#ifndef __YUCCATOOLS_BASICRECOGNIZER_H__
#define __YUCCATOOLS_BASICRECOGNIZER_H__

#include <aknappui.h>
#include <aknapp.h>
#include <akndoc.h>

#include "Public_Interfaces.h"

	class CYBRecognizer1: public CYBrowserFileRecognizer
	{
	public:	//	New methods
		void RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName,const TDesC8& aBuffer);
		TInt MinimumFileSize(void);
		TInt FileTypeCount(void);
		HBufC* FileTypeL(TInt aIndex,TBool& aPartial);
		HBufC* FileTypeNameL(TInt aIndex);
		CGulIcon* GetIconL(TInt aIndex, TSize aSize);
		HBufC* GetNameL(void);
		HBufC* GetCreditsL(void);
	private:
		CGulIcon* GetListIconL(const TDesC& aFileName,TInt aImage,TInt aMask, TSize aSize);
	};
	

#endif // __YUCCATOOLS_BASICRECOGNIZER_H__

