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
#ifndef MPEG4_DEC_H_INCLUDED
#define MPEG4_DEC_H_INCLUDED

#ifndef OMX_Component_h
#include "OMX_Component.h"
#endif

#ifndef _MP4DEC_API_H_
#include "mp4dec_api.h"
#endif

#ifndef _MP4DECLIB_H_
#include "mp4dec_lib.h"
#endif

#ifndef PV_OMXDEFS_H_INCLUDED
#include "pv_omxdefs.h"
#endif

class Mpeg4Decoder_OMX
{
    public:

        Mpeg4Decoder_OMX() {};
        Mpeg4Decoder_OMX(class OmxComponentBase *pComp);

        OMX_S32 InitializeVideoDecode(OMX_S32* aWidth, OMX_S32* aHeight,
                                      OMX_U8** aBuffer, OMX_S32* aSize, OMX_S32 mode);

        OMX_ERRORTYPE Mp4DecInit();


        OMX_BOOL Mp4DecodeVideo(OMX_BUFFERHEADERTYPE* aOutBuffer, OMX_U32* aOutputLength,
                                OMX_U8** aInputBuf, OMX_U32* aInBufSize,
                                OMX_PARAM_PORTDEFINITIONTYPE* aPortParam,
                                OMX_S32* aFrameCount, OMX_BOOL aMarkerFlag, OMX_BOOL *aResizeFlag);

        OMX_ERRORTYPE Mp4DecDeinit();

        OMX_S32 GetVideoHeader(int32 aLayer, uint8 *aBuf, int32 aMaxSize);

        OMX_BOOL Mpeg4InitCompleteFlag;


#if PROFILING_ON
        OMX_U32 iTotalTicks;
#endif

        VideoDecControls VideoCtrl;

    private:

        OmxComponentBase *ipOMXComponent;
        MP4DecodingMode CodecMode;
        OMX_BOOL iReferenceYUVWasSet;
        OMX_U32  iFrameSize;
        OMX_U32 *ipRefCtrPreviousReferenceBuffer;
        OMX_S32 iDisplay_Width, iDisplay_Height;
        OMX_S32 iShortVideoHeader;

        OMX_U8 VO_START_CODE1[4];
        OMX_U8 VOSH_START_CODE1[4];
        OMX_U8 VOP_START_CODE1[4];
        OMX_U8 H263_START_CODE1[3];
};


#endif ///#ifndef MPEG4_DEC_H_INCLUDED
