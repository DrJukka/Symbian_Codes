/*
	Copyright (C) 2006 Jukka Silvennoinen
	All right reserved
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
#include <coecntrl.h>
#include <EIKRTED.H>

#include "Public_Interfaces.h"		
		
class CMailBoxContainer;
class CMyHelpContainer;

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
		void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID);
		void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID,MDesCArray& aFileArray);
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
		MYBrowserFileHandlerUtils* 		iFileHandlerUtils;
		RFile 							iDebugFile;
		CMailBoxContainer*				iMailBoxContainer;
		CMyHelpContainer*				iMyHelpContainer;

	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

