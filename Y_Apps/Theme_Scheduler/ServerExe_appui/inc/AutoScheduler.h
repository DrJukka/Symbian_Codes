/* 
	Copyright (c) 2001 - 2007, 
	Jukka Silvennoinen 
	S-One Telecom LTd. All rights reserved 
*/
#ifndef __AUTO_SCHEDULER__
#define __AUTO_SCHEDULER__

#include <e32base.h>
#include <S32FILE.H>
#include <centralrepository.h>
#include <aknseffectanim.h>
#include <aknsitemdef.h>
#include <aknspkgid.h>
#include <aknspointerstore.h>
#include <aknsrendererwrapper.h>
#include <aknsskinuid.h>
#include <aknssrvchunklookup.h>
#include <aknssrvclient.h>
#include <aknssrvskininformationpkg.h>
#include <aknssrvvariant.hrh>
#include <AknSkinsInternalCRKeys.h>
#include <aknsbasicbackgroundcontrolcontext.h> 

#include "CTimeOutTimer.h"

	class MScheduleObserver
	{
	public:
		virtual void StuffDoneL(TInt aError) = 0;
		};

class CAutoScheduler : public CBase,MTimeOutNotify, public MAknsSkinChangeObserver
    {
public : 
    static CAutoScheduler* NewL(MScheduleObserver& aUpdate);
    static CAutoScheduler* NewLC(MScheduleObserver& aUpdate);
    ~CAutoScheduler();
	void NewScheduleL(TTime aNewTime, TUint32 aTheme,const TDesC& aImageName);
	void DoDoTheStuffNowL(const TUint32& aThemeId,const TDesC& aImageName);
	void ReFreshRandomL(void);
protected:
	void SkinContentChanged ();
	void SkinConfigurationChanged (const TAknsSkinStatusConfigurationChangeReason aReason);
	void SkinPackageChanged (const TAknsSkinStatusPackageChangeReason aReason);
	void TimerExpired();
private: 
	TUint32 GetRandomThemeL(void);
	void ShowNoteL(const TDesC& aMessage);
	void SetNewSkinIdL( TAknsPkgID aPkgId );
	void LogCurrentTime(void);
	void LogScheduleTime(void);
	CAutoScheduler(MScheduleObserver& aUpdate);
    void ConstructL(void);
    void GetCurrentSkinID(TAknsPkgID& aPkgId);
private:
	MScheduleObserver& 	iUpdate;
	CTimeOutTimer* 		iTimeOutTimer;
    TTime				iTime;
	TUint32 			iTheme;
	TBool 				iHasTimed;
	RAknsSrvSession		iAknsSrvSession;
	CArrayFixFlat<TUint32>*	iRandomArray;
	RFs 				iFsSession;
	TUint32 			iLastRandom;
	RFile				iFile;
	TFileName			iImageFile;
    };


#endif //__AUTO_SCHEDULER__

