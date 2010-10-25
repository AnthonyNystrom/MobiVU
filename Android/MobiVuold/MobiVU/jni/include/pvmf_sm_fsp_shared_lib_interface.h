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
#ifndef PVMF_SM_FSP_SHARED_LIB_INTERFACE_H_INCLUDED
#define PVMF_SM_FSP_SHARED_LIB_INTERFACE_H_INCLUDED

#ifndef OSCL_BASE_H_INCLUDED
#include "oscl_base.h"
#endif
#ifndef OSCL_UUID_H_INCLUDED
#include "oscl_uuid.h"
#endif

/**
    This is the interface that will be implemented by the shared library that
    implements the feature specific plugin.
    FSP lib is expected to implement the interface that implements
    SMNodeFSPSharedLibraryInterface and OsclSharedLibraryInterface interfaces.
    User of the FSP lib can then query for SMNodeFSPSharedLibraryInterface by
    calling OsclSharedLibraryInterface::SharedLibraryLookup for
    SMNODE_FSP_SHARED_LIBRARY_INTERFACE uuid.
    Thereafter, SMNodeFSPSharedLibraryInterface::QueryLibInterface can be used
    to retrieve the interfaces for creation(SMNODE_CREATE_FSP_LIB_INTERFACE)
    and destruction of the FSP(SMNODE_RELEASE_FSP_LIB_INTERFACE)
*/

#define SMNODE_FSP_SHARED_LIBRARY_INTERFACE OsclUuid(0xc79672d0,0x79b9,0x11de,0x8a,0x39,0x08,0x00,0x20,0x0c,0x9a,0x66)
#define SMNODE_CREATE_FSP_LIB_INTERFACE     OsclUuid(0xc79672d1,0x79b9,0x11de,0x8a,0x39,0x08,0x00,0x20,0x0c,0x9a,0x66)
#define SMNODE_RELEASE_FSP_LIB_INTERFACE    OsclUuid(0xc79672d2,0x79b9,0x11de,0x8a,0x39,0x08,0x00,0x20,0x0c,0x9a,0x66)

class SMNodeFSPSharedLibraryInterface
{
    public:
        /**
         * Query for the instance of a particular interface based on the interface UUID
         *
         * @param aInterfaceId - give interface UUID.
         **/
        virtual OsclAny* QueryLibInterface(const OsclUuid& aInterfaceId) = 0;
};

#endif // PVMF_SM_FSP_SHARED_LIB_INTERFACE_H_INCLUDED
