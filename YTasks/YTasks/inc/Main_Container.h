/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#ifndef __RSS_FEEDS_CONTAINER_H__
#define __RSS_FEEDS_CONTAINER_H__

#include <COECNTRL.H>
#include <D32DBMS.H>
#include <AknLists.h>

#include <YTasks_PublicInterface.h>
#include "MainServerSession.h"
#include "CTimeOutTimer.h"
/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
class CEcomHandler;
class CAknGrid;
class CMyHelpContainer;

/*
----------------------------------------------------------------
*/
class CMainContainer : public CCoeControl,MTasksHandlerExit,MTimeOutNotify
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
	void TimerExpired(void);
	void HandlerExitL(CYTasksContainer* aHandler);
private:
	void UpdateScrollBar(CEikListBox* aListBox);
	void GetValuesL(CDictionaryFileStore* aDStore,TInt aUID,TUint32& aValue);
	void GetSettingsL(void);
	void SaveValuesL(CDictionaryFileStore* aDStore,TInt aUID, TUint32 aValue);
	void SaveValuesL(void);
	void SetGridOnL(const TBool& aGridOn);
	TInt HandleViewSelectedIndex(void);
	TBool IsServerOn(const TDesC& aName);
	CGulIcon* GetMyOwnIconL(const TSize& aSize);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void MakeGridBoxL(void);
	void MakeListBoxL();
	CDesCArray* GetProfilesArrayL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize);
	void AddIconL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk);		
 private:
	CEikButtonGroupContainer* 		iCba;
	CAknGrid*						iProfileBox;
	CAknDoubleLargeStyleListBox* 	iMyListBox;
	CEcomHandler*				iEcomHandler;
	CYTasksContainer*			iYTasksContainer;
	RMainServerClient			iMainServerClient;
	CTimeOutTimer*				iTimeOutTimer;
	TBool 						iGridIsOn;
	CMyHelpContainer*			iMyHelpContainer;
};



#endif // __RSS_FEEDS_CONTAINER_H__
