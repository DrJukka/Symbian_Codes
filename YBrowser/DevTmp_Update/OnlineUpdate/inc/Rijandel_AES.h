
#ifndef __CRYPTING_TEST_H__
#define __CRYPTING_TEST_H__

#include <e32base.h>
#include <F32FILE.H>
#include <cryptosymmetric.h>
#include <hash.h>
#include <cryptopadding.h>


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
		CAESEncryptor* 		iAESEncryptor;
		CModeCBCEncryptor*	iModeCBCEncryptor;
		CAESDecryptor*		iAESDecryptor;
		CModeCBCDecryptor*	iModeCBCDecryptor;
	    };



#endif // __CRYPTING_TEST_H__
