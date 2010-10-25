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
#ifndef PVPLSFILEPARSER_H_INCLUDED
#define PVPLSFILEPARSER_H_INCLUDED

#ifndef OSCL_BASE_H_INCLUDED
#include "oscl_base.h"
#endif

#ifndef OSCL_STRING_H_INCLUDED
#include "oscl_string.h"
#endif

#ifndef OSCL_FILE_IO_H_INCLUDED
#include "oscl_file_io.h"
#endif

#ifndef OSCL_MEM_H_INCLUDED
#include "oscl_mem.h"
#endif

#ifndef OSCL_VECTOR_H_INCLUDED
#include "oscl_vector.h"
#endif

#ifndef PVMI_DATASTREAMUSER_INTERFACE_H_INCLUDED
#include "pvmi_datastreamuser_interface.h"
#endif


#ifndef PVLOGGER_H_INCLUDED
#include "pvlogger.h"
#endif

#ifndef PVFILE_H_INCLUDED
#include "pvfile.h"
#endif

#define PLS_VERSION_SUPPORTED 2

#define PLSFF_MIN_DATA_SIZE_FOR_RECOGNITION 10
#define PLSFF_MAX_LINE_LENGTH 1024

enum PVPLSParserReturnCode
{
    // Return codes
    PVPLSFF_PARSER_OK = 0,
    PVPLSFF_PARSER_NOT_PARSED_ERROR = -1,
    PVPLSFF_PARSER_FILE_ERROR = -2,
    PVPLSFF_PARSER_MEM_ERROR = -3,
    PVPLSFF_PARSER_MISC_ERROR = -4,
};

typedef struct
{
    int32 numOfEntries;
    int32 versionNum;

} PVPLSFileInfo;


class PVPLSEntry
{
    public:

        PVPLSEntry()
        {
            iIndex = 0;
            iUrl = NULL;
            iTitle = NULL;
            iLength = 0;
        }

        PVPLSEntry(int32 aIndex)
        {
            iIndex = aIndex;
            iUrl = NULL;
            iTitle = NULL;
            iLength = 0;
        }

        ~PVPLSEntry()
        {
            if (iUrl)
            {
                OSCL_DELETE(iUrl);
                iUrl = NULL;
            }
            if (iTitle)
            {
                OSCL_DELETE(iTitle);
                iTitle = NULL;
            }
        }

        PVPLSEntry& operator=(const PVPLSEntry& aSrcEntry)
        {
            if (iUrl)
            {
                OSCL_wHeapString<OsclMemAllocator>* oldUrl = (OSCL_wHeapString<OsclMemAllocator>*)iUrl;
                OSCL_DELETE(oldUrl);
                iUrl = NULL;
            }
            iUrl = OSCL_NEW(OSCL_wHeapString<OsclMemAllocator>, (*aSrcEntry.iUrl));

            if (iTitle)
            {
                OSCL_wHeapString<OsclMemAllocator>* oldTitle = (OSCL_wHeapString<OsclMemAllocator>*)iTitle;
                OSCL_DELETE(oldTitle);
                iTitle = NULL;
            }
            iTitle = OSCL_NEW(OSCL_wHeapString<OsclMemAllocator>, (*aSrcEntry.iTitle));

            iIndex = aSrcEntry.iIndex;
            iLength = aSrcEntry.iLength;

            return *this;
        }

        int32 iIndex;                                   // index
        OSCL_wHeapString<OsclMemAllocator>* iUrl;       // url
        OSCL_wHeapString<OsclMemAllocator>* iTitle;     // title
        int32 iLength;                                  // duration in seconds, -1 = infinite
};

// PVPLSInfo stores all the info in the PLS file
class PVPLSInfo
{
    public:

        PVPLSInfo(int32 aNumOfEntries, int32 aVersion)
        {
            iNumEntries = aNumOfEntries;
            iVersion = aVersion;
        }

        ~PVPLSInfo()
        {
            while (iEntryTable.size() != 0)
            {
                PVPLSEntry* entry = iEntryTable.front();
                OSCL_DELETE(entry);
                iEntryTable.erase(iEntryTable.begin());
            }
        }

        int32 iNumEntries;                                      // number of entries in playlist
        Oscl_Vector<PVPLSEntry*, OsclMemAllocator> iEntryTable; // vector of entries
        int32 iVersion;                                         // PLS verion number, only 2 is valid
};


class PVPLSFFParser
{
    public:
        OSCL_IMPORT_REF PVPLSFFParser();

        OSCL_IMPORT_REF ~PVPLSFFParser();

        // Validate the PLS file
        OSCL_IMPORT_REF PVPLSParserReturnCode IsPLSFile(PVMFDataStreamFactory& aSourceDataStreamFactory);

        // Parse PLS File
        OSCL_IMPORT_REF PVPLSParserReturnCode ParseFile(OSCL_wString& aFileName);

        // Returns PLS file info: number of entries and version number
        OSCL_IMPORT_REF PVPLSParserReturnCode GetFileInfo(PVPLSFileInfo& aInfo);

        // Returns the entry specified by the entry number (1 based)
        OSCL_IMPORT_REF PVPLSParserReturnCode GetEntry(PVPLSEntry& aEntry, int32 aEntryNum);

    private:

        bool getTextLine(char* strBuffer, int32 strLength, char** textLineStart, int32* nextIndex, int32* textLength);

        bool isCommentLine(char* textLineStart);

        bool isTag(char* textLineStart, int32 textLineLength, const char* tagString, int32 tagStringLen);

        // return true if line is "tagString=123" or if line is "tagString1=234"
        // if there is not entry number between the tagString and '=', entryNum = -1
        bool isTagReturnIntegerValue(mbchar* textLineStart, int32 textLineLength,
                                     const char* tagString, int32 tagStringLen,
                                     int32* entryNum, int32* intValue);

        // return true if line is "tagString0=xyz"
        bool isTagReturnStringValue(mbchar* textLineStart, int32 textLineLength,
                                    const char* tagString, int32 tagStringLen,
                                    int32* entryNum, char** entryString, int32* entryStringLen);

        void cleanupParser(void);

        Oscl_File* iPLSFile;
        PVPLSInfo* iPLSInfo;
        PVLogger* iLogger;
};


#endif // PVPLSFILEPARSER_H_INCLUDED
