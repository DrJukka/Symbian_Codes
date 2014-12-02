/* 
	Copyright (c) 2006-2009, 
	Dr. Jukka Silvennoinen
	All rights reserved 
*/
#ifndef __POPUPALARM_PLAYADAPTER__
#define __POPUPALARM_PLAYADAPTER__


#include <MdaAudioSamplePlayer.h>

class MPlaybackObserver 
{
 public:
  virtual void NotifyPlayComplete(TInt aError) = 0;
  virtual void NotifyFileOpened(TInt aError) = 0;
};


class CPlayerAdapter : public CBase, public MMdaAudioPlayerCallback
    {
    enum   
    { 
  	EOk = KErrNone, EGeneralError = -30100, EUnknownMIME = -30101, EVersionNotSupported = -30102, 
  	ESessionError = -30103, ENoRights = -30104, ERightsDBCorrupted = -30105, EUnsupported = -30106, 
  	ERightsExpired = -30107, EInvalidRights = -30108 
	};
public:
	enum TState {
    EClosed,
    EFirstOpen,
    EOpen,
    EPlaying,
    EPaused
  	};
    static CPlayerAdapter* NewL(MPlaybackObserver* aObserver);
    static CPlayerAdapter* NewLC(MPlaybackObserver* aObserver);
    ~CPlayerAdapter();
public: // from MAudioAdapter
	void GetTimeInfo(TInt64& aDuration,TInt64& aPosition);
	void MovepositionL(TInt aPLusMinus);
	TInt State(void);
    void PlayL(void);
    void PauseL(TBool aResetTime);
    void StopL(void);
    void Close(void);  
    void SetVolume(TInt aVolume);
    void OpenL(const TDesC& aFileName);
public: 
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration);
	void MapcPlayComplete(TInt aError);
private:
    CPlayerAdapter(MPlaybackObserver* aObserver); 
    void ConstructL(void);
private:
	TFileName 						iFileName; 
    TInt 							iState;
	TInt 							iError,iVolume;
	TInt64							iCurrentTime;
    CMdaAudioPlayerUtility* 		iMdaAudioPlayerUtility;
    MPlaybackObserver* 				iObserver;
    TBool							iInitOk;
    RFs								iFsSession;
	RFile							iFile;
 };

#endif // __POPUPALARM_PLAYADAPTER__



