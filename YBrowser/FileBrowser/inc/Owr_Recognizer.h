/* 	Copyright (c) 2006, 
	Jukka Silvennoinen,
 	All rights reserved 
*/

#ifndef __YBROWSER_OWRRECOCNIZER_H__
#define __YBROWSER_OWRRECOCNIZER_H__

#include <aknappui.h>
#include <aknapp.h>
#include <akndoc.h>

#include "Public_Interfaces.h"

class CFtASSItem;

	class CYBOWRRecognizer: public CYBrowserFileRecognizer
	{
	public:	
	//	New methods
		~CYBOWRRecognizer();
		void ConstructL();
		void ReFreshFTOverWritesL(void);
	public:	
		void RecognizeFile(CYBRecognitionResult& aResult,const TDesC& aFileName,const TDesC8& aBuffer);
		TInt MinimumFileSize(void);
		TInt FileTypeCount(void);
		void FileTypeL(TInt aIndex,TDes& aFileType);
		CGulIcon* GetIconL(TInt aIndex, TSize aSize);
		HBufC* GetCreditsL(void);
	private:
		CGulIcon* GetAppIccon(const TDesC& aFileName, TSize aSize);
	private:
		RPointerArray<CFtASSItem> iTypesArray;
	};
	

#endif // __YBROWSER_OWRRECOCNIZER_H__

