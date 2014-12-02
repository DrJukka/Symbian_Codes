/* 	
	Copyright (c) 2001 - 2007, 
	Dr. Jukka Silvennoinen.
	All rights reserved 
*/

#ifndef __ALARM_ABOUT_CONTAINER_H__
#define __ALARM_ABOUT_CONTAINER_H__

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
class CAboutContainer : public CCoeControl
    {
public:
    CAboutContainer(MMyGrapProvider& aProvider);
    void ConstructL(const CCoeControl& aContainer,const TRect& aRect);
     ~CAboutContainer();
public:	
	void SetTextColot(const TRgb& aColor,const TRgb& aSelColor);
    void Draw(const TRect& aRect) const;
	void HandleViewCommandL(TInt aCommand);
	void SetMenuL(void);
	void AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts);
private:
	void OpenBrowserL(const TDesC& aAddr );
	void DrawItem(CWindowGc& aGc,const TRect& aRect,TInt aItem, TBool aSelected) const;
	virtual void SizeChanged();
 private:
 	MMyGrapProvider& 				iProvider;
	TInt							iMaxItems;
	TRgb							iItemTxtColor;
};



#endif // __ALARM_ABOUT_CONTAINER_H__
