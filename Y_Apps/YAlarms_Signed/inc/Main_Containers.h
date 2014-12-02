/* 	
	Copyright (c) 2001 - 2007, 
	Dr. Jukka Silvennoinen.
	All rights reserved 
*/

#ifndef __ALARM_MAIN_CONTAINER_H__
#define __ALARM_MAIN_CONTAINER_H__

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

#include "Alarm_Containers.h" 

/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
class CGulIcon;

class CAboutContainer;
class CUpdateContainer;
class CAlarmContainer;
class CSoundsContainer;
class CWorkDContainer;

#include "Status_Control.h"

/*
----------------------------------------------------------------
*/
class CMainContainer : public CCoeControl
    {
public:
    CMainContainer(MMyGrapProvider& aProvider);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
     ~CMainContainer();
public:	
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void SetTextColot(const TRgb& aColor,const TRgb& aSelColor);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void GetAlarmDueInL(const TTime& aTime,TDes& aBuffer);
	void AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts);
private:
	void ShowNoteL(const TDesC& aMessage);
	void GetAlarmsArrayL(void);
	virtual void SizeChanged();
	void DrawItem(CWindowGc& aGc,const TRect& aRect, CMyAlarmItem* aItem, TBool aSelected) const;
	void DrawTime(const TRect& aRect,CWindowGc& aGc,TTime aTime) const;
	void DrawNumber(const TRect& aRect,CWindowGc& aGc,CGulIcon* aIcon) const;
	static TInt DoChangeAddImageL(TAny* aPtr);
 private:
 	MMyGrapProvider& 				iProvider;
	CPeriodic*						iFrameReadTimer;
	TRect							iScrollRect;
	TInt							iMaxItems, iCurrentItem,iFirstInList;
	RPointerArray<CMyAlarmItem>		iAlarmsArray;
	TRgb							iItemTxtColor,iItemTxtColorSel;
	CWorkDContainer*				iWorkDContainer;
	CAboutContainer*				iAboutContainer;
	CUpdateContainer*				iUpdateContainer;
	CAlarmContainer*				iAlarmContainer;
	CSoundsContainer*				iSoundsContainer;
};



#endif // __ALARM_MAIN_CONTAINER_H__
