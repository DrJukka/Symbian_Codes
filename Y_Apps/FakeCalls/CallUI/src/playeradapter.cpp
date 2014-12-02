/* 
	Copyright (c) 2006-2009, 
	Dr. Jukka Silvennoinen
	All rights reserved 
*/
#include <eikmenup.h>
#include <bautils.h>
#include <EIKENV.H>
#include <aknglobalnote.h> 
#include <AknLists.h>
#include <AknPopup.h>

#include <MdaAudioSampleEditor.h>


#include "playeradapter.h"



/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CPlayerAdapter* CPlayerAdapter::NewL(MPlaybackObserver* aObserver)
    {
    CPlayerAdapter* self = NewLC(aObserver);
    CleanupStack::Pop(self);  
    return self;
    }
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CPlayerAdapter* CPlayerAdapter::NewLC(MPlaybackObserver* aObserver)
    {
    CPlayerAdapter* self = new (ELeave) CPlayerAdapter(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
 
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CPlayerAdapter::CPlayerAdapter(MPlaybackObserver* aObserver)
:iState(CPlayerAdapter::EClosed),iVolume(1),iMdaAudioPlayerUtility(NULL)
,iObserver(aObserver)
    {

    }
 /*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
CPlayerAdapter::~CPlayerAdapter()
 { 
 
	if(iMdaAudioPlayerUtility)
	{		
		iMdaAudioPlayerUtility->Stop();
		iMdaAudioPlayerUtility->Close();

		delete iMdaAudioPlayerUtility;
		iMdaAudioPlayerUtility = NULL;
	}
	
	if(iFile.SubSessionHandle())
	{
	  	iFile.Close();
	}
	iFsSession.Close();
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::ConstructL(void)
{
	iFsSession.Connect();
	
/*	_LIT(KRecFilename			,"C:\\ppdrwPlay.txt");
	iFsSession.Delete(KRecFilename);		
	User::LeaveIfError(iFile.Create(iFsSession,KRecFilename,EFileWrite|EFileShareAny));	
*/	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("start, "));
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::OpenL(const TDesC& aFileName)
{
	iInitOk = EFalse;
	iState = CPlayerAdapter::EClosed;
	
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8("open: "));
		TBuf8<255> jjjjj;
		jjjjj.Copy(aFileName);
		iFile.Write(jjjjj);
		iFile.Write(_L8(" "));
	}
	
	if(BaflUtils::FileExists(CEikonEnv::Static()->FsSession(),aFileName))
	{
		iFileName.Copy(aFileName);
		iState = CPlayerAdapter::EClosed;
	
		if(iFile.SubSessionHandle())
		{	
			iFile.Write(_L8("oppen me, "));
		}
	 	
		if(iMdaAudioPlayerUtility)
		{			
			iMdaAudioPlayerUtility->Stop();
			iMdaAudioPlayerUtility->Close();
			
			delete iMdaAudioPlayerUtility;
			iMdaAudioPlayerUtility = NULL;
		}

		iMdaAudioPlayerUtility = CMdaAudioPlayerUtility::NewFilePlayerL(aFileName,*this,EMdaPriorityNormal,EMdaPriorityPreferenceTimeAndQuality);							  	
  	}
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/

void CPlayerAdapter::PlayL()
{
	if(iFile.SubSessionHandle())
	{	
		iFile.Write(_L8(", play me now, "));
	}
	
	if(iState == CPlayerAdapter::EFirstOpen 
	|| iState == CPlayerAdapter::EOpen)
	{
		if(iMdaAudioPlayerUtility)
  		{
	  		if(iFile.SubSessionHandle())
			{	
				iFile.Write(_L8(", okok, "));
			}
	
	  		iMdaAudioPlayerUtility->SetPosition(TTimeIntervalMicroSeconds((iCurrentTime)));    	
	    	iState = CPlayerAdapter::EPlaying;
    		iMdaAudioPlayerUtility->Play();	
    		SetVolume(iVolume);
  		}
	}
	else if(iState == CPlayerAdapter::EPaused)
	{
		if(iMdaAudioPlayerUtility)
  		{
  			if(iFile.SubSessionHandle())
			{	
				iFile.Write(_L8(", okok, "));
			}
			
			iMdaAudioPlayerUtility->SetPosition(TTimeIntervalMicroSeconds((iCurrentTime)));
	    	iState = CPlayerAdapter::EPlaying;
	    	iMdaAudioPlayerUtility->Play();
	    	SetVolume(iVolume);
  		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::SetVolume(TInt aVolume)
{
	if(iFile.SubSessionHandle())
	{	
		TBuf8<50> volvovl(_L8(",Vol: "));
		volvovl.AppendNum(aVolume);
		iFile.Write(volvovl);
	}
			
	if(iMdaAudioPlayerUtility && iInitOk)
	{
		TInt MaxVol= iMdaAudioPlayerUtility->MaxVolume();
	
		MaxVol = ((MaxVol * aVolume) / 10);
	
		if(iFile.SubSessionHandle())
		{	
			TBuf8<50> volvovl(_L8(",Vol set to: "));
			volvovl.AppendNum(MaxVol);
			iFile.Write(volvovl);
		}
			
	    iMdaAudioPlayerUtility->SetVolume(MaxVol);
	}
	
	if(iVolume >= 0 && iVolume <= 10)
	{
		iVolume = aVolume;
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::PauseL(TBool aResetTime)
{
	if(iState == CPlayerAdapter::EPlaying)
	{
		TTimeIntervalMicroSeconds CurPosition(0);
		
		if(iMdaAudioPlayerUtility)
		{			
			iMdaAudioPlayerUtility->GetPosition(CurPosition);		
			iMdaAudioPlayerUtility->Pause();
			iState = CPlayerAdapter::EPaused;
		}
		
		if(aResetTime)
		{
			iCurrentTime = 0;
		}
		else
		{
			iCurrentTime = CurPosition.Int64();
		}
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::StopL()
{
	Close();
	
	iState = CPlayerAdapter::EClosed;
 	
	if(iMdaAudioPlayerUtility)
	{		
		iMdaAudioPlayerUtility->Stop();
		iMdaAudioPlayerUtility->Close();
		
		delete iMdaAudioPlayerUtility;
		iMdaAudioPlayerUtility = NULL;
	}
	
	iCurrentTime = 0;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::Close()
{	
	if(iMdaAudioPlayerUtility)
	{
		if(iState == CPlayerAdapter::EPlaying)
		{
			iMdaAudioPlayerUtility->Stop();
		}

		iMdaAudioPlayerUtility->Close();		
	}
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
	if(iFile.SubSessionHandle())
	{	
		TBuf8<50> hghghg(_L8(", Oppened: "));
		hghghg.AppendNum(aError);
		iFile.Write(hghghg);
	}
		
	iInitOk = ETrue;
	
	iState = CPlayerAdapter::EFirstOpen;
 	
	SetVolume(iVolume);
	
	if(iObserver)
		iObserver->NotifyFileOpened(aError);
}


/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::MapcPlayComplete(TInt aError)
{
	if(iFile.SubSessionHandle())
	{	
		TBuf8<50> hghghg(_L8(", played: "));
		hghghg.AppendNum(aError);
		iFile.Write(hghghg);
	}

	iState = CPlayerAdapter::EOpen;
	
	if(iObserver)
		iObserver->NotifyPlayComplete(aError);
}

/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
TInt CPlayerAdapter::State(void)
{
	return iState;
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::MovepositionL(TInt aPLusMinus)
{
	if(iMdaAudioPlayerUtility && iInitOk
	&& iState == CPlayerAdapter::EPaused)
	{
		TTimeIntervalMicroSeconds CurPosition(0);
		iMdaAudioPlayerUtility->GetPosition(CurPosition);
		
		TTimeIntervalMicroSeconds EndPosition(iMdaAudioPlayerUtility->Duration());	
		TTimeIntervalMicroSeconds NewPosition(CurPosition.Int64() + (aPLusMinus * 1000000));
		
		if(NewPosition.Int64() < 0)
		{
			NewPosition = 0;	
			iCurrentTime = NewPosition.Int64();
			iMdaAudioPlayerUtility->SetPosition(NewPosition);
		}
		else if(NewPosition.Int64() < EndPosition.Int64())
		{
			iCurrentTime = NewPosition.Int64();
			iMdaAudioPlayerUtility->SetPosition(NewPosition);
		}
	}
}
/*
-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
*/
void CPlayerAdapter::GetTimeInfo(TInt64& aDuration,TInt64& aPosition)
{
	if(iMdaAudioPlayerUtility && iInitOk)
	{
		TTimeIntervalMicroSeconds CurPosition(0);
		iMdaAudioPlayerUtility->GetPosition(CurPosition);
		
		TInt64 HelpPos = CurPosition.Int64();	
		aPosition = HelpPos / 1000;
		
		TInt64 Help = iMdaAudioPlayerUtility->Duration().Int64();
		aDuration = Help / 1000;
	}
	else
	{
		aPosition = 0;
		aDuration = 0;
	}
}
