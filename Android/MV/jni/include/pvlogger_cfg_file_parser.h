/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */
#ifndef PVLOGGER_CFG_FILE_PARSER_H_INCLUDED
#define PVLOGGER_CFG_FILE_PARSER_H_INCLUDED

//******************************************************************************
//                                                                    INCLUDES
//******************************************************************************

#ifndef PVLOGGER_H_INCLUDED
#include "pvlogger.h"
#endif
#ifndef OSCL_TYPES_H_INCLUDED
#include "oscl_types.h"
#endif
#ifndef OSCL_DEFALLOC_H_INCLUDED
#include "oscl_defalloc.h"
#endif

//******************************************************************************
//                                                                     DEFINES
//******************************************************************************

// Default values for the log configuration path and the log output path.
// Typically the path is derived by catenating the two related defines, i.e.
// PVLOG_PREPEND_CFG_FILENAME + PVLOG_CFG_FILENAME
// This enables the file name itself and the path to the file to be separately
// modifiable. These values should be overridden where applicable. Notice that
// if they are already defined, they will not be over-ridden. Project code
// should adopt the convention of undefining and then defining the symbol to
// ensure the project's definition is in effect.
#ifndef PVLOG_PREPEND_CFG_FILENAME
#define PVLOG_PREPEND_CFG_FILENAME _STRLIT_CHAR("")
#endif
#ifndef PVLOG_PREPEND_OUT_FILENAME
#define PVLOG_PREPEND_OUT_FILENAME _STRLIT_CHAR("")
#endif
#ifndef PVLOG_CFG_FILENAME
#define PVLOG_CFG_FILENAME _STRLIT_CHAR("pvlogcfg.txt")
#endif
#ifndef PVLOG_OUT_FILENAME
#define PVLOG_OUT_FILENAME _STRLIT_CHAR("pvlogger.out")
#endif


//******************************************************************************
// PVLoggerCfgFileParser                                                 CLASS
//******************************************************************************
// Logger configuration file handling class. Initialize the logging nodes. By
// default logging output is directed to stderr for non-Windows or ATL Trace for
// Win32/WinMobile.
//
// -----------------------------------------------------------------------------
// Format of logging configuration file:
//
//  Lines beginning with a pound sign '#' are ignored.
//
//  First uncommented line shall be one of the following:
//    NO_LOGGING ==> no logs are created and the rest of the file is skipped.
//    LOGTOFILE  ==> output is redirected to a new file using a text file appender
//    LOGTOMEM   ==> output is redirected to a new file using a memory appender
//
//  All other lines must be of the format:
//    NODENAME;LOGLEVEL
//  where NODENAME is the case-sensitive name of a node such as EngineHandler or
//  PVPlayer (use ALLNODES if you want all nodes to be logged) and LOGLEVEL is
//  one of:
//   PVLOGMSG_EMERG
//   PVLOGMSG_ALERT
//   PVLOGMSG_CRIT
//   PVLOGMSG_ERR
//   PVLOGMSG_WARNING
//   PVLOGMSG_NOTICE
//   PVLOGMSG_INFO
//   PVLOGMSG_STACK_TRACE
// -----------------------------------------------------------------------------
//   ## sample logger configuration file
//   LOGTOFILE
//   PVAuthorEngine;PVLOGMSG_DEBUG
//   PVAuthorEngine;PVLOGMSG_WARNING
// -----------------------------------------------------------------------------
//
class PVLoggerCfgFileParser
{

    public:
//==============================================================================
// PUBLIC: ENUMERATIONS
//==============================================================================

        typedef enum
        {
            ePVLOG_APPENDER_FILE,
            ePVLOG_APPENDER_MEMORY,
            ePVLOG_APPENDER_STDERR
        } eAppenderType_t;


//==============================================================================
// PUBLIC: STATIC METHODS
//==============================================================================

        //==========================================================================
        // Parse                                                     STATIC PUBLIC
        //==========================================================================
        // Open pszCfgFileName to read nodes (and levels) to log. Returns true on
        // success, false otherwise.
        //
        OSCL_IMPORT_REF static bool Parse
        (
            const char* pszCfgFileName,
            const char* pszLogFileName
        );

        //==========================================================================
        // CreateLogAppender                                         STATIC PUBLIC
        //==========================================================================
        //
        //  [IN] eAppenderType_t
        //         The appender type.
        //  [IN] const char*
        //         Name of output log file, as a null-terminated ASCII string.
        //  [OUT] OsclRefCounter*&
        //         The created reference counter object.
        //  [OUT] OsclSharedPtr<PVLoggerAppender>&
        //         The created PVLoggerAppender.
        //
        OSCL_IMPORT_REF static int32 CreateLogAppender
        (
            eAppenderType_t eAppenderType,
            const char* pszLogFileName,
            OsclRefCounter*& pRC,
            OsclSharedPtr<PVLoggerAppender>& appenderPtr
        );

        //==========================================================================
        // AttachLogAppender                                         STATIC PUBLIC
        //==========================================================================
        //
        //  [IN] OsclSharedPtr<PVLoggerAppender>&
        //         The PVLoggerAppender to attach to the specified log node.
        //  [IN] const char*
        //         Name of log node to attach appender to, as a null-terminated
        //         ASCII string.
        //  [IN] const PVLogger::log_level_type eLogLevel
        //         Log level.
        //
        OSCL_IMPORT_REF static bool AttachLogAppender
        (
            OsclSharedPtr<PVLoggerAppender>& appenderPtr,
            const char* pszLogNode = "",
            const PVLogger::log_level_type eLogLevel = PVLOGMSG_DEBUG
        );


        //==========================================================================
        // SetupLogAppender                                          STATIC PUBLIC
        //==========================================================================
        //
        //  [IN] eAppenderType_t
        //         The appender type.
        //  [IN] const char*
        //         Name of output log file, as a null-terminated ASCII string.
        //  [IN] const char*
        //         Name of log node to attach appender to, as a null-terminated
        //         ASCII string.
        //  [IN] const PVLogger::log_level_type eLogLevel
        //         Log level.
        //  [IN/OUT] OsclSharedPtr<PVLoggerAppender>**
        //         Optional parameter. When not null, the shared object pointed to
        //         will contain a pointer to the newly created OsclSharedPtr object.
        //
        OSCL_IMPORT_REF static bool SetupLogAppender
        (
            eAppenderType_t eAppenderType,
            const char* pszLogFileName = 0,               // name of output log file
            const char* pszLogNode = _STRLIT_CHAR(""),
            const PVLogger::log_level_type eLogLevel = PVLOGMSG_DEBUG,
            OsclSharedPtr<PVLoggerAppender>* pSharedAppenderPtr = 0 // optional
        );

};


//******************************************************************************
// LogAppenderDestructDealloc                                         TEMPLATE
//******************************************************************************
// For smart ptrs call delete when done (used by LogAppender)
//
template<class DestructClass>
class LogAppenderDestructDealloc
        : public OsclDestructDealloc
{

    public:
//==============================================================================
// PUBLIC: STATIC METHODS
//==============================================================================

        //==========================================================================
        // destruct_and_dealloc                                             PUBLIC
        //==========================================================================
        //
        virtual void destruct_and_dealloc
        (
            OsclAny*ptr
        )
        {
            delete((DestructClass*)ptr);
        }

};


#endif // PVLOGGER_CFG_FILE_PARSER_H_INCLUDED

// END FILE

