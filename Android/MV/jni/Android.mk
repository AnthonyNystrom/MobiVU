# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := h263
LOCAL_SRC_FILES := h263/libr263.cpp \
 h263/huffman.cpp \
 h263/global.cpp \
 h263/coder.cpp \
 h263/countbit.cpp \
 h263/dct.cpp \
 h263/mot_est.cpp \
 h263/pred.cpp \
 h263/quant.cpp \
 h263/rlib.cpp \
 h263/stream.cpp \
 h263/Tmndec.cpp \
 h263/GetPic.cpp \
 h263/convert.cpp \
 h263/DGlobal.cpp \
 h263/GetBits.cpp \
 h263/GetBlk.cpp \
 h263/GetHdr.cpp \
 h263/GetVlc.cpp \
 h263/Idct.cpp \
 h263/Idctref.cpp \
 h263/Indices.cpp \
 h263/Recon.cpp \
 h263/Sac.cpp \
 h263/Sactbls.cpp \
 h263/YuvToRgb.cpp \
 h263/mylog.cpp 
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := opencore
LOCAL_SRC_FILES := opencore.cpp \
	encodervideo.cpp \
	decodervideo.cpp \
	encoderaudio.cpp \
	decoderaudio.cpp 	
	
	
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_C_INCLUDES := include
LOCAL_STATIC_LIBRARIES = 	libpvmp4decoder \
							libpvm4vencoder \
							libosclproc \
							libcolorconvert \
							libosclerror \
							libosclbase \
							libosclutil \
							libosclmemory \
							libpvencoder_gsmamr \
							libpvdecoder_gsmamr \
							libpv_amr_nb_common_lib \
							h263
							

include $(BUILD_SHARED_LIBRARY)
