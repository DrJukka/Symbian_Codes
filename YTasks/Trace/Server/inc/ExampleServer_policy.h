/* 
	Copyright (c) 2006 , 
	J.P. Silvennoinen. 
	All rights reserved 
*/


#ifndef __SCLIENTSERVER_STUFF_H__
#define __SCLIENTSERVER_STUFF_H__

#include <e32base.h>

/*
 ----------------------------------------------------------------------------------------
// Server's policy is defined here
----------------------------------------------------------------------------------------
*/

	
//---------------------- Total number of ranges -----------------------------//
const TUint SClientServerRangeCount = 4;

//----------------- Definition of the ranges of IPC numbers -----------------//
const TInt SClientServerRanges[SClientServerRangeCount] = 
        {
        0, // 0 	; 
        1, // 1  	; 
        2, // 2 	; 
        8, // ; non implemented function end of range check ; ENotSupported
        }; 

//Policy to implement for each of the above ranges        
const TUint8 SClientServerElementsIndex[SClientServerRangeCount] = 
        {
        CPolicyServer::EAlwaysPass,	//applies to 0th range, i.e EExamplePServGetCount
        CPolicyServer::ECustomCheck,  //applies to 1st range, i.e EExamplePServReadValueAndName
        CPolicyServer::ECustomCheck,  //applies to 2nd range  i.e EExamplePServSetValue & EExamplePServSetValueAndName
        CPolicyServer::ENotSupported  //applies to 4th range (out of range IPC)
        };

//Specific capability checks
const CPolicyServer::TPolicyElement SClientServerElements[] = 
        {
        //policy "0"; fail call if ReadUserData not present, used for EExamplePServReadValueAndName
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient},  
        //policy "1"; panic call if WriteUserData not present, used for EExamplePServSetValue & EExamplePServSetValueAndName
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EPanicClient} 
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy SClientServerPolicy =
        {
        CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
        SClientServerRangeCount,	//number of ranges                                   
        SClientServerRanges,		//ranges array
        SClientServerElementsIndex,	//elements<->ranges index
        SClientServerElements,		//array of elements
        };



// ----------------------------------------------------------------------------------------
// Server's panic codes here
// ----------------------------------------------------------------------------------------
enum TSClientServerPanic
	{
	EPanicGeneral,
	EPanicIllegalFunction
	};

void PanicClient(const RMessagePtr2& aMessage,TSClientServerPanic aPanic);






#endif //__SCLIENTSERVER_STUFF_H__
