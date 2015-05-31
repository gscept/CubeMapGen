//--------------------------------------------------------------------------------------
// ErrorMsg.h
// 
//   Common error message handling functions
//
//--------------------------------------------------------------------------------------
// (C) 2005 ATI Research, Inc., All rights reserved.
//--------------------------------------------------------------------------------------

#ifndef ERRORMSG_H
#define ERRORMSG_H 

#ifdef _WIN32
#include <windows.h>
#include <wtypes.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#include "Types.h"

//do not allow "dxstdafx.h" to depricate any core string functions
#pragma warning( disable : 4995 )

#define EM_EXIT_NO_ERROR                  0
#define EM_EXIT_NONFATAL_ERROR_OCCURRED  -1
#define EM_FATAL_ERROR                  -15


#define EM_MAX_MESSAGE_LENGTH             16384

#define EM_MESSAGE_MEDIUM_MESSAGEBOX         0
#define EM_MESSAGE_MEDIUM_CALLBACK_FUNCTION  1

//default message output method
#define EM_DEFAULT_MESSAGE_MEDIUM  EM_MESSAGE_MEDIUM_MESSAGEBOX

//default error message callback
void DefaultErrorMessageCallback(const char *a_Title, const char *a_Message );

//set error function
void SetErrorMessageCallback( void(*a_MessageOutputFunc)(const char *,const char *) );


#if defined(DEBUG) | defined(_DEBUG)
    #ifndef V_MSG
        #define V_MSG(x, m) {hr = x; if( FAILED(hr) ){OutputMessage("Warning", m ); {DXUTTrace( __FILE__, (DWORD)__LINE__, hr, #x, true );} }}
        #endif
    #ifndef V_MSG_RETURN
        #define V_MSG_RETURN(x, m) {hr = x; if( FAILED(hr) ){OutputMessage("Warning", m); {return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, #x, true ); }}
    #endif
    #ifndef V_MSG_FATAL
        #define V_MSG_FATAL(x, m) {hr = x; if(FAILED(hr)) {OutputMessage("Fatal Error", m); {DXUTTrace( __FILE__, (DWORD)__LINE__, hr, #x, true ); exit(EM_FATAL_ERROR); }}
    #endif
#else
    #ifndef V_MSG
        #define V_MSG(x, m) {hr = x; if(FAILED(hr)) {OutputMessage("Warning", m); } }
    #endif
    #ifndef V_MSG_RETURN
        #define V_MSG_RETURN(x, m) {hr = x; if(FAILED(hr)) {OutputMessage("Warning", m);} } 
    #endif
    #ifndef V_MSG_FATAL
        #define V_MSG_FATAL(x, m) {hr = x; if(FAILED(hr)) {OutputMessage("Fatal Error", m); exit(EM_FATAL_ERROR);} }
    #endif
#endif

//no variable arguementlist
void OutputMessageString(const char *a_Title, const char *a_Message);

//has variable arguement list
void OutputMessage(const char *a_Message, ... );
bool OutputQuestion( const char *a_Message, ... );
HRESULT OutputMessageOnFail(HRESULT a_hr, const char *a_Message, ... );
HRESULT OutputFatalMessageOnFail(HRESULT a_hr, const char *a_Message, ... );

#endif //ERRORMSG_H 
