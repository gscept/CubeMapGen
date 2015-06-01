//--------------------------------------------------------------------------------------
// ErrorMsg.cpp
// 
//   Common error message handling functions
//
//--------------------------------------------------------------------------------------
// (C) 2005 ATI Research, Inc., All rights reserved.
//--------------------------------------------------------------------------------------

#include "Types.h"
#include "ErrorMsg.h"

uint32 sg_MessageType = EM_DEFAULT_MESSAGE_MEDIUM;

//MessageOutputCallback
void (*sg_MessageOutputCallback)(const char *, const char *) = DefaultErrorMessageCallback; 


//--------------------------------------------------------------------------------------
// SetErrorMessageCallback
//
//--------------------------------------------------------------------------------------
void DefaultErrorMessageCallback(const char *a_Title, const char *a_Message )
{
   MessageBox(NULL, a_Message, a_Title, MB_OK);
}


//--------------------------------------------------------------------------------------
// SetErrorMessageCallback
//
//--------------------------------------------------------------------------------------
void SetErrorMessageCallback( void(*a_MessageOutputFunc)(const char *, const char *) )
{
   sg_MessageType = EM_MESSAGE_MEDIUM_CALLBACK_FUNCTION;
   sg_MessageOutputCallback = a_MessageOutputFunc;
}


//--------------------------------------------------------------------------------------
//  Pulls up a message box, or calls a custom message function with an error message
//
//  note: this function is used to route all error messages though a common output 
//   mechanism..  in the future output could be rerouted through the console using 
//   this mechanism.
//--------------------------------------------------------------------------------------
void OutputMessageString(const char *a_Title, const char *a_Message )
{
   switch(sg_MessageType)
   {
      case EM_MESSAGE_MEDIUM_MESSAGEBOX:
         MessageBox(NULL, a_Message, a_Title, MB_OK);
      break;
      case EM_MESSAGE_MEDIUM_CALLBACK_FUNCTION:
         sg_MessageOutputCallback(a_Title, a_Message);
      break;
      default:
      break;
   }
}


//--------------------------------------------------------------------------------------
// variable arguement version of output message
//--------------------------------------------------------------------------------------
void OutputMessage(const char *a_Message, ... )
{   
    int32 numCharOutput = 0;
    char msgBuffer[EM_MAX_MESSAGE_LENGTH];
    va_list args;
    va_start(args, a_Message);

    
    numCharOutput = snprintf( msgBuffer, EM_MAX_MESSAGE_LENGTH, a_Message, args);

    //va_end(args, a_Message);
    OutputMessageString("Message:", msgBuffer);
}

//--------------------------------------------------------------------------------------
// displays the message with a YES / NO response; returns true on YES and false on NO
//--------------------------------------------------------------------------------------
bool OutputQuestion( const char *a_Message, ... )
{
#ifndef WIN32
     exit(-1);
#else
   char msgBuffer[ EM_MAX_MESSAGE_LENGTH ];
   va_list args;
   va_start( args, a_Message );

   snprintf( msgBuffer, EM_MAX_MESSAGE_LENGTH, a_Message, args );

   switch( sg_MessageType )
   {
      case EM_MESSAGE_MEDIUM_MESSAGEBOX:
         return MessageBox( NULL, msgBuffer, "Question:", MB_YESNO ) == IDYES;
      break;
      case EM_MESSAGE_MEDIUM_CALLBACK_FUNCTION:
         sg_MessageOutputCallback( "Question:", msgBuffer );
      break;
      default:
      break;
   }
#endif
   return false;
}

//--------------------------------------------------------------------------------------
// output message if HRESULT indicates failure
//  
//--------------------------------------------------------------------------------------
HRESULT OutputMessageOnFail(HRESULT a_hr, const char *a_Message, ... )
{   
    int32 numCharOutput = 0;
    char msgBuffer[EM_MAX_MESSAGE_LENGTH];
    va_list args;

    if(FAILED(a_hr))
    {
        va_start(args, a_Message);

        numCharOutput = snprintf( msgBuffer, EM_MAX_MESSAGE_LENGTH, a_Message, args);

        //va_end(args, a_Message);

        OutputMessageString("Error!", msgBuffer);
    }

    return a_hr;
}


//--------------------------------------------------------------------------------------
// output message and exit program if HRESULT indicates failure
//  
//--------------------------------------------------------------------------------------
HRESULT OutputFatalMessageOnFail(HRESULT a_hr, const char *a_Message, ... )
{   
    int32 numCharOutput = 0;
    char msgBuffer[EM_MAX_MESSAGE_LENGTH];
    va_list args;

    if(FAILED(a_hr))
    {
        va_start(args, a_Message);

        numCharOutput = snprintf( msgBuffer, EM_MAX_MESSAGE_LENGTH, a_Message, args);

        //va_end(args, a_Message);

        OutputMessageString("Fatal Error!", msgBuffer);
        exit(EM_FATAL_ERROR);
    }

    return a_hr;
}


