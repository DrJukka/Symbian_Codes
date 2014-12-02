#ifndef JUKKAS_MD5_H
#define JUKKAS_MD5_H

#include <E32BASE.H>

struct MD5Context 
{
   TUint32 buf[4];
   TUint32 bits[2];
   TUint8  in[64];
};

class CMD5_Handler : public CBase
    {
public:
	void MD5Init(struct MD5Context *ctx);
	void MD5Update(struct MD5Context *ctx,TUint8* buf,TUint16 len);
	void MD5Final(TUint8 digest[16], struct MD5Context *ctx);
private:
	TAny* MemCopy(TAny* aDst, const TAny *aScr,TInt aSize);
	void MD5Transform(TUint32 buf[4],TUint32 in[16]);
	void byteReverse(TUint8* buf, TUint16 longs);	
    };

#endif /* JUKKAS_MD5_H */
