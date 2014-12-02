/* ====================================================================
 * File: MainListView.h
 * Created: 01/07/07
 * Author: Jukka Silvennoinen
 * Copyright (c): 2007, All rights reserved
 * ==================================================================== */

#ifndef __MAINLISTVIEW_H__
#define __MAINLISTVIEW_H__

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

_LIT(KtxAppUid							,"AppUid:\n\t0x");
_LIT(KtxAppFullName						,"\nFullname:\n\t");
_LIT(KtxAppCaption						,"\nCaption:\n\t");
_LIT(KtxAppShortCaption					,"\nShort caption:\n\t");
_LIT(KtxAppEmbeddability				,"\nEmbeddability:\n\t");

_LIT(KtxAppNotEmbeddable				,"Not embeddable");
_LIT(KtxAppEmbeddable					,"Embeddable");
_LIT(KtxAppEmbeddableOnly				,"Embeddable only");
_LIT(KtxAppEmbeddableUiOrStandAlone		,"Embeddable UI or standAlone");
_LIT(KtxAppEmbeddableUiNotStandAlone	,"Embeddable UI not standAlone");

_LIT(KtxAppSupportsNewFile				,"\nSupports new file:\n\t");
_LIT(KtxAppHidden						,"\nHidden:\n\t");
_LIT(KtxAppLaunchBackGround				,"\nLaunch in background:\n\t");


_LIT(KtxYes								,"Yes");
_LIT(KtxNo								,"No");

_LIT(KtxLineFeedAndTab					,"\n\t");
_LIT(KtxLineFeed						,"\n");

_LIT(KtxAppGroupName					,"\nGroup name:\n\t");
_LIT(KtxAppLanguageCode					,"\nUI language code:\n\t");

_LIT(KtxAppOwedFiles					,"\nOwned files:");

_LIT(KtxAppSupportsFileTypes			,"\nSupported file types:\n\t");


/*! 
  @class CMainListView
  
  @discussion An instance of the Application View object for the YScalableExample 
  example application
  */
class CMainListView : public CCoeControl
    {
public:
    static CMainListView* NewL(CEikButtonGroupContainer& aCba);
    static CMainListView* NewLC(CEikButtonGroupContainer& aCba);
     ~CMainListView();
    void MakeListBoxL();
public:  // from CCoeControl
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	void HandleViewCommandL(TInt aCommand);
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aEventCode);
	void SetMenuL(void);
	TBool GetSelUid(TUid& aAppUid);
private:
	TInt GetSelectedIndexL(void);
	void SizeChangedForFindBox();
	CAknSearchField* CreateFindBoxL(CEikListBox* aListBox,CTextListBoxModel* aModel, CAknSearchField::TSearchFieldStyle aStyle);
	HBufC* GetApplicationInfoLC(TUid aAppUid,TDes& aTitle);
	CDesCArray* GetApplicationArrayL(CArrayPtr<CGulIcon>* aIcons);
	void LaunchApplicationL(TUid aAppUid);
	void UpdateScrollBar(CEikListBox* aListBox);
	virtual void SizeChanged();
	void Draw(const TRect& aRect) const;
    void ConstructL();
    CMainListView(CEikButtonGroupContainer& aCba);
private:
	CEikButtonGroupContainer& 			iCba;
	CAknSingleGraphicStyleListBox*	 	iListBox;
	CAknSearchField* 					iFindBox;
	CArrayFixFlat<TUid>*				iAppUIDs;	
    };


#endif // __MAINLISTVIEW_H__
