/* 
	Copyright (c) 2001 - 2007 ,
	Jukka Silvennoinen. 
	All rights reserved 
*/
#ifndef __MENUNOTES_STATUSCONTROL_H__
#define __MENUNOTES_STATUSCONTROL_H__

#include <coecntrl.h>
#include <EIKRTED.H>
#include <eikmfne.h>      
#include <GULICON.H>
#include <e32base.h>
#include <AknIconUtils.h>

#include "StatusGetters.h"
#include "CTimeOutTimer.h"
class CFbsBitmap;
	
class MMyMenuProvided
	{
	public:
		virtual void AppendMenuL(CArrayFixFlat<TInt>& aCommands,CDesCArray& aTexts) = 0;
		virtual void GetFileName(TDes& aFileName) = 0;
	};


//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMyMenuControl : public CCoeControl
    {
public:
	static CMyMenuControl* NewL(const TRect& aRect,MMyMenuProvided& aMenuProvided,TRgb aFontColor,TRgb aSelColor, TInt aAddInterval);
	~CMyMenuControl();
	TInt SelectedCommand(void);
	void UpAndDown(TBool aUp);
	void ActivateDeActivate(TBool aActive);
	void SetTextColot(const TRgb& aColor,const TRgb& aSelColor);
private:
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode = EAspectRatioPreservedAndUnusedSpaceRemoved);
	virtual void SizeChanged();
    CMyMenuControl(MMyMenuProvided& aMenuProvided,TRgb aFontColor,TRgb aSelColor, TInt aAddInterval);
    void ConstructL(const TRect& aRect);
    static TInt DoChangeAddImageL(TAny* aPtr);
	void Draw(const TRect& aRect) const;
	void DrawItem(CWindowGc& aGc,const TRect& aRect,const TDesC& aText, TBool aSelected) const;
private:	
	MMyMenuProvided& 	iMenuProvided;
	const CFont* 		iUseFont;
	TInt 				iAddInterval;
	TRgb				iFontColor;
	TRgb				iSelColor;
	CArrayFixFlat<TInt>* iCommands;
	CDesCArrayFlat*		 iTexts;
	CGulIcon* 				iListSel;
	RPointerArray<CGulIcon>	iBackAdd;
	CGulIcon* 				iBackMask;
	CFbsBitmap*				iBackGround;
	TInt 				iCurrentItem,iMaxItems,iFirstInList,iCurrentAdd;
	TRect				iScrollRect;
	CPeriodic*			iFrameReadTimer;
	
	CGulIcon* 			iScrollBack;
	CGulIcon* 			iScroller;
};

//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
class CMyMultiControl : public CCoeControl
    {
public:
	enum TEditorType
	{
		EDateTimeEd,
		EMessageEd
	};
	CMyMultiControl(MMyMenuProvided& aMenuProvided,TRgb aFontColor);
	~CMyMultiControl();
	void ConstructL(const TRect& aRect);
	void SetTextColot(const TRgb& aColor);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
 	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
public:
	TEditorType GetType(void){ return iEdType;};
	TTime GetTime(TTime& aOriginal);
	HBufC* GetText(void);
	void StartEditorL(HBufC* aMessage,const TDesC& aTitle);
	void StartTimeEditorL(TTime aTime,const TDesC& aTitle);
	void StartDateEditorL(TTime aTime,const TDesC& aTitle);
private:
	void MakeNewsEditorL();
	void AddTextToNewsEditorL(const TDesC& aText);
	
	CGulIcon* LoadImageL(const TDesC& aFileName, TInt aImg, TInt aMsk,const TSize& aSize,TScaleMode aScaleMode = EAspectRatioPreservedAndUnusedSpaceRemoved);
	virtual void SizeChanged(); 
	void CalculateRects(void);
	void DeleteAllControls(void);
	void Draw(const TRect& aRect) const;
private:	
	MMyMenuProvided& 	iMenuProvided;
	const CFont* 		iUseFont;
	TRgb				iFontColor;
	CGulIcon* 			iBackMask;
	CFbsBitmap*			iBackGround;
	TEditorType			iEdType;
	HBufC*				iTitle;
	TRect				iTitleRect, iControlRect;
	TTime				iSavedTime;
	CEikRichTextEditor* 		iEditor;
	CParaFormatLayer* 			iParaform;
	CCharFormatLayer* 			iCharform;
	CRichText* 					iRichMessage;
	CEikDateEditor*		iDateEditor;
	CEikTimeEditor*		iTimeEditor;
	TInt 				iEdState;
	TInt 				iState1,iState2,iState3,iState4,iState5;
};

#endif // __MENUNOTES_STATUSCONTROL_H__
