

#ifndef __FILETYPESDB_H__
#define __FILETYPESDB_H__

#include <e32base.h>
#include <S32FILE.H>
#include <D32DBMS.H>
#include <BADESCA.H>



class CFtASSItem: public CBase
    {
    public:
        CFtASSItem(): iIndex(-1){};
        ~CFtASSItem()
        {
        	delete iTypeName;
        	delete iHandler;
        	delete iExtensions;
        }
    public:
        TInt 			iIndex;
        HBufC* 			iTypeName;
        HBufC* 			iHandler;
        CDesCArrayFlat* iExtensions;
        TBool			iOwn;
    };
    


class CDisabledItem: public CBase
    {
    public:
        CDisabledItem(): iIndex(-1){};
        ~CDisabledItem()
        {
        	delete iName;
        }
    public:
        TInt 		iIndex;
        HBufC* 		iName;
        TUint32		iUid;
        TBool		iRecognizer;
    };
    
 class CHandlerOwrItem: public CBase
    {
    public:
        CHandlerOwrItem(): iIndex(-1){};
        ~CHandlerOwrItem()
        {
        	delete iName;
        	delete iType;
        }
    public:
        TInt 		iIndex;
        HBufC* 		iName;
        HBufC* 		iType;
        TUint32		iUid;
        TBool		iRecognizer;
    };

class CMyDBClass : public CBase
    {
public : 
	void ConstructL();
    ~CMyDBClass();
public: // filetype association functions
	void DeleteFromDatabaseL(TInt& aIndex);
	void UpdateDatabaseL(const TDesC& aTypeName,const TDesC& aHandler,CDesCArray* aExtensions,TBool aOwn,TInt& aIndex);
	void SaveToDatabaseL(const TDesC& aTypeName,const TDesC& aHandler,CDesCArray* aExtensions,TBool aOwn,TInt& aIndex);
	void ReadDbItemsL(RPointerArray<CFtASSItem>& aItemArray);
	void ReadTypeItemsL(RPointerArray<CFtASSItem>&	aItemArray);
	void ReadHandlersItemsL(RPointerArray<CFtASSItem>&	aItemArray);
public: // recognizer & handler disable functions
	void GetDisabledL(RPointerArray<CDisabledItem>& aArray);
	void SetDisabledL(const TDesC& aName, const TUint32& aUid,const TBool& aRecognizer,TInt& aIndex);
	TBool SetEnabledL(const TDesC& aName, const TUint32& aUid,const TBool& aRecognizer);
public: // handler filetype handling functions
	void GetHandlerTypesOWRL(const TDesC& aName, const TUint32& aUid,CDesCArray& aTypesArray);
	void DeleteHandlerTypesOWRL(const TDesC& aName, const TUint32& aUid);
	void SetHandlerTypesOWRL(const TDesC& aName, const TUint32& aUid,CDesCArray& aTypesArray);
public: // recognizer & handler disable functions
	TBool GetDefaultHandL(TDes& aName,TUint32& aUid);
	void SetDefaultHandL(const TDesC& aName, const TUint32& aUid);
	void RemoveDefaultHandL(void);
private:
	CDesCArrayFlat* GetExtensionArrayL(const TDesC& aData);
	void GetBufferFromArrayL(CDesCArray* aExtensions,TDes& aData);
	void CreateTypesTableL(RDbDatabase& aDatabase);
	void CreateDisableTableL(RDbDatabase& aDatabase);
	void CreateHandlerTypesL(RDbDatabase& aDatabase);
	void CreateHandlerDefaultL(RDbDatabase& aDatabase);
private:
	RDbNamedDatabase 		iItemsDatabase;

    };


	
#endif //__FILETYPESDB_H__
