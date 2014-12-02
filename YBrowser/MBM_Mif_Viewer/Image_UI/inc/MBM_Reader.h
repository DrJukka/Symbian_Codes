/* Copyright (c) 2001-2005, Jukka Silvennoinen, S-One Telecom co., Ltd.. All rights reserved */

#ifndef __MBM_READER_H__
#define __MBM_READER_H__

#include <e32base.h>
#include <coecntrl.h>
#include <w32std.h>
#include <e32std.h>
#include <cntdef.h>
#include <cntdb.h> 
	

	class MMBMReadUpdate
	{
	public:
		virtual void ReadDoneL(TInt aCount) = 0;
		};
		
	class CMBM_Reader : public CActive
	{
	public:
	    static CMBM_Reader* NewL(MMBMReadUpdate* aUpdate,const TDesC& aFileName);
	    static CMBM_Reader* NewLC(MMBMReadUpdate* aUpdate,const TDesC& aFileName);
	    ~CMBM_Reader();
	    CFbsBitmap* LoadImageL(TInt aIndex,const TSize aSize);
	    TBool IsMif(void){ return iIsMif;};
	protected:
		void DoCancel();
		void RunL();
		void FinnishReadL(void);
	    CMBM_Reader(MMBMReadUpdate* aUpdate,const TDesC& aFileName);
	    void ConstructL(void);
	private:
		TPtrC					iFileName;
		MMBMReadUpdate* 		iUpdate;
		TBool 					iDone,iCancel,iIsMif;
		TInt 					iError,iCurrCount;
	};




#endif // __MBM_READER_H__

