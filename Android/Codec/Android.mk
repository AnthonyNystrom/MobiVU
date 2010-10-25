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
# ENCODER H263 MODULES

include $(CLEAR_VARS)
LOCAL_MODULE    := coder
LOCAL_SRC_FILES := coder.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := countbit
LOCAL_SRC_FILES := countbit.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := dct
LOCAL_SRC_FILES := dct.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := global
LOCAL_SRC_FILES := global.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := huffman
LOCAL_SRC_FILES := huffman.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libr263
LOCAL_SRC_FILES := libr263.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := mot_est
LOCAL_SRC_FILES := mot_est.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := pred
LOCAL_SRC_FILES := pred.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := quant
LOCAL_SRC_FILES := quant.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := rlib
LOCAL_SRC_FILES := rlib.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := stream
LOCAL_SRC_FILES := stream.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := call_encoder
LOCAL_SRC_FILES := call_encoder.cpp
include $(BUILD_STATIC_LIBRARY)

#***********************************************************************
# DECODER H263 MODULES

include $(CLEAR_VARS)
LOCAL_MODULE    := convert
LOCAL_SRC_FILES := convert.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := DGlobal
LOCAL_SRC_FILES := DGlobal.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := GetBits
LOCAL_SRC_FILES := GetBits.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := GetBlk
LOCAL_SRC_FILES := GetBlk.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := GetHdr
LOCAL_SRC_FILES := GetHdr.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := GetPic
LOCAL_SRC_FILES := GetPic.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := GetVlc
LOCAL_SRC_FILES := GetVlc.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Idct
LOCAL_SRC_FILES := Idct.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Idctref
LOCAL_SRC_FILES := Idctref.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Indices
LOCAL_SRC_FILES := Indices.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Recon
LOCAL_SRC_FILES := Recon.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Sac
LOCAL_SRC_FILES := Sac.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Sactbls
LOCAL_SRC_FILES := Sactbls.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Tmndec
LOCAL_SRC_FILES := Tmndec.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := YuvToRgb
LOCAL_SRC_FILES := YuvToRgb.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := mylog
LOCAL_SRC_FILES := mylog.cpp
include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE    := call_decoder
LOCAL_SRC_FILES := call_decoder.cpp
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := call_mylog
LOCAL_SRC_FILES := call_mylog.cpp
include $(BUILD_STATIC_LIBRARY)

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
LOCAL_SRC_FILES := encdec.cpp
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_STATIC_LIBRARIES := call_mylog call_encoder call_decoder  libr263 huffman global coder countbit dct mot_est pred quant rlib stream Tmndec GetPic convert DGlobal GetBits GetBlk GetHdr  GetVlc Idct Idctref Indices Recon Sac Sactbls YuvToRgb mylog interf_dec sp_dec
include $(BUILD_SHARED_LIBRARY)







