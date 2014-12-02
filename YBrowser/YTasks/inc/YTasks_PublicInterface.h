/* 	
	Copyright (c) 2001 - 2007, 
	J.P. Silvennoinen.
	All rights reserved 
*/

#ifndef __YTASKS_PUBLICINTERFACE_H__
#define __YTASKS_PUBLICINTERFACE_H__

#include <E32BASE.H>
#include <COECNTRL.H>
#include <EIKBTGPC.H>
#include <EIKMENUP.H>
#include <ecom\ecom.h>
/*
----------------------------------------------------------------
*/
class CYTasksContainer;


class MTasksHandlerExit
{
public:	
	virtual void HandlerExitL(CYTasksContainer* aHandler) = 0;
};
	


class CYTasksContainer : public CCoeControl
{	
public:	
	virtual void ConstructL(CEikButtonGroupContainer* aCba) = 0;
	virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) = 0;
    virtual CCoeControl* ComponentControl( TInt aIndex) const = 0;
	virtual TInt CountComponentControls() const = 0;
	virtual void HandleCommandL(TInt aCommand) = 0;
	virtual void SetMenuL(void) = 0;
	virtual void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane) = 0;
	virtual void ForegroundChangedL(TBool aForeground) = 0;
public:
	MTasksHandlerExit*			iTasksHandlerExit;
	TUid 						iDestructorIDKey;
};


#endif // __YTASKS_PUBLICINTERFACE_H__
