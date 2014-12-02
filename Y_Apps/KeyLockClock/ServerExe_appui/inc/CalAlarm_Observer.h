
#ifndef __CALALARMOBSERVER_H__
#define __CALALARMOBSERVER_H__

#include <e32base.h>
#include <f32file.h>
#include <ASCliSession.h>


class MMyLogCallBack
{
public:
	virtual void DoShowAlarmNoteL(const TDes& aMessage)=0;
	virtual void DoHideAlarmNoteL(void)=0;
};

class CCalAlarmObserver : public CActive
  {
public: 
  	static CCalAlarmObserver* NewLC(MMyLogCallBack &aCallBack);
  	static CCalAlarmObserver* NewL(MMyLogCallBack &aCallBack);
  	~CCalAlarmObserver();
protected:
	void DoCancel();
  	void RunL();
private: 
	void LogTime(const TTime& aTime, TDes8& aBuffer);
	void StartMonitoring();
  	void ConstructL();
  	CCalAlarmObserver(MMyLogCallBack &aCallBack);
private:
	MMyLogCallBack&	iCallBack;
	RASCliSession	iASCliSession;
	TAlarmId 		iAlarmId;
	TFileName		iMessage;
	TBool			iAlarmIsUp;
	TBool			iAlarmIsSnoozed;
	
	RFs				iFsSession;
	RFile			iFile;
};

#endif //__CALALARMOBSERVER_H__

