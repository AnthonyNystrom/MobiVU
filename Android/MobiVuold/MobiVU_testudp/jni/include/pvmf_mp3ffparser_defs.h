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

#ifndef PVMF_MP3FFPARSER_DEFS_H_INCLUDED
#define PVMF_MP3FFPARSER_DEFS_H_INCLUDED


#ifndef OSCL_BASE_H_INCLUDED
#include "oscl_base.h"
#endif

// Playback clock timescale
#define COMMON_PLAYBACK_CLOCK_TIMESCALE 1000

// Constants used for memory pools
#define PVMP3FF_MEDIADATA_CHUNKS_IN_POOL    8
#define PVMP3FF_MEDIADATA_CHUNKSIZE         256
// the maximum frame size depends on K * bitrate/sampling_rate
#define PVMP3FF_DEFAULT_MAX_FRAMESIZE       4096
#define PVMF3FF_DEFAULT_NUM_OF_FRAMES       5
#define PVMF3FF_DURATION_SCAN_AO_DELAY      1000

#if PV_HAS_SHOUTCAST_SUPPORT_ENABLED
#define PVMF_MP3FFPARSER_NODE_METADATA_RESERVE 2
#define SHOUTCAST_MEDIA_DATA_LENGTH_STRING "x-pvmf/net/shoutcast-media-data-length"
#define SHOUTCAST_CLIP_BITRATE_STRING "x-pvmf/net/shoutcast-clip-bitrate"
#define SHOUTCAST_IS_SHOUTCAST_SESSION_STRING "x-pvmf/net/is-shoutcast-session"
#endif

/**
 *  Port tags, enumerated list of port tags supported by the node,
 *  for the port requests.
 **/
typedef enum
{
    PVMF_MP3FFPARSER_NODE_PORT_TYPE_SOURCE

} PVMFMP3FFParserNodePortType;

// Capability mime strings
#define PVMF_MP3FFPARSER_OUTPORT_FORMATS "x-pvmf/port/formattype"
#define PVMF_MP3FFPARSER_OUTPORT_FORMATS_VALTYPE "x-pvmf/port/formattype;valtype=int32"

static const char PVMF_MP3_PARSER_NODE_ALL_METADATA_KEY[] = "all";

#define PVMF_MP3_PARSER_NODE_MAX_CPM_METADATA_KEYS 256

#endif // end PVMF_MP3FFPARSER_DEFS_H_INCLUDED

