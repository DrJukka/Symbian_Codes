/* 
	Copyright (c) 2001 - 2006
	Jukka Silvennoinen 
	All rights reserved 
*/
#ifndef __SONEREGISTRATION_H__
#define __SONEREGISTRATION_H__

#include <E32BASE.H>
#include <E32STD.H>
#include <COEDEF.H>
#include <W32STD.H>
#include <FBS.H>
	
	_LIT(Ktx7DayTrial				,"7");
	_LIT(Ktx15DayTrial				,"5");
	_LIT(Ktx30DayTrial				,"3");

	const TInt KDemo = 0xFF00;
	const TInt KFull = 0x00FF;


	class CSoneSettingsItem 
	{
	public:
		~CSoneSettingsItem(){};
		CSoneSettingsItem(){};
	public:
		TInt 		iType;
		TTime 		iStartTime;
		TTime 		iLastTime;
		TTime 		iEndTime;
		TBuf<24> 	iBuffer;
		TBuf8<16>	iCheck;
	};
	
	class MRegCodeValidator
	{
	public:
		virtual TBool IsCodeOkL(const TDesC& aCode,const TDesC8& aString) = 0;
		};

	
	class CNProtRegister :public CBase
		{
	public:
		CNProtRegister(MRegCodeValidator& aValidator);
		~CNProtRegister();
		
		TBool CheckRegStatusL(TInt& aDaysLeft, TUint& aResult, TBool aThai);
		void TryRegisterApp(TDesC& aBuffer, TUint& aResult, TTime& aStartTime, TBool aThai);
		void BackUpPasswordL(TDes& aNumber);
		void RegistrationNumberL(TDes& aNumber);
		void RegistrationNumberL(TDes& aNumber, const TDesC8& aString);
		void GetImeiL(TDes& aImei);
		void SetIMEI(const TDesC& aImei);
		TBool IsThaiNetWorkL(TDes& aOperator, const TDesC& aCodes);
	private:
		HBufC8* MixOutPutLC(const TDesC8& aString);
	private:
		TBuf<20> 							iImei;
		MRegCodeValidator& 					iValidator;
	};



#endif // __SONEREGISTRATION_H__
