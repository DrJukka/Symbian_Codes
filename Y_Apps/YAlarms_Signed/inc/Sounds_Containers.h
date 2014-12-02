/* 	
	Copyright (c) 2001 - 2007, 
	Dr. Jukka Silvennoinen.
	All rights reserved 
*/

#ifndef __ALARM_SOUNDS_CONTAINER_H__
#define __ALARM_SOUNDS_CONTAINER_H__

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
#include <drmaudiosampleplayer.h> 

/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
class CGulIcon;


#include "Status_Control.h"

static const TUid KUidCalenAlarmSound = {0x101F874B};
static const TUid KUidClockAlarmSound = {0x101F874D};

/*
----------------------------------------------------------------
*/
class CSoundsContainer : public CCoeControl,MDrmAudioPlayerCallback
    {
public:
    CSoundsContainer(MMyGrapProvider& aProvider);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
     ~CSoundsContainer();
public:	
	void SetTextColot(const TRgb& aColor,const TRgb& aSelColor);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
    void DrawItem(CWindowGc& aGc,const TRect& aRect,TInt aItem, TBool aSelected) const;
	void StopSound(void);
	void PlaySoundL(void);
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts);
protected:
	void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MdapcPlayComplete(TInt aError);
private:
	void CheckFileL(void);
	void CheckFile2L(void);
	void SetFileL(const TDesC& aFileName);
	void SetFile2L(const TDesC& aFileName);
	void SetSnooze2L(const TInt& aVal);
	void SetSnoozeL(const TInt& aVal);
	void ShowNoteL(const TDesC& aMessage);
	virtual void SizeChanged();
 private:
 	MMyGrapProvider& 	iProvider;
	TRect				iScrollRect;
	TInt				iItemCount,iMaxItems, iCurrentItem,iFirstInList;
	TRgb				iItemTxtColor,iItemTxtColorSel;
	TFileName			iClockFile,iCalenderFile;
	TInt				iClockSnoz,iCalenderSnoz;
	CDrmPlayerUtility*	iDrmPlayerUtility;
};




#endif // __ALARM_SOUNDS_CONTAINER_H__
