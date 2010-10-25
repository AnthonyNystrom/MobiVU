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
    @file pv_omxcomponent.h
    OpenMax decoder_component base header file.

*/

#ifndef PV_OMXCOMPONENT_H_INCLUDED
#define PV_OMXCOMPONENT_H_INCLUDED

#ifndef OMX_Component_h
#include "OMX_Component.h"
#endif

#ifndef OSCL_BASE_H_INCLUDED
#include "oscl_base.h"
#endif

#ifndef OSCL_SCHEDULER_H_INCLUDED
#include "oscl_scheduler.h"
#endif

#ifndef PVLOGGER_H_INCLUDED
#include "pvlogger.h"
#endif

#ifndef PV_OMX_QUEUE_H_INCLUDED
#include "pv_omx_queue.h"
#endif

#ifndef PV_OMXCORE_H_INCLUDED
#include "pv_omxcore.h"
#endif

#if PROFILING_ON
#ifndef OSCL_TICKCOUNT_H_INCLUDED
#include "oscl_tickcount.h"
#endif
#endif


#define OMX_PORT_INPUTPORT_INDEX OMX_DirInput
#define OMX_PORT_OUTPUTPORT_INDEX OMX_DirOutput
#define OMX_PORT_ALLPORT_INDEX -1

#define OMX_PORT_NUMBER_FORMATS_SUPPORTED 4

#define OMX_RV_MAX_INPUT_SEGMENTS 32

typedef struct RvSegmentInfoType
{
    OMX_S32 IsValid;
    OMX_U32 PartialFragmentOffset;
} RvSegmentInfoType;


/* Application's private data */
typedef struct ComponentPrivateType
{
    OMX_HANDLETYPE CompHandle;

} ComponentPrivateType;

/*
 buffer ref. counter is attached to output buffers
in case video components need to keep buffers as references

NOTES about the refcounter and iNumAvailableOutputBuffers:

0) The refcounter for each buffer is initially 1 (indicating that
omx client owns the buffer). The max value of the refcounter is 2
(sent to omx client (e.g. for rendering) and used for reference by codec).
The min value of the refcounter is 0 (queued in omx component queue and available).
If the value of the refcounter is 1 - the buffer is either outside
the omx component but not used by the codec, or is used by the codec
and is still owned by omx component.

1) The refcounter is decremented when the buffer is
 - received i.e. queued in the omx component
 - codec releases (unbinds) it because it no longer needs it for reference

2) The refcounter is incremented when the output buffer is
 - sent back to the client (fill buffer done callback)
 - codec binds it because it wants to use it as reference

3) When the codec releases the buffer - the buffer can be either:

 - in the omx component queue (in which case it becomes available)
 - outside the omx component (i.e. sent for rendering). This buffer will
 become available when it gets queued

4) When the buffer is owned by the omx component - it can either be:
 - in the omx component queue (iIsBufferInComponentQueue is false in this case)

 - dequeued and used inside the codec as a reference (iIsBufferInComponentQueue is true in this case)

5) There could be multiple buffers that are used as reference and
are also outstanding (sent for rendering).
When such buffers then come back to the omx component - they are in
the omx component queue - but they are not available. The codec
needs to release/unbind them in order for them to become available.

6) When OMX_PortFlush is done - the omx component  returns all the
buffers from teh omx queue. But the decoder also needs to release
the buffers that it is holding. The flag "iIsBufferInComponentQueue"
helps the omx component differentiate between buffers that are
held by the decoder or simply queued in the omx queue.

7) The Flag "iIsBufferInComponentQueue" is true
only when the buffer is held in the omx component queue.
If the buffer is held by the decoder or the buffer is sent to omx client
iIsBufferInComponentQueue is false.

*/

typedef struct BufferCtrlStruct
{
    OMX_U32 iRefCount;
    OMX_BOOL iIsBufferInComponentQueue;
} BufferCtrlStruct;

/**
 * This is the Component template from which all
 * other Component instances are factored by the core.
 */
/**
 * The structure for port Type.
 */
typedef struct ComponentPortType
{
    /** @param pBuffer An array of pointers to buffer headers. */
    OMX_BUFFERHEADERTYPE** pBuffer;
    /** @param BufferState The State of the Buffer whether assigned or allocated */
    OMX_U32* BufferState;
    /** @param NumAssignedBuffers Number of buffer assigned on each port */
    OMX_U32 NumAssignedBuffers;
    /** @param pBufferQueue queue for buffer to be processed by the port */
    QueueType* pBufferQueue;
    OMX_STATETYPE TransientState;
    /** @param BufferUnderProcess  Boolean variables indicate whether the port is processing any buffer */
    OMX_BOOL BufferUnderProcess;
    OMX_PARAM_PORTDEFINITIONTYPE PortParam;
    /** @param NumBufferFlushed Number of buffer Flushed */
    OMX_U32 NumBufferFlushed;
    /** @param IsPortFlushed Boolean variables indicate port is being flushed at the moment */
    OMX_BOOL IsPortFlushed;

    //AUDIO SPECIFIC PARAMETERS
    OMX_AUDIO_PARAM_PORTFORMATTYPE  AudioParam;
    OMX_AUDIO_PARAM_PCMMODETYPE     AudioPcmMode;
    OMX_AUDIO_PARAM_WMATYPE         AudioWmaParam;
    OMX_AUDIO_PARAM_MP3TYPE         AudioMp3Param;
    OMX_AUDIO_CONFIG_EQUALIZERTYPE  AudioEqualizerType;
    OMX_AUDIO_PARAM_AACPROFILETYPE  AudioAacParam;
    OMX_AUDIO_PARAM_AMRTYPE         AudioAmrParam;
    OMX_AUDIO_PARAM_RATYPE          AudioRaParam;

    //VIDEO SPECIFIC PARAMETERS
    OMX_VIDEO_PARAM_PORTFORMATTYPE   VideoParam[OMX_PORT_NUMBER_FORMATS_SUPPORTED];
    OMX_VIDEO_PARAM_PROFILELEVELTYPE ProfileLevel;
    OMX_VIDEO_PARAM_MPEG4TYPE        VideoMpeg4;
    OMX_VIDEO_PARAM_H263TYPE         VideoH263;
    OMX_VIDEO_PARAM_AVCTYPE          VideoAvc;
    OMX_VIDEO_PARAM_WMVTYPE          VideoWmv;
    OMX_VIDEO_PARAM_RVTYPE           VideoRv;

    //VIDEO ENCODER SPECIFIC PARAMETERS
    OMX_CONFIG_ROTATIONTYPE             VideoOrientationType;
    OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE VideoErrorCorrection;
    OMX_VIDEO_PARAM_BITRATETYPE         VideoRateType;      //Only for rate control type in mpeg4
    OMX_VIDEO_PARAM_BITRATETYPE         VideoMaxRate;       //to store max bit rate
    OMX_VIDEO_PARAM_QUANTIZATIONTYPE    VideoQuantType;
    OMX_VIDEO_PARAM_VBSMCTYPE           VideoBlockMotionSize;
    OMX_VIDEO_PARAM_MOTIONVECTORTYPE    VideoMotionVector;
    OMX_VIDEO_PARAM_INTRAREFRESHTYPE    VideoIntraRefresh;
    OMX_VIDEO_PARAM_AVCSLICEFMO         AvcSliceFMO;

    OMX_CONFIG_FRAMERATETYPE            VideoConfigFrameRateType;
    OMX_VIDEO_CONFIG_BITRATETYPE        VideoConfigBitRateType;

    //This will be used to dynamically request the Iframe using SetConfig API
    OMX_CONFIG_INTRAREFRESHVOPTYPE      VideoIFrame;

    OMX_U32  ActualNumPortFormatsSupported;

} ComponentPortType;



class OmxComponentBase : public OsclActiveObject
{
    public:

        OSCL_IMPORT_REF OmxComponentBase();
        OSCL_IMPORT_REF virtual ~OmxComponentBase();

        /** Component entry points declarations without proxy interface*/
        static OMX_ERRORTYPE BaseComponentGetComponentVersion(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STRING pComponentName,
            OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
            OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
            OMX_OUT OMX_UUIDTYPE* pComponentUUID);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentGetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_INOUT OMX_PTR pComponentConfigStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentSetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_IN  OMX_PTR pComponentConfigStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentGetExtensionIndex(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentGetState(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STATETYPE* pState);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentGetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR ComponentParameterStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentSetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_IN  OMX_PTR ComponentParameterStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentUseBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentAllocateBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentFreeBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentSendCommand(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_COMMANDTYPE Cmd,
            OMX_IN  OMX_U32 nParam,
            OMX_IN  OMX_PTR pCmdData);

        static OMX_ERRORTYPE BaseComponentComponentDeInit(
            OMX_IN  OMX_HANDLETYPE hComponent);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentEmptyThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentFillThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentSetCallbacks(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_CALLBACKTYPE* pCallbacks,
            OMX_IN  OMX_PTR pAppData);

        /** Component entry points declarations with proxy interface*/
#if PROXY_INTERFACE
        static OMX_ERRORTYPE BaseComponentProxyGetComponentVersion(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STRING pComponentName,
            OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
            OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
            OMX_OUT OMX_UUIDTYPE* pComponentUUID);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyGetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR ComponentParameterStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxySetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_IN  OMX_PTR ComponentParameterStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyGetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_INOUT OMX_PTR pComponentConfigStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxySetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_IN  OMX_PTR pComponentConfigStructure);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyGetExtensionIndex(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyGetState(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_OUT OMX_STATETYPE* pState);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyUseBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyAllocateBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyFreeBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxySendCommand(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_COMMANDTYPE Cmd,
            OMX_IN  OMX_U32 nParam,
            OMX_IN  OMX_PTR pCmdData);

        static OMX_ERRORTYPE BaseComponentProxyComponentDeInit(
            OMX_IN  OMX_HANDLETYPE hComponent);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyEmptyThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        static OSCL_IMPORT_REF OMX_ERRORTYPE BaseComponentProxyFillThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        static OMX_ERRORTYPE BaseComponentProxySetCallbacks(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_CALLBACKTYPE* pCallbacks,
            OMX_IN  OMX_PTR pAppData);

#endif // PROXY_INTERFACE

        /*NON STATIC COUNTERPARTS OF STATIC MEMBER API'S */

        //Pure virtual functions, definition to be written in derived class
        virtual OMX_ERRORTYPE GetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR ComponentParameterStructure) = 0;

        virtual OMX_ERRORTYPE SetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_IN  OMX_PTR ComponentParameterStructure) = 0;

        virtual OSCL_IMPORT_REF OMX_ERRORTYPE GetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_INOUT OMX_PTR pComponentConfigStructure);

        //Making Setconfig as virtual function to be implemented in respective component class
        virtual OSCL_IMPORT_REF OMX_ERRORTYPE SetConfig(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nIndex,
            OMX_IN  OMX_PTR pComponentConfigStructure);

        OMX_ERRORTYPE GetExtensionIndex(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_STRING cParameterName,
            OMX_OUT OMX_INDEXTYPE* pIndexType);

        void GetState(OMX_OUT OMX_STATETYPE* pState);

        OMX_ERRORTYPE UseBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes,
            OMX_IN OMX_U8* pBuffer);

        OMX_ERRORTYPE AllocateBuffer(
            OMX_IN OMX_HANDLETYPE hComponent,
            OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
            OMX_IN OMX_U32 nPortIndex,
            OMX_IN OMX_PTR pAppPrivate,
            OMX_IN OMX_U32 nSizeBytes);

        OMX_ERRORTYPE FreeBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        OMX_ERRORTYPE SendCommand(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_COMMANDTYPE Cmd,
            OMX_IN  OMX_S32 nParam,
            OMX_IN  OMX_PTR pCmdData);

        OMX_ERRORTYPE EmptyThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        OMX_ERRORTYPE FillThisBuffer(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

        OMX_ERRORTYPE SetCallbacks(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_CALLBACKTYPE* pCallbacks,
            OMX_IN  OMX_PTR pAppData);


        OMX_PTR GetOmxHandle()
        {
            return &iOmxComponent;
        };

        /*OTHER PROCESSING FUNCTIONS */

        //Pure virtual function called from base, must have a definition in derived components
        //virtual void Decode() = 0;
        virtual void ProcessData() = 0;

        virtual OSCL_IMPORT_REF void BufferMgmtFunction();
        virtual OSCL_IMPORT_REF OMX_BOOL BufferMgmtWithoutMarker();
        //Extra routines called from BufferMgmtWithoutMarker that may vary from component to component
        virtual OSCL_IMPORT_REF void ComponentBufferMgmtWithoutMarker();
        OSCL_IMPORT_REF void TempInputBufferMgmtWithoutMarker();


        virtual void ProcessInBufferFlag() {};

        /* This function will exist only for audio components, thats why can't be made puire virtual
           Also putting it into the audio base class will require a check everytime in the
           BufferMgmtFunction() to create either an audio or video pointer to access this function
        */
        virtual void SyncWithInputTimestamp() {};
        virtual void ResetComponent() {};
        virtual void ReleaseReferenceBuffers() {};
        virtual QueueType *GetOutputQueue()
        {
            QueueType*              pOutputQueue = ipPorts[OMX_PORT_OUTPUTPORT_INDEX]->pBufferQueue;
            return pOutputQueue;
        };

        // helper function for avc
        virtual void InvalidateOutputBuffer()
        {
            ipOutputBuffer = NULL;
            iNewOutBufRequired = OMX_TRUE;
        }


        virtual OMX_ERRORTYPE ReAllocatePartialAssemblyBuffers(OMX_BUFFERHEADERTYPE* aInputBufferHdr)
        {
            OSCL_UNUSED_ARG(aInputBufferHdr);
            return OMX_ErrorNone;
        }

        OMX_BOOL AssemblePartialFrames(OMX_BUFFERHEADERTYPE* aInputBuffer);
        OSCL_IMPORT_REF virtual OMX_BOOL ParseFullAVCFramesIntoNALs(OMX_BUFFERHEADERTYPE* aInputBuffer);
        OSCL_IMPORT_REF virtual OMX_BOOL DetectStartCodeLength(OMX_U8* aBitstream, OMX_U8** aNalUnit, OMX_U32 aBufSize, OMX_U32* aSCSize);
        OMX_ERRORTYPE MessageHandler(CoreMessage* Message);
        OMX_ERRORTYPE DoStateSet(OMX_U32);

        OMX_ERRORTYPE DisablePort(OMX_S32 PortIndex);
        void DisableSinglePort(OMX_U32 PortIndex);

        OMX_ERRORTYPE EnablePort(OMX_S32 PortIndex);
        void EnableSinglePort(OMX_U32 PortIndex);

        OMX_ERRORTYPE FlushPort(OMX_S32 PortIndex);
        void SetPortFlushFlag(OMX_S32, OMX_S32 index, OMX_BOOL value);
        void SetNumBufferFlush(OMX_S32, OMX_S32 index, OMX_S32 value);

        OSCL_IMPORT_REF void ReturnInputBuffer(OMX_BUFFERHEADERTYPE* pInputBuffer, ComponentPortType* pPort);
        OSCL_IMPORT_REF void ReturnOutputBuffer(OMX_BUFFERHEADERTYPE* pOutputBuffer, ComponentPortType* pPort);


        virtual OMX_ERRORTYPE ComponentInit() = 0;
        virtual OMX_ERRORTYPE ComponentDeInit() = 0;

        OSCL_IMPORT_REF OMX_ERRORTYPE ConstructBaseComponent(OMX_PTR pAppData);
        OSCL_IMPORT_REF OMX_ERRORTYPE DestroyBaseComponent();

        OMX_ERRORTYPE TunnelRequest(
            OMX_IN  OMX_HANDLETYPE hComp,
            OMX_IN  OMX_U32 nPort,
            OMX_IN  OMX_HANDLETYPE hTunneledComp,
            OMX_IN  OMX_U32 nTunneledPort,
            OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup);

        OMX_ERRORTYPE ParameterSanityCheck(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_U32 nPortIndex,
            OMX_IN  OMX_PTR pStructure,
            OMX_IN  size_t size);

        OSCL_IMPORT_REF void SetHeader(OMX_PTR aheader, OMX_U32 asize);
        OSCL_IMPORT_REF OMX_ERRORTYPE CheckHeader(OMX_PTR aheader, OMX_U32 asize);

        void ResetAfterFlush(OMX_S32 PortIndex);

        //Flag to call BufferMgmtFunction in the RunL() when the component state is executing
        OMX_BOOL                iBufferExecuteFlag;
        ComponentPrivateType*   ipAppPriv;
        OMX_U32             iNumAvailableOutputBuffers; // make it public to be able to modify it from
        // codec apis without much hassle

    protected:

        PVLogger* iLogger;

#if PROFILING_ON
        PVLogger* iDiagnosticsLogger;
#endif

        OSCL_IMPORT_REF void Run();


        OMX_CALLBACKTYPE*   ipCallbacks;
        OMX_PTR             iCallbackData;
        OMX_STATETYPE       iState;

        CoreDescriptorType* ipCoreDescriptor;
        OMX_U32             iNumInputBuffer;

        OMX_U8*             ipFrameDecodeBuffer;
        OMX_BOOL            iPartialFrameAssembly;
        OMX_BOOL            iIsInputBufferEnded;
        OMX_BOOL            iEndofStream;
        OMX_U8*             ipTempInputBuffer;
        OMX_U32             iTempInputBufferLength;
        OMX_COMPONENTTYPE*  ipTargetComponent;
        OMX_PTR             iTargetMarkData;
        OMX_COMPONENTTYPE*  ipTempTargetComponent;
        OMX_PTR             iTempTargetMarkData;
        OMX_BOOL            iMarkPropagate;
        OMX_BOOL            iNewInBufferRequired;
        OMX_BOOL            iNewOutBufRequired;
        OMX_U32             iTempConsumedLength;
        OMX_U32             iOutBufferCount;

        OMX_BOOL            iCodecReady;
        OMX_U8*             ipInputCurrBuffer;
        OMX_U32             iInputCurrBufferSize;
        OMX_U32             iInputCurrLength;
        OMX_S32             iFrameCount;
        OMX_BOOL            iStateTransitionFlag;

        OMX_BOOL                iEndOfFrameFlag;
        OMX_BUFFERHEADERTYPE*   ipInputBuffer;

        OMX_BUFFERHEADERTYPE*   ipOutputBuffer;

        OMX_U32                 iOutputFrameLength;
        OMX_COMPONENTTYPE       iOmxComponent;  // structure
        OMX_U32                 iNumPorts;
        OMX_U32                 iCompressedFormatPortNum;
        OMX_PTR                 ipComponentProxy;

        PV_OMXComponentCapabilityFlagsType iPVCapabilityFlags;

        //The ports of the component
        ComponentPortType** ipPorts;
        //Indicate whether component has been already initialized */
        OMX_BOOL iIsInit;
        //OpenMAX standard parameter that contains a short description of the available ports
        OMX_PORT_PARAM_TYPE iPortTypesParam;
        OMX_U32 iGroupPriority;
        //ID of a group of components that share the same logical chain
        OMX_U32 iGroupID;
        //Roles of the component
        OMX_U8 iComponentRole[OMX_MAX_STRINGNAME_SIZE];
        //Flag to indicate whether role has been set by client or not
        OMX_BOOL iComponentRoleFlag;
        //This field holds the private data associated with a mark request, if any
        OMX_MARKTYPE* ipMark;

        OMX_BOOL                iEosProcessing;
        OMX_BOOL                iFirstFragment;
        OMX_TICKS               iFrameTimestamp;
        OMX_BOOL                iRepositionFlag;
        OMX_U32                 iSamplesPerFrame;
        OMX_BOOL                iSilenceInsertionInProgress;
        OMX_U32                 iSilenceFramesNeeded;
        OMX_BOOL                iIsFirstOutputFrame;
        OMX_S32                 iInputBufferRemainingBytes;
        OMX_BOOL                iResizePending;
        OMX_U8*                 ipTempOutBufferForPortReconfig;
        OMX_U32                 iSizeOutBufferForPortReconfig;
        OMX_BOOL                iSendOutBufferAfterPortReconfigFlag;
        OMX_TICKS               iTimestampOutBufferForPortReconfig;
        OMX_TICKS               iOutputMicroSecPerFrame;


        OMX_BOOL                iIsAudioComponent;
        OMX_U32                 iNumOfPartialFragmentsReceived;
        RvSegmentInfoType       iPartialFragmentInfo[OMX_RV_MAX_INPUT_SEGMENTS];


};


class OmxComponentAudio : public OmxComponentBase
{
    public:
        OSCL_IMPORT_REF OmxComponentAudio();
        OSCL_IMPORT_REF virtual ~OmxComponentAudio();

        OSCL_IMPORT_REF OMX_ERRORTYPE GetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR ComponentParameterStructure);

        OSCL_IMPORT_REF OMX_ERRORTYPE SetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_IN  OMX_PTR ComponentParameterStructure);

        virtual void UpdateAACPlusFlag(OMX_BOOL aAacPlusFlag)
        {
            OSCL_UNUSED_ARG(aAacPlusFlag);
        }
};



class OmxComponentVideo : public OmxComponentBase
{
    public:
        OSCL_IMPORT_REF OmxComponentVideo();
        OSCL_IMPORT_REF virtual ~OmxComponentVideo();

        OSCL_IMPORT_REF OMX_ERRORTYPE GetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_INOUT OMX_PTR ComponentParameterStructure);

        OSCL_IMPORT_REF OMX_ERRORTYPE SetParameter(
            OMX_IN  OMX_HANDLETYPE hComponent,
            OMX_IN  OMX_INDEXTYPE nParamIndex,
            OMX_IN  OMX_PTR ComponentParameterStructure);

        OSCL_IMPORT_REF virtual void CalculateBufferParameters(OMX_U32 PortIndex);
};


#endif      //#ifndef PV_OMXCOMPONENT_H_INCLUDED
