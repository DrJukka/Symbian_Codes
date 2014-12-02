/* 
	Copyright (c) 2001 - 2008, 
	Dr. Jukka Silvennoinen. 
	All rights reserved 
*/

#ifndef __RSS_FEEDS_CONTAINER_H__
#define __RSS_FEEDS_CONTAINER_H__

#include <COECNTRL.H>
#include <AknLists.h>
#include <D32DBMS.H>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>


	
/*
----------------------------------------------------------------
*/
class CEikButtonGroupContainer;
class CAknDoubleNumberStyleListBox;
class CEikMenuPane;
class CEikListBox;
class CGulIcon;
class CSendFriendContainer;
class CEikRichTextEditor;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;

class CFakeSMSSender;
/*
----------------------------------------------------------------
*/
class CSMSContainer : public CCoeControl
    {
public:
    CSMSContainer(void);
    void ConstructL(void);
     ~CSMSContainer();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void Draw(const TRect& aRect) const;
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
public:
	HBufC* GetSMSMessageL(void);
	void AddTextToEditorL(const TDesC& aText);
protected:
	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
private:
	void UpdateCharacterCountL(void);
	
	void SetTextsL(const TDesC& aText,CRichText& aRichText);
	void MakeEditorL(void);
	virtual void SizeChanged();
private:
	CAknsBasicBackgroundControlContext*	iBgContext;
	const CFont*				iUseAllFont;
	CEikRichTextEditor* 		iEditor;
	CParaFormatLayer* 			iParaformMe;
	CCharFormatLayer* 			iCharformMe;
	CRichText* 					iRichMessageMe;
	TRgb						iTextColor;
	TInt 						iLastCount,iCurrCount;
};



#endif // __RSS_FEEDS_CONTAINER_H__
