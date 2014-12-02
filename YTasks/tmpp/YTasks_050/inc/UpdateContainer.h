
#ifndef __UPDATECONTAINER_H__
#define __UPDATECONTAINER_H__

#include<COECNTRL.H>
#include<EIKLBX.H>
#include<GULICON.H>
#include<AknLists.h>
#include <AknProgressDialog.h>
#include <documenthandler.h> 
#include <aknserverapp.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>

#include "InformationGetters.h"
#include "HTTP_Engine.h"

_LIT(KtxNoData				,"Empty list");

/*
-----------------------------------------------------------------------------
*/

class CUpdateContainer : public CCoeControl,MClientObserver
,MProgressDialogCallback
    {
public:
    CUpdateContainer(CEikButtonGroupContainer* aCba,CInfoGetter& aInfoGetter);
    void ConstructL(void);
     ~CUpdateContainer();
public:
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void HandleViewCommandL(TInt aCommand);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
protected:
	void ClientEvent(const TDesC& aEventDescription,TInt aProgress,TInt aFinal);
	void ClientEvent(const TDesC& aEventDescriptionm,TInt aError);
	void ClientBodyReceived(const TDesC& aFileName);
	void ClientCancelled(const TDesC& aFileName,TInt aError);
	void DialogDismissedL (TInt aButtonId);
private:
	void ShowNoteL(const TDesC& aMessage);
	TBool GetFeedL(const TDesC& aUrl,const TDesC& aFileName, TBool aGet);

	virtual void SizeChanged();
	void Draw(const TRect& aRect) const;
	CDesCArray* GetArrayL(CArrayPtr<CGulIcon>* aIcon);
	void MakeListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
 	CEikButtonGroupContainer* 	 iCba;
 	CInfoGetter& 				 iInfoGetter;
	CAknSingleLargeStyleListBox* iMyListBox;
	CHTTPEngine*				 iEngine;
	CAknProgressDialog*			 iProgressDialog;
	CEikProgressInfo*			 iProgressInfo;
};


#endif // __UPDATECONTAINER_H__
