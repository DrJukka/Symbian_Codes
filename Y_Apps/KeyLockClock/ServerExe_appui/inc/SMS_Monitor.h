
#ifndef __SMSSENDERSSSS_GETTER_H__
#define __SMSSENDERSSSS_GETTER_H__

#include <e32base.h>
#include <F32FILE.H>
#include <d32dbms.h>
#include <BADESCA.H>

#include <MTCLREG.H>


class MInboxCallBack
{
public:
	virtual void InboxChangedL(TInt aSMS,TInt aMMS)=0;
};


	class CSMSMonitor : public CBase, MMsvSessionObserver	    
		{
	public:
		static CSMSMonitor* NewL(MInboxCallBack* aObserver);
		~CSMSMonitor();
	protected:
		void HandleSessionEventL(TMsvSessionEvent aEvent, TAny *aArg1, TAny *aArg2, TAny *aArg3);
	private:
		CSMSMonitor(MInboxCallBack* aObserver);
	    void ConstructL(void);	
	    void CheckInboxL(void);
	private:
		MInboxCallBack* 		iObserver;
		CMsvSession*			iSession;
		CClientMtmRegistry*		iMtmRegistry;
	    };
	    


#endif // __SMSSENDERSSSS_GETTER_H__
