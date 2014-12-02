
#ifndef __CRYPTING_TEST_H__
#define __CRYPTING_TEST_H__

#include <e32base.h>
#include <F32FILE.H>

#ifdef __SERIES60_3X__
	#include <cryptosymmetric.h>
	#include <hash.h>
	#include <cryptopadding.h>
#else
#endif

_LIT8(MyOwnPadding			,"                    ");

	class CMyAESCrypter : public CBase
	    {
	    
	public:
		static CMyAESCrypter* NewL(const TDesC8& aPassword);
		static CMyAESCrypter* NewLC(const TDesC8& aPassword);
		~CMyAESCrypter();
	public:
		void Encrypt(const TDesC8& aInput, TDes8& aOutput);
		void Decrypt(const TDesC8& aInput, TDes8& aOutput);
		void Base64Decode(const TDesC8& aSource, TDes8& aDestination);
		void Base64Encode(const TDesC8& aSource, TDes8& aDestination);
	private:
		CMyAESCrypter(void);
	    void ConstructL(const TDesC8& aPassword);
	private:
	#ifdef __SERIES60_3X__
		CAESEncryptor* 		iAESEncryptor;
		CModeCBCEncryptor*	iModeCBCEncryptor;
		CAESDecryptor*		iAESDecryptor;
		CModeCBCDecryptor*	iModeCBCDecryptor;
	#else
	#endif
	    };



#endif // __IMSI_IMEI_GETTER_H__
