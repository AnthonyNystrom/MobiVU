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
#ifndef PVMF_CPMPLUGIN_LICENSE_INTERFACE_TYPES_H_INCLUDED
#define PVMF_CPMPLUGIN_LICENSE_INTERFACE_TYPES_H_INCLUDED

#ifndef OSCL_BASE_H_INCLUDED
#include "oscl_base.h"
#endif
#ifndef OSCL_TYPES_H_INCLUDED
#include "oscl_types.h"
#endif
#ifndef OSCL_VECTOR_H_INCLUDED
#include "oscl_vector.h"
#endif
#ifndef OSCL_STRING_H_INCLUDED
#include "oscl_string.h"
#endif
#ifndef PV_INTERFACE_H_INCLUDED
#include "pv_interface.h"
#endif
#include "pvmf_return_codes.h"
#include "pvmf_event_handling.h"
#include "pvmf_cpmplugin_domain_interface_types.h"

//A class to hold detailed license status information
class PVMFCPMLicenseStatus
{
    public:
        PVMFCPMLicenseStatus():
                iNumLicenseChallengesSent(0)
                , iNumLicenseResponsesReceived(0)
                , iLastLicenseResponseResult(0)
                , iNumLicenseAckChallengesSent(0)
                , iNumLicenseAckResponsesReceived(0)
                , iLastLicenseAckResponseResult(0)
                , iNumJoinChallengesSent(0)
                , iNumJoinResponsesReceived(0)
                , iLastJoinResponseResult(0)
                , iNumLeaveChallengesSent(0)
                , iNumLeaveResponsesReceived(0)
                , iLastLeaveResponseResult(0)
                , iAccountIdReceived(false)
                , iAccountId(EMPTY_UUID)
                , iServiceIdReceived(false)
                , iServiceId(EMPTY_UUID)
                , iLastErrorResult(0)
        {}

        // The URL to which the license request was last sent.
        OSCL_HeapString<OsclMemAllocator> iLastLicenseURL;
        // Number of license challenges sent.
        uint32 iNumLicenseChallengesSent;
        // Number of license challenge responses received.
        uint32 iNumLicenseResponsesReceived;
        // The result of the last license response received.
        uint32 iLastLicenseResponseResult;

        // Number of license ack challenges sent.
        uint32 iNumLicenseAckChallengesSent;
        // Number of license ack challenge response received.
        uint32 iNumLicenseAckResponsesReceived;
        // The result of the last license ack response received.
        uint32 iLastLicenseAckResponseResult;


        // The URL to which the JoinDomain request was last sent.
        OSCL_HeapString<OsclMemAllocator> iLastJoinURL;
        // Number of JoinDomain challenges sent.
        uint32 iNumJoinChallengesSent;
        // Number of JoinDomain responses received.
        uint32 iNumJoinResponsesReceived;
        // The result of the last JoinDomain response received.
        uint32 iLastJoinResponseResult;

        // The URL to which the LeaveDomain request was last sent.
        OSCL_HeapString<OsclMemAllocator> iLastLeaveURL;
        // Number of LeaveDomain challenges sent.
        uint32 iNumLeaveChallengesSent;
        // Number of LeaveDomain responses received.
        uint32 iNumLeaveResponsesReceived;
        // The result of the last LeaveDomain response received.
        uint32 iLastLeaveResponseResult;

        // The URL to which the registration call should be made.
        OSCL_HeapString<OsclMemAllocator> iRedirectURL;

        bool iAccountIdReceived;
        // The account Id required for registration.
        PVUuid iAccountId;

        bool iServiceIdReceived;
        // The service Id required for registration.
        PVUuid iServiceId;

        // Custom data sent by the server
        OSCL_HeapString<OsclMemAllocator> iCustomData;

        uint32 iLastErrorResult;

        void Clear()
        {
            iLastLicenseURL = "";
            iNumLicenseChallengesSent = iNumLicenseResponsesReceived = iLastLicenseResponseResult = 0;
            iNumLicenseAckChallengesSent = iNumLicenseAckResponsesReceived = iLastLicenseAckResponseResult = 0;

            iLastJoinURL = "";
            iNumJoinChallengesSent = iNumJoinResponsesReceived = iLastJoinResponseResult = 0;

            iLastLeaveURL = "";
            iNumLeaveChallengesSent = iNumLeaveResponsesReceived = iLastLeaveResponseResult = 0;

            iRedirectURL = "";

            iAccountIdReceived = false;
            iAccountId = EMPTY_UUID;

            iServiceIdReceived = false;
            iServiceId = EMPTY_UUID;

            iCustomData = "";

            iLastErrorResult = 0;
        }
        void Set(const PVMFCPMLicenseStatus& aStatus)
        {
            iLastLicenseURL = aStatus.iLastLicenseURL;
            iNumLicenseChallengesSent = aStatus.iNumLicenseChallengesSent;
            iNumLicenseResponsesReceived = aStatus.iNumLicenseResponsesReceived;
            iLastLicenseResponseResult = aStatus.iLastLicenseResponseResult;
            iNumLicenseAckChallengesSent = aStatus.iNumLicenseAckChallengesSent;
            iNumLicenseAckResponsesReceived = aStatus.iNumLicenseAckResponsesReceived;
            iLastLicenseAckResponseResult = aStatus.iLastLicenseAckResponseResult;
            iLastJoinURL = aStatus.iLastJoinURL;
            iNumJoinChallengesSent = aStatus.iNumJoinChallengesSent;
            iNumJoinResponsesReceived = aStatus.iNumJoinResponsesReceived;
            iLastJoinResponseResult = aStatus.iLastJoinResponseResult;
            iLastLeaveURL = aStatus.iLastLeaveURL;
            iNumLeaveChallengesSent = aStatus.iNumLeaveChallengesSent;
            iNumLeaveResponsesReceived = aStatus.iNumLeaveResponsesReceived;
            iLastLeaveResponseResult = aStatus.iLastLeaveResponseResult;
            iRedirectURL = aStatus.iRedirectURL;
            iAccountIdReceived = aStatus.iAccountIdReceived;
            iAccountId = aStatus.iAccountId;
            iServiceIdReceived = aStatus.iServiceIdReceived;
            iServiceId = aStatus.iServiceId;
            iCustomData = aStatus.iCustomData;
            iLastErrorResult = aStatus.iLastErrorResult;
        }
};

#endif //PVMF_CPMPLUGIN_LICENSE_INTERFACE_TYPES_H_INCLUDED

