/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#ifndef __RSS_FEEDS_CONTAINER_H__
#define __RSS_FEEDS_CONTAINER_H__

#include <COECNTRL.H>
#include <D32DBMS.H>

#include "Public_Interfaces.h"	
#include "YTasks_PublicInterface.h"

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


/*
----------------------------------------------------------------
*/
class CMainContainer : public CYBrowserFileHandler, MTasksHandlerExit,MTimeOutNotify
    {
public:
	void SetUtils(MYBrowserFileHandlerUtils* aFileHandlerUtils);
	void ConstructL(void);
    
     ~CMainContainer();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
public:
	void ShowNoteL(const TDesC& aMessage);
	void SetMenuL(void);
	void OpenFileL(RFile& aOpenFile,const TDesC& aFileTypeID);
	void OpenFileL(const TDesC& aFileName,const TDesC& aFileTypeID);
	void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID);
	void NewFileL(const TDesC& aPath,const TDesC& aFileTypeID,MDesCArray& aFileArray);
	void AddFilesL(const TDesC& aFileName,MDesCArray& aFileArray);
	// activate/deactivate handler
	void SetFocusL(TBool aFocus);
	
	void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void HandleCommandL(TInt aCommand);
protected:
	void TimerExpired(void);
	void HandlerExitL(CYTasksContainer* aHandler);
private:
	TBool IsServerOn(const TDesC& aName);
	CGulIcon* GetMyOwnIconL(const TSize& aSize);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	void MakeGridBoxL(void);
	CDesCArray* GetProfilesArrayL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize);
	void AddIconL(CArrayPtr<CGulIcon>* aIcon,const TSize& aIconsize,const TDesC& aFileName, TInt aImg, TInt aMsk);		
 private:
	MYBrowserFileHandlerUtils* 	iFileHandlerUtils;
	TInt						iResId;
	CAknGrid*					iProfileBox;
	CEcomHandler*				iEcomHandler;
	CYTasksContainer*			iYTasksContainer;
	CTimeOutTimer*				iTimeOutTimer;
};



#endif // __RSS_FEEDS_CONTAINER_H__
