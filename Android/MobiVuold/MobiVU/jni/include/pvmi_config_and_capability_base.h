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
#ifndef PVMI_CONFIG_AND_CAPABILITY_UTILS_BASE_H_INCLUDED
#define PVMI_CONFIG_AND_CAPABILITY_UTILS_BASE_H_INCLUDED

#ifndef PVMI_CONFIG_AND_CAPABILITY_H_INCLUDED
#include "pvmi_config_and_capability.h"
#endif


//******************************************************************************
// PvmiCapabilityAndConfigBase                                           CLASS
//******************************************************************************
//
class PvmiCapabilityAndConfigBase : public PvmiCapabilityAndConfig
{

    public:
//==============================================================================
// PUBLIC METHODS
//==============================================================================

        //==========================================================================
        // Constructor                                          PUBLIC CONSTRUCTOR
        //==========================================================================
        //
        PvmiCapabilityAndConfigBase() {}

        //==========================================================================
        // Destructor                                            PUBLIC DESTRUCTOR
        //==========================================================================
        //
        virtual ~PvmiCapabilityAndConfigBase() {}

        //==========================================================================
        // createContext                                                    PUBLIC
        //==========================================================================
        //
        void createContext
        (
            PvmiMIOSession aSession,
            PvmiCapabilityContext& aContext
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aContext);
            OSCL_LEAVE(PVMFErrNotSupported);
        }

        //==========================================================================
        // DeleteContext                                                    PUBLIC
        //==========================================================================
        //
        void DeleteContext
        (
            PvmiMIOSession aSession,
            PvmiCapabilityContext& aContext
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aContext);
            OSCL_LEAVE(PVMFErrNotSupported);
        }

        //==========================================================================
        // getCapabilityMetric                                              PUBLIC
        //==========================================================================
        //
        uint32 getCapabilityMetric
        (
            PvmiMIOSession aSession
        )
        {
            OSCL_UNUSED_ARG(aSession);
            return 0;
        }

        //==========================================================================
        // getParametersSync                                                PUBLIC
        //==========================================================================
        //
        PVMFStatus getParametersSync
        (
            PvmiMIOSession aSession,
            PvmiKeyType aIdentifier,
            PvmiKvp*& aParameters,
            int& aNumParamElements,
            PvmiCapabilityContext aContext
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aIdentifier);
            OSCL_UNUSED_ARG(aParameters);
            OSCL_UNUSED_ARG(aNumParamElements);
            OSCL_UNUSED_ARG(aContext);
            return PVMFErrNotSupported;
        }

        //==========================================================================
        // releaseParameters                                                PUBLIC
        //==========================================================================
        //
        PVMFStatus releaseParameters
        (
            PvmiMIOSession aSession,
            PvmiKvp* aParameters,
            int num_elements
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aParameters);
            OSCL_UNUSED_ARG(num_elements);
            return PVMFErrNotSupported;
        }

        //==========================================================================
        // setContextParameters                                             PUBLIC
        //==========================================================================
        //
        void setContextParameters
        (
            PvmiMIOSession aSession,
            PvmiCapabilityContext& aContext,
            PvmiKvp* aParameters,
            int num_parameter_elements
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aContext);
            OSCL_UNUSED_ARG(aParameters);
            OSCL_UNUSED_ARG(num_parameter_elements);
            OSCL_LEAVE(PVMFErrNotSupported);
        }

        //==========================================================================
        // setObserver                                                      PUBLIC
        //==========================================================================
        //
        void setObserver
        (
            PvmiConfigAndCapabilityCmdObserver* aObserver
        )
        {
            OSCL_UNUSED_ARG(aObserver);
        }

        //==========================================================================
        // setParametersAsync                                               PUBLIC
        //==========================================================================
        //
        PVMFCommandId setParametersAsync
        (
            PvmiMIOSession aSession,
            PvmiKvp* aParameters,
            int num_elements,
            PvmiKvp*& aRet_kvp,
            OsclAny* context
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aParameters);
            OSCL_UNUSED_ARG(num_elements);
            OSCL_UNUSED_ARG(aRet_kvp);
            OSCL_UNUSED_ARG(context);
            OSCL_LEAVE(OsclErrNotSupported);
            return 0;                        // to satisfy compiler, need to return
        }

        //==========================================================================
        // setParametersSync                                                PUBLIC
        //==========================================================================
        //
        void setParametersSync
        (
            PvmiMIOSession aSession,
            PvmiKvp* aParameters,
            int aNumElements,
            PvmiKvp* &aRetKVP
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aNumElements);
            aRetKVP = aParameters;// set output arg to input to indicate nothing set
        }

        //==========================================================================
        // verifyParametersSync                                             PUBLIC
        //==========================================================================
        //
        PVMFStatus verifyParametersSync
        (
            PvmiMIOSession aSession,
            PvmiKvp* aParameters,
            int num_elements
        )
        {
            OSCL_UNUSED_ARG(aSession);
            OSCL_UNUSED_ARG(aParameters);
            OSCL_UNUSED_ARG(num_elements);
            return PVMFErrNotSupported;
        }

};

#endif // PVMI_CONFIG_AND_CAPABILITY_UTILS_BASE_H_INCLUDED

// END FILE

