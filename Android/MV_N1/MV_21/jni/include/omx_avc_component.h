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
/**
    @file omx_Avc_component.h
    OpenMax decoder_component component.

*/

#ifndef OMX_AVC_COMPONENT_H_INCLUDED
#define OMX_AVC_COMPONENT_H_INCLUDED

#ifndef PV_OMXCOMPONENT_H_INCLUDED
#include "pv_omxcomponent.h"
#endif

#ifndef AVC_DEC_H_INCLUDED
#include "avc_dec.h"
#endif

#if (defined(TEST_FULL_AVC_FRAME_MODE) || defined(TEST_FULL_AVC_FRAME_MODE_SC) || defined(TEST_FULL_AVC_FRAME_MODE_I2BNS) || defined(TEST_FULL_AVC_FRAME_MODE_I4BNS))
#define INPUT_BUFFER_SIZE_AVC (2000 * MAX_NAL_PER_FRAME)
#else
#define INPUT_BUFFER_SIZE_AVC 2000
#undef MAX_NAL_PER_FRAME
#define MAX_NAL_PER_FRAME 1 /* this component doesn't need to suppoart more than 1 NAL per frame in this mode, so overwrite default */
#endif

//qcif - output 176*144*3/2
#define OUTPUT_BUFFER_SIZE_AVC 38016

#define NUMBER_INPUT_BUFFER_AVC  10
#define NUMBER_OUTPUT_BUFFER_AVC 10
// NOTE: The PV AVC decoder can hold maximum of 17 frames. We allocate 10 (it doesn't make sense e.g. to allocate all 17 QVGA
// If there is need for more than 10 - there will be a port reconfiguration


class OpenmaxAvcAO : public OmxComponentVideo
{
    public:

        OpenmaxAvcAO();
        ~OpenmaxAvcAO();

        OMX_ERRORTYPE ConstructComponent(OMX_PTR pAppData, OMX_PTR pProxy);
        OMX_ERRORTYPE DestroyComponent();

        OMX_ERRORTYPE ComponentInit();
        OMX_ERRORTYPE ComponentDeInit();



        void ComponentBufferMgmtWithoutMarker();
        OMX_BOOL ParseFullAVCFramesIntoNALs(OMX_BUFFERHEADERTYPE* aInputBuffer);
        void ProcessData();
        void DecodeWithoutMarker();
        void DecodeWithMarker();
        void ResetComponent();
        void ReleaseReferenceBuffers();


        OMX_ERRORTYPE GetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_INOUT OMX_PTR pComponentConfigStructure);

        OMX_BOOL DetectStartCodeLength(OMX_U8* aBitstream,
                                       OMX_U8** aNalUnit,
                                       OMX_U32 aBufSize,
                                       OMX_U32* aSCSize);

        // ipOutputBuffer is fed to the decoder which may keep it as a reference
        // The decoder "spits" out another output buffer for rendering
        OMX_BUFFERHEADERTYPE *ipOutputBufferForRendering;

    private:

        AvcDecoder_OMX* ipAvcDec;
        OMX_BOOL                iDecodeReturn;

        // variables for "frame" mode i.e. iOMXComponentNeedsFullAVCFrames is turned on
        OMX_U32 iNALSizeArray[MAX_NAL_PER_FRAME]; // 100 should be more than enough NALs per frame
        OMX_U32 iNALStartCodeSizeArray[MAX_NAL_PER_FRAME]; // 100 should be more than enough NALs per frame
        OMX_U32 iCurrNAL;
        OMX_U32 iNumNALs;
        OMX_U32 iNALOffset;
        OMX_U32 iSizeOfNALSize;
};



#endif // OMX_AVC_COMPONENT_H_INCLUDED
