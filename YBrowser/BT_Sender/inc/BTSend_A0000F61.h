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
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include "Log.h"

#include "Public_Interfaces.h"
#include "CExampleTimer.h"

class CObjectExchangeClient;
class CMyHelpContainer;
/*
----------------------------------------------
*/	
	class  CMyFileSender : public CYBrowserFileSender
	,MProgressDialogCallback,MExampleTimerNotify,MLog
	{	
	public: 
		~CMyFileSender();
		// default constructors	
		void SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils);
		void ConstructL();
	public:
		// handler settings and support
		void SendFilesL(MDesCArray& aFileArray);
		// activate/deactivate handler
		void SetFocusL(TBool aFocus);
		// commands handling
		void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    	void HandleCommandL(TInt aCommand);
    protected://
        void LogL( const TDesC& aText );
        void LogL( const TDesC& aText, const TDesC& aExtraText );
        void LogL( const TDesC& aText, TInt aNumber );
        void DoneL(TInt aError,const TDesC& aFileName);
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    	void DialogDismissedL(TInt aButtonId);
    	void TimerExpired(TAny* aTimer,TInt aError);
    private:
    	void UpdateScrollBar(CEikListBox* aListBox);
		void DisplayListBoxL(MDesCArray& aFileArray);
    	// functions from CCoeControl, called by Y-Browser
    	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	private:// own functions for operations
		void SetMenuL(void);
		TInt CountComponentControls() const;
		CCoeControl* ComponentControl(TInt aIndex) const;
		void Draw(const TRect& aRect) const;
		virtual void SizeChanged();
	private:	
		MYBrowserFileHandlerUtils* 	iFileHandlerUtils;
		TInt						iSendCount;
		CAknProgressDialog* 		iProgressDialog;
		CEikProgressInfo* 			iProgressInfo;
		CDesCArrayFlat* 			iSendFilesList;
		CExampleTimer*				iExampleTimer;
		CObjectExchangeClient*		iObjectExchangeClient;
		CAknSingleStyleListBox* 	iSelectionBox;
		CAknsBasicBackgroundControlContext*	iBgContext;
		CMyHelpContainer*				iMyHelpContainer;
	};


#endif // __YUCCATOOLS_BASICAPPSTUFF_H__

