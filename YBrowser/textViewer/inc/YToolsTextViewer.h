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
#include "FileReader.h"

class CMyHelpContainer;

	class  CYBrowserFileHandler1 : public CYBrowserFileHandler
	,MProgressDialogCallback,MMyFileReadObserver
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
		void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID);
		void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID,MDesCArray& aFileArray);
		void AddFilesL(const TDesC& aFileName,MDesCArray& aFileArray);
		// activate/deactivate handler
		void SetFocusL(TBool aFocus);
		// commands handling
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    	void HandleCommandL(TInt aCommand);
    protected:
		void ReadprocessL(const TDesC& aStuff, TInt aLeft);
		void ReadDoneL(TInt aError);
    protected://
    	void DialogDismissedL (TInt aButtonId);
    private:
    	// functions from CCoeControl, called by Y-Browser
    	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl(TInt aIndex) const;
	private:// new functions.
		void MakeEdwinL(void);
		TBool AskToSelectReadStyleL(void);
		TBool AskToSelectConverterL(TUint& aConverter);
	private:// own functions for operations
		void SetMenuL(void);
		void Draw(const TRect& aRect) const;
		virtual void SizeChanged();
	private:	
		MYBrowserFileHandlerUtils* 	iFileHandlerUtils;
	//	TInt						iResId;
		CAknProgressDialog* 		iProgressDialog;
		CEikProgressInfo* 			iProgressInfo;
		CFileReader*				iFileReader;
		RFile 						iDebugFile;
		//CEikGlobalTextEditor* 		iEditor;
		CEikRichTextEditor* 		iEditor;
		CParaFormatLayer* 			iParaformMe;
		CCharFormatLayer* 			iCharformMe;
		CRichText* 					iRichMessageMe;
		SEdwinFindModel* 			iModel;
		CDesCArrayFlat* 			iFindList;
		CDesCArray* 				iReplaceList;
		CMyHelpContainer*			iMyHelpContainer;
	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

