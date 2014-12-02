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
#include "Gif_Reader.h"


_LIT(KtxTypeImageGif			,"image/gif");

_LIT(KMyResourceFileName		,"\\resource\\apps\\YTools_GIFUI_res.RSC");


	class  CYBrowserFileHandler1 : public CYBrowserFileHandler
	{	
	public: 
		~CYBrowserFileHandler1();
		// default constructors	
		void SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils);
		void ConstructL();
	public:
		// file opening function.
		void OpenFileL(RFile& /*aOpenFile*/,const TDesC& /*aFileTypeID*/){}; // never called, since not supported, by this plug-in
		void OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID);
		void NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/){}; // never called, since not supported, by this plug-in
		void NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/,MDesCArray& /*aFileArray*/){}; // never called, since not supported, by this plug-in
		void AddFilesL(const TDesC& /*aFileName*/,MDesCArray& /*aFileArray*/){}; // never called, since not supported, by this plug-in
		// activate/deactivate handler
		void SetFocusL(TBool aFocus);
		// commands handling
		void DynInitMenuPaneL(TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/){};
    	void HandleCommandL(TInt aCommand);
    protected://MZipCompressObserver
    	MYBrowserFileHandlerUtils& GetFileHandler(){ return *iFileHandlerUtils;};
	private:// own functions for operations
		void Draw(const TRect& aRect) const;
	private:	
		MYBrowserFileHandlerUtils* 	iFileHandlerUtils;
		TInt						iResId;
		CGif_Reader*				iGif_Reader; 
		
	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

