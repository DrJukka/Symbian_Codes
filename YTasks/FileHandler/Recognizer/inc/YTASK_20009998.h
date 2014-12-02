// RecogEx.H
//
//
// Copyright (C) 2003 Nokia Corporation. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__RECOGEX_H__)
#define __RECOGEX_H__

#include <apmrec.h>
//#include<apaflrec.h>
#include <apmstd.h>
#include <f32file.h>

#ifdef EKA2
	#include <ImplementationProxy.h>
#endif


class CMyRecItem: public CBase
    {
    public:
        CMyRecItem(){};
        ~CMyRecItem(){delete iType;delete iExt; delete iData;};
    public:
    	HBufC8* 	iType;
        HBufC*  	iExt;
        HBufC8*  	iData;
    };




class CApaExRecognizer : public CApaDataRecognizerType
	{
public: // from CApaDataRecognizerType
	CApaExRecognizer();
	virtual ~CApaExRecognizer();

	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;
#ifdef EKA2
        static CApaDataRecognizerType* CreateRecognizerL();
#endif
private: // from CApaDataRecognizerType
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	TInt GetTypesArrayL(void);
private:
	RPointerArray<CMyRecItem>	iArray;
	TTime						iDbModified;
	};

#endif

