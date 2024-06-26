/****************************************************************************
*
*    Copyright (c) 2005 - 2023 by Vivante Corp.  All rights reserved.
*
*    The material in this file is confidential and contains trade secrets
*    of Vivante Corporation. This is proprietary information owned by
*    Vivante Corporation. No part of this work may be disclosed,
*    reproduced, copied, transmitted, or used in any way for any purpose,
*    without the express written permission of Vivante Corporation.
*
*****************************************************************************/


#include "gc_hal_kernel_vxworks.h"

gctINT
gckMATH_ModuloInt(
    IN gctINT X,
    IN gctINT Y
    )
{
    if(Y ==0) {return 0;}
    else {return X % Y;}
}
