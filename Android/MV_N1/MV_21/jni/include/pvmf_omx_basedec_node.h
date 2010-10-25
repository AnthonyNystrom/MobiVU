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
#ifndef PVMF_OMX_BASEDEC_NODE_H_INCLUDED
#define PVMF_OMX_BASEDEC_NODE_H_INCLUDED

#ifndef PVMF_NODE_INTERFACE_IMPL_H_INCLUDED
#include "pvmf_node_interface_impl.h"
#endif
#ifndef OSCL_MEM_MEMPOOL_H_INCLUDED
#include "oscl_mem_mempool.h"
#endif
#ifndef PVMF_SIMPLE_MEDIA_BUFFER_H_INCLUDED
#include "pvmf_simple_media_buffer.h"
#endif
#ifndef PVMF_POOL_BUFFER_ALLOCATOR_H_INCLUDED
#include "pvmf_pool_buffer_allocator.h"
#endif
#ifndef PVMF_OMX_BASEDEC_PORT_H_INCLUDED
#include "pvmf_omx_basedec_port.h"
#endif
#ifndef PVMF_OMX_BASEDEC_NODE_EXTENSION_INTERFACE_H_INCLUDED
#include "pvmf_omx_basedec_node_extension_interface.h"
#endif
#ifndef PVMF_META_DATA_EXTENSION_H_INCLUDED
#include "pvmf_meta_data_extension.h"
#endif
#ifndef PV_MIME_STRING_UTILS_H_INCLUDED
#include "pv_mime_string_utils.h"
#endif
#ifndef OMX_Core_h
#include "OMX_Core.h"
#endif
#ifndef OMX_Component_h
#include "OMX_Component.h"
#endif
#ifndef PVMF_OMX_BASEDEC_CALLBACKS_H_INCLUDED
#include "pvmf_omx_basedec_callbacks.h"
#endif
#ifndef __MEDIA_CLOCK_CONVERTER_H
#include "media_clock_converter.h"
#endif
#if (PVLOGGER_INST_LEVEL >= PVLOGMSG_INST_REL)
#ifndef PVMF_MEDIA_CLOCK_H_INCLUDED
#include "pvmf_media_clock.h"
#endif
#endif
#ifndef PVMF_FIXEDSIZE_BUFFER_ALLOC_H_INCLUDED
#include "pvmf_fixedsize_buffer_alloc.h"
#endif
#ifndef PVMF_MEDIA_CMD_H_INCLUDED
#include "pvmf_media_cmd.h"
#endif
#ifndef PVMF_MEDIA_MSG_FORMAT_IDS_H_INCLUDED
#include "pvmf_media_msg_format_ids.h"
#endif
#ifndef PVMI_KVP_UTIL_H_INCLUDED
#include "pvmi_kvp_util.h"
#endif
#ifndef PV_OMXCORE_H_INCLUDED
#include "pv_omxcore.h"
#endif
#ifndef PV_OMX_CONFIG_PARSER_H_INCLUDED
#include "pv_omx_config_parser.h"
#endif

#define MAX_NAL_PER_FRAME 100

typedef struct OutputBufCtrlStruct
{
    OMX_BUFFERHEADERTYPE *pBufHdr;
    OsclAny *pMemPoolEntry;
} OutputBufCtrlStruct;

class InputBufCtrlStruct
{
    public:
        InputBufCtrlStruct(): pBufHdr(NULL), pMemPoolEntry(NULL) {}

    public:
        OMX_BUFFERHEADERTYPE *pBufHdr;
        // PVMFSharedMediaDataPtr is a OsclSharedPtr and this object is already set to NULL in its constructor.
        PVMFSharedMediaDataPtr pMediaData;
        OsclAny *pMemPoolEntry;
};

// fwd class declaration
class PVLogger;
//memory allocator type for this node.
typedef OsclMemAllocator PVMFOMXBaseDecNodeAllocator;


// CALLBACK PROTOTYPES
OMX_ERRORTYPE CallbackEventHandler(OMX_OUT OMX_HANDLETYPE aComponent,
                                   OMX_OUT OMX_PTR aAppData,
                                   OMX_OUT OMX_EVENTTYPE aEvent,
                                   OMX_OUT OMX_U32 aData1,
                                   OMX_OUT OMX_U32 aData2,
                                   OMX_OUT OMX_PTR aEventData);

OMX_ERRORTYPE CallbackEmptyBufferDone(OMX_OUT OMX_HANDLETYPE aComponent,
                                      OMX_OUT OMX_PTR aAppData,
                                      OMX_OUT OMX_BUFFERHEADERTYPE* aBuffer);

OMX_ERRORTYPE CallbackFillBufferDone(OMX_OUT OMX_HANDLETYPE aComponent,
                                     OMX_OUT OMX_PTR aAppData,
                                     OMX_OUT OMX_BUFFERHEADERTYPE* aBuffer);

//Default values for number of Input/Output buffers. If the component needs more than this, it will be
// negotiated. If the component does not need more than this number, the default is used
#define NUMBER_INPUT_BUFFER 5
#define NUMBER_OUTPUT_BUFFER 19

// nal start code is 0001
#define NAL_START_CODE_SIZE 4
const unsigned char NAL_START_CODE[4] = {0, 0, 0, 1};

/////////////////////////
// CUSTOM DEALLOCATOR FOR MEDIA DATA SHARED PTR WRAPPER:
//                      1) Deallocates the underlying output buffer
//                      2) Deallocates the pvci buffer wrapper and the rest of accompanying structures
//                    Deallocator is created as part of the wrapper, and travels with the buffer wrapper

class PVOMXDecBufferSharedPtrWrapperCombinedCleanupDA : public OsclDestructDealloc
{
    public:
        PVOMXDecBufferSharedPtrWrapperCombinedCleanupDA(Oscl_DefAlloc* allocator, void *pMempoolData) :
                buf_alloc(allocator), ptr_to_data_to_dealloc(pMempoolData) {};
        virtual ~PVOMXDecBufferSharedPtrWrapperCombinedCleanupDA() {};

        virtual void destruct_and_dealloc(OsclAny* ptr)
        {
            // call buffer deallocator
            if (buf_alloc != NULL)
            {
                buf_alloc->deallocate(ptr_to_data_to_dealloc);
            }

            const uint8* const my_ptr = (uint8*)ptr;
            const uint aligned_refcnt_size = oscl_mem_aligned_size(sizeof(OsclRefCounterDA));
            const uint aligned_cleanup_size = oscl_mem_aligned_size(sizeof(PVOMXDecBufferSharedPtrWrapperCombinedCleanupDA));
            PVMFMediaDataImpl* media_data_ptr = OSCL_REINTERPRET_CAST(PVMFMediaDataImpl*, (my_ptr + aligned_refcnt_size + aligned_cleanup_size));
            media_data_ptr->~PVMFMediaDataImpl();
            // finally, free the shared ptr wrapper memory
            oscl_free(ptr);
        }

    private:
        Oscl_DefAlloc* buf_alloc;
        void *ptr_to_data_to_dealloc;
};

class PVOMXDecInputBufferSharedPtrWrapperCombinedCleanupDA : public OsclDestructDealloc
{
    public:
        PVOMXDecInputBufferSharedPtrWrapperCombinedCleanupDA(OsclRefCounterDA* refcnt) :
                ref_cnt(refcnt)
        {
            ref_cnt->addRef();
        };
        virtual ~PVOMXDecInputBufferSharedPtrWrapperCombinedCleanupDA() {};

        virtual void destruct_and_dealloc(OsclAny* ptr)
        {
            ref_cnt->removeRef();
            // this is needed to completely free PVMFMediaDataImpl, since it allocates memory for the frag list
            ((PVMFMediaDataImpl*)ptr)->~PVMFMediaDataImpl();
            oscl_free(ptr);
        }

    private:
        OsclRefCounterDA* ref_cnt;
        Oscl_DefAlloc* buf_alloc;
        void *ptr_to_data_to_dealloc;
};

//Default vector reserve size
#define PVMF_OMXBASEDEC_NODE_COMMAND_VECTOR_RESERVE 10

//Starting value for command IDs
#define PVMF_OMXBASEDEC_NODE_COMMAND_ID_START 6000

// The number of characters to allocate for the key string
#define PVOMXBASEDECNODECONFIG_KEYSTRING_SIZE 128

//Mimetypes for the custom interface
#define PVMF_OMX_BASE_DEC_NODE_MIMETYPE "pvxxx/OMXBaseDecNode"
#define PVMF_BASEMIMETYPE "pvxxx"

// Structure to hold the key string info for capability-and-config
struct PVOMXBaseDecNodeKeyStringData
{
    char iString[64];
    PvmiKvpType iType;
    PvmiKvpValueType iValueType;
};

class PVMFOMXBaseDecNode: public PVMFNodeInterfaceImpl
        , public OsclMemPoolFixedChunkAllocatorObserver
        , public PVMFOMXBaseDecNodeExtensionInterface
        , public PVMFMetadataExtensionInterface
        , public PvmiCapabilityAndConfigBase
{
    public:
        OSCL_IMPORT_REF PVMFOMXBaseDecNode(int32 aPriority, const char aAOName[]);
        OSCL_IMPORT_REF virtual ~PVMFOMXBaseDecNode();

        // From PVMFPortActivityHandler
        OSCL_IMPORT_REF void HandlePortActivity(const PVMFPortActivity& aActivity);

        // From PVInterface
        OSCL_IMPORT_REF virtual void addRef();
        OSCL_IMPORT_REF virtual void removeRef();
        OSCL_IMPORT_REF virtual bool queryInterface(const PVUuid& uuid, PVInterface*& iface);
        OSCL_IMPORT_REF virtual PVMFStatus SetDecoderNodeConfiguration(PVMFOMXBaseDecNodeConfig& aConfig);

        //**********begin PVMFMetadataExtensionInterface
        OSCL_IMPORT_REF virtual uint32 GetNumMetadataKeys(char* query_key = NULL) = 0;
        OSCL_IMPORT_REF virtual uint32 GetNumMetadataValues(PVMFMetadataList& aKeyList) = 0;
        OSCL_IMPORT_REF PVMFCommandId GetNodeMetadataKeys(PVMFSessionId aSessionId, PVMFMetadataList& aKeyList, uint32 starting_index, int32 max_entries,
                char* query_key = NULL, const OsclAny* aContextData = NULL);
        OSCL_IMPORT_REF PVMFCommandId GetNodeMetadataValues(PVMFSessionId aSessionId, PVMFMetadataList& aKeyList,
                Oscl_Vector<PvmiKvp, OsclMemAllocator>& aValueList, uint32 starting_index, int32 max_entries, const OsclAny* aContextData = NULL);
        OSCL_IMPORT_REF PVMFStatus ReleaseNodeMetadataKeys(PVMFMetadataList& aKeyList, uint32 starting_index, uint32 end_index);
        OSCL_IMPORT_REF PVMFStatus ReleaseNodeMetadataValues(Oscl_Vector<PvmiKvp, OsclMemAllocator>& aValueList, uint32 starting_index, uint32 end_index);
        //**********End PVMFMetadataExtensionInterface
        OSCL_IMPORT_REF virtual bool VerifyParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters, int num_elements) = 0;

        //********** CB Functions to serve OpenMAX Decoder

        //Process callback functions. They will be executed in testapp thread context
        //  These callbacks are used only in the Multithreaded component case
        OSCL_IMPORT_REF OsclReturnCode ProcessCallbackEventHandler_MultiThreaded(OsclAny* P);
        OSCL_IMPORT_REF OsclReturnCode ProcessCallbackEmptyBufferDone_MultiThreaded(OsclAny* P);
        OSCL_IMPORT_REF OsclReturnCode ProcessCallbackFillBufferDone_MultiThreaded(OsclAny* P);

        //Callback objects - again, these are used only in the case of Multithreaded component
        EventHandlerThreadSafeCallbackAO*    iThreadSafeHandlerEventHandler;
        EmptyBufferDoneThreadSafeCallbackAO* iThreadSafeHandlerEmptyBufferDone;
        FillBufferDoneThreadSafeCallbackAO*  iThreadSafeHandlerFillBufferDone;

        OMX_CALLBACKTYPE       iCallbacks; // structure that contains callback ptrs.
        // OMX CALLBACKS
        // 1) AO OMX component running in the same thread as the OMX node
        //  In this case, the callbacks can be called directly from the component
        //  The callback: OMX Component->CallbackEventHandler->EventHandlerProcessing
        //  The callback can perform do RunIfNotReady

        // 2) Multithreaded component
        //  In this case, the callback is made using the threadsafe callback (TSCB) AO
        //  Component thread : OMX Component->CallbackEventHandler->TSCB(ReceiveEvent)
        //  Node thread      : TSCB(ProcessEvent)->ProcessCallbackEventHandler_MultiThreaded->EventHandlerProcessing


        //==============================================================================

        OSCL_IMPORT_REF OMX_ERRORTYPE EventHandlerProcessing(OMX_OUT OMX_HANDLETYPE aComponent,
                OMX_OUT OMX_PTR aAppData,
                OMX_OUT OMX_EVENTTYPE aEvent,
                OMX_OUT OMX_U32 aData1,
                OMX_OUT OMX_U32 aData2,
                OMX_OUT OMX_PTR aEventData);

        OSCL_IMPORT_REF OMX_ERRORTYPE EmptyBufferDoneProcessing(OMX_OUT OMX_HANDLETYPE aComponent,
                OMX_OUT OMX_PTR aAppData,
                OMX_OUT OMX_BUFFERHEADERTYPE* aBuffer);

        OSCL_IMPORT_REF OMX_ERRORTYPE FillBufferDoneProcessing(OMX_OUT OMX_HANDLETYPE aComponent,
                OMX_OUT OMX_PTR aAppData,
                OMX_OUT OMX_BUFFERHEADERTYPE* aBuffer);


        bool IsComponentMultiThreaded()
        {
            return iIsOMXComponentMultiThreaded;
        };

        // From PvmiCapabilityAndConfig
        OSCL_IMPORT_REF PVMFStatus getParametersSync(PvmiMIOSession aSession, PvmiKeyType aIdentifier, PvmiKvp*& aParameters, int& aNumParamElements, PvmiCapabilityContext aContext);
        OSCL_IMPORT_REF PVMFStatus releaseParameters(PvmiMIOSession aSession, PvmiKvp* aParameters, int aNumElements);
        OSCL_IMPORT_REF void setParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters, int aNumElements, PvmiKvp* &aRetKVP);
        OSCL_IMPORT_REF PVMFStatus verifyParametersSync(PvmiMIOSession aSession, PvmiKvp* aParameters, int aNumElements);

        OSCL_IMPORT_REF virtual bool ProcessIncomingMsg(PVMFPortInterface* aPort);
        OSCL_IMPORT_REF virtual void Run();

    protected:
        virtual PVMFStatus DoQueryUuid() = 0;
        virtual PVMFStatus DoRequestPort(PVMFPortInterface*&) = 0;
        virtual PVMFStatus DoGetNodeMetadataKey() = 0;
        virtual PVMFStatus DoGetNodeMetadataValue() = 0;
        OSCL_IMPORT_REF PVMFStatus DoQueryInterface();
        OSCL_IMPORT_REF PVMFStatus DoReleasePort();
        OSCL_IMPORT_REF PVMFStatus DoInit();
        OSCL_IMPORT_REF PVMFStatus DoPrepare();
        OSCL_IMPORT_REF PVMFStatus DoStart();
        OSCL_IMPORT_REF PVMFStatus DoStop();
        OSCL_IMPORT_REF PVMFStatus DoPause();
        OSCL_IMPORT_REF PVMFStatus DoReset();
        OSCL_IMPORT_REF PVMFStatus DoFlush();

        //bool ProcessCommand(PVMFOMXBaseDecNodeCommand& aCmd);
        bool ProcessOutgoingMsg(PVMFPortInterface* aPort);
        PVMFStatus HandleProcessingState();
        virtual PVMFStatus HandlePortReEnable() = 0;

        virtual bool InitDecoder(PVMFSharedMediaDataPtr&) = 0;

        OSCL_IMPORT_REF OsclAny* AllocateKVPKeyArray(int32& aLeaveCode, PvmiKvpValueType aValueType, int32 aNumElements);
        int32 PushKVPKey(OSCL_HeapString<OsclMemAllocator>& aString, PVMFMetadataList* aKeyList)
        {
            int32 leavecode = OsclErrNone;
            OSCL_TRY(leavecode, aKeyList->push_back(aString));
            return leavecode;
        }

        int32 PushKVPKey(const char* aString, PVMFMetadataList& aKeyList)
        {
            int32 leavecode = OsclErrNone;
            OSCL_TRY(leavecode, aKeyList.push_back(aString));
            return leavecode;
        }

        int32 PushKVP(PvmiKvp aKVP, Oscl_Vector<PvmiKvp, OsclMemAllocator>& aValueList)
        {
            int32 leavecode = OsclErrNone;
            OSCL_TRY(leavecode, aValueList.push_back(aKVP));
            return leavecode;
        }

        int32 AllocateChunkFromMemPool(OsclAny*& aCtrlStructPtr, OsclMemPoolFixedChunkAllocator *aMemPool, int32 aAllocSize)
        {
            int32 leavecode = OsclErrNone;
            OSCL_TRY(leavecode, aCtrlStructPtr = (OsclAny *) aMemPool->allocate(aAllocSize));
            return leavecode;
        }

        // DV:
        virtual bool NegotiateComponentParameters(OMX_PTR aOutputParameters) = 0;

        bool CheckComponentForMultRoles(OMX_STRING aCompName, OMX_STRING aRole);

        bool CheckComponentCapabilities(PVMFFormatType* aFormat, OMX_PTR aOutputParameters);

        bool SetDefaultCapabilityFlags();
        OSCL_IMPORT_REF bool CreateOutMemPool(uint32 num);
        OSCL_IMPORT_REF bool CreateInputMemPool(uint32 num);
        OSCL_IMPORT_REF bool ProvideBuffersToComponent(OsclMemPoolFixedChunkAllocator *aMemPool, // allocator
                uint32 aAllocSize,   // size to allocate from pool (hdr only or hdr+ buffer)
                uint32 aNumBuffers,    // number of buffers
                uint32 aActualBufferSize, // aactual buffer size
                uint32 aPortIndex,      // port idx
                bool aUseBufferOK,  // can component use OMX_UseBuffer?
                bool    aIsThisInputBuffer // is this input or output
                                                      );

        bool FreeBuffersFromComponent(OsclMemPoolFixedChunkAllocator *aMemPool, // allocator
                                      uint32 aAllocSize,     // size to allocate from pool (hdr only or hdr+ buffer)
                                      uint32 aNumBuffers,    // number of buffers
                                      uint32 aPortIndex,      // port idx
                                      bool  aIsThisInputBuffer      // is this input or output
                                     );

        OsclSharedPtr<class PVMFMediaDataImpl> WrapOutputBuffer(uint8 *pData, uint32 aDataLen, OsclAny *pContext);
        virtual bool QueueOutputBuffer(OsclSharedPtr<PVMFMediaDataImpl> &mediadataimplout, uint32 aDataLen) = 0;

        bool SendOutputBufferToOMXComponent();
        OSCL_IMPORT_REF bool SendInputBufferToOMXComponent();

        OSCL_IMPORT_REF bool SendConfigBufferToOMXComponent(uint8 *initbuffer, uint32 initbufsize);
        bool SendEOSBufferToOMXComponent();

        bool HandleRepositioning(void);
        bool SendBeginOfMediaStreamCommand(void);
        bool SendEndOfTrackCommand(void);

        OSCL_IMPORT_REF bool AppendExtraDataToBuffer(InputBufCtrlStruct* aInputBuffer, OMX_EXTRADATATYPE aType, uint8* aExtraData, uint8 aDataLength);

        virtual bool ReleaseAllPorts() = 0;
        bool DeleteOMXBaseDecoder();

        OSCL_IMPORT_REF void ChangeNodeState(TPVMFNodeInterfaceState aNewState);

        OSCL_IMPORT_REF void HandleComponentStateChange(OMX_U32 decoder_state);

        // Capability And Config Helper Methods
        OSCL_IMPORT_REF virtual PVMFStatus DoCapConfigGetParametersSync(PvmiKeyType aIdentifier, PvmiKvp*& aParameters, int& aNumParamElements, PvmiCapabilityContext aContext);
        OSCL_IMPORT_REF virtual PVMFStatus DoCapConfigReleaseParameters(PvmiKvp* aParameters, int aNumElements);
        OSCL_IMPORT_REF virtual void DoCapConfigSetParameters(PvmiKvp* aParameters, int aNumElements, PvmiKvp* &aRetKVP);
        OSCL_IMPORT_REF virtual PVMFStatus DoCapConfigVerifyParameters(PvmiKvp* aParameters, int aNumElements);

        // From OsclMemPoolFixedChunkAllocatorObserver
        OSCL_IMPORT_REF void freechunkavailable(OsclAny*);

        //OSCL_IMPORT_REF PVMFCommandId QueueCommandL(PVMFOMXBaseDecNodeCommand& aCmd);

        OSCL_IMPORT_REF virtual int32 GetNAL_OMXNode(uint8** bitstream, uint32* size);
        OSCL_IMPORT_REF virtual bool ParseAndReWrapH264RAW(PVMFSharedMediaDataPtr& aMediaDataPtr);
        OSCL_IMPORT_REF virtual bool CreateAACConfigDataFromASF(uint8 *inptr, uint32 inlen, uint8 *outptr, uint32 &outlen);

        OSCL_IMPORT_REF void AllocatePvmiKey(PvmiKeyType* KvpKey, OsclMemAllocator* alloc, int32 KeyLength);

        friend class PVMFOMXBaseDecPort;

        // Ports pointers
        PVMFPortInterface* iInPort;
        PVMFPortInterface* iOutPort;

        // Shared pointer for Media Msg.Input buffer
        PVMFSharedMediaDataPtr iDataIn;

        // OUTPUT BUFFER RELATED MEMBERS
        // Output buffer memory pool
        OsclMemPoolFixedChunkAllocator *iOutBufMemoryPool;

        // Memory pool for simple output media data
        OsclMemPoolFixedChunkAllocator *iOutputMediaDataMemPool;

        // Memory pool for multi frag input media data
        OsclMemPoolFixedChunkAllocator *iInputMediaDataMemPool;

        // Fragment pool for format specific info
        PVMFBufferPoolAllocator iFsiFragmentAlloc;
        // Fragment pool for private data format specific info
        PVMFBufferPoolAllocator iPrivateDataFsiFragmentAlloc;

        // Size of output buffer (negotiated with component)
        uint32 iOMXComponentOutputBufferSize;

        // size of output to allocate (OMX_ALLOCATE_BUFFER =  size of buf header )
        // (OMX_USE_BUFFER = size of buf header + iOMXCoponentOutputBufferSize)
        uint32 iOutputAllocSize;

        // Number of output buffers (negotiated with component)
        uint32 iNumOutputBuffers;

        uint32 iOutputBufferAlignment;

        // Number of output buffers in possession of the component or downstream,
        // namely, number of unavailable buffers
        uint32 iNumOutstandingOutputBuffers;

        // flag to prevent sending output buffers downstream during flushing etc.
        bool iDoNotSendOutputBuffersDownstreamFlag;

        // flag to prevent freeing the buffers twice
        bool iOutputBuffersFreed;


        OsclAny *ipPrivateData;

        // INPUT BUFFER RELATED MEMBERS
        OsclMemPoolFixedChunkAllocator *iInBufMemoryPool;
        uint32 iOMXComponentInputBufferSize; // size of input buffer that the component sees (negotiated with the component)
        uint32 iInputAllocSize;     // size of input buffer to allocate (OMX_ALLOCATE_BUFFER =  size of buf header )
        // (OMX_USE_BUFFER = size of buf header + iOMXCoponentInputBufferSize)
        uint32 iNumInputBuffers; // total num of input buffers (negotiated with component)

        uint32 iInputBufferAlignment;

        uint32 iNumOutstandingInputBuffers; // number of input buffers in use (i.e. unavailable)

        bool iDoNotSaveInputBuffersFlag;

        // flag that requires the component to process config data (and report port settings changed event)
        // or return the config buffer before we dequeue any other data (BOS, EOS etc.)
        bool iIsConfigDataProcessingCompletionNeeded;

        // flag to prevent freeing buffers twice
        bool iInputBuffersFreed;

        // input buffer fragmentation etc.
        uint32 iCopyPosition;               // for copying memfrag data into a buffer
        uint32 iFragmentSizeRemainingToCopy;
        bool    iIsNewDataFragment;
        // partial frame assembly logic flags
        bool    iFirstPieceOfPartialFrame;
        bool    iObtainNewInputBuffer;
        bool    iKeepDroppingMsgsUntilMarkerBit;
        bool    iFirstDataMsgAfterBOS;
        InputBufCtrlStruct *iInputBufferUnderConstruction;
        bool    iIncompleteFrame;

        OSCL_IMPORT_REF void DropCurrentBufferUnderConstruction();
        OSCL_IMPORT_REF void SendIncompleteBufferUnderConstruction();

        // input data info
        uint32 iCurrFragNum;
        uint32 iCodecSeqNum;    // sequence number tracking
        uint32 iInPacketSeqNum;

        uint32 iInTimestamp;
        uint32 iInDuration;
        uint32 iInNumFrags;
        uint32 iCurrentMsgMarkerBit;

        // DYNAMIC PORT RE-CONFIGURATION
        uint32 iInputPortIndex;
        uint32 iOutputPortIndex;
        OMX_PARAM_PORTDEFINITIONTYPE iParamPort;
        uint32 iPortIndexForDynamicReconfig;
        bool iSecondPortReportedChange;
        bool iDynamicReconfigInProgress;
        uint32 iSecondPortToReconfig;
        bool iPauseCommandWasSentToComponent;
        bool iStopCommandWasSentToComponent;

        OMX_BUFFERHEADERTYPE *iInputBufferToResendToComponent; // ptr to input buffer that is not empty, but that the OMX component returned
        // we need to resend this same buffer back to the component

        ////////////////// OMX COMPONENT CAPABILITY RELATED MEMBERS
        bool iOMXComponentSupportsExternalOutputBufferAlloc;
        bool iOMXComponentSupportsExternalInputBufferAlloc;
        bool iOMXComponentSupportsMovableInputBuffers;
        bool iSetMarkerBitForEveryFrag; // is every fragment complete frame (e.g. AVC file playback = each fragment is a NAL)
        bool iIsOMXComponentMultiThreaded;
        bool iOMXComponentSupportsPartialFrames;
        bool iOMXComponentUsesNALStartCodes;
        bool iOMXComponentUsesFullAVCFrames;
        bool iOMXComponentCanHandleIncompleteFrames;
        bool iOMXComponentUsesInterleaved2BNALSizes;
        bool iOMXComponentUsesInterleaved4BNALSizes;

        // State definitions for HandleProcessingState() state machine
        typedef enum
        {
            EPVMFOMXBaseDecNodeProcessingState_Idle,                  //default state after constraction/reset
            EPVMFOMXBaseDecNodeProcessingState_InitDecoder,           //initialization of H264 decoder after handle was obtained
            EPVMFOMXBaseDecNodeProcessingState_WaitForInitCompletion, // waiting for init completion
            EPVMFOMXBaseDecNodeProcessingState_ReadyToDecode,         //nornal operation state of the decoder
            EPVMFOMXBaseDecNodeProcessingState_WaitForOutputBuffer,   //wait state for avalible media output buffer
            EPVMFOMXBaseDecNodeProcessingState_WaitForOutputPort,     //wait state, output port is busy
            EPVMFOMXBaseDecNodeProcessingState_WaitForOutgoingQueue,   //wait state, outgoing queue
            EPVMFOMXBaseDecNodeProcessingState_PortReconfig,            // Dynamic Port Reconfiguration - step 1
            EPVMFOMXBaseDecNodeProcessingState_WaitForBufferReturn,     //  step 2
            EPVMFOMXBaseDecNodeProcessingState_WaitForPortDisable,      // Dynamic Port Reconfiguration - step 3
            EPVMFOMXBaseDecNodeProcessingState_PortReEnable,            // Dynamic Port Reconfiguration - step 4
            EPVMFOMXBaseDecNodeProcessingState_WaitForPortEnable,       // step 5
            EPVMFOMXBaseDecNodeProcessingState_Stopping,                // when STOP command is issued, the node has to wait for component to transition into
            // idle state. The buffers keep coming back , the node is rescheduled
            // to run. Prevent the node from sending buffers back
            EPVMFOMXBaseDecNodeProcessingState_Pausing                  // when PAUSE command is issued, the node has to wait for component to transition into
            // paused state.
            // Video: This prevents the node from sending buffers back
            // Audio: The buffers may still keep coming back , the node is rescheduled
            // to run. Prevent the node from sending buffers back to component

        } PVMFOMXBaseDecNode_ProcessingState;

        // State of HandleProcessingState() state machine
        PVMFOMXBaseDecNode_ProcessingState iProcessingState;

        // Handle of OMX Component
        OMX_HANDLETYPE iOMXDecoder;

        // Current State of the component
        OMX_STATETYPE iCurrentDecoderState;

        // BOS
        bool iSendBOS;
        uint32 iStreamID;
        uint32 iBOSTimestamp;

        // repositioning related flags
        bool iIsRepositioningRequestSentToComponent;
        bool iIsRepositionDoneReceivedFromComponent;
        bool iIsOutputPortFlushed;
        bool iIsInputPortFlushed;

        //EOS control flags
        bool iIsEOSSentToComponent;
        bool iIsEOSReceivedFromComponent;

        // Send Fsi configuration flag
        bool    sendFsi;

        // Pointer to input data fragment
        uint8* iBitstreamBuffer;
        // Size of input data fragment
        int32 iBitstreamSize;

        // Output frame sequence counter
        uint32 iSeqNum;

        // Input frame sequence counter
        uint32 iSeqNum_In;

        // Added to Scheduler Flag
        bool iIsAdded;

        // Log related
        PVLogger* iLogger;
        PVLogger* iDataPathLogger;
        PVLogger* iClockLogger;
        PVLogger *iRunlLogger;

        // Counter of fragment read from current Media Msg.Input buffer
        uint fragnum;
        // Number of fragments in the Media Msg.Input buffer
        uint numfrags;

        // Time stamp to be used on output buffer
        uint32 iOutTimeStamp;

        // Duration to be used on output buffer
        // Maintain a vector to store the sample duration.
        Oscl_Vector<uint32, OsclMemAllocator> iSampleDurationVec;
        // Also store the timestamp in a vector. This will help us to
        // validate if the input sample was properly decoded.
        Oscl_Vector<uint32, OsclMemAllocator> iTimestampVec;

        // Node configuration update
        PVMFOMXBaseDecNodeConfig iNodeConfig;

        // Capability exchange
        PVMFNodeCapability iCapability;

        // Reference counter for extension
        uint32 iExtensionRefCount;

        // Vector for KVP
        Oscl_Vector<OSCL_HeapString<OsclMemAllocator>, OsclMemAllocator> iAvailableMetadataKeys;

        // EOS flag
        bool iEndOfDataReached;
        // Time stame upon EOS
        PVMFTimestamp iEndOfDataTimestamp;

        /* Diagnostic log related */
        PVLogger* iDiagnosticsLogger;
        bool iDiagnosticsLogged;
        void LogDiagnostics();

        uint32 iFrameCounter;

        uint32 iAvgBitrateValue;
        bool iResetInProgress;
        bool iResetMsgSent;
        bool iStopInResetMsgSent;

        // AVC NAL counter and size array
        uint32 iNALCount;
        uint32 iNALSizeArray[MAX_NAL_PER_FRAME]; // NAL count shouldn't exceed 100

        OutputBufCtrlStruct *out_ctrl_struct_ptr;

        InputBufCtrlStruct *in_ctrl_struct_ptr;


        PVInterface* ipExternalOutputBufferAllocatorInterface;
        PVMFFixedSizeBufferAlloc* ipFixedSizeBufferAlloc;
        bool iCompactFSISettingSucceeded;

        // Timestamp, timescale, OMX Ticks related conversions
        uint32 iInTimeScale;
        uint32 iOutTimeScale;
        uint32 iTimeScale;
        MediaClockConverter iInputTimestampClock;
        OMX_TICKS iOMXTicksTimestamp;
        uint32 iTimestampDeltaForMemFragment;
        OSCL_IMPORT_REF OMX_TICKS ConvertTimestampIntoOMXTicks(const MediaClockConverter &src);
        uint32 ConvertOMXTicksIntoTimestamp(const OMX_TICKS &src);

        uint8 iAACConfigData[4];
        uint32 iAACConfigDataLength;

        bool iIsVC1;
        bool iIsVC1AdvancedProfile;

        uint32 iTrackUnderVerificationConfigSize;
        uint8 *iTrackUnderVerificationConfig;
        Oscl_Vector<OMX_STRING, OsclMemAllocator> iOMXPreferredComponentOrderVec;

    private:
        OSCL_IMPORT_REF PVMFStatus HandleExtensionAPICommands();
        OSCL_IMPORT_REF PVMFStatus CancelCurrentCommand();
};


#endif // PVMF_OMXBASEDEC_NODE_H_INCLUDED

