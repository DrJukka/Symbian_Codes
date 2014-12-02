/*
* ============================================================================

* ============================================================================
*/

#ifndef MMSEXAMPLECONTAINER_H
#define MMSEXAMPLECONTAINER_H

// INCLUDES
#include <msvapi.h>                         // for MMsvSessionObserver 
#include <mmsclient.h>                      // for CMmsClientMtm
#include <coecntrl.h>
#include <AknProgressDialog.h>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>
#include <EIKRTED.H>
#include <eikprogi.h>
#include <aknwaitdialog.h> 
#include <AknProgressDialog.h>

#include "Contacts_Container.h"
#include "SMS_Sender.h"

// Forward declarations
class CEikButtonGroupContainer;
class CEikMenuPane;
class CAknWaitDialog;
class CMyHelpContainer;
//
// Container class to draw text on screen
//
class CMMSExampleContainer : public CCoeControl, MProgressDialogCallback, public MEikEdwinObserver,MMsvObserver
    {
public:
    void ConstructL(const TRect& aRect);
    ~CMMSExampleContainer();
    CMMSExampleContainer(CEikButtonGroupContainer* aCba);
public:    
	void SetRecipientL(const TDesC& aRecipient);
	void HandleCommandL(TInt aCommand);
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
protected:
	void DialogDismissedL (TInt aButtonId);
protected:
	void HandleStatusChange(MMsvObserver::TStatus aStatus);
	void HandleError(const TInt& aError);
	    
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
	void HandleEdwinEventL(CEikEdwin *aEdwin, TEdwinEvent aEventType);
private: 
	void UpdateCharacterCountL(void);
	void SendNextMessageL(void);
	void MakeRecipientEdL(void);
	void MakeEditorL(void);
	HBufC* GetMessageL(void);
private:
	void ConvertFromUnicodeL(const TDesC& aFrom,TInt& aUncorvetible);
	TBool CheckMessageForUnicodeL(const TDesC& aMessage, TBool& aSaveNow);
	
	void SetMenuL(void);
    void Draw(const TRect& aRect) const;
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
private:
	CEikButtonGroupContainer* 			iCba;
	CAknsBasicBackgroundControlContext*	iBgContext;
    RPointerArray<CMyContact> 			iContactArray;
    CContacsContainer*					iContacsContainer;
    TBool								iIsPenEnabled;
    TBuf<50>							iToBuffer;
    const CFont*						iUseAllFont;
    	
    CEikRichTextEditor* 		iRecEditor;
    CParaFormatLayer* 			iRecParaformMe;
    CCharFormatLayer* 			iRecCharformMe;
    CRichText* 					iRecRichMessageMe;
    
    CEikRichTextEditor* 		iEditor;
    CParaFormatLayer* 			iParaformMe;
    CCharFormatLayer* 			iCharformMe;
    CRichText* 					iRichMessageMe;
    TRect						iEditorRect,iRecleRect,iToRect;
    	
    TRgb						iTextColor,iLineColor;
    TPoint						iLinePoint1,iLinePoint2,iLinePoint3;
    TInt 						iLastCount,iCurrCount;
    
    CSMSSender*					iSMSSender;
    TBool						iIsUnoCode;
    HBufC* 						iMessage;
    TInt						iCurrIndex;
    
    CAknProgressDialog*  		iProgressDialog;
	CEikProgressInfo*    		iProgressInfo;
	
	CMyHelpContainer*			iMyHelpContainer;
    };
    
#endif

