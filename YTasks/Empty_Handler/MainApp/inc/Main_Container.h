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
#else

#endif

#include "YTasks_PublicInterface.h"


/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
/*
----------------------------------------------------------------
*/
class CMainContainer : public CCoeControl,MTasksHandlerExit
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
	void HandlerExitL(CYTasksContainer* aHandler);
private:
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
 private:
	CEikButtonGroupContainer* 	iCba;
	CYTasksContainer*			iYTasksContainer;
};



#endif // __RSS_FEEDS_CONTAINER_H__
