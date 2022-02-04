
//==============================================================================
//
//   critsec.h - exported_h
//
//==============================================================================
//  cybercastor - made in quebec 2020 <cybercastor@icloud.com>
//==============================================================================



#ifndef __CRITSEC_H__
#define __CRITSEC_H__

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <process.h>

 
class CritSec
{
public:
	CritSec() {
		InitializeCriticalSection(&critsection); 
	}
      ~CritSec() {
		  DeleteCriticalSection(&critsection); 
	  }
      void Enter() { EnterCriticalSection(&critsection); }
      void Leave() { LeaveCriticalSection(&critsection); }

private:
    CRITICAL_SECTION critsection;
	bool initialized;
};
extern CritSec ccs;

class Syslock
{
	CritSec section;
public:
	Syslock()
    {
        Lock();
    }

    ~Syslock()
    {
		Unlock();
    }

    void Lock() { ccs.Enter(); }
    void Unlock() { ccs.Leave(); }
};

 

#endif // __CRITSEC_H__