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

class CImageView;

_LIT(KtxtFileExistsReplace			,"File exists, replace ?");
_LIT(KtxtExtractToCurrentFolder		,"Extract files to current folder ?");

_LIT(KtxMBMViewer					,"MBM Viewer");
_LIT(KtxMIFViewer					,"MIF Viewer");
_LIT(KtxIMGViewer					,"Image Viewer");


	class  CYBrowserFileHandler1 : public CYBrowserFileHandler,MAknFileFilter
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
    protected://MZipCompressObserver
    	MYBrowserFileHandlerUtils& GetFileHandler(){ return *iFileHandlerUtils;};
 	protected:// other system interface functions
    	TBool Accept (const TDesC& aDriveAndPath, const TEntry &aEntry) const ;
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
		RFile						iRFile;
		RFile						iDebugFile;
		TInt						iResId;
		CImageView*					iImageView;
		TBuf<50>					iContextName;
		TFileName					iZipFileName;
	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

