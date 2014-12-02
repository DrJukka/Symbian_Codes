/* Copyright (c) 2001-2006,Jukka Silvennoinen, All rights reserved */

#ifndef __YUCCAINFO_H__
#define __YUCCAINFO_H__

// INCLUDES
#include <coecntrl.h>
#include <EIKRTED.H>
	
/*! 
  @class CYuccaInfo
  */


#ifdef __SERIES60_3X__

#else
	
#endif

class CYuccaInfo : public CCoeControl
    {
public: 
	static CYuccaInfo* NewL(const TRect& aRect);
    ~CYuccaInfo();
public:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void AddThreadDetailsL(TFindThread find); 	
	void AddProcessDetailsL(TFindProcess find);
private: // from CoeControl
	void ConstructL(const TRect& aRect);
	CYuccaInfo();
	void MakeEditorL(void);
	void AddApplicationDetailsL(TInt aUidVal);
	virtual void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
private:
	CEikButtonGroupContainer* 	iCba;
	CEikRichTextEditor* 		iEditor;
	CParaFormatLayer* 			iParaformMe;
	CCharFormatLayer* 			iCharformMe;
	CRichText* 					iRichMessageMe; 
	TFileName 					iProcessName;
   };

#endif // __YUCCAINFO_H__
