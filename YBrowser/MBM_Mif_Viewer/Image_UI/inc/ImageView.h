/* 	
	Copyright (c) 2001 - 2006 , 
	J.P. Silvennoinen. 
	All rights reserved 
*/
#ifndef __MG_IMAGEVIEW_H__
#define __MG_IMAGEVIEW_H__

#include <coecntrl.h>
#include <MdaImageConverter.h>
#include <aknprogressdialog.h> 
#include <aknwaitdialog.h> 
#include <EIKMENUP.H>
#include <Aknslider.h>


#include "MBM_Reader.h"
#include "CTimeOutTimer.h"

class MYBrowserFileHandlerUtils;
// Container 
//

class CZoomSelect : public CCoeControl
    {
public:
	static CZoomSelect* NewL(TInt aMax,TInt aMin, TInt aCurrent,TInt aStep, const TRect aRect);
	static CZoomSelect* NewLC(TInt aMax,TInt aMin, TInt aCurrent,TInt aStep, const TRect aRect);
	~CZoomSelect();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	TInt GetValue(void);
	void SetNewRect(const TRect aRect);
	void SetLabels(const TDesC& aMin,const TDesC& aMax,const TDesC& aTitle);
private:
	void ConstructL(const TRect aRect);
	CZoomSelect(TInt aMax,TInt aMin, TInt aCurrent,TInt aStep);
    void Draw(const TRect& aRect) const;
private:
	CAknSlider* iSlider;
	TInt 		iMax,iMin,iCurrent,iStep;
	TBuf<20>	iMinLabel,iMaxLabel,iTitleLabel;
};

	
class CImageView : public CCoeControl, public MMdaImageUtilObserver
,MProgressDialogCallback,MMBMReadUpdate,MTimeOutNotify
    {
public:
	static CImageView* NewL(MYBrowserFileHandlerUtils* aUtils, const TRect& aRect);
	static CImageView* NewLC(MYBrowserFileHandlerUtils* aUtils, const TRect& aRect);
	~CImageView();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	CFbsBitmap* GetPicture();
	TBool CurrZoomState(void){return iZoomState;};
	void DialogDismissedL (TInt aButtonId);
	void SetMenuL(void);
	void HandleViewCommandL(TInt aCommand);
	void InitMenuPanelL(TInt aResourceId, CEikMenuPane* aMenuPane);
	CCoeControl* ComponentControl( TInt aIndex) const;
	TInt CountComponentControls() const;
public:
	void OpenFileL(const TDesC& aFileName);
	void OpenMBMFileL(const TDesC& aFileName);
protected:
	void ReadDoneL(TInt aCount);
	void TimerExpired();
	void ImageLoadedL(TInt aIndex,CFbsBitmap* aImage);
private:
	void ChangeFrameL(TBool aNext);
	TBool CheckHasEnoughMemory(TInt& aMemory);
	virtual void SizeChanged();
	void SetImageDrawRect(TRect& aImgRect,const TRect& aDrawRect) const;
	TBool Flip90or270Ok(void);
	void CheckCurrentPoint(void);
	void FlipImageL(TBool aVertical, TBool aHorizontal,TBool aRotate);
	void RevercePixelBuffer(TDes8& aTarget,const TDesC8& aBuffer,TInt aBitsPerPixel, TInt aLength);
	CFbsBitmap* FlipVertToHorL(CFbsBitmap* aBitmap);
	void MiuoOpenComplete(TInt aError);
	void MiuoCreateComplete(TInt aError);
	void MiuoConvertComplete(TInt aError);
	CImageView(MYBrowserFileHandlerUtils* aUtils);
    void ConstructL(const TRect& aRect);
    void Draw(const TRect& aRect) const;
private:
	MYBrowserFileHandlerUtils* 		iUtils;
	CFbsBitmap* 					iBitmap;
	CMdaImageFileToBitmapUtility* 	iOpener;
	TPoint 							iCurrPoint;
	TInt 							iZoomState,iHigherOrLower,iHighLowAdjust;
	TInt 							iImageCount,iCurrentImage;
	CAknWaitDialog* 				iWaitDialog;
	CZoomSelect* 					iZoomSelect;
	CMBM_Reader*					iMBM_Reader;
	TBool							iFullScreen;
	CTimeOutTimer*					iTimeOutTimer;
	TInt 							iSlideTime;
    };



#endif // __MG_IMAGEVIEW_H__
