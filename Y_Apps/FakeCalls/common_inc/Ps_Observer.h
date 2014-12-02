
#ifndef __CALLOBSERVER_H__
#define __CALLOBSERVER_H__

#include <e32base.h>
#include <e32property.h>

class MMyPsObsCallBack
{
public:
	virtual void PsValueChangedL(TInt aValue,TInt aError)=0;
	virtual void PsValueChangedL(const TDesC8& aValue,TInt aError)=0;
};

class CmyPsObserver : public CActive
  {
public: 
  	static CmyPsObserver* NewLC(MMyPsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType);
  	static CmyPsObserver* NewL(MMyPsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType);
  	~CmyPsObserver();
  	void Start(TBool aDefine);
  	
  	TInt SetPropertyL(TInt aValue);
  	TInt GetPropertyL(TInt& aValue);
  	TInt GetPropertyL(TDes8& aValue);
	TInt SetPropertyL(const TDesC8 aValue);
protected:
	void DoCancel();
  	void RunL();
private: 
  	void ConstructL();
  	CmyPsObserver(MMyPsObsCallBack &aCallBack,const TUid& aCategory, TUint aKey, TBool aIntType);
private:
	MMyPsObsCallBack&	iCallBack;
	TUid 				iCategory;
    TUint 				iKey;
	RProperty 			iProperty;
	TBool 				iIntType;
};

#endif //__CALLOBSERVER_H__

