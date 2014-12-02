/* Copyright (c) 2001 - 2007 , J.P. Silvennoinen. All rights reserved */

#ifndef __THEMES_MAINCONTAINER_H__
#define __THEMES_MAINCONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <AknsPkgID.h>

#include "Theme_Settings.h"
#include "ThemeDataBase.h"
#include "CTimeOutTimer.h"
#include "ThemePreview_Container.h"
/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CThemeSettings;
class CGulIcon;
class CPreviewContainer;

/*
----------------------------------------------------------------
*/
class CMainContainer : public CCoeControl
    {
public:
    CMainContainer(CEikButtonGroupContainer* aCba,TUint aRegStatus,TInt aSpeed);
    void ConstructL(void);
     ~CMainContainer();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
public:
	TBool GetSetThemeAndCurrentL(TUint32& aCurrent,TUint32& aSet);
	void CheckChangesOnValues(TBool RegOk);
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
	TBool SelectTimesL(TInt& aTimes);
	TBool SelectStartTimeL(TInt& aHour);
	void ScheduleSelectedThemesL(CScheduleDB& aScheduleDB,CArrayFixFlat<TUint32>& aThemes);
	void SetThemesStuffL(CScheduleDB& aScheduleDB);
	void ResolvePreviousSchduleTimeL(TTime& aNextSchedule, TInt& aIndex,RArray<TThemeSched>& aItemArray);
	void ResolveNextSchduleTimeL(TTime& aNextSchedule, TInt& aIndex,RArray<TThemeSched>& aItemArray);
	TBool SelectThemeL(TUint32& aThemeId);
	TBool IsDrawerOn(void);
	void GetCurrentSkinIdL(void);
	TBool SetOriginalSkinOnL(TAknSkinSrvSkinPackageLocation aLocation);
	virtual void SizeChanged();
	virtual void HandleResourceChange(TInt aType);
	CDesCArray* GetProfilesArrayL(void);
	void MakeProfileBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
	CEikButtonGroupContainer* 		iCba;
	CAknDoubleTimeStyleListBox* 	iProfileBox;
	CThemeSettings*					iThemeSettings;
	RArray<TThemeSched>				iItemArray;
	CPreviewContainer*				iPreviewContainer;
	TUint 							iRegStatus;
	TInt							iSpeed;
	TAknsPkgID 						iCurrentThemeId;
};



#endif // __THEMES_MAINCONTAINER_H__
