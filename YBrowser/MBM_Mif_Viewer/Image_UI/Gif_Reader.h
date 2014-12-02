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

	class MGifReadUpdate
	{
	public:
		virtual void ImageLoadedL(TInt aIndex,CFbsBitmap* aImage) = 0;
		};
		
	class CGif_Reader : public CActive
	{
	public:
	    CGif_Reader(MGifReadUpdate* aUpdate,const TDesC& aFileName);
	    TInt ConstructL(void);
	    ~CGif_Reader();
	    void LoadImageL(TInt aIndex);
	protected:
		void DoCancel();
		void RunL();
		void FinnishReadL(void);
	private:
		TPtrC			iFileName;
		MGifReadUpdate* iUpdate;
		TBool 			iCancel;
		TInt 			iError,iCurrImg,iCurrCount;
		CImageDecoder*	iImageDecoder;
		CFbsBitmap*		iFrame;
	};




#endif // __GIF_READER_H__

