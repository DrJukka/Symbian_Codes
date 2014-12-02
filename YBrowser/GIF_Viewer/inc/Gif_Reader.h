/* Copyright (c) 2001-2005, Jukka Silvennoinen, S-One Telecom co., Ltd.. All rights reserved */

#ifndef __GIF_READER_H__
#define __GIF_READER_H__

#include <e32base.h>
#include <coecntrl.h>
#include <w32std.h>
#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h> 
#include <ImageConversion.h>

#include "CTimeOutTimer.h"

_LIT8(KtxTypeImageGif_8			,"image/gif");

const TInt KFrameTimeOut = 200000;// 0.2 sec frame timeout
	
	class CGif_Reader : public CActive,MTimeOutNotify
	{
	public:
	    CGif_Reader(CCoeControl& aParent);
	    void ConstructL(const TDesC& aFileName);
	    ~CGif_Reader();
	    CFbsBitmap* Bitmap();
	protected:
		void TimerExpired();
		void DoCancel();
		void RunL();
		void FinnishReadL(void);
	private:
		CCoeControl& 		iParent;
		TInt 				iCurrImg,iCurrCount;
		CImageDecoder*		iImageDecoder;
		CFbsBitmap*			iFrame;
		CFbsBitmap*			iFrameMsk;
		CFbsBitmap*			iFrameImg;
		CTimeOutTimer*		iTimeOutTimer;
		CFbsBitGc* 			iGraphicsContext;
		CFbsBitmapDevice* 	iBitmapDevice;
	};


#endif // __GIF_READER_H__

