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

#ifndef COLORCONV_CONFIG_H_INCLUDED
#define COLORCONV_CONFIG_H_INCLUDED

/** For scaling support, define CCSCALING to 1, else set it to 0 */
#define CCSCALING   1

/** For rotation support, define CCROTATE to 1, else set it to 0  */
#define CCROTATE    1

/** To specify RGB format. define RGB_FORMAT to 1. For, BGR format, set it to 0 */
#define RGB_FORMAT  0

/* If the target supports preload (PLD), define SUPPORT_ARM_PLD, else don't define it */
//#define SUPPORT_ARM_PLD

/********************************************************************************************
 For YUV422 to YUV420 conversion, the Input YUV422 data can be in three forms:-
    UY0VY1 :
        Cb1 Y1 Cr1 Y2 Cb2 Y3 Cr2 Y4 .....

    Y1VY0U :
        Y2 Cr1 Y1 Cb1 Y4 Cr2 Y3 Cb2 ....

    Y0VY1U :
        Y1 Cr1 Y2 Cb1 Y3 Cr2 Y4 Cb2 ....

*********************************************************************************************/


#endif // COLORCONV_CONFIG_H_INCLUDED
