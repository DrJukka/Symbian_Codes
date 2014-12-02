/* 

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
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include "Public_Interfaces.h"

#include "YTools_A000257A.hrh"
#include "InformationGetters.h"
#include "HTTP_Engine.h"
#include "Items_Parser.h"


		
	class  CYUpdateContainer : public CYBrowserFileHandler,MInfoObserver,MClientObserver,MDataReceiver,MProgressDialogCallback,MItemParserCallBack
	{	
	public: 
		~CYUpdateContainer();
		// default constructors	
		void SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils){iFileHandlerUtils = aFileHandlerUtils;};
		void ConstructL();
	public:
		// file opening function.
		void OpenFileL(RFile& /*aOpenFile*/,const TDesC& /*aFileTypeID*/){HandleCommandL(EBacktoYBrowser);};
		void OpenFileL(const TDesC& /*aFileName*/,const TDesC& /*aFileTypeID*/){HandleCommandL(EBacktoYBrowser);};
		void NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/,MDesCArray& /*aFileArray*/){HandleCommandL(EBacktoYBrowser);};
		void NewFileL(const TDesC& /*aPath*/,const TDesC& /*aFileTypeID*/){HandleCommandL(EBacktoYBrowser);};
		void AddFilesL(const TDesC& /*aFileName*/,MDesCArray& /*aFileArray*/){HandleCommandL(EBacktoYBrowser);};
		// activate/deactivate handler
		void SetFocusL(TBool aFocus);
		// commands handling
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    	void HandleCommandL(TInt aCommand);
    	
    	void ShowNoteL(const TDesC& aMessage);
    	TBool GetFeedL(const TDesC& aUrl,const TDesC& aFileName, TBool aGet);
    protected:
    	void GotAllInfoL(void);
    	void HTMLDataReceivedL(const TDesC8& aData, TInt aLength);
		void ClientEvent(const TDesC& aText1,const TDesC& aText2,TInt aProgress,TInt aFinal);
		void ClientEvent(const TDesC& aText1,const TDesC& aText2,TInt aError);
		void ClientBodyReceived(const TDesC& aFileName);
		void ClientCancelled(const TDesC& aFileName,TInt aError);
		void DialogDismissedL (TInt aButtonId);
    	void NewsParsedL();
    private:
    	CGulIcon* GetIconL(const TDesC& aName, TInt aIcon, TInt aMask);
    	void ReadFinishedL(TInt /*aError*/);
    	void UpdateScrollBar(CEikListBox* aListBox);
    	TInt CurrentlySelectedLine(void);
    	void DisplayBoxL(TInt aSelected);
    	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    	// functions from CCoeControl, called by Y-Browser
    	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl(TInt aIndex) const;
	private:// own functions for operations
		void SetMenuL(void);
		void Draw(const TRect& aRect) const;
		virtual void SizeChanged();
	private:	
		MYBrowserFileHandlerUtils* 			iFileHandlerUtils;
		TInt								iResId;
		CAknsBasicBackgroundControlContext*	iBgContext;
		TBool						 	iInfoIsReady;
		CInfoGetter*				 	iInfoGetter;
		CHTTPEngine*				 	iHTTPEngine;
		CAknProgressDialog*			 	iProgressDialog;
		CEikProgressInfo*			 	iProgressInfo;
		CRSSFeedParser*				 	iRSSFeedParser;
		TFileName 					 	iInfoXMLFile;
		
		CAknDoubleLargeStyleListBox*	iSmallBox;
	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

