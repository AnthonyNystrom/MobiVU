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

#***********************************************************************

#***********************************************************************
# DECODER AMR MODULES

include $(CLEAR_VARS)
LOCAL_MODULE    := sp_dec
LOCAL_SRC_FILES := sp_dec.c
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := interf_dec
LOCAL_SRC_FILES := interf_dec.c
include $(BUILD_STATIC_LIBRARY)

#***********************************************************************
# SHARED LIB

include $(CLEAR_VARS)
LOCAL_MODULE    := encdec
LOCAL_SRC_FILES := encdec.cpp \
call_mylog.cpp \
call_encoder.cpp \
 call_decoder.cpp \
 libr263.cpp \
 huffman.cpp \
 global.cpp \
 coder.cpp \
 countbit.cpp \
 dct.cpp \
 mot_est.cpp \
 pred.cpp \
 quant.cpp \
 rlib.cpp \
 stream.cpp \
 Tmndec.cpp \
 GetPic.cpp \
 convert.cpp \
 DGlobal.cpp \
 GetBits.cpp \
 GetBlk.cpp \
 GetHdr.cpp \
 GetVlc.cpp \
 Idct.cpp \
 Idctref.cpp \
 Indices.cpp \
 Recon.cpp \
 Sac.cpp \
 Sactbls.cpp \
 YuvToRgb.cpp \
 mylog.cpp 

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
include $(BUILD_SHARED_LIBRARY)







