/* 
Copyright (c) 2007 Dr. Jukka Silvennoinen www.DrJukka.com

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. THE SOFTWARE IS ALSO SUBJECT TO CHANGE WITHOUT PRIOR NOTICE, 
NO COMPATIBILITY BETWEEN MODIFICATIONS IS GUARANTEED. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name(s) of the above copyright holders 
shall not be used in advertising or otherwise to promote the sale, use or other 
dealings in this Software without prior written authorization.

*/

#ifndef __YUCCATOOLS_BASICAPPSTUFF_H__
#define __YUCCATOOLS_BASICAPPSTUFF_H__

#include <aknappui.h>
#include <aknapp.h>
#include <akndoc.h>
#include <aknwaitdialog.h> 
#include <AknProgressDialog.h>
#include <EIKPROGI.H>
#include <akncommondialogs.h> 
#include <maknfilefilter.h> 
#include <aknlists.h> 
#include <eikclbd.h>

#include "Public_Interfaces.h"

_LIT(KtxMyTitle						,"File handler");

class CZipCompressor;

	class  CYBrowserFileHandler1 : public CYBrowserFileHandler
	{	
	public: 
		~CYBrowserFileHandler1();
		// default constructors	
		void SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils);
		void ConstructL();
	public:
		// file opening function.
		void OpenFileL(RFile& aOpenFile,const TDesC& aFileTypeID);
		void OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID);
		void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID,MDesCArray& aFileArray);
		void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID);
		void AddFilesL(const TDesC& aFileName,MDesCArray& aFileArray);
		// activate/deactivate handler
		void SetFocusL(TBool aFocus);
		// commands handling
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    	void HandleCommandL(TInt aCommand);
    private:
    	// functions from CCoeControl, called by Y-Browser
    	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl(TInt aIndex) const;
	private:// own functions for operations
		void SetMenuL(void);
		void Draw(const TRect& aRect) const;
		virtual void SizeChanged();
	private:	
		MYBrowserFileHandlerUtils* 	iFileHandlerUtils;
		TInt						iResId;
	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

