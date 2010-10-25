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
#ifndef PVMF_NODE_INTERFACE_IMPL_H_INCLUDED
#define PVMF_NODE_INTERFACE_IMPL_H_INCLUDED

#ifndef PVMF_NODE_UTILS_H_INCLUDED
#include "pvmf_node_utils.h"
#endif

#ifndef PVMI_CONFIG_AND_CAPABILITY_H_INCLUDED
#include "pvmi_config_and_capability.h"
#endif

#ifndef PVLOGGER_H_INCLUDED
#include "pvlogger.h"
#endif

#ifndef OSCL_SCHEDULER_AO_H_INCLUDED
#include "oscl_scheduler_ao.h"
#endif

#ifndef PVMF_MEDIA_CLOCK_H_INCLUDED
#include "pvmf_media_clock.h"
#endif

#ifndef PVMF_NODE_INTERFACE_H_INCLUDED
#include "pvmf_node_interface.h"
#endif

#ifndef PVMF_META_DATA_TYPES_H_INCLUDED
#include "pvmf_meta_data_types.h"
#endif

#ifndef PVMF_BASIC_ERRORINFOMESSAGE_H_INCLUDED
#include "pvmf_basic_errorinfomessage.h"
#endif

#ifndef OSCL_MEM_AUDIT_H_INCLUDED
#include "oscl_mem_audit.h"
#endif

/* Default vector reserve size */
#define PVMF_NODE_VECTOR_RESERVE 10
/* Starting value for command IDs  */
#define PVMF_NODE_COMMAND_ID_START 0

// Allocation/Deallocation macros
#define PVMF_BASE_NODE_NEW(T,params) OSCL_AUDIT_NEW(iAuditCB, T, params)
#define PVMF_BASE_NODE_ARRAY_NEW(T,params) OSCL_AUDIT_ARRAY_NEW(iAuditCB, T,params)
#define PVMF_BASE_NODE_DELETE(ptr) OSCL_DELETE(ptr)
#define PVMF_BASE_NODE_ARRAY_DELETE(ptr) OSCL_ARRAY_DELETE(ptr)

// Default value of media sample Duration
#define PVMF_DEFAULT_TRACK_DURATION 0xFFFFFFFF

#define PVMFNodeCommandBase PVMFGenericNodeCommand<OsclMemAllocator>  // to remove typedef warning on symbian
class PVMFNodeCommand: public PVMFNodeCommandBase
{

    public:
        // Constructor and parser for GetNodeMetadataKeys
        void Construct(PVMFSessionId s, int32 cmd
                       , PVMFMetadataList& aKeyList
                       , uint32 aStartingIndex
                       , int32 aMaxEntries
                       , char* aQueryKey
                       , const OsclAny* aContext)
        {
            PVMFNodeCommandBase::Construct(s, cmd, aContext);
            iParam1 = (OsclAny*) & aKeyList;
            iParam2 = (OsclAny*)aStartingIndex;
            iParam3 = (OsclAny*)aMaxEntries;
            if (aQueryKey)
            {
                //allocate a copy of the query key string.
                Oscl_TAlloc<OSCL_HeapString<OsclMemAllocator>, OsclMemAllocator> str;
                iParam4 = str.ALLOC_AND_CONSTRUCT(aQueryKey);
            }
        }
        void Parse(PVMFMetadataList*& MetaDataListPtr, uint32 &aStartingIndex, int32 &aMaxEntries, char*& aQueryKey)
        {
            MetaDataListPtr = (PVMFMetadataList*)iParam1;
            aStartingIndex = (uint32)iParam2;
            aMaxEntries = (int32)iParam3;
            aQueryKey = NULL;
            if (iParam4)
            {
                OSCL_HeapString<OsclMemAllocator>* keystring = (OSCL_HeapString<OsclMemAllocator>*)iParam4;
                aQueryKey = keystring->get_str();
            }
        }

        // Constructor and parser for GetNodeMetadataValue
        void Construct(PVMFSessionId s, int32 cmd, PVMFMetadataList& aKeyList, Oscl_Vector<PvmiKvp, OsclMemAllocator>& aValueList, uint32 aStartIndex, int32 aMaxEntries, const OsclAny* aContext)
        {
            PVMFNodeCommandBase::Construct(s, cmd, aContext);
            iParam1 = (OsclAny*) & aKeyList;
            iParam2 = (OsclAny*) & aValueList;
            iParam3 = (OsclAny*)aStartIndex;
            iParam4 = (OsclAny*)aMaxEntries;

        }
        void Parse(PVMFMetadataList* &aKeyList, Oscl_Vector<PvmiKvp, OsclMemAllocator>* &aValueList, uint32 &aStartingIndex, int32 &aMaxEntries)
        {
            aKeyList = (PVMFMetadataList*)iParam1;
            aValueList = (Oscl_Vector<PvmiKvp, OsclMemAllocator>*)iParam2;
            aStartingIndex = (uint32)iParam3;
            aMaxEntries = (int32)iParam4;
        }

        // Constructor and parser for SetDataSourcePosition
        void Construct(PVMFSessionId s, int32 cmd, PVMFTimestamp aTargetNPT, PVMFTimestamp* aActualNPT, PVMFTimestamp* aActualMediaDataTS,
                       bool aSeekToSyncPoint, uint32 aStreamID, const OsclAny*aContext)
        {
            PVMFNodeCommandBase::Construct(s, cmd, aContext);
            iParam1 = (OsclAny*)aTargetNPT;
            iParam2 = (OsclAny*)aActualNPT;
            iParam3 = (OsclAny*)aActualMediaDataTS;
            iParam4 = (OsclAny*)aSeekToSyncPoint;
            iParam5 = (OsclAny*)aStreamID;
        }
        void Parse(PVMFTimestamp& aTargetNPT, PVMFTimestamp* &aActualNPT, PVMFTimestamp* &aActualMediaDataTS, bool& aSeekToSyncPoint, uint32& aStreamID)
        {
            aTargetNPT = (PVMFTimestamp)iParam1;
            aActualNPT = (PVMFTimestamp*)iParam2;
            aActualMediaDataTS = (PVMFTimestamp*)iParam3;
            aSeekToSyncPoint = (iParam4 ? true : false);
            aStreamID = (uint32)iParam5;
        }

        // Constructor and parser for QueryDataSourcePosition
        void Construct(PVMFSessionId s, int32 cmd, PVMFTimestamp aTargetNPT, PVMFTimestamp* aActualNPT, bool aSeekToSyncPoint, const OsclAny*aContext)
        {
            PVMFNodeCommandBase::Construct(s, cmd, aContext);
            iParam1 = (OsclAny*)aTargetNPT;
            iParam2 = (OsclAny*)aActualNPT;
            iParam3 = (OsclAny*)aSeekToSyncPoint;
            iParam4 = NULL;
            iParam5 = NULL;
        }
        void Parse(PVMFTimestamp& aTargetNPT, PVMFTimestamp* &aActualNPT, bool& aSeekToSyncPoint)
        {
            aTargetNPT = (PVMFTimestamp)iParam1;
            aActualNPT = (PVMFTimestamp*)iParam2;
            aSeekToSyncPoint = (iParam3 ? true : false);
        }

        // Constructor and parser for SetDataSourceRate
        void Construct(PVMFSessionId s, int32 cmd, int32 aRate, PVMFTimebase* aTimebase, const OsclAny* aContext)
        {
            PVMFNodeCommandBase::Construct(s, cmd, aContext);
            iParam1 = (OsclAny*)aRate;
            iParam2 = (OsclAny*)aTimebase;
            iParam3 = NULL;
            iParam4 = NULL;
            iParam5 = NULL;
        }
        void Parse(int32& aRate, PVMFTimebase*& aTimebase)
        {
            aRate = (int32)iParam1;
            aTimebase = (PVMFTimebase*)iParam2;
        }

        //need to overload the base Destroy routine to cleanup metadata key.
        void Destroy()
        {
            PVMFGenericNodeCommand<OsclMemAllocator>::Destroy();
            switch (iCmd)
            {
                case PVMF_GENERIC_NODE_GETNODEMETADATAKEYS:
                    if (iParam4)
                    {
                        //cleanup the allocated string
                        Oscl_TAlloc<OSCL_HeapString<OsclMemAllocator>, OsclMemAllocator> str;
                        str.destruct_and_dealloc(iParam4);
                    }
                    break;
                default:
                    break;
            }
            //Set the command ID as invalid after destroying it.
            iCmd = PVMF_GENERIC_NODE_COMMAND_INVALID;
        }

        //need to overlaod the base Copy routine to copy metadata key.
        void Copy(const PVMFGenericNodeCommand<OsclMemAllocator>& aCmd)
        {
            PVMFGenericNodeCommand<OsclMemAllocator>::Copy(aCmd);
            switch (aCmd.iCmd)
            {
                case PVMF_GENERIC_NODE_GETNODEMETADATAKEYS:
                    if (aCmd.iParam4)
                    {
                        //copy the allocated string
                        OSCL_HeapString<OsclMemAllocator>* aStr = (OSCL_HeapString<OsclMemAllocator>*)aCmd.iParam4;
                        Oscl_TAlloc<OSCL_HeapString<OsclMemAllocator>, OsclMemAllocator> str;
                        iParam4 = str.ALLOC_AND_CONSTRUCT(*aStr);
                    }
                    break;
                default:
                    break;
            }
        }
};

/************************************** PVMFNODEINTERFACEIMPL ***********************************/

//Command queue type
typedef PVMFNodeCommandQueue<PVMFNodeCommand, OsclMemAllocator> PVMFNodeCmdQ;

class PVMFNodeInterfaceImpl : public PVMFNodeInterface,
        public OsclActiveObject
{
    public:
        // from OsclActiveObject
        OSCL_IMPORT_REF virtual void DoCancel();

        // contstructor
        OSCL_IMPORT_REF PVMFNodeInterfaceImpl(int32 aPriority, const char name[]);
        // destructor
        OSCL_IMPORT_REF virtual ~PVMFNodeInterfaceImpl();

        // from PVMFNodeInterface
        OSCL_IMPORT_REF PVMFStatus ThreadLogon();
        OSCL_IMPORT_REF PVMFStatus ThreadLogoff();
        OSCL_IMPORT_REF virtual PVMFSessionId Connect(const PVMFNodeSessionInfo &aSession);
        OSCL_IMPORT_REF virtual PVMFStatus Disconnect(PVMFSessionId aSessionId);
        OSCL_IMPORT_REF virtual PVMFStatus GetCapability(PVMFNodeCapability& aNodeCapability);
        OSCL_IMPORT_REF virtual PVMFPortIter* GetPorts(const PVMFPortFilter* aFilter = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId QueryUUID(PVMFSessionId aSession,
                const PvmfMimeString& aMimeType,
                Oscl_Vector<PVUuid, OsclMemAllocator>& aUuids,
                bool aExactUuidsOnly = false,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId QueryInterface(PVMFSessionId aSession,
                const PVUuid& aUuid,
                PVInterface*& aInterfacePtr,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId RequestPort(PVMFSessionId aSession,
                int32 aPortTag,
                const PvmfMimeString* aPortConfig = NULL,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId ReleasePort(PVMFSessionId aSession,
                PVMFPortInterface& aPort,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId Init(PVMFSessionId aSession,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId Prepare(PVMFSessionId aSession,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId Start(PVMFSessionId aSession,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId Stop(PVMFSessionId aSession,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId Flush(PVMFSessionId aSession,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId Pause(PVMFSessionId aSession,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId Reset(PVMFSessionId aSession,
                const OsclAny* aContext = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId CancelAllCommands(PVMFSessionId aSession,
                const OsclAny* aContextData = NULL);
        OSCL_IMPORT_REF virtual PVMFCommandId CancelCommand(PVMFSessionId aSession,
                PVMFCommandId aCmdId,
                const OsclAny* aContextData = NULL);
        OSCL_IMPORT_REF virtual void HandlePortActivity(const PVMFPortActivity& aActivity);

        OSCL_IMPORT_REF PVMFCommandId QueueCommandL(PVMFNodeCommand& aCmd);

        OSCL_IMPORT_REF void SetState(TPVMFNodeInterfaceState);
        OSCL_IMPORT_REF TPVMFNodeInterfaceState GetState();
        OSCL_IMPORT_REF bool IsCommandInProgress(PVMFNodeCommand&);
        OSCL_IMPORT_REF virtual void ReportCmdCompleteEvent(PVMFSessionId s, PVMFCmdResp &resp);
        OSCL_IMPORT_REF virtual void ReportErrorEvent(PVMFEventType aEventType, OsclAny* aEventData = NULL, PVInterface*aExtMsg = NULL, int32* aEventCode = 0);
        OSCL_IMPORT_REF virtual void ReportErrorEvent(PVMFAsyncEvent&aEvent);
        OSCL_IMPORT_REF virtual void ReportInfoEvent(PVMFEventType aEventType, OsclAny* aEventData = NULL, PVInterface*aExtMsg = NULL);
        OSCL_IMPORT_REF virtual void ReportInfoEvent(PVMFAsyncEvent&aEvent);
        OSCL_IMPORT_REF virtual void Reschedule();


    protected:
        // protected routines
        OSCL_IMPORT_REF bool SendEndOfTrackCommand(PVMFPortInterface* aPort, int32 aStreamID, PVMFTimestamp aTimestamp, int32 aSeqNum, uint32 aDuration = PVMF_DEFAULT_TRACK_DURATION);
        OSCL_IMPORT_REF bool SendBeginOfMediaStreamCommand(PVMFPortInterface* aPort, int32 aStreamID, PVMFTimestamp aTimestamp);
        OSCL_IMPORT_REF void CommandComplete(PVMFNodeCommand& aCmd, PVMFStatus aStatus,
                                             PVInterface* aExtMsg = NULL, OsclAny* aEventData = NULL, PVUuid* aEventUUID = NULL, int32* aEventCode = NULL);
        // command dispatcher routiness
        OSCL_IMPORT_REF bool ProcessCommand();
        OSCL_IMPORT_REF virtual PVMFStatus HandleExtensionAPICommands() = 0;
        OSCL_IMPORT_REF virtual PVMFStatus CancelCurrentCommand() = 0;

        //command handlers
        OSCL_IMPORT_REF virtual PVMFStatus DoQueryUuid() = 0;
        OSCL_IMPORT_REF virtual PVMFStatus DoQueryInterface() = 0;
        OSCL_IMPORT_REF virtual PVMFStatus DoInit() = 0;
        OSCL_IMPORT_REF virtual PVMFStatus DoStop() = 0;
        OSCL_IMPORT_REF virtual PVMFStatus DoReset() = 0;
        OSCL_IMPORT_REF virtual PVMFStatus DoRequestPort(PVMFPortInterface*& aPort) = 0;
        OSCL_IMPORT_REF virtual PVMFStatus DoReleasePort() = 0;

        // command handlers have implementation in base node class.
        OSCL_IMPORT_REF virtual PVMFStatus DoPrepare();
        OSCL_IMPORT_REF virtual PVMFStatus DoStart();
        OSCL_IMPORT_REF virtual PVMFStatus DoPause();
        OSCL_IMPORT_REF virtual PVMFStatus DoCancelAllCommands();
        OSCL_IMPORT_REF virtual PVMFStatus DoCancelCommand();
        OSCL_IMPORT_REF virtual PVMFStatus DoFlush();

        // Method to tell if a flush operation is in progress.
        OSCL_IMPORT_REF virtual bool IsFlushPending();

    protected:
        // protected members
        // audit control block
        OsclAuditCB iAuditCB;
        // input command queue
        PVMFNodeCmdQ iInputCommands;
        // current command
        PVMFNodeCommand iCurrentCommand;
        // cancel command
        PVMFNodeCommand iCancelCommand;
        //stream id
        uint32 iStreamID;
        //node's capabilities
        PVMFNodeCapability iNodeCapability;
        // Reference counter for extension
        uint32 iExtensionRefCount;
        // node name
        OsclNameString<PVEXECNAMELEN> iNodeName;
    private:
        PVLogger* iBaseLogger;
};

#endif //PVMF_NODE_INTERFACE_IMPL_H_INCLUDED
