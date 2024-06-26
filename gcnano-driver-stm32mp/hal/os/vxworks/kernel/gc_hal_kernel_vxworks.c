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

#define _GC_OBJ_ZONE    gcvZONE_KERNEL

/******************************************************************************\
******************************* gckKERNEL API Code ******************************
\******************************************************************************/

/*******************************************************************************
**
**  gckKERNEL_QueryVideoMemory
**
**  Query the amount of video memory.
**
**  INPUT:
**
**      gckKERNEL Kernel
**          Pointer to an gckKERNEL object.
**
**  OUTPUT:
**
**      gcsHAL_INTERFACE * Interface
**          Pointer to an gcsHAL_INTERFACE structure that will be filled in with
**          the memory information.
*/
gceSTATUS
gckKERNEL_QueryVideoMemory(
    IN gckKERNEL Kernel,
    OUT gcsHAL_INTERFACE * Interface
    )
{
    gckGALDEVICE device;

    gcmkHEADER_ARG("Kernel=%p", Kernel);

    /* Verify the arguments. */
    gcmkVERIFY_OBJECT(Kernel, gcvOBJ_KERNEL);
    gcmkVERIFY_ARGUMENT(Interface != NULL);

    /* Extract the pointer to the gckGALDEVICE class. */
    device = (gckGALDEVICE) Kernel->context;

    /* Get internal memory size and physical address. */
    Interface->u.QueryVideoMemory.internalSize = device->internalSize;
    Interface->u.QueryVideoMemory.internalPhysName = device->internalPhysName;

    /* Get external memory size and physical address. */
    Interface->u.QueryVideoMemory.externalSize = device->externalSize;
    Interface->u.QueryVideoMemory.externalPhysName = device->externalPhysName;

    /* Get contiguous memory size and physical address. */
    Interface->u.QueryVideoMemory.contiguousSize = device->contiguousSize;
    Interface->u.QueryVideoMemory.contiguousPhysName = device->contiguousPhysName;

    /* Success. */
    gcmkFOOTER_NO();
    return gcvSTATUS_OK;
}

/*******************************************************************************
**
**  gckKERNEL_GetVideoMemoryPool
**
**  Get the gckVIDMEM object belonging to the specified pool.
**
**  INPUT:
**
**      gckKERNEL Kernel
**          Pointer to an gckKERNEL object.
**
**      gcePOOL Pool
**          Pool to query gckVIDMEM object for.
**
**  OUTPUT:
**
**      gckVIDMEM * VideoMemory
**          Pointer to a variable that will hold the pointer to the gckVIDMEM
**          object belonging to the requested pool.
*/
gceSTATUS
gckKERNEL_GetVideoMemoryPool(
    IN gckKERNEL Kernel,
    IN gcePOOL Pool,
    OUT gckVIDMEM * VideoMemory
    )
{
    gckGALDEVICE device;
    gckVIDMEM videoMemory;

    gcmkHEADER_ARG("Kernel=%p Pool=%d", Kernel, Pool);

    /* Verify the arguments. */
    gcmkVERIFY_OBJECT(Kernel, gcvOBJ_KERNEL);
    gcmkVERIFY_ARGUMENT(VideoMemory != NULL);

    /* Extract the pointer to the gckGALDEVICE class. */
    device = (gckGALDEVICE) Kernel->context;

    /* Dispatch on pool. */
    switch (Pool)
    {
    case gcvPOOL_LOCAL_INTERNAL:
        /* Internal memory. */
        videoMemory = device->internalVidMem;
        break;

    case gcvPOOL_LOCAL_EXTERNAL:
        /* External memory. */
        videoMemory = device->externalVidMem;
        break;

    case gcvPOOL_SYSTEM:
        /* System memory. */
        videoMemory = device->contiguousVidMem;
        break;

    default:
        /* Unknown pool. */
        videoMemory = NULL;
    }

    /* Return pointer to the gckVIDMEM object. */
    *VideoMemory = videoMemory;

    /* Return status. */
    gcmkFOOTER_ARG("*VideoMemory=%p", *VideoMemory);
    return (videoMemory == NULL) ? gcvSTATUS_OUT_OF_MEMORY : gcvSTATUS_OK;
}

/*******************************************************************************
**
**  gckKERNEL_MapMemory
**
**  Map video memory into the current process space.
**
**  INPUT:
**
**      gckKERNEL Kernel
**          Pointer to an gckKERNEL object.
**
**      gctPHYS_ADDR Physical
**          Physical address of video memory to map.
**
**      gctSIZE_T Bytes
**          Number of bytes to map.
**
**  OUTPUT:
**
**      gctPOINTER * Logical
**          Pointer to a variable that will hold the base address of the mapped
**          memory region.
*/
gceSTATUS
gckKERNEL_MapMemory(
    IN gckKERNEL Kernel,
    IN gctPHYS_ADDR Physical,
    IN gctSIZE_T Bytes,
    OUT gctPOINTER * Logical
    )
{
    gckKERNEL kernel = Kernel;
    gctPHYS_ADDR physical = gcmNAME_TO_PTR(Physical);

    return gckOS_MapMemory(Kernel->os, physical, Bytes, Logical);
}

/*******************************************************************************
**
**  gckKERNEL_UnmapMemory
**
**  Unmap video memory from the current process space.
**
**  INPUT:
**
**      gckKERNEL Kernel
**          Pointer to an gckKERNEL object.
**
**      gctPHYS_ADDR Physical
**          Physical address of video memory to map.
**
**      gctSIZE_T Bytes
**          Number of bytes to map.
**
**      gctPOINTER Logical
**          Base address of the mapped memory region.
**
**  OUTPUT:
**
**      Nothing.
*/
gceSTATUS
gckKERNEL_UnmapMemory(
    IN gckKERNEL Kernel,
    IN gctPHYS_ADDR Physical,
    IN gctSIZE_T Bytes,
    IN gctPOINTER Logical,
    IN gctUINT32 ProcessID
    )
{
    gckKERNEL kernel = Kernel;
    gctPHYS_ADDR physical = gcmNAME_TO_PTR(Physical);

    return gckOS_UnmapMemoryEx(Kernel->os, physical, Bytes, Logical, ProcessID);
}

/****************************************************************************
**
**  gckKERNEL_DestroyProcessReservedUserMap
**
**  Destroy process reserved memory
**
**  INPUT:
**
**      gctPHYS_ADDR Physical
**          Physical address of video memory to map.
**
**      gctUINT32 Pid
**          Process ID.
*/
gceSTATUS
gckKERNEL_DestroyProcessReservedUserMap(
    IN gckKERNEL Kernel,
    IN gctUINT32 Pid
    )
{
    return gcvSTATUS_OK;
}

/*******************************************************************************
**
**  gckKERNEL_MapVideoMemory
**
**  Get the logical address for a hardware specific memory address for the
**  current process.
**
**  INPUT:
**
**      gckKERNEL Kernel
**          Pointer to an gckKERNEL object.
**
**      gctBOOL InUserSpace
**          gcvTRUE to map the memory into the user space.
**
**      gcePOOL Pool
**          Specify pool type.
**
**      gctSIZE_T Offset
**          Offset to pool start.
**
**      gctSIZE_T Bytes
**          Number of bytes to map.
**
**  OUTPUT:
**
**      gctPOINTER * Logical
**          Pointer to a variable that will hold the logical address of the
**          specified memory address.
*/
gceSTATUS
gckKERNEL_MapVideoMemory(
    IN gckKERNEL Kernel,
    IN gctBOOL InUserSpace,
    IN gcePOOL Pool,
    IN gctPHYS_ADDR Physical,
    IN gctSIZE_T Offset,
    IN gctSIZE_T Bytes,
    OUT gctPOINTER * Logical
    )
{
    gckGALDEVICE device   = gcvNULL;
    gctSIZE_T bytes       = 0;
    gctPHYS_ADDR physHandle = gcvNULL;
    gceSTATUS status      = gcvSTATUS_OK;
    gctPOINTER logical    = gcvNULL;

    gcmkHEADER_ARG("Kernel=%p InUserSpace=%d Pool=%d Offset=%X Bytes=%X",
                   Kernel, InUserSpace, Pool, Offset, Bytes);

    /* Verify the arguments. */
    gcmkVERIFY_OBJECT(Kernel, gcvOBJ_KERNEL);
    gcmkVERIFY_ARGUMENT(Logical != NULL);

    /* Extract the pointer to the gckGALDEVICE class. */
    device = (gckGALDEVICE) Kernel->context;

    /* Dispatch on pool. */
    switch (Pool)
    {
    case gcvPOOL_LOCAL_INTERNAL:
        physHandle = (PVX_MDL)device->internalPhysical;
        bytes = device->internalSize;
        break;

    case gcvPOOL_LOCAL_EXTERNAL:
        physHandle = (PVX_MDL)device->externalPhysical;
        bytes = device->externalSize;
        break;

    case gcvPOOL_SYSTEM:
        /* System memory. */
        physHandle = (PVX_MDL)device->contiguousPhysical;
        bytes = device->contiguousSize;
        break;

    default:
        /* Invalid memory pool. */
        gcmkONERROR(gcvSTATUS_INVALID_ARGUMENT);
    }

    gcmkONERROR(gckOS_MapMemory(Kernel->os, physHandle, bytes, &logical));

    /* Build logical address of specified address. */
    *Logical = (gctPOINTER)((gctUINT8_PTR)logical + Offset);

OnError:
    /* Retunn the status. */
    gcmkFOOTER_ARG("*Logical=%p", gcmOPT_POINTER(Logical));
    return status;
}

/*******************************************************************************
**
**  gckKERNEL_UnmapVideoMemory
**
**  Unmap video memory for the current process.
**
**  INPUT:
**
**      gckKERNEL Kernel
**          Pointer to an gckKERNEL object.
**
**      gcePOOL Pool
**          Specify pool type.
**
**      gctUINT32 Address
**          Hardware specific memory address.
**
**      gctUINT32 Pid
**          Process ID of the current process.
**
**      gctSIZE_T Bytes
**          Number of bytes to map.
**
**  OUTPUT:
**
**      Nothing.
*/
gceSTATUS
gckKERNEL_UnmapVideoMemory(
    IN gckKERNEL Kernel,
    IN gcePOOL Pool,
    IN gctPHYS_ADDR Physical,
    IN gctPOINTER Logical,
    IN gctUINT32 Pid,
    IN gctSIZE_T Bytes
    )
{
    return gcvSTATUS_NOT_SUPPORTED;
}

/*******************************************************************************
**
**  gckKERNEL_Notify
**
**  This function iscalled by clients to notify the gckKERNRL object of an event.
**
**  INPUT:
**
**      gckKERNEL Kernel
**          Pointer to an gckKERNEL object.
**
**      gceNOTIFY Notification
**          Notification event.
**
**  OUTPUT:
**
**      Nothing.
*/
gceSTATUS
gckKERNEL_Notify(
    IN gckKERNEL Kernel,
    IN gceNOTIFY Notification
    )
{
    gceSTATUS status = gcvSTATUS_OK;

    gcmkHEADER_ARG("Kernel=%p Notification=%d", Kernel, Notification);

    /* Verify the arguments. */
    gcmkVERIFY_OBJECT(Kernel, gcvOBJ_KERNEL);

    /* Dispatch on notifcation. */
    switch (Notification)
    {
    case gcvNOTIFY_INTERRUPT:
        /* Process the interrupt. */
#if COMMAND_PROCESSOR_VERSION > 1
        status = gckINTERRUPT_Notify(Kernel->interrupt, 0);
#else
        status = gckHARDWARE_Notify(Kernel->hardware);
#endif
        break;

    default:
        break;
    }

    /* Success. */
    gcmkFOOTER();
    return status;
}

#if gcdENABLE_VIDEO_MEMORY_MIRROR
gceSTATUS
gckKERNEL_SyncVideoMemoryMirror(
    IN gckKERNEL Kernel,
    IN gckVIDMEM_NODE Node,
    IN gctSIZE_T Offset,
    IN gctSIZE_T Bytes,
    IN gctUINT32 Reason
    )
{
    return gcvSTATUS_NOT_SUPPORTED;
}
#endif
