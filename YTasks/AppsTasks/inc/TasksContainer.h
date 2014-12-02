
#ifndef __TASKSCONTAINER_H__
#define __TASKSCONTAINER_H__

#include<COECNTRL.H>
#include<EIKLBX.H>
#include<GULICON.H>
#include<AknLists.h>

_LIT(KtxNoData				,"Empty list");

/*
-----------------------------------------------------------------------------
*/

class CTasksContainer : public CCoeControl
    {
public:
    CTasksContainer(void);
    void ConstructL(void);
     ~CTasksContainer();
public:
	HBufC* GetTaskInfoL(TDes& aTitle);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
	void HandleViewCommandL(TInt aCommand);
private:
	void AddProcessDetailsL(RProcess& ph,TDes& aBuffer);
	void AddThreadDetailsL(RThread aThread,TDes& aBuffer);
	void AddApplicationDetailsL(TUid aUid, TDes& aBuffer, TDes& aName);
	virtual void SizeChanged();
	void Draw(const TRect& aRect) const;
	CDesCArray* GetArrayL(CArrayPtr<CGulIcon>* aIcon);
	void MakeListBoxL(void);
	void UpdateScrollBar(CEikListBox* aListBox);
 private:
	CAknDoubleLargeStyleListBox* 	iMyListBox;
	CArrayFixFlat<TUid>*			iAppUIDs;
};


#endif // __TASKSCONTAINER_H__

