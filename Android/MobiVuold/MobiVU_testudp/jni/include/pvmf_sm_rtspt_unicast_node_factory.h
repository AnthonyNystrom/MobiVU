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
#ifndef PVMF_SM_RTSPT_UNICAST_NODE_FACTORY_H_INCLUDED
#define PVMF_SM_RTSPT_UNICAST_NODE_FACTORY_H_INCLUDED

#ifndef PVMF_EVENT_HANDLING_H_INCLUDED
#include "pvmf_event_handling.h"
#endif

/* Forward declaration */
class PVMFSMFSPBaseNode;

#define KPVMFSMRTSPTUnicastNodeUuid PVUuid(0x2ecbcc18,0x0e70,0x40de,0xba,0xde,0x76,0x6a,0xbd,0x23,0xe5,0x10)

/**
 * PVMFSMRTSPTUnicastNodeFactory Class
 *
 * PVMFSMRTSPTUnicastNodeFactory class is a singleton class
 * which instantiates and provides access to rtspt unicast plugin.
 * It returns a PVMFSMFSPBaseNode reference, the interface class of the
 * plugins.
 *
 * The client is expected to contain and maintain a pointer to the instance
 * created while the node is active.
 */

class PVMFSMRTSPTUnicastNodeFactory
{
    public:
        /**
         * Creates an instance of a CreateSMRTSPTUnicastNode.
         * If the creation fails, this function will leave.
         *
         * @param priority for the underlying node active object
         *
         * @returns A pointer to an instance of CreateSMRTSPTUnicastNode
         * or leaves if instantiation fails
         **/
        OSCL_IMPORT_REF static PVMFSMFSPBaseNode* CreateSMRTSPTUnicastNode(int32 aPriority);

        /**
         * Deletes an instance of PVMFSMRTSPUnicastNode
         * and reclaims all allocated resources.  An instance can be deleted
         * only in the created state. An attempt to delete in any other state
         * will result returning false
         *
         * @param aNode The PVMFSMFSPBaseNode instance to be deleted
         * @returns bool
         *  true: node was in created state
         *  false: either node was not in created state or deletion failed
         **/
        OSCL_IMPORT_REF static bool DeleteSMRTSPTUnicastNode(PVMFSMFSPBaseNode* aNode);
};

#endif


