/*
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *     Lina Sun<lina.sun@intel.com>
 *     Wei Lin<wei.w.lin@intel.com>
 *     Yuting Yang<yuting.yang@intel.com>
 */

#include "os_interface.h"
#include "hw_interface.h"
#include "hw_interface_g75.h"
#include "hw_interface_g8.h"
#include "hw_interface_g9.h"
#include <math.h>

#define TABLE_PHASE_PREC            5
#define TABLE_PHASE_COUNT           (1 << TABLE_PHASE_PREC)
#define SCALER_UV_WIN_SIZE          4
#define TBL_COEF_PREC               6

#define GENHW_MEDIA_THREADS_MAX_G9_GT1  126
#define GENHW_MEDIA_THREADS_MAX_G9_GT2  161
#define GENHW_MEDIA_THREADS_MAX_G9_GT3  329
#define GENHW_MEDIA_THREADS_MAX_G9_GT4  497
#define GENHW_MEDIA_THREADS_MAX_G9_GTC  72
#define GENHW_MEDIA_THREADS_MAX_G9_GTA  108
#define GENHW_MEDIA_THREADS_MAX_G9_GTX  144

CONST GENHW_GSH_SETTINGS g_GSH_Settings_g9 = {
	GENHW_SYNC_SIZE_G75,
	GENHW_MEDIA_STATES_G75,
	GENHW_MEDIA_IDS_G75,
	GENHW_CURBE_SIZE_G8,
	GENHW_KERNEL_COUNT_G75,
	GENHW_KERNEL_HEAP_G75,
	GENHW_KERNEL_BLOCK_SIZE_G75,
	0
};

CONST GENHW_HW_CAPS g_IntelGen_HwCaps_g9_gt1 = {
	GENHW_SSH_SURFACES_PER_BT_MAX - 1,
	GENHW_MEDIA_THREADS_MAX_G9_GT1,
	512,
	GENHW_URB_SIZE_MAX_G75,
	GENHW_URB_ENTRIES_MAX_G75_GT1,
	GENHW_URB_ENTRY_SIZE_MAX_G75,
	GENHW_CURBE_SIZE_MAX_G75,
	GENHW_INTERFACE_DESCRIPTOR_ENTRIES_MAX_G75
};

CONST GENHW_HW_CAPS g_IntelGen_HwCaps_g9_gt2 = {
	GENHW_SSH_SURFACES_PER_BT_MAX - 1,
	GENHW_MEDIA_THREADS_MAX_G9_GT2,
	512,
	GENHW_URB_SIZE_MAX_G75,
	GENHW_URB_ENTRIES_MAX_G75_GT2,
	GENHW_URB_ENTRY_SIZE_MAX_G75,
	GENHW_CURBE_SIZE_MAX_G75,
	GENHW_INTERFACE_DESCRIPTOR_ENTRIES_MAX_G75
};

CONST GENHW_HW_CAPS g_IntelGen_HwCaps_g9_gt3 = {
	GENHW_SSH_SURFACES_PER_BT_MAX - 1,
	GENHW_MEDIA_THREADS_MAX_G9_GT3,
	512,
	GENHW_URB_SIZE_MAX_G75,
	GENHW_URB_ENTRIES_MAX_G75_GT3,
	GENHW_URB_ENTRY_SIZE_MAX_G75,
	GENHW_CURBE_SIZE_MAX_G75,
	GENHW_INTERFACE_DESCRIPTOR_ENTRIES_MAX_G75
};

CONST GENHW_HW_CAPS g_IntelGen_HwCaps_g9_gt4 = {
	GENHW_SSH_SURFACES_PER_BT_MAX - 1,
	GENHW_MEDIA_THREADS_MAX_G9_GT4,
	512,
	GENHW_URB_SIZE_MAX_G75,
	GENHW_URB_ENTRIES_MAX_G75_GT3,
	GENHW_URB_ENTRY_SIZE_MAX_G75,
	GENHW_CURBE_SIZE_MAX_G75,
	GENHW_INTERFACE_DESCRIPTOR_ENTRIES_MAX_G75
};

CONST GENHW_HW_CAPS g_IntelGen_HwCaps_g9_gtc = {
	GENHW_SSH_SURFACES_PER_BT_MAX - 1,
	GENHW_MEDIA_THREADS_MAX_G9_GTC,
	512,
	GENHW_URB_SIZE_MAX_G75,
	GENHW_URB_ENTRIES_MAX_G75_GT3,
	GENHW_URB_ENTRY_SIZE_MAX_G75,
	GENHW_CURBE_SIZE_MAX_G75,
	GENHW_INTERFACE_DESCRIPTOR_ENTRIES_MAX_G75
};

CONST GENHW_HW_CAPS g_IntelGen_HwCaps_g9_gta = {
	GENHW_SSH_SURFACES_PER_BT_MAX - 1,
	GENHW_MEDIA_THREADS_MAX_G9_GTA,
	512,
	GENHW_URB_SIZE_MAX_G75,
	GENHW_URB_ENTRIES_MAX_G75_GT3,
	GENHW_URB_ENTRY_SIZE_MAX_G75,
	GENHW_CURBE_SIZE_MAX_G75,
	GENHW_INTERFACE_DESCRIPTOR_ENTRIES_MAX_G75
};

CONST GENHW_HW_CAPS g_IntelGen_HwCaps_g9_gtx = {
	GENHW_SSH_SURFACES_PER_BT_MAX - 1,
	GENHW_MEDIA_THREADS_MAX_G9_GTX,
	512,
	GENHW_URB_SIZE_MAX_G75,
	GENHW_URB_ENTRIES_MAX_G75_GT3,
	GENHW_URB_ENTRY_SIZE_MAX_G75,
	GENHW_CURBE_SIZE_MAX_G75,
	GENHW_INTERFACE_DESCRIPTOR_ENTRIES_MAX_G75
};

UINT IntelGen_HwGetMediaWalkerBlockSize_g9(PGENHW_HW_INTERFACE pHwInterface)
{
	return 16;
};

GENOS_STATUS IntelGen_HwSetupBufferSurfaceState_g9(PGENHW_HW_INTERFACE
						   pHwInterface,
						   PGENHW_SURFACE pSurface,
						   PGENHW_SURFACE_STATE_PARAMS
						   pParams,
						   PGENHW_SURFACE_STATE_ENTRY *
						   ppSurfaceEntry)
{
	GENOS_STATUS eStatus;
	PGENHW_SSH pSSH;
	PSURFACE_STATE_G9 pState_g9;
	PGENHW_SURFACE_STATE_ENTRY pSurfaceEntry;
	BYTE iWidth;
	USHORT iHeight;
	BYTE iDepth;
	DWORD bufferSize;

	GENHW_HW_ASSERT(pHwInterface);
	GENHW_HW_ASSERT(pSurface);
	GENHW_HW_ASSERT(ppSurfaceEntry);
	GENHW_HW_ASSERT(pSurface->dwWidth > 0);

	eStatus = GENOS_STATUS_SUCCESS;
	pSSH = pHwInterface->pSurfaceStateHeap;

	pParams->Type = GENHW_SURFACE_TYPE_G9;

	GENHW_HW_CHK_STATUS(pHwInterface->pfnAssignSurfaceState(pHwInterface,
								GENHW_SURFACE_TYPE_G9,
								ppSurfaceEntry));

	GENHW_HW_ASSERT(*ppSurfaceEntry);
	pSurfaceEntry = *ppSurfaceEntry;

	switch (pSurface->Format) {
	case Format_Buffer:
		{
			pSurfaceEntry->dwFormat =
			    GFX3DSTATE_SURFACEFORMAT_L8_UNORM;
			break;
		}
	case Format_RAW:
		{
			pSurfaceEntry->dwFormat = GFX3DSTATE_SURFACEFORMAT_RAW;
			break;
		}
	default:
		{
			GENHW_HW_ASSERTMESSAGE
			    ("Invalid Buffer Surface Format.");
			break;
		}
	}

	pSurfaceEntry->dwSurfStateOffset =
	    IntelGen_HwGetCurSurfaceStateBase(pSSH) +
	    (pSurfaceEntry->iSurfStateID * sizeof(SURFACE_STATE_G9));

	bufferSize = pSurface->dwWidth - 1;

	iWidth = (BYTE) (bufferSize & GFX_MASK(0, 6));
	iHeight = (USHORT) ((bufferSize & GFX_MASK(7, 20)) >> 7);
	iDepth = (BYTE) ((bufferSize & GFX_MASK(21, 29)) >> 21);

	pState_g9 =
	    &pSurfaceEntry->pSurfaceState->
	    PacketSurfaceState_g9.cmdSurfaceState_g9;

	*pState_g9 = *(pHwInterface->pHwCommands->pSurfaceState_g9);

	pState_g9->DW0.SurfaceFormat = pSurfaceEntry->dwFormat;
	pState_g9->DW0.TileMode = 0;
	pState_g9->DW0.SurfaceType = GFX3DSTATE_SURFACETYPE_BUFFER;
	pState_g9->DW1.SurfaceMemObjCtrlState =
	    pHwInterface->pfnGetSurfaceMemoryObjectControl(pHwInterface,
							   pParams);
	pState_g9->DW2.Width = iWidth;
	pState_g9->DW2.Height = iHeight;
	pState_g9->DW3.Depth = iDepth;
	pState_g9->DW3.SurfacePitch = 0;
	pState_g9->DW8.SurfaceBaseAddress = 0;

	GENHW_HW_CHK_STATUS(pHwInterface->pfnSetupSurfaceStateOs(pHwInterface,
								 pSurface,
								 pParams,
								 pSurfaceEntry));

 finish:
	return eStatus;
}

GENOS_STATUS IntelGen_HwSetupSurfaceState_g9(PGENHW_HW_INTERFACE pHwInterface,
					     PGENHW_SURFACE pSurface,
					     PGENHW_SURFACE_STATE_PARAMS
					     pParams, PINT piNumEntries,
					     PGENHW_SURFACE_STATE_ENTRY *
					     ppSurfaceEntries)
{
	PGENHW_HW_COMMANDS pHwCommands;
	PGENHW_SSH pSSH;
	PSURFACE_STATE_G9 pSurfaceState_g9;
	PGENHW_SURFACE_STATE_ENTRY pSurfaceEntry;
	PGENHW_PLANE_OFFSET pPlaneOffset;
	INT i;
	DWORD dwPixelsPerSampleUV;
	GENOS_STATUS eStatus;

	GENHW_HW_ASSERT(pHwInterface);
	GENHW_HW_ASSERT(pSurface);
	GENHW_HW_ASSERT(pParams);
	GENHW_HW_ASSERT(ppSurfaceEntries);
	GENHW_HW_ASSERT(pHwInterface->pHwCommands);
	GENHW_HW_ASSERT(pHwInterface->pSurfaceStateHeap);

#define GET_SURFACE_STATE_G9_TILEMODE(_pSurfaceEntry)   ((_pSurfaceEntry->bTiledSurface) ? ((_pSurfaceEntry->bTileWalk == 0) ?  \
                                                             2 : 3 ) : 0 )

	eStatus = GENOS_STATUS_UNKNOWN;
	pSSH = pHwInterface->pSurfaceStateHeap;
	pHwCommands = pHwInterface->pHwCommands;

	GENHW_HW_CHK_STATUS(pHwInterface->pfnGetSurfaceStateEntries
			    (pHwInterface, pSurface, pParams, piNumEntries,
			     ppSurfaceEntries));

	for (i = 0; i < *piNumEntries; i++) {
		pSurfaceEntry = ppSurfaceEntries[i];

		pSurfaceEntry->dwSurfStateOffset =
		    IntelGen_HwGetCurSurfaceStateBase(pSSH) +
		    (pSurfaceEntry->iSurfStateID * sizeof(SURFACE_STATE_G9));

		if (pSurfaceEntry->bAVS) {

		} else {
			pSurfaceState_g9 =
			    &pSurfaceEntry->
			    pSurfaceState->PacketSurfaceState_g9.
			    cmdSurfaceState_g9;

			*pSurfaceState_g9 = *(pHwCommands->pSurfaceState_g9);

			if (pSurfaceEntry->YUVPlane == GENHW_U_PLANE ||
			    pSurfaceEntry->YUVPlane == GENHW_V_PLANE) {
				pPlaneOffset =
				    (pSurfaceEntry->YUVPlane ==
				     GENHW_U_PLANE) ? &pSurface->UPlaneOffset :
				    &pSurface->VPlaneOffset;

				pSurfaceState_g9->DW5.YOffset =
				    pPlaneOffset->iYOffset >> 2;

				if (pParams->bWidthInDword_UV) {
					IntelGen_GetPixelsPerSample
					    (pSurface->Format,
					     &dwPixelsPerSampleUV);
				} else {
					dwPixelsPerSampleUV = 1;
				}

				if (dwPixelsPerSampleUV == 1) {
					pSurfaceState_g9->DW5.XOffset =
					    pPlaneOffset->iXOffset >> 2;
				} else {
					pSurfaceState_g9->DW5.XOffset =
					    (pPlaneOffset->iXOffset /
					     (DWORD) sizeof(DWORD)) >> 2;
				}
			} else {
				pSurfaceState_g9->DW5.XOffset =
				    (pSurface->YPlaneOffset.iXOffset /
				     (DWORD) sizeof(DWORD)) >> 2;
				pSurfaceState_g9->DW5.YOffset =
				    pSurface->YPlaneOffset.iYOffset >> 2;

				if ((pSurfaceEntry->YUVPlane == GENHW_Y_PLANE)
				    && (pSurfaceEntry->dwFormat ==
					GFX3DSTATE_SURFACEFORMAT_PLANAR_420_8))
				{
					if (pSurface->Format == Format_YV12) {
						pSurfaceState_g9->
						    DW6.SeperateUVPlaneEnable =
						    1;
						pSurfaceState_g9->
						    DW6.XOffsetUOrUVPlane = 0;
						pSurfaceState_g9->
						    DW6.YOffsetUOrUVPlane =
						    (WORD) (pSurface->dwHeight *
							    2 +
							    pSurface->dwHeight /
							    2);

						pSurfaceState_g9->
						    DW11.XOffsetVPlane = 0;
						pSurfaceState_g9->
						    DW11.YOffsetVPlane =
						    (WORD) (pSurface->dwHeight *
							    2);
					} else {
						pSurfaceState_g9->
						    DW6.SeperateUVPlaneEnable =
						    FALSE;
						pSurfaceState_g9->
						    DW6.XOffsetUOrUVPlane = 0;
						pSurfaceState_g9->
						    DW6.YOffsetUOrUVPlane =
						    (WORD) pSurface->dwHeight;

						pSurfaceState_g9->
						    DW11.XOffsetVPlane = 0;
						pSurfaceState_g9->
						    DW11.YOffsetVPlane = 0;
					}
				}
			}

			pSurfaceState_g9->DW0.SurfaceType =
			    (pSurface->dwDepth >
			     1) ? GFX3DSTATE_SURFACETYPE_3D :
			    GFX3DSTATE_SURFACETYPE_2D;

			pSurfaceState_g9->DW0.VerticalLineStrideOffset =
			    pSurfaceEntry->bVertStrideOffs;
			pSurfaceState_g9->DW0.VerticalLineStride =
			    pSurfaceEntry->bVertStride;
			pSurfaceState_g9->DW0.TileMode =
			    GET_SURFACE_STATE_G9_TILEMODE(pSurfaceEntry);
			pSurfaceState_g9->DW0.SurfaceFormat =
			    pSurfaceEntry->dwFormat;
			pSurfaceState_g9->DW1.SurfaceMemObjCtrlState =
			    pHwInterface->pfnGetSurfaceMemoryObjectControl
			    (pHwInterface, pParams);
			pSurfaceState_g9->DW2.Width =
			    pSurfaceEntry->dwWidth - 1;
			pSurfaceState_g9->DW2.Height =
			    pSurfaceEntry->dwHeight - 1;
			pSurfaceState_g9->DW3.SurfacePitch =
			    pSurfaceEntry->dwPitch - 1;
			pSurfaceState_g9->DW3.Depth =
			    MAX(1, pSurface->dwDepth) - 1;

			pSurfaceState_g9->DW8.SurfaceBaseAddress = 0;
			pSurfaceState_g9->DW9.SurfaceBaseAddress64 = 0;
		}

		GENHW_HW_CHK_STATUS(pHwInterface->pfnSetupSurfaceStateOs
				    (pHwInterface, pSurface, pParams,
				     pSurfaceEntry));
	}

	eStatus = GENOS_STATUS_SUCCESS;

 finish:
	return eStatus;
}

static inline DWORD EncodeSLMSize(DWORD SLMSize)
{
	DWORD EncodedValue;
	if (SLMSize <= 2)
		EncodedValue = SLMSize;
	else {
		EncodedValue = 0;
		do {
			SLMSize >>= 1;
			EncodedValue++;
		} while (SLMSize);
	}
	return EncodedValue;
}

VOID IntelGen_HwSetupInterfaceDescriptor_g9(PGENHW_HW_INTERFACE pHwInterface,
					    PGENHW_MEDIA_STATE pMediaState,
					    PGENHW_KRN_ALLOCATION
					    pKernelAllocation,
					    PGENHW_INTERFACE_DESCRIPTOR_PARAMS
					    pInterfaceDescriptorParams,
					    PGENHW_GPGPU_WALKER_PARAMS
					    pGpGpuWalkerParams)
{
	PGENHW_GSH pGSH;
	PGENHW_SSH pSSH;
	PINTERFACE_DESCRIPTOR_DATA_G8 pInterfaceDescriptor;
	DWORD dwBTOffset;

	pGSH = pHwInterface->pGeneralStateHeap;
	pSSH = pHwInterface->pSurfaceStateHeap;

	dwBTOffset = IntelGen_HwGetCurBindingTableBase(pSSH) +
	    (pInterfaceDescriptorParams->iBindingTableID *
	     pSSH->iBindingTableSize);

	pInterfaceDescriptor = (PINTERFACE_DESCRIPTOR_DATA_G8)
	    (pGSH->pGSH +
	     pMediaState->dwOffset +
	     pGSH->dwOffsetMediaID +
	     (pInterfaceDescriptorParams->iMediaID * pGSH->dwSizeMediaID));

	pInterfaceDescriptor->DW0.KernelStartPointer =
	    pKernelAllocation->dwOffset >> 6;
	pInterfaceDescriptor->DW4.BindingTablePointer = dwBTOffset >> 5;
	pInterfaceDescriptor->DW5.ConstantURBEntryReadOffset =
	    pInterfaceDescriptorParams->iCurbeOffset >> 5;
	pInterfaceDescriptor->DW5.ConstantURBEntryReadLength =
	    pInterfaceDescriptorParams->iCurbeLength >> 5;
	pInterfaceDescriptor->DW7.CrsThdConDataRdLn =
	    pInterfaceDescriptorParams->iCrsThrdConstDataLn >> 5;

	if (pGpGpuWalkerParams && pGpGpuWalkerParams->GpGpuEnable) {
		pInterfaceDescriptor->DW6.BarrierEnable = 1;
		pInterfaceDescriptor->DW6.NumberofThreadsInGPGPUGroup =
		    pGpGpuWalkerParams->ThreadWidth *
		    pGpGpuWalkerParams->ThreadHeight;
		pInterfaceDescriptor->DW6.SharedLocalMemorySize =
		    EncodeSLMSize(pGpGpuWalkerParams->SLMSize);
	}
}

GENOS_STATUS IntelGen_HwSendVfeState_g9(PGENHW_HW_INTERFACE pHwInterface,
					PGENOS_COMMAND_BUFFER pCmdBuffer,
					BOOL blGpGpuWalkerMode)
{
	PMEDIA_VFE_STATE_CMD_G9 pVideoFrontEnd;
	PGENHW_HW_COMMANDS pHwCommands;
	PGENHW_GSH pGSH;
	PCGENHW_HW_CAPS pHwCaps;
	DWORD dwMaxURBSize;
	DWORD dwCURBEAllocationSize;
	DWORD dwURBEntryAllocationSize;
	DWORD dwNumberofURBEntries;
	DWORD dwMaxInterfaceDescriptorEntries;
	GENOS_STATUS eStatus;
	INT iSize;
	DWORD dwCmdSize;
	INT iRemain;
	INT iPerThreadScratchSize;

	GENHW_HW_ASSERT(pHwInterface);
	GENHW_HW_ASSERT(pCmdBuffer);
	GENHW_HW_ASSERT(pHwInterface->pHwCommands);
	GENHW_HW_ASSERT(pHwInterface->pGeneralStateHeap);

	eStatus = GENOS_STATUS_SUCCESS;
	dwCmdSize = sizeof(MEDIA_VFE_STATE_CMD_G9);
	pHwCommands = pHwInterface->pHwCommands;
	pGSH = pHwInterface->pGeneralStateHeap;
	pHwCaps = pHwInterface->pHwCaps;

	pVideoFrontEnd =
	    (PMEDIA_VFE_STATE_CMD_G9) IntelGen_OsGetCmdBufferSpace(pCmdBuffer,
								   dwCmdSize);
	GENHW_HW_CHK_NULL(pVideoFrontEnd);

	*pVideoFrontEnd = *(pHwCommands->pVideoFrontEnd_g9);

	if (pHwInterface->GshSettings.iPerThreadScratchSize > 0) {

		iPerThreadScratchSize =
		    pHwInterface->GshSettings.iPerThreadScratchSize >> 10;
		iRemain = iPerThreadScratchSize % 2;
		iPerThreadScratchSize = iPerThreadScratchSize / 2;
		iSize = 0;

		while (!iRemain && (iPerThreadScratchSize / 2)) {
			iSize++;
			iRemain = iPerThreadScratchSize % 2;
			iPerThreadScratchSize = iPerThreadScratchSize / 2;
		}

		GENHW_HW_ASSERT(!iRemain && iPerThreadScratchSize);
		GENHW_HW_ASSERT(iSize < 12);
		pVideoFrontEnd->DW1.PerThreadScratchSpace = iSize;

		pVideoFrontEnd->DW1.ScratchSpaceBasePointer =
		    pGSH->dwScratchSpaceBase >> 10;
		pVideoFrontEnd->DW2.ScratchSpaceBasePointer64 = 0;
	}

	dwMaxURBSize = pHwCaps->dwMaxURBSize;
	dwMaxInterfaceDescriptorEntries =
	    pHwCaps->dwMaxInterfaceDescriptorEntries;

	dwCURBEAllocationSize =
	    GENOS_ROUNDUP_SHIFT(pHwInterface->
				VfeStateParams.dwCURBEAllocationSize, 5);

	dwURBEntryAllocationSize =
	    GENOS_ROUNDUP_SHIFT(pHwInterface->
				VfeStateParams.dwURBEntryAllocationSize, 5);
	dwURBEntryAllocationSize = GFX_MAX(1, dwURBEntryAllocationSize);

	dwNumberofURBEntries =
	    (dwMaxURBSize - dwCURBEAllocationSize -
	     dwMaxInterfaceDescriptorEntries) / dwURBEntryAllocationSize;
	dwNumberofURBEntries = GFX_CLAMP_MIN_MAX(dwNumberofURBEntries, 1, 32);

	pVideoFrontEnd->DW3.DebugCounterControl =
	    pHwInterface->VfeStateParams.dwDebugCounterControl;
	pVideoFrontEnd->DW3.NumberofURBEntries = dwNumberofURBEntries;
	pVideoFrontEnd->DW3.MaximumNumberofThreads =
	    pHwInterface->VfeStateParams.dwMaximumNumberofThreads - 1;
	pVideoFrontEnd->DW5.CURBEAllocationSize = dwCURBEAllocationSize;
	pVideoFrontEnd->DW5.URBEntryAllocationSize = dwURBEntryAllocationSize;

	if (pHwInterface->VfeScoreboard.ScoreboardEnable) {
		pVideoFrontEnd->DW6.ScoreboardEnable = 1;
		pVideoFrontEnd->DW6.ScoreboardMask =
		    pHwInterface->VfeScoreboard.ScoreboardMask;
		pVideoFrontEnd->DW6.ScoreboardType =
		    pHwInterface->VfeScoreboard.ScoreboardType;
		pVideoFrontEnd->DW7.Value =
		    pHwInterface->VfeScoreboard.Value[0];
		pVideoFrontEnd->DW8.Value =
		    pHwInterface->VfeScoreboard.Value[1];
	}

	GENHW_HW_ASSERT(pVideoFrontEnd->DW3.NumberofURBEntries <=
			pHwCaps->dwMaxURBEntries);
	GENHW_HW_ASSERT(pVideoFrontEnd->DW5.CURBEAllocationSize <=
			pHwCaps->dwMaxCURBEAllocationSize);
	GENHW_HW_ASSERT(pVideoFrontEnd->DW5.URBEntryAllocationSize <=
			pHwCaps->dwMaxURBEntryAllocationSize);
	GENHW_HW_ASSERT(pVideoFrontEnd->DW3.NumberofURBEntries *
			pVideoFrontEnd->DW5.URBEntryAllocationSize +
			pVideoFrontEnd->DW5.CURBEAllocationSize +
			dwMaxInterfaceDescriptorEntries <= dwMaxURBSize);

	IntelGen_OsAdjustCmdBufferFreeSpace(pCmdBuffer, dwCmdSize);

 finish:
	return eStatus;
}

GENOS_STATUS IntelGen_HwSendWalkerState_g9(PGENHW_HW_INTERFACE pHwInterface,
					   PGENOS_COMMAND_BUFFER pCmdBuffer,
					   PGENHW_WALKER_PARAMS pWalkerParams)
{
	PMEDIA_OBJECT_WALKER_CMD_G9 pMediaObjWalker;
	GENOS_STATUS eStatus;
	DWORD dwCmdSize;

	eStatus = GENOS_STATUS_SUCCESS;
	dwCmdSize = sizeof(MEDIA_OBJECT_WALKER_CMD_G9);

	pMediaObjWalker = (PMEDIA_OBJECT_WALKER_CMD_G9)
	    IntelGen_OsGetCmdBufferSpace(pCmdBuffer, dwCmdSize);
	GENHW_HW_CHK_NULL(pMediaObjWalker);

	*pMediaObjWalker = *pHwInterface->pHwCommands->pMediaWalker_g9;

	pMediaObjWalker->DW0.Length =
	    OP_LENGTH(SIZE32(MEDIA_OBJECT_WALKER_CMD_G9)) +
	    pWalkerParams->InlineDataLength / sizeof(DWORD);
	pMediaObjWalker->DW1.InterfaceDescriptorOffset =
	    pWalkerParams->InterfaceDescriptorOffset;
	pMediaObjWalker->DW2.UseScoreboard =
	    pHwInterface->VfeScoreboard.ScoreboardEnable;
	pMediaObjWalker->DW5.ScoreboardMask =
	    pHwInterface->VfeScoreboard.ScoreboardMask;
	pMediaObjWalker->DW6.ColorCountMinusOne =
	    pWalkerParams->ColorCountMinusOne;

	pMediaObjWalker->DW6.MidLoopUnitX = pWalkerParams->MidLoopUnitX;
	pMediaObjWalker->DW6.MidLoopUnitY = pWalkerParams->MidLoopUnitY;
	pMediaObjWalker->DW6.MidLoopExtraSteps =
	    pWalkerParams->MiddleLoopExtraSteps;

	pMediaObjWalker->DW7.Value = pWalkerParams->LoopExecCount.value;
	pMediaObjWalker->DW8.Value = pWalkerParams->BlockResolution.value;
	pMediaObjWalker->DW9.Value = pWalkerParams->LocalStart.value;
	pMediaObjWalker->DW10.Value = pWalkerParams->LocalEnd.value;
	pMediaObjWalker->DW11.Value = pWalkerParams->LocalOutLoopStride.value;
	pMediaObjWalker->DW12.Value = pWalkerParams->LocalInnerLoopUnit.value;
	pMediaObjWalker->DW13.Value = pWalkerParams->GlobalResolution.value;
	pMediaObjWalker->DW14.Value = pWalkerParams->GlobalStart.value;
	pMediaObjWalker->DW15.Value =
	    pWalkerParams->GlobalOutlerLoopStride.value;
	pMediaObjWalker->DW16.Value = pWalkerParams->GlobalInnerLoopUnit.value;

	IntelGen_OsAdjustCmdBufferFreeSpace(pCmdBuffer, dwCmdSize);

	if (pWalkerParams->InlineDataLength) {
		GENHW_HW_CHK_STATUS(IntelGen_OsAddCommand(pCmdBuffer,
							  pWalkerParams->pInlineData,
							  pWalkerParams->InlineDataLength));
	}

 finish:
	return eStatus;
}

GENOS_STATUS IntelGen_HwSendPipelineSelectCmd_g9(PGENHW_HW_INTERFACE
						 pHwInterface,
						 PGENOS_COMMAND_BUFFER
						 pCmdBuffer,
						 DWORD dwGfxPipelineSelect)
{
	GENOS_STATUS eStatus;
	PPIPELINE_SELECT_CMD_G9 pPipelineSelectCmd;
	DWORD dwCmdSize;

	GENHW_HW_ASSERT(pHwInterface);
	GENHW_HW_ASSERT(pCmdBuffer);

	eStatus = GENOS_STATUS_SUCCESS;
	dwCmdSize = sizeof(PIPELINE_SELECT_CMD_G9);

	pPipelineSelectCmd =
	    (PPIPELINE_SELECT_CMD_G9) IntelGen_OsGetCmdBufferSpace(pCmdBuffer,
								   dwCmdSize);
	GENHW_HW_CHK_NULL(pPipelineSelectCmd);

	*pPipelineSelectCmd =
	    *(pHwInterface->pHwCommands->pPipelineSelectMedia_g9);
	pPipelineSelectCmd->DW0.PipelineSelect = dwGfxPipelineSelect;

	IntelGen_OsAdjustCmdBufferFreeSpace(pCmdBuffer, dwCmdSize);

 finish:
	return eStatus;
}

VOID IntelGen_HwInitCommands_g9(PGENHW_HW_INTERFACE pHwInterface)
{
	PGENHW_HW_COMMANDS pHwCommands;

	GENHW_HW_ASSERT(pHwInterface);

	pHwInterface->pfnInitCommandsCommon(pHwInterface);
	pHwCommands = pHwInterface->pHwCommands;

	pHwCommands->dwMediaObjectHeaderCmdSize =
	    sizeof(MEDIA_OBJECT_HEADER_G6);

	pHwCommands->pPipelineSelectMedia_g9 =
	    &g_cInit_PIPELINE_SELECT_CMD_MEDIA_G9;

	pHwCommands->pcPipeControlParam = &g_PipeControlParam_g75;
}

VOID IntelGen_HwInitCommands_g95(PGENHW_HW_INTERFACE pHwInterface)
{
	PGENHW_HW_COMMANDS pHwCommands;

	GENHW_HW_ASSERT(pHwInterface);

	pHwInterface->pfnInitCommandsCommon(pHwInterface);
	pHwCommands = pHwInterface->pHwCommands;

	pHwCommands->dwMediaObjectHeaderCmdSize =
	    sizeof(MEDIA_OBJECT_HEADER_G6);

	pHwCommands->pPipelineSelectMedia_g9 =
	    &g_cInit_PIPELINE_SELECT_CMD_MEDIA_G9;

	pHwCommands->pcPipeControlParam = &g_PipeControlParam_g75;
}

GENOS_STATUS IntelGen_HwSendSurfaces_g9(PGENHW_HW_INTERFACE pHwInterface,
					PGENOS_COMMAND_BUFFER pCmdBuffer)
{
	PGENOS_INTERFACE pOsInterface;
	PBYTE pIndirectState;
	UINT IndirectStateBase;
	UINT IndirectStateSize;
	PGENHW_SSH pSSH;
	PBYTE pBindingTablePtr;
	PSURFACE_STATE_TOKEN_G75 pSurfaceStateToken;
	PBINDING_TABLE_STATE_G8 pBindingTableCurrent;
	PBINDING_TABLE_STATE_G8 pBindingTableOutput;
	PGENHW_SURFACE_STATE pSurfaceState;
	PGENHW_SURFACE_STATE pSurfaceStateBase;
	PSURFACE_STATE_G9 pSurfaceStateOutput;
	INT iSurfaceStateOffset;
	INT iSurfaceStateIndex;
	INT iCurrentBindingTableOffset;
	INT i;
	INT j;
	GENOS_STATUS eStatus;

	GENHW_HW_CHK_NULL(pHwInterface);
	GENHW_HW_CHK_NULL(pHwInterface->pHwCommands);
	GENHW_HW_CHK_NULL(pHwInterface->pSurfaceStateHeap);

	eStatus = GENOS_STATUS_SUCCESS;
	pSSH = pHwInterface->pSurfaceStateHeap;
	pOsInterface = pHwInterface->pfnGetOsInterface(pHwInterface);

	GENHW_HW_CHK_STATUS(pOsInterface->pfnGetIndirectState(pOsInterface,
							      &IndirectStateBase,
							      &IndirectStateSize));

	pIndirectState = (PBYTE) pCmdBuffer->pCmdBase + IndirectStateBase;

	pSurfaceStateBase = (PGENHW_SURFACE_STATE)
	    (pSSH->pSshBuffer + pSSH->iSurfaceStateOffset);

	iCurrentBindingTableOffset = pSSH->iBindingTableOffset;
	pBindingTablePtr = pSSH->pSshBuffer + pSSH->iBindingTableOffset;

	for (i = pSSH->iCurrentBindingTable;
	     i > 0;
	     i--, iCurrentBindingTableOffset +=
	     pSSH->iBindingTableSize, pBindingTablePtr +=
	     pSSH->iBindingTableSize) {
		pBindingTableCurrent =
		    (PBINDING_TABLE_STATE_G8) pBindingTablePtr;
		pBindingTableOutput =
		    (PBINDING_TABLE_STATE_G8) (pIndirectState +
					       iCurrentBindingTableOffset);

		for (j = pHwInterface->SshSettings.iSurfacesPerBT;
		     j > 0;
		     j--, pBindingTableCurrent++, pBindingTableOutput++) {
			pBindingTableOutput->DW0.Value = 0;
			pBindingTableOutput->DW0.SurfaceStatePointer =
			    pBindingTableCurrent->DW0.SurfaceStatePointer;

			if (!pBindingTableCurrent->DW0.Copy) {
				continue;
			}

			iSurfaceStateOffset = pBindingTableOutput->DW0.Value;
			iSurfaceStateIndex =
			    (iSurfaceStateOffset -
			     pSSH->iSurfaceStateOffset) >> 6;

			GENHW_HW_ASSERT(iSurfaceStateOffset >= 0);
			GENHW_HW_ASSERT(iSurfaceStateIndex >= 0);

			pSurfaceState = pSurfaceStateBase + iSurfaceStateIndex;
			pSurfaceStateToken =
			    &pSurfaceState->PacketSurfaceState_g9.Token;
			pSurfaceStateOutput =
			    (PSURFACE_STATE_G9) (pIndirectState +
						 iSurfaceStateOffset);

			if (pSurfaceStateToken->DW3.SurfaceStateType == 0) {
				iSurfaceStateOffset += 8 * sizeof(DWORD);

				GENOS_SecureMemcpy(pSurfaceStateOutput,
						   sizeof(SURFACE_STATE_G9),
						   &pSurfaceState->PacketSurfaceState_g9.cmdSurfaceState_g9,
						   sizeof(SURFACE_STATE_G9));
			} else {
				GENHW_HW_ASSERT(0);
			}

			pOsInterface->pfnSetPatchEntry(pOsInterface,
						       pSurfaceStateToken->
						       DW1.SurfaceAllocationIndex,
						       pSurfaceStateToken->
						       DW2.SurfaceOffset,
						       IndirectStateBase +
						       iSurfaceStateOffset);
		}
	}

 finish:
	GENHW_HW_ASSERT(eStatus == GENOS_STATUS_SUCCESS);
	return eStatus;
}

static GENOS_STATUS IntelGen_HwSendDebugCtl_g9(PGENHW_HW_INTERFACE pHw,
					       PGENOS_COMMAND_BUFFER pCmdBuffer)
{
	GENOS_STATUS eStatus = GENOS_STATUS_SUCCESS;
	GENHW_LOAD_REGISTER_IMM_PARAM lri;

	GENHW_HW_ASSERT(pHw);

	// CS_DEBUG_MODE1, global debug enable
	lri.dwRegisterAddress	= GENHW_REG_CS_DEBUG_MODE1;
	lri.dwData		= GENHW_REG_CS_DEBUG_MODE1_GLOBAL_DEBUG;
	lri.dwData	       |= (lri.dwData << 16);
	GENHW_HW_CHK_STATUS(pHw->pfnSendLoadRegImmCmd(pHw, pCmdBuffer, &lri));

	// TD_CTL, force thread breakpoint enable
	lri.dwRegisterAddress	= GENHW_REG_TD_CTL;
	lri.dwData		= GENHW_REG_TD_CTL_FORCE_BKPT_ENABLE |
				  GENHW_REG_TD_CTL_FORCE_EXCEPTION_ENABLE;
	GENHW_HW_CHK_STATUS(pHw->pfnSendLoadRegImmCmd(pHw, pCmdBuffer, &lri));

finish:
	return eStatus;
}

VOID IntelGen_HwInitInterface_g9(PGENHW_HW_INTERFACE pHwInterface)
{
	GENHW_HW_ASSERT(pHwInterface);

	if (pHwInterface->Platform.GtType == GTTYPE_GT1) {
		pHwInterface->pHwCaps = &g_IntelGen_HwCaps_g9_gt1;
	} else if (pHwInterface->Platform.GtType == GTTYPE_GT2) {
		pHwInterface->pHwCaps = &g_IntelGen_HwCaps_g9_gt2;
	} else if (pHwInterface->Platform.GtType == GTTYPE_GT3) {
		pHwInterface->pHwCaps = &g_IntelGen_HwCaps_g9_gt3;
	} else if (pHwInterface->Platform.GtType == GTTYPE_GT4) {
		pHwInterface->pHwCaps = &g_IntelGen_HwCaps_g9_gt4;
	} else if (pHwInterface->Platform.GtType == GTTYPE_GTA) {
		pHwInterface->pHwCaps = &g_IntelGen_HwCaps_g9_gta;
	} else if (pHwInterface->Platform.GtType == GTTYPE_GTC) {
		pHwInterface->pHwCaps = &g_IntelGen_HwCaps_g9_gtc;
	} else if (pHwInterface->Platform.GtType == GTTYPE_GTX) {
		pHwInterface->pHwCaps = &g_IntelGen_HwCaps_g9_gtx;
	} else {
		GENHW_HW_ASSERTMESSAGE("Unsupported GT type.");
	}

	pHwInterface->GshSettings = g_GSH_Settings_g9;

	pHwInterface->SshSettings = g_SSH_Settings_g8;

	pHwInterface->SurfaceTypeDefault = GENHW_SURFACE_TYPE_G8;
	pHwInterface->bUsesPatchList = TRUE;

	pHwInterface->iSizeBindingTableState = sizeof(BINDING_TABLE_STATE_G8);
	pHwInterface->iSizeInstructionCache = GENHW_INSTRUCTION_CACHE_G75;
	pHwInterface->iSizeInterfaceDescriptor =
	    sizeof(INTERFACE_DESCRIPTOR_DATA_G8);
	pHwInterface->bEnableYV12SinglePass = FALSE;

	pHwInterface->VfeStateParams.dwDebugCounterControl = 0;
	pHwInterface->VfeStateParams.dwMaximumNumberofThreads =
	    pHwInterface->pHwCaps->dwMaxThreads;

	pHwInterface->pfnAssignBindingTable = IntelGen_HwAssignBindingTable_g8;
	pHwInterface->pfnSetupBufferSurfaceState =
	    IntelGen_HwSetupBufferSurfaceState_g9;
	pHwInterface->pfnInitInterfaceDescriptor =
	    IntelGen_HwInitInterfaceDescriptor_g8;
	pHwInterface->pfnSetupInterfaceDescriptor =
	    IntelGen_HwSetupInterfaceDescriptor_g9;
	pHwInterface->pfnGetMediaWalkerStatus =
	    IntelGen_HwGetMediaWalkerStatus_g75;
	pHwInterface->pfnGetMediaWalkerBlockSize =
	    IntelGen_HwGetMediaWalkerBlockSize_g9;
	pHwInterface->pfnSendCurbeLoad = IntelGen_HwSendCurbeLoad_g75;
	pHwInterface->pfnSendIDLoad = IntelGen_HwSendIDLoad_g75;
	pHwInterface->pfnLoadCurbeData = IntelGen_HwLoadCurbeData_g8;
	pHwInterface->pfnSendMediaStateFlush =
	    IntelGen_HwSendMediaStateFlush_g75;
	pHwInterface->pfnGetSurfaceMemoryObjectControl =
	    IntelGen_HwGetSurfaceMemoryObjectControl_g8;

	pHwInterface->pfnConvertToNanoSeconds =
	    IntelGen_HwConvertToNanoSeconds_g75;
	pHwInterface->pfnSendGpGpuWalkerState =
	    IntelGen_HwSendGpGpuWalkerState_g8;
	pHwInterface->pfnSkipPipeControlCmdBb =
	    IntelGen_HwSkipPipeControlCmdBb_g75;
	pHwInterface->pfnAddPipeControlCmdBb =
	    IntelGen_HwAddPipeControlCmdBb_g75;

	pHwInterface->pfnSetupSurfaceState = IntelGen_HwSetupSurfaceState_g9;
	pHwInterface->pfnSendVfeState = IntelGen_HwSendVfeState_g9;
	pHwInterface->pfnSendMediaObjectWalker = IntelGen_HwSendWalkerState_g9;
	pHwInterface->pfnGetScratchSpaceSize = IntelGen_GetScratchSpaceSize_g8;
	pHwInterface->pfnSendPipelineSelectCmd =
	    IntelGen_HwSendPipelineSelectCmd_g9;

	pHwInterface->pfnInitCommands = IntelGen_HwInitCommands_g9;

	pHwInterface->pfnIs2PlaneNV12Needed = IntelGen_HwIs2PlaneNV12Needed_g75;

	pHwInterface->pfnSendStateSip = IntelGen_HwSendStateSip_g8;
	pHwInterface->pfnSendDebugCtl = IntelGen_HwSendDebugCtl_g9;
}
