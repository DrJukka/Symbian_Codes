/* 	Copyright (c) 2006, 
	Jukka Silvennoinen,
 	All rights reserved 
*/

#ifndef __YUCCATOOLS_BASICAPPSTUFF_H__
#define __YUCCATOOLS_BASICAPPSTUFF_H__

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
		void FileTypeL(TInt aIndex,TDes& aFileType);
		CGulIcon* GetIconL(TInt aIndex, TSize aSize);
		HBufC* GetCreditsL(void);
	private:
		CGulIcon* AddIconFromFileL(const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk);
	};
	
	

#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

