/* ====================================================================
 * File: MainListView.h
 * Created: 01/07/07
 * Author: Jukka Silvennoinen
 * Copyright (c): 2007, All rights reserved
 * ==================================================================== */

#ifndef __NEWTYPESLISTVIEW_H__
#define __NEWTYPESLISTVIEW_H__

#include <coecntrl.h>
#include <EIKBTGPC.H>
#include <EIKMENUP.H>
#include <aknlists.h> 
#include <EIKLBX.H>
#include <aknsfld.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <D32DBMS.H>

class CImeiSettings;

class CMyRecItem: public CBase
    {
    public:
        CMyRecItem():iIndex(-1){};
        ~CMyRecItem(){delete iType;delete iExt; delete iData;};
    public:
    	TInt 		iIndex;
    	HBufC8* 	iType;
        HBufC*  	iExt;
        HBufC8*  	iData;
    };

/*! 
  @class CMyTepesView
  
  @discussion An instance of the Application View object for the YScalableExample 
  example application
  */
class CMyTepesView : public CCoeControl
    {
public:
    static CMyTepesView* NewL(CEikButtonGroupContainer& aCba);
    static CMyTepesView* NewLC(CEikButtonGroupContainer& aCba);
     ~CMyTepesView();
    void MakeListBoxL();
public:  // from CCoeControl
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void HandleViewCommandL(TInt aCommand);
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode);
	void SetMenuL(void);
private:
	TInt GetTypesArrayL(void);
	void CreateTableL(RDbDatabase& aDatabase);
	void DeleteFromDatabaseL(TInt& aIndex);
	void UpdateDatabaseL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExt,TInt& aIndex);
	void SaveToDatabaseL(const TDesC8& aType,const TDesC8& aData,const TDesC& aExt,TInt& aIndex);


	TInt GetSelectedIndexL(void);
	CDesCArray* GetListBoxArrayL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
	virtual void SizeChanged();
	void Draw(const TRect& aRect) const;
    void ConstructL();
    CMyTepesView(CEikButtonGroupContainer& aCba);
private:
	CEikButtonGroupContainer& 	iCba;
	CAknDoubleStyleListBox*	 	iListBox;	
	RPointerArray<CMyRecItem>	iArray;
	CImeiSettings*				iImeiSettings;
    };


#endif // __NEWTYPESLISTVIEW_H__
