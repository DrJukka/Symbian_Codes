/* Copyright (c) 2001-2009, Dr. Jukka Silvennoinen, All rights reserved */

#ifndef __MYHELPCONTAINER_H__
#define __MYHELPCONTAINER_H__

// INCLUDES
#include <coecntrl.h>
#include <EIKRTED.H>
#include <AknsBasicBackgroundControlContext.h> 
#include <aknsdrawutils.h>
#include <aknscontrolcontext.h>
#include <AknsSkinInstance.h>
#include <aknsutils.h>	
/*! 
  @class CMyHelpContainer
  */


class CMyHelpContainer : public CCoeControl
    {
public: 
	static CMyHelpContainer* NewL();
    ~CMyHelpContainer();
public:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void ConstructL();
	void AddTextToEditorL();
private: // from CoeControl
	virtual void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId); // needed for skins stuff
private:
	CEikRichTextEditor* 	iEditor;
	CParaFormatLayer* 		iParaformMe;
	CCharFormatLayer* 		iCharformMe;
	CRichText* 				iRichMessageMe; 
	CAknsBasicBackgroundControlContext*	iBgContext; // skins are drawn with this one
	TBool                  iMirroRed;
   };


#endif // __MYHELPCONTAINER_H__
