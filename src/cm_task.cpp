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
 *     Wei Lin<wei.w.lin@intel.com>
 *     Yuting Yang<yuting.yang@intel.com>
 */

#include "cm_task.h"
#include "cm_kernel.h"
#include "cm_def.h"
#include "cm_thread_space.h"
#include "cm_device.h"

INT CmTask_RT::Create(CmDevice_RT * pCmDevice, UINT index, UINT max_kernel_count,
		   CmTask_RT * &pKernelArray)
{
	INT result = CM_SUCCESS;
	pKernelArray =
	    new(std::nothrow) CmTask_RT(pCmDevice, index, max_kernel_count);
	if (pKernelArray) {
		result = pKernelArray->Initialize();
		if (result != CM_SUCCESS) {
			CmTask_RT::Destroy(pKernelArray);
		}
	} else {
		CM_ASSERT(0);
		result = CM_OUT_OF_HOST_MEMORY;
	}
	return result;
}

INT CmTask_RT::Destroy(CmTask_RT * &pKernelArray)
{
	if (pKernelArray) {
		delete pKernelArray;
		pKernelArray = NULL;
	}

	return CM_SUCCESS;
}

 CmTask_RT::CmTask_RT(CmDevice_RT * pCmDevice, UINT index, UINT max_kernel_count):
m_pKernelArray(NULL),
m_KernelCount(0),
m_MaxKernelCount(max_kernel_count),
m_IndexTaskArray(index), m_ui64SyncBitmap(0), m_pCmDev(pCmDevice)
{
	CmSafeMemSet(&m_PowerOption, 0, sizeof(m_PowerOption));
	m_PreemptionMode = UN_PREEMPTABLE_MODE;
}

CmTask_RT::~CmTask_RT(void)
{
	CmSafeDeleteArray(m_pKernelArray);
}

INT CmTask_RT::Initialize()
{
	m_pKernelArray = new(std::nothrow) CmKernel *[m_MaxKernelCount];

	if (m_pKernelArray) {
		CmSafeMemSet(m_pKernelArray, 0,
			     sizeof(CmKernel *) * m_MaxKernelCount);
		return CM_SUCCESS;
	} else {
		CM_ASSERT(0);
		return CM_OUT_OF_HOST_MEMORY;
	}
}

CM_RT_API INT CmTask_RT::AddKernel(CmKernel * pKernel)
{
	if (m_MaxKernelCount <= m_KernelCount) {
		return CM_EXCEED_MAX_KERNEL_PER_ENQUEUE;
	}
	if (pKernel == NULL) {
		CM_ASSERT(0);
		return CM_INVALID_ARG_VALUE;
	}

	m_pKernelArray[m_KernelCount] = pKernel;
	CmKernel_RT* pKernel_RT = static_cast<CmKernel_RT*>(pKernel);
	pKernel_RT->SetIndexInTask(m_KernelCount);

	m_KernelCount++;

	return CM_SUCCESS;
}

CM_RT_API INT CmTask_RT::Reset(void)
{
	m_KernelCount = 0;
	m_ui64SyncBitmap = 0;

	if (m_pKernelArray) {
		CmSafeMemSet(m_pKernelArray, 0,
			     sizeof(CmKernel *) * m_MaxKernelCount);
		return CM_SUCCESS;
	} else {
		CM_ASSERT(0);
		return CM_FAILURE;
	}
}

UINT CmTask_RT::GetKernelCount()
{
	return m_KernelCount;
}

CmKernel *CmTask_RT::GetKernelPointer(UINT index)
{
	if (index >= m_KernelCount) {
		CM_ASSERT(0);
		return NULL;
	}
	return m_pKernelArray[index];
}

UINT CmTask_RT::GetIndexInTaskArray()
{
	return m_IndexTaskArray;
}

BOOLEAN CmTask_RT::IntegrityCheckKernelThreadspace(void)
{
	INT hr = CM_SUCCESS;
	UINT kernelCount = 0;
	UINT i = 0;
	UINT j = 0;
	CmKernel_RT *pKernel_RT = NULL;
	CmKernel_RT *pKernTmp = NULL;
	UINT threadCount = 0;
	CmThreadSpace *pKernelTS = NULL;
	UINT width = 0;
	UINT height = 0;
	BOOLEAN **pTSMapping = NULL;
	BOOLEAN *pKernelInScoreboard = NULL;
	CM_THREAD_SPACE_UNIT *pThreadSpaceUnit = NULL;
	UINT kernelIndex = 0;
	UINT unassociated = 0;

	kernelCount = this->GetKernelCount();

	pTSMapping = new(std::nothrow) BOOLEAN *[kernelCount];
	pKernelInScoreboard = new(std::nothrow) BOOLEAN[kernelCount];

	CMCHK_NULL_RETURN(pTSMapping, CM_OUT_OF_HOST_MEMORY);
	CMCHK_NULL_RETURN(pKernelInScoreboard, CM_OUT_OF_HOST_MEMORY);

	CmSafeMemSet(pTSMapping, 0, kernelCount * sizeof(BOOLEAN *));
	CmSafeMemSet(pKernelInScoreboard, 0, kernelCount * sizeof(BOOLEAN));

	for (i = 0; i < kernelCount; ++i) {
                CmKernel *pKernel = NULL;
		pKernel = this->GetKernelPointer(i);
                pKernel_RT=(static_cast<CmKernel_RT*>(pKernel));
		CMCHK_NULL(pKernel_RT);

		CMCHK_HR(pKernel_RT->GetThreadSpace(pKernelTS));
		CMCHK_NULL_RETURN(pKernelTS, CM_KERNEL_THREADSPACE_NOT_SET);

		CMCHK_HR(pKernelTS->GetThreadSpaceSize(width, height));
		CMCHK_HR(pKernel_RT->GetThreadCount(threadCount));

		if (threadCount != (width * height)) {
			CM_ASSERT(0);
			hr = CM_INVALID_KERNEL_THREADSPACE;
			goto finish;
		}

		if (pKernelTS->IsThreadAssociated()) {
			pTSMapping[i] = new(std::nothrow) BOOLEAN[threadCount];
			CMCHK_NULL_RETURN(pTSMapping[i], CM_OUT_OF_HOST_MEMORY);
			CmSafeMemSet(pTSMapping[i], 0,
				     threadCount * sizeof(BOOLEAN));
			pKernelInScoreboard[i] = FALSE;

			hr = pKernelTS->GetThreadSpaceUnit(pThreadSpaceUnit);
			if (hr != CM_SUCCESS || pThreadSpaceUnit == NULL) {
				CM_ASSERT(0);
				CmSafeDeleteArray(pTSMapping[i]);
				hr = CM_FAILURE;
				goto finish;
			}

			for (j = 0; j < width * height; ++j) {
				pKernTmp =
				    static_cast < CmKernel_RT * >(pThreadSpaceUnit[j].pKernel);
				if (pKernTmp == NULL) {
					if (pKernelTS->GetNeedSetKernelPointer
					    ()) {
						pKernTmp =static_cast < CmKernel_RT * >
						    (pKernelTS->GetKernelPointer());
					}
					if (pKernTmp == NULL) {
						CM_ASSERT(0);
						CmSafeDeleteArray(pTSMapping
								  [i]);
						hr = CM_FAILURE;
						goto finish;
					}
				}

				kernelIndex = pKernTmp->GetIndexInTask();
				pTSMapping[kernelIndex][pThreadSpaceUnit
							[j].threadId] = TRUE;
				pKernelInScoreboard[kernelIndex] = TRUE;
			}

			if (pKernelInScoreboard[i] == TRUE) {
				pKernel_RT->SetAssociatedToTSFlag(TRUE);
				for (j = 0; j < threadCount; ++j) {
					if (pTSMapping[i][j] == FALSE) {
						unassociated++;
						break;
					}
				}
			}
			CmSafeDeleteArray(pTSMapping[i]);
		}

		if (unassociated != 0) {
			CM_ASSERT(0);
			hr = CM_KERNEL_THREADSPACE_THREADS_NOT_ASSOCIATED;
			goto finish;
		}
	}

 finish:

	CmSafeDeleteArray(pTSMapping);
	CmSafeDeleteArray(pKernelInScoreboard);

	return (hr == CM_SUCCESS) ? TRUE : FALSE;
}

CM_RT_API INT CmTask_RT::AddSync(void)
{
	if (m_KernelCount > 0) {
		m_ui64SyncBitmap |= (UINT64) 1 << (m_KernelCount - 1);
	}

	return CM_SUCCESS;
}

UINT64 CmTask_RT::GetSyncBitmap()
{
	return m_ui64SyncBitmap;
}

CM_RT_API INT CmTask_RT::SetPowerOption(PCM_HAL_POWER_OPTION_PARAM pPowerOption)
{
	UINT nGTPlatform;
	size_t nGTPlatformSize;
	UINT nGPUPlatform;
	size_t nGPUPlatformSize;
	CM_HAL_POWER_OPTION_PARAM PowerOptionLimit;

	nGTPlatform = 0;
	nGTPlatformSize = sizeof(nGTPlatform);
	nGPUPlatform = 0;
	nGPUPlatformSize = sizeof(nGPUPlatform);

	m_pCmDev->GetCaps(CAP_GT_PLATFORM, nGTPlatformSize, &nGTPlatform);
	m_pCmDev->GetCaps(CAP_GPU_PLATFORM, nGPUPlatformSize, &nGPUPlatform);
	PowerOptionLimit =
	    CM_PLATFORM_POWER_CONFIGURATION[nGPUPlatform][nGTPlatform];

	if ((pPowerOption->nSlice > PowerOptionLimit.nSlice) ||
	    (pPowerOption->nSubSlice > PowerOptionLimit.nSubSlice) ||
	    (pPowerOption->nEU > PowerOptionLimit.nEU)) {
		return CM_EXCEED_MAX_POWER_OPTION_FOR_PLATFORM;
	}

	CmFastMemCopy(&m_PowerOption, pPowerOption, sizeof(m_PowerOption));
	return CM_SUCCESS;
}

PCM_HAL_POWER_OPTION_PARAM CmTask_RT::GetPowerOption()
{
	return &m_PowerOption;
}

CM_RT_API INT CmTask_RT::SetPreemptionMode(CM_HAL_PREEMPTION_MODE mode)
{
	m_PreemptionMode = mode;

	return CM_SUCCESS;
}

CM_RT_API CM_HAL_PREEMPTION_MODE CmTask_RT::GetPreemptionMode()
{
	return m_PreemptionMode;
}
