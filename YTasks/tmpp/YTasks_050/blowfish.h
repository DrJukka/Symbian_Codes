// blowfish.h     interface file for blowfish.cpp
// _THE BLOWFISH ENCRYPTION ALGORITHM_
// by Bruce Schneier
// Revised code--3/20/94
// Converted to C++ class 5/96, Jim Conger
// Converted to Symbian May/2007, Jukka Silvennoinen

#ifndef __BLOWFISH_H__
#define __BLOWFISH_H__

#include <e32base.h>

#define MAXKEYBYTES 	56		// 448 bits max
#define NPASS           16		// SBox passes

class CMD5_Handler;

//#define WORD  		unsigned short
//#define BYTE  		unsigned char

class CBlowFish: public CBase
{
public:	
	static CBlowFish* NewL(void);
    static CBlowFish* NewLC(void);
	~CBlowFish () ;
	void Decrypt(TDes8& aKey, TDes8& aData);
	void Decrypt(TDes8& aKey, TDes& aData);
	void Encrypt(TDes8& aKey, TDes& aData);
	void Encrypt(TDes8& aKey, TDes8& aData);
private:
	CBlowFish();
	void ConstructL(void);
	void 		Initialize (TUint8 key[], int keybytes) ;
	TUint32		GetOutputLength (TUint32 lInputLong) ;
	TUint32		Encode (TUint8 * pInput, TUint8 * pOutput, TUint32 lSize) ;
	void		Decode (TUint8 * pInput, TUint8 * pOutput, TUint32 lSize) ;
private:
	void 		Blowfish_encipher (TUint32 *xl, TUint32 *xr) ;
	void 		Blowfish_decipher (TUint32 *xl, TUint32 *xr) ;
private:
	TUint32 		* PArray ;
	TUint32			(* SBoxes)[256];
	CMD5_Handler*	iMD5_Handler;	
} ;

// choose a byte order for your hardware
#define ORDER_DCBA	// chosing Intel in this case

#ifdef ORDER_DCBA  	// DCBA - little endian - intel
	union aword {
	  TUint32 dword;
	  TUint8 byte [4];
	  struct {
	    TUint byte3:8;
	    TUint byte2:8;
	    TUint byte1:8;
	    TUint byte0:8;
	  } w;
	};
#endif

#ifdef ORDER_ABCD  	// ABCD - big endian - motorola
	union aword {
	  TUint32 dword;
	  TUint8 byte [4];
	  struct {
	    TUint byte0:8;
	    TUint byte1:8;
	    TUint byte2:8;
	    TUint byte3:8;
	  } w;
	};
#endif

#ifdef ORDER_BADC  	// BADC - vax
	union aword {
	  TUint32 dword;
	  TUint8 byte [4];
	  struct {
	    TUint byte1:8;
	    TUint byte0:8;
	    TUint byte3:8;
	    TUint byte2:8;
	  } w;
};
#endif

#endif // __BLOWFISH_H__
