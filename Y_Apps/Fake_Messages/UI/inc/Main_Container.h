/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __EZANS_MAINCONTAINER_H__
#define __EZANS_MAINCONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <EIKBTGPC.H>

#include "MsgDataBase.h"
#include "Common.h"

#ifdef LAL_VERSION    
	#include <MAdtronicApiClient.h>
#endif  

/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CProfileSettings;
class CGulIcon;
class CMyHelpContainer;
/*
----------------------------------------------------------------
*/

class CMainContainer : public CCoeControl
#ifdef LAL_VERSION  
	,public MAdtronicStatus
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
public:
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
protected:
	void installationValid();
	void installationInvalid();
	void adtronicNotInstalled();
private:
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	CDesCArray* GetProfilesArrayL(void);
	void MakeProfileBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
	CEikButtonGroupContainer* 		iCba;
	CAknDoubleTimeStyleListBox* 	iProfileBox;
	CProfileSettings*				iProfileSettings;
	RPointerArray<CMsgSched>		iItemArray;
	CMyHelpContainer*				iMyHelpContainer;
#ifdef LAL_VERSION    
	MAdtronicApiClient* 			iApi;
#endif 		
	TBool							iAddIsValid;
};



#endif // __EZANS_MAINCONTAINER_H__
