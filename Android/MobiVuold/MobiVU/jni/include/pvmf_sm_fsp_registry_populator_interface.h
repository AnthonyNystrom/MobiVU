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
#ifndef PVMF_SM_FSP_REGISTRY_POPULATOR_INTERFACE_H_INCLUDED
#define PVMF_SM_FSP_REGISTRY_POPULATOR_INTERFACE_H_INCLUDED

#ifndef OSCL_UUID_H_INCLUDED
#include "oscl_uuid.h"
#endif


/*
** NodeRegistryPopulatorInterface is an abstract interface that is used to register and
** unregister feature specfic plugins to the SM node. A registry uses this interface to allow registry populators
** to add and remove objects in the registry.
*/
#define PVMF_SM_FSP_REGISTRY_POPULATOR_INTERFACE OsclUuid(0x47652190,0x4874,0x4057,0x94,0x68,0x3f,0x08,0x29,0xef,0x06,0xb1)

class PVMFFSPRegistryInterface;

class PVMFSMFSPRegistryPopulatorInterface
{
    public:
        /**
        ** The PVMFFSPRegistryInterface that wants to be populated by the
        ** PVMFSMFSPRegistryPopulatorInterface of some dll has to register
        ** itself with it.
        */
        virtual void Register(PVMFFSPRegistryInterface* aRegistry) = 0;

        /**
        ** The PVMFFSPRegistryInterface that wants to be depopulated by the
        ** PVMFSMFSPRegistryPopulatorInterface of some dll has to unregister
        ** itself with it.
        */
        virtual void Unregister(PVMFFSPRegistryInterface* aRegistry) = 0;
};

#endif // PVMF_SM_FSP_REGISTRY_POPULATOR_INTERFACE_H_INCLUDED


