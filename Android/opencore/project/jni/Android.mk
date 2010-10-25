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
							libpv_amr_nb_common_lib

include $(BUILD_SHARED_LIBRARY)
