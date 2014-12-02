/* 	
	Copyright (c) 2001 - 2007, 
	Dr. Jukka Silvennoinen.
	All rights reserved 
*/

#ifndef __ALARM_UPDATE_CONTAINER_H__
#define __ALARM_UPDATE_CONTAINER_H__

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

/*
----------------------------------------------------------------
*/
class CUpdateContainer : public CCoeControl
    {
public:
    CUpdateContainer(MMyGrapProvider& aProvider);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
     ~CUpdateContainer();
public:	
	void SetTextColot(const TRgb& aColor,const TRgb& aSelColor);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts);
private:
	void DrawItem(CWindowGc& aGc,const TRect& aRect,const TDesC& aText, TBool aSelected) const;
	void ShowNoteL(const TDesC& aMessage);
	virtual void SizeChanged();
 private:
 	MMyGrapProvider& 				iProvider;
	TRect							iScrollRect;
	TInt							iMaxItems, iCurrentItem,iFirstInList;
	TRgb							iItemTxtColor,iItemTxtColorSel;
	CArrayFix<TPtrC>* 				iTextArray;
	HBufC*							iTextBuffer;
};



#endif // __ALARM_UPDATE_CONTAINER_H__
