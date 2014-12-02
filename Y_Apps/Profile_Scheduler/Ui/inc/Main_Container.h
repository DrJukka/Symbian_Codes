/* Copyright (c) 2001 - 2008, Dr. Jukka Silvennoinen. All rights reserved */

#ifndef __EZANS_MAINCONTAINER_H__
#define __EZANS_MAINCONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include "ThemeDataBase.h"
#include "Common.h"

#ifdef LAL_VERSION    
	#include <MAdtronicApiClient.h>
#endif 

class TProfSchedExt 
    {
    public:
        TProfSchedExt(): iIndex(-1){};
    public:
        TInt 		iIndex;
        TInt		iProfileId;
        TInt		iRepeat;
        TTime		iTime;
        TTime		iNextSchedule;
        TBool		iEnabled;
    };
    
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
	void ShowNoteL(const TDesC& aMessage);
protected:
	void installationValid();
	void installationInvalid();
	void adtronicNotInstalled();
private:
	TTimeIntervalDays AddNextScheduleDays(TDay aDay, TInt aType);
	TTimeIntervalDays CheckDaysType(TDay aDay, TInt aType);
	void ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aProfile,RArray<TProfSched>& aItemArray);
	TBool IsTimeBeforeL(TTime& aTime,TTime& aCompare);
private:
	void SetRandomProfsL(void);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	CDesCArray* GetProfilesArrayL(void);
	void MakeProfileBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	TTimeIntervalDays AddDaysToWorkDays(TDay aDay);
	TTimeIntervalDays AddDaysToNonWorkDays(TDay aDay);
		
 private:
	CEikButtonGroupContainer* 		iCba;
	CAknDoubleTimeStyleListBox* 	iProfileBox;
	CProfileSettings*				iProfileSettings;
	RArray<TProfSchedExt>			iItemArray;
#ifdef LAL_VERSION    
	MAdtronicApiClient* 			iApi;
#endif 		
	CMyHelpContainer*				iMyHelpContainer;
	TBool							iAddIsValid;
};



#endif // __EZANS_MAINCONTAINER_H__
