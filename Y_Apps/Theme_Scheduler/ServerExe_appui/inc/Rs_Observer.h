
#ifndef __RSCALLOBSERVER_H__
#define __RSCALLOBSERVER_H__

#include <e32base.h>
#include <centralrepository.h>

class CmyRsObserver;

class MMyRsObsCallBack
{
public:
	virtual void RsValueChangedL(CmyRsObserver* aWhat, TInt aValue,TInt aError)=0;
	virtual void RsValueChangedL(CmyRsObserver* aWhat, const TDesC8& aValue,TInt aError)=0;
};

class CmyRsObserver : public CActive
  {
public: 
  	static CmyRsObserver* NewLC(MMyRsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType);
  	static CmyRsObserver* NewL(MMyRsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType);
  	~CmyRsObserver();
public:   	
	void StartMonitoringL();
  	TInt SetPropertyL(TInt aValue);
  	TInt GetPropertyL(TInt& aValue);
  	TInt GetPropertyL(TDes8& aValue);
	TInt SetPropertyL(const TDesC8 aValue);
protected:
	void DoCancel();
  	void RunL();
private: 
  	void ConstructL();
  	CmyRsObserver(MMyRsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType);
private:
	MMyRsObsCallBack&	iCallBack;
	TUid 				iCategory;
    TUint 				iKey;
    CRepository* 		iRepository; // ReadUserData WriteUserData ReadDeviceData WriteDeviceData
	TBool 				iIntType;
};

#endif //__RSCALLOBSERVER_H__

