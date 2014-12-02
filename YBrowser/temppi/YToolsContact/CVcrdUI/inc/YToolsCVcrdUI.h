/*
	Copyright (C) 2006 Jukka Silvennoinen
	
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


_LIT(KtxtDefaultFileExtension		,".YCC");

_LIT(KtxtFileExistsReplace			,"File exists, replace ?");
_LIT(KtxtExtractToCurrentFolder		,"Extract files to current folder ?");

class CZipCompressor;

	class  CYBrowserFileHandler1 : public CYBrowserFileHandler,MProgressDialogCallback,MAknFileFilter
	{	
	public: 
		~CYBrowserFileHandler1();
		// default constructors	
		void SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils);
		void ConstructL();
	public:
		// handler settings and support
		HBufC* GetSupportedItemTypes(TBool& aPartialString,TUint32& aFlags, TInt aIndex);
		TInt GetSupportedItemTypesCount(void);
		// file opening function.
		void OpenFileL(RFile& aOpenFile,const TDesC& aFileTypeID);
		void OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID);
		void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID,MDesCArray& aFileArray);
		void AddFilesL(const TDesC& aFileName,MDesCArray& aFileArray);
		// activate/deactivate handler
		void SetFocusL(TBool aFocus);
		// commands handling
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    	void HandleCommandL(TInt aCommand);
    protected://MZipCompressObserver
    	MYBrowserFileHandlerUtils& GetFileHandler(){ return *iFileHandlerUtils;};

		void AddFileToListL(TUint32 aCompressedSize,TUint32 aUncompressedSize,const TDesC& aCurrentFile,TInt aIconId);
    protected:// other system interface functions
    	TBool Accept (const TDesC& aDriveAndPath, const TEntry &aEntry) const ;
    	void DialogDismissedL (TInt aButtonId);
    private:
    	TSize GetIconSize(void);
    	// functions from CCoeControl, called by Y-Browser
    	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl(TInt aIndex) const;
	private:// new functions.
	    CDesCArrayFlat* GetSelectedFilesL(void);
	    void UpdateScrollBar(CEikListBox* aListBox);
		void DisplayListBoxL(void);
	private:// own functions for operations
		void SetMenuL(void);
		void Draw(const TRect& aRect) const;
		virtual void SizeChanged();
	private:	
		MYBrowserFileHandlerUtils* 	iFileHandlerUtils;
		RFile						iRFile;
		TFileName					iZipFileName;
		TInt						iResId;
		CDesCArrayFlat*				iFilesArray;
		CAknProgressDialog* 		iProgressDialog;
		CEikProgressInfo* 			iProgressInfo;
		CAknWaitDialog*				iWaitDialog;
		CAknDoubleGraphicStyleListBox* 	iSelectionBox;
		CDesCArrayFlat* 				iSelectionBoxArray;
		RFile						iDebugFile;
	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

