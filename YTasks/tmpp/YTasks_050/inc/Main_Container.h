/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#ifndef __RSS_FEEDS_CONTAINER_H__
#define __RSS_FEEDS_CONTAINER_H__

#include <COECNTRL.H>
#include <D32DBMS.H>

#ifdef __SERIES60_3X__
	#include <AknLists.h>
	#include <AknProgressDialog.h>
	#include "InformationGetters.h"
	#include "log.h"
#else

#endif

#include <YTasks_PublicInterface.h>
#include "MainServerSession.h"

/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
class CEcomHandler;
class CUpdateContainer;
class CObjectExchangeClient;
class CMailBoxReader;
/*
----------------------------------------------------------------
*/
class CMainContainer : public CCoeControl,MTasksHandlerExit
#ifdef __SERIES60_3X__
,MLog,MInfoObserver,MProgressDialogCallback
#else

#endif
    {
public:
    CMainContainer(CEikButtonGroupContainer* aCba);
    void ConstructL(void);
     ~CMainContainer();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleViewCommandL(TInt aCommand);
public:
	void ForegroundChangedL(TBool aForeground);
	void ShowNoteL(const TDesC& aMessage);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
protected:
	void DialogDismissedL (TInt aButtonId);
	void DoneL(TInt aError,TBool aRealSis);
 	void LogL( const TDesC& aText);
    void LogL( const TDesC& aText, const TDesC& aExtraText);
    void LogL( const TDesC& aText, TInt aNumber);
	void GotAllInfoL(void);
	void HandlerExitL(CYTasksContainer* aHandler);
private:
	TBool IsServerOn(const TDesC& aName);

	CGulIcon* GetMyOwnIconL(const TSize& aSize);
	virtual void SizeChanged();
#ifdef __SERIES60_3X__
	virtual void HandleResourceChange(TInt aType);
	void MakeGridBoxL(void);
#else

#endif
	CDesCArray* GetProfilesArrayL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize);
	void AddIconL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk);
	
	void MakeListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
	CEikButtonGroupContainer* 		iCba;
	CEikListBox*					iProfileBox;
	TBool							iHasGrid;
	CEcomHandler*					iEcomHandler;
	CYTasksContainer*				iYTasksContainer;
#ifdef __SERIES60_3X__
	CUpdateContainer*				iUpdateContainer;
	CInfoGetter*					iInfoGetter;
	CObjectExchangeClient*			iObjectExchangeClient;
	CAknWaitDialog*			 		iProgressDialog;
	CMailBoxReader*					iMailBoxReader;
//#ifdef __SERIES60_3X__
#else

#endif
	RMainServerClient				iMainServerClient;
};



#endif // __RSS_FEEDS_CONTAINER_H__
