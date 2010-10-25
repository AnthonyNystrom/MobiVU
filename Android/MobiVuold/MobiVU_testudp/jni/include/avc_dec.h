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
#ifndef AVC_DEC_H_INCLUDED
#define AVC_DEC_H_INCLUDED

#ifndef OMX_Component_h
#include "OMX_Component.h"
#endif

#ifndef _AVCDEC_API_H_
#include "avcdec_api.h"
#endif

#ifndef OSCL_MEM_H_INCLUDED
#include "oscl_mem.h"
#endif

#ifndef PV_OMXDEFS_H_INCLUDED
#include "pv_omxdefs.h"
#endif

#define MAX_NUM_REF_FRAMES_PLUS_ONE 17

class AVCCleanupObject_OMX
{
        AVCHandle* ipavcHandle;

    public:
        AVCCleanupObject_OMX(AVCHandle* aAvcHandle = NULL)
        {
            ipavcHandle = aAvcHandle;
        }

        //! Use destructor to do all the clean up work
        ~AVCCleanupObject_OMX();
};


class AvcDecoder_OMX
{
    public:
        AvcDecoder_OMX() {};
        AvcDecoder_OMX(class OmxComponentBase *pComp)
        {
            // initialize members
            pCleanObject = NULL;

            FrameSize = 0;
            MaxNumFs = 0;
            MaxWidth = 0;
            MaxHeight = 0;

            oscl_memset(DisplayTimestampArray, 0, sizeof(OMX_TICKS)*MAX_NUM_REF_FRAMES_PLUS_ONE);
            oscl_memset(ReferenceBufferHdrPtr, 0, sizeof(OMX_BUFFERHEADERTYPE*)*MAX_NUM_REF_FRAMES_PLUS_ONE);
            CurrInputTimestamp = 0;
            InputBytesConsumed = 0;
            iAvcActiveFlag = OMX_FALSE;
            iSkipToIDR = OMX_FALSE;
            pCurrentBufferHdr = NULL;
            ipOMXComponent = pComp;

#if PROFILING_ON
            iTotalTicks = 0;
#endif
        };

        ~AvcDecoder_OMX() { };

        AVCCleanupObject_OMX*   pCleanObject;
        AVCHandle       AvcHandle;
        AVCDecSPSInfo   SeqInfo;
        uint32          FrameSize;
        OMX_U32         MaxNumFs;
        OMX_S32         MaxWidth;
        OMX_S32         MaxHeight;

        OMX_TICKS       DisplayTimestampArray[MAX_NUM_REF_FRAMES_PLUS_ONE];
        OMX_BUFFERHEADERTYPE* ReferenceBufferHdrPtr[MAX_NUM_REF_FRAMES_PLUS_ONE];
        OMX_TICKS       CurrInputTimestamp;
        OMX_U32         InputBytesConsumed;
        OMX_BOOL        iAvcActiveFlag;
        OMX_BOOL        iSkipToIDR;

#if PROFILING_ON
        OMX_U32 iTotalTicks;
#endif

        OMX_ERRORTYPE AvcDecInit_OMX();

        OMX_BOOL AvcDecodeVideo_OMX(OMX_BUFFERHEADERTYPE *aOutBuffer, OMX_BUFFERHEADERTYPE ** aOutBufferForRendering,
                                    OMX_U8** aInputBuf, OMX_U32* aInBufSize,
                                    OMX_PARAM_PORTDEFINITIONTYPE* aPortParam,
                                    OMX_S32* iFrameCount, OMX_BOOL aMarkerFlag,
                                    OMX_BOOL *aResizeFlag);

        OMX_ERRORTYPE AvcDecDeinit_OMX();

        OMX_BOOL InitializeVideoDecode_OMX();

        OMX_BOOL FlushOutput_OMX(OMX_BUFFERHEADERTYPE **aOutBuffer);

        AVCDec_Status GetNextFullNAL_OMX(uint8** aNalBuffer, int* aNalSize, OMX_U8* aInputBuf, OMX_U32* aInBufSize);

        static int AllocateBuffer_OMX(void* aUserData, int i, uint8** aYuvBuffer);

        static int ActivateSPS_OMX(void* aUserData, uint aSizeInMbs, uint aNumBuffers);

        int32 NSAllocateBuffer_OMX(void* aUserData, int32 i, uint8** aYuvBuffer);

        int32 NSActivateSPS_OMX(void* aUserData, uint aSizeInMbs, uint aNumBuffers);

        void ResetDecoder(); // for repositioning

        void ReleaseReferenceBuffers();

        OMX_BUFFERHEADERTYPE *pCurrentBufferHdr;
        OmxComponentBase *ipOMXComponent;
};

typedef class AvcDecoder_OMX AvcDecoder_OMX;

#endif  //#ifndef AVC_DEC_H_INCLUDED

