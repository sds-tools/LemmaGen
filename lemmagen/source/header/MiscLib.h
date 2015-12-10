/******************************************************************************
This file is part of the lemmagen library. It gives support for lemmatization.
Copyright (C) 2006-2007 Matjaz Jursic <matjaz@gmail.com>

The lemmagen library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#pragma once

//-------------------------------------------------------------------------------------------
//includes
//#include <crtdbg.h>
#include <cstdlib>
#include <limits.h>
#include <new>

#include <utility>
#include <ctime>
#include <cmath>
#include <float.h>

#include <string>
#include <cstring>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <list>
#include <algorithm>

//#define dbgmem
#ifdef dbgmem
	#include "DebugMemory.h"
#endif


//-------------------------------------------------------------------------------------------
//using
using namespace std;

//-------------------------------------------------------------------------------------------
//typedefs
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef unsigned long long qword;

//-------------------------------------------------------------------------------------------
//container for some usual string (char*) or (byte*) functions
class Str {
private:
	//consts
	static const byte esc = (byte)'\\';
	static const byte nln = (byte)'n';
	static const byte crt = (byte)'r';
	static const byte quo = (byte)'"';
	static const byte apo = (byte)'\'';
	static const byte nlnr = (byte)'\n';
	static const byte crtr = (byte)'\r';

	static const word _esc = ((word)esc << 8) + esc;
	static const word _nln = ((word)nln << 8) + esc;
	static const word _crt = ((word)crt << 8) + esc;
	static const word _quo = ((word)quo << 8) + esc;
	static const word _apo = ((word)apo << 8) + esc;

public:
	//length functions
	static int eqPrefixLen(const char *s1, const char *s2);
	static int eqEndLen(const char *s1, const char *s2, int iConfirmedLen=0);
	static int eqEndLenFast(const char *s1end, const char *s2end, const int iConfirmedLen = 0, const int iMaxLen=INT_MAX);
	
	//lessthan comparators
	static int bytecmp(const byte *s1, const byte *s2);
	static int bytecmpBack(const byte *s1, const byte *s2);
	
	//change case
	static byte lower(byte ch);
	static void lower(char *s);

	//appending 3 words
	static char *cropJoin(const char *acFrom, const char *acArrow, const char *acTo, const int iEqLen = -1);

	//copy
	static byte *copy(byte *s);
	static char *copy(char *s);
	static char *copyAppend(const char cPrefix, char *s);
	static char *copyConcat(const char *s1, const char *s2);

	//string noramlization eg "\n" -> newline, ...
	static byte *norm(byte *s);

	//less than function objest comparators
	struct LowerCharStr {
		bool operator()( const char* s1, const char* s2 ) const;
	};

	struct LowerByteStr {
		bool operator()( const byte *s1, const byte *s2 ) const;
	};

};

//-------------------------------------------------------------------------------------------
//processing true c++ strings
class TrueStr {
public:
	static void Tokenize(const string& str, vector<string>& tokens, const string& delimiters = "|"); 
};

//-------------------------------------------------------------------------------------------
//testing for files read/write status
class CheckFile {
public:
	static bool CanRead(const char *sFileName);
	static bool CanWrite(const char *sFileName, bool bAskOverwrite = true);
};

//-------------------------------------------------------------------------------------------
//container for more acurate random functios
class Rand {
private:
#if defined(_WIN64) || defined(_WIN32)
  static const int  RAND_MAX_REAL = RAND_MAX + 1;
#else
  static const int  RAND_MAX_REAL = ((int)(RAND_MAX/2));
#endif

public:
	static double nextDouble();
	static int nextInt(int iMin, int iMax);
    static void seed(int iSeed);
};

//-------------------------------------------------------------------------------------------
//container for timing functions
class Timing {
private:
	clock_t clkStart;
	bool bSuppresReset;

public:
	Timing();

	operator double();
	Timing &operator-();

	void Reset();
	double ElapSec(bool bReset = true);
	const char *ElapString(bool bReset = true);
	const void Print(ostream &os=cout, const char* acPreText="Time ", const char* acPostText=".");
};
//-------------------------------------------------------------------------------------------
//output ot streams
ostream &operator<<(ostream &os, Timing &time);

//-------------------------------------------------------------------------------------------
//wait for user click
inline void wait(bool bVerbose = false) {
	cout << endl;
	if (bVerbose) cout << string(80, '-');
	getchar();
	//system("pause");
}

//-------------------------------------------------------------------------------------------
//definition of legal word characters
#define WORDCHARS \
((acBuf[i]>=48 && acBuf[i]<=57) ||  /* 0-9  */ \
(acBuf[i]>=65 && acBuf[i]<=90) ||   /* A-Z  */ \
(acBuf[i]>=97 && acBuf[i]<=122) ||  /* a-z  */ \
acBuf[i]==45 || acBuf[i]==95 ||     /* -, _ */ \
acBuf[i]==97 || acBuf[i]==94 ||      /* ', ^*/ \
acBuf[i]>127 || acBuf[i]<0)         /*special & unicode letters*/

//definition of legal non-space characters
#define NONSPACECHARS \
((acBuf[i]>=33 && acBuf[i]<=126) ||  /* all printable non-space ascii character  */ \
acBuf[i]>127 || acBuf[i]<0)         /*special & unicode letters*/

