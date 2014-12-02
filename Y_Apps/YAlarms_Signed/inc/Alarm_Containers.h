/* 	
	Copyright (c) 2001 - 2007, 
	Dr. Jukka Silvennoinen.
	All rights reserved 
*/

#ifndef __ALARM_ALARM_CONTAINER_H__
#define __ALARM_ALARM_CONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <AknProgressDialog.h>
#include <documenthandler.h> 
#include <aknserverapp.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <ASCliSession.h>
#include <ASShdAlarm.h>

#include "Status_Control.h"
#include "MenuNotes_Control.h"
/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
class CGulIcon;
class CMyMenuControl;
class CMyMultiControl;

class CMyAlarmItem : public CBase
  { 
public:
    CMyAlarmItem(void){};
	~CMyAlarmItem(){delete iMessage;};
public:
	HBufC*			iMessage;
	TTime			iTime;
	TBool 			iClockAlarm;
	TAlarmStatus	iStatus;
	TAlarmRepeatDefinition iRepeat;
	TAlarmId		iAlarmId;
   };
 
/*
----------------------------------------------------------------
*/
class CAlarmContainer : public CCoeControl,MMyMenuProvided
    {
    enum TRepeatCommands
    	{
    	ENext24Rep,
    	EOnceRep,
    	EDailyRep,
    	EWorkDaysRep,
    	EMondaysRep,
    	ETusdaysRep,
    	EWednesdaysRep,
    	EThursdaysRep,
    	EFridaysRep,
    	ESaturdaysRep,
    	ESundaysRep
    	};
public:
    CAlarmContainer(MMyGrapProvider& aProvider);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
     ~CAlarmContainer();
     CCoeControl* ComponentControl( TInt aIndex) const;
	 TInt CountComponentControls() const;
     void SetAlarmDataL(CMyAlarmItem* aData);
public:	
	void SetTextColot(const TRgb& aColor,const TRgb& aSelColor);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	void DrawItem(CWindowGc& aGc,const TRect& aRect,TInt aItem, TBool aSelected) const;
	TBool SetAlarmL(TTime& aAlarmTime);
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts);
	void GetFileName(TDes& aFileName);
private:
	TBool IsShowingMenu(void) const;
	void CheckLinesValues();
	void ShowNoteL(const TDesC& aMessage);
	virtual void SizeChanged();
 private:
	CMyAlarmItem*		iMyAlarmItem;
 	MMyGrapProvider& 	iProvider;
	TRect				iScrollRect;
	TInt				iItemCount,iMaxItems, iCurrentItem,iFirstInList;
	TRgb				iItemTxtColor,iItemTxtColorSel;
	CMyMenuControl*		iMyMenuControl;
	TBool				iShowMenu;
	CMyMultiControl*	iMyMultiControl;
};



#endif // __ALARM_ALARM_CONTAINER_H__
