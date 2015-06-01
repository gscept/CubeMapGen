//========================================================================================================================================================
// Types.h -- Variable prototypes
//
//========================================================================================================================================================
// (C) 2004-2005 ATI Research, Inc., All rights reserved.
//========================================================================================================================================================

#ifndef TYPES_H
#define TYPES_H

static char sgCopyrightString[] = "\r\n\r\n(C) 2004 ATI Research, Inc.\r\n\r\n";

#ifndef WIN32
#include <inttypes.h>
#endif

// DEFINES ===============================================================================================================================================
#ifdef TRUE
 #undef TRUE
#endif
#define TRUE  1

#ifdef FALSE
 #undef FALSE
#endif
#define FALSE 0
#ifdef _WIN32
#define snprintf _snprintf
#else
#include <unistd.h>
#include <stdlib.h>
#define Sleep usleep   
#define HRESULT int
#define FAILED(x) ((x)!=0)
#define MessageBox(dummy, text, caption, dummy2) fprintf(stderr, "Message:\n%s\n%s", text, caption)
#define WINAPI
#define max std::max
#define min std::min
#endif

   //=========//
   // Windows //
   //=========//
   //Signed
   typedef char bool8;
   typedef char char8;

   typedef char    int8;
   typedef short   int16;
   typedef int     int32;
   typedef long long int64;

   typedef float       float32;
   typedef double      float64;
   //typedef long double float80; //Windows treats this the same as a double

   //Unsigned
   typedef unsigned char    uint8;
   typedef unsigned short   uint16;
   typedef unsigned int     uint32;
   typedef unsigned long long uint64;
   
   //String
   typedef const char cstr;


   

#endif
