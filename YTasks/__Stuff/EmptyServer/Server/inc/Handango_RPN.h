/*
*  Copyright (c) 2001-2005 J.P.Silvennoinen, S-One Telecom Co., Ltd 
*/

#ifndef RPN_CALCULATOR_H
#define RPN_CALCULATOR_H

#include <E32BASE.H>


const TInt STACK_MAX_SIZE                  = 60;



class CRPNCalculator : public CBase
    {
   
public:
    static CRPNCalculator* NewL(void);
    static CRPNCalculator* NewLC(void);
    ~CRPNCalculator();
    TUint16 ComputeUnlockKey(const TDesC8& aIdString,const TDesC& aRPNString);
	void GetBufferFromNumber(TUint16 aNumber, TDes& aString);
	TInt GetError(TDes& aError);
private: 
    void Error(const TDesC& aSting, TInt aStatus);
    TInt  ParseRPNString(const TDesC& aRPNString);
    void Push(TInt32 aElement);
	TInt32 Pop();
	TInt32 Algorithm(TInt32 aKey, TUint8 aVarI, TUint8 aVarC);
private:
    void ConstructL(void);
    CRPNCalculator();
private:
	TInt 			iError,iNumOperators,iTOSIndex;
	TUint32			iStackOperators[STACK_MAX_SIZE]; 
	TUint32			iStack[STACK_MAX_SIZE];
    TBuf<100> 		iErrBuf;
    };

#endif // RPN_CALCULATOR_H

