/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/
#ifndef __INFOVIEW_H__
#define __INFOVIEW_H__

// INCLUDES
#include <coecntrl.h>
#include <EIKRTED.H>


	
/*! 
  @class CInfoView
  */


class CInfoView : public CCoeControl
    {
public: 
	static CInfoView* NewL(const TUid aAppUid);
    ~CInfoView();
public:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
private: // from CoeControl
	HBufC* GetApplicationInfoLC(TUid aAppUid,TDes& aTitle);
	CInfoView(const TUid aAppUid);
	void ConstructL(void);
	void MakeEditorL(void);
	void AddTextToEditorL(const TDesC& aStuff);
	virtual void SizeChanged();
    void Draw(const TRect& aRect) const;
private:
	TUid 					iAppUid;
	CEikRichTextEditor* 	iEditor;
	CParaFormatLayer* 		iParaformMe;
	CCharFormatLayer* 		iCharformMe;
	CRichText* 				iRichMessageMe; 
	const CFont*			iTitleFont;
	TFileName				iNameBuffer;
   };

#endif // __INFOVIEW_H__
