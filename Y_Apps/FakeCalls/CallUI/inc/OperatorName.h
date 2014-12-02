#ifndef __PHONEEXAMPLE_OPNAME_H__
#define __PHONEEXAMPLE_OPNAME_H__

#include <Etel3rdParty.h>


class MOperatorNameCallback
	{
	public:
		virtual void OperatorNameL(const TDesC& aOpName, TInt aError) = 0;
	};
		
	
class CGetOperatorName : public CActive
  { 
public:// public constructors & destructor
	static CGetOperatorName* NewLC(MOperatorNameCallback &aCallBack);
  	static CGetOperatorName* NewL(MOperatorNameCallback &aCallBack);
	~CGetOperatorName();
protected: // from CActive
    void RunL();
    void DoCancel();
private:// private constructors
    CGetOperatorName(MOperatorNameCallback& aCallBack);
    void ConstructL(void);
private:
	MOperatorNameCallback& 			iCallBack;
    CTelephony* 					iTelephony;
    CTelephony::TOperatorNameV1  	iIdV1;    
    CTelephony::TOperatorNameV1Pckg iIdV1Pkg;
    TBool   iGettingOperator;
    CTelephony::TNetworkNameV1      iIdV2;    
    CTelephony::TNetworkNameV1Pckg  iIdV2Pkg;
   };

#endif //__PHONEEXAMPLE_OPNAME_H__
