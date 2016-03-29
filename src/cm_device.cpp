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
 *     Lina Sun<lina.sun@intel.com>
 *     Zhao Yakui <yakui.zhao@intel.com>
 */

#include "cm_device.h"
#include "cm_queue.h"
#include "cm_surface_manager.h"
#include "cm_program.h"
#include "cm_kernel.h"
#include "cm_task.h"
#include "cm_buffer.h"
#include "cm_thread_space.h"
#include "cm_debug.h"
#include "cm_def.h"
#include "cm_group_space.h"
#include "cm_surface_2d.h"
#include "debugger.h"
#include "hal_cm.h"
#include "readconf.h"

CSync CmDevice_RT::GlobalCriticalSection_Surf2DUserDataLock = CSync();
CM_DLL_FILE_VERSION CmDevice_RT::m_RTDllVersion = {(WORD)MANVERSION,
                                                   (WORD)MANREVISION,
                                                   (WORD)SUBREVISION,
                                                   (WORD)BUILD_NUMBER};

INT CmDevice_RT::Create(CmDriverContext * pDriverContext, CmDevice_RT * &pDevice,
		     UINT DevCreateOption)
{
	INT result = CM_FAILURE;

	if (pDevice != NULL) {
		pDevice->Acquire();
		return CM_SUCCESS;
	}

	pDevice = new(std::nothrow) CmDevice_RT(DevCreateOption);
	if (pDevice) {
		pDevice->Acquire();
		result = pDevice->Initialize(pDriverContext);
		if (result != CM_SUCCESS) {
			CM_ASSERT(0);
			CmDevice_RT::Destroy(pDevice);
			pDevice = NULL;
		}
	} else {
		CM_ASSERT(0);
		result = CM_OUT_OF_HOST_MEMORY;
	}

    if (result == CM_SUCCESS)
        DbgNotifyNewDevice(pDevice);

	return result;
}

INT CmDevice_RT::Acquire(void)
{
	CLock locker(m_CriticalSection_DeviceRefCount);

	m_CmDeviceRefCount++;
	return CM_SUCCESS;
}

INT CmDevice_RT::Release(void)
{
	CLock locker(m_CriticalSection_DeviceRefCount);

	m_CmDeviceRefCount--;

	return m_CmDeviceRefCount;
}

INT CmDevice_RT::Destroy(CmDevice_RT * &pDevice)
{
	INT result = CM_SUCCESS;

	INT refCount = pDevice->Release();

	if (refCount == 0) {
        DbgNotifyDeviceDestruction(pDevice);
		CmSafeDelete(pDevice);
	}

	return result;
}

 CmDevice_RT::CmDevice_RT(UINT DevCreateOption):
m_pUmdContext(NULL),
m_pAccelData(NULL),
m_AccelSize(0),
m_pSurfaceMgr(NULL),
m_pQueue(NULL),
m_ProgramArray(CM_INIT_PROGRAM_COUNT),
m_ProgramCount(0),
m_KernelArray(CM_INIT_KERNEL_COUNT),
m_KernelCount(0),
m_ThreadSpaceArray(CM_INIT_THREADSPACE_COUNT),
m_ThreadSpaceCount(0),
m_hJITDll(NULL),
m_fJITCompile(NULL),
m_fFreeBlock(NULL),
m_fJITVersion(NULL),
m_DDIVersion(0),
m_Platform(IGFX_UNKNOWN_CORE),
m_CmDeviceRefCount(0),
m_ThreadGroupSpaceArray(CM_INIT_THREADGROUPSPACE_COUNT),
m_ThreadGroupSpaceCount(0), m_TaskArray(CM_INIT_TASK_COUNT), m_TaskCount(0)
{
        CmSafeMemSet(&m_l3_c, 0, sizeof(L3_CONFIG_REGISTER_VALUES));
	InitDevCreateOption(m_DevCreateOption, DevCreateOption);
}

CmDevice_RT::~CmDevice_RT(void)
{
	for (UINT i = 0; i < m_KernelCount; i++) {
		CmKernel_RT *pKernel = (CmKernel_RT *) m_KernelArray.GetElement(i);
		if (pKernel) {
			CmProgram_RT *pProgram = NULL;
			pKernel->GetCmProgram(pProgram);
			UINT indexInProgramArray;
			for (indexInProgramArray = 0;
			     indexInProgramArray < m_ProgramCount;
			     indexInProgramArray++) {
				if (pProgram ==
				    m_ProgramArray.GetElement
				    (indexInProgramArray)) {
					break;
				}
			}
			CmKernel_RT::Destroy(pKernel, pProgram);
			if ((pProgram == NULL)
			    && (indexInProgramArray < m_ProgramCount)) {
				m_ProgramArray.SetElement(indexInProgramArray,
							  NULL);
			}
		}
	}
	m_KernelArray.Delete();

	for (UINT i = 0; i < m_ProgramCount; i++) {
		CmProgram_RT *pProgram =
		    (CmProgram_RT *) m_ProgramArray.GetElement(i);
		while (pProgram) {
			CmProgram_RT::Destroy(pProgram);
		}
	}
	m_ProgramArray.Delete();

	UINT ThreadSpaceArrayUsedSize = m_ThreadSpaceArray.GetSize();
	for (UINT i = 0; i < ThreadSpaceArrayUsedSize; i++) {
		CmThreadSpace *pTS_RT =
		    (CmThreadSpace *) m_ThreadSpaceArray.GetElement(i);
		if (pTS_RT) {
			CmThreadSpace::Destroy(pTS_RT);
		}
	}
	m_ThreadSpaceArray.Delete();

	for (UINT i = 0; i < m_ThreadGroupSpaceCount; i++) {
		CmThreadGroupSpace *pTGS = (CmThreadGroupSpace *)
		    m_ThreadGroupSpaceArray.GetElement(i);
		if (pTGS) {
			CmThreadGroupSpace_RT::Destroy(pTGS);
		}
	}
	m_ThreadGroupSpaceArray.Delete();

	UINT TaskArrayUsedSize = m_TaskArray.GetSize();
	for (UINT i = 0; i < TaskArrayUsedSize; i++) {
		CmTask_RT *pTask = (CmTask_RT *) m_TaskArray.GetElement(i);
		if (pTask) {
			CmTask_RT::Destroy(pTask);
		}
	}
	m_TaskArray.Delete();

	CmSurfaceManager::Destroy(m_pSurfaceMgr);
	DestroyQueue(m_pQueue);

	if (m_hJITDll) {
		FreeLibrary(m_hJITDll);
	}

	DestroyAuxDevice();
};

INT CmDevice_RT::Initialize(CmDriverContext * pDriverContext)
{
	INT result = CreateAuxDevice(pDriverContext);

	if (result != CM_SUCCESS) {
		CM_ASSERT(0);
		return result;
	}

	m_pSurfaceMgr = NULL;
	result = CmSurfaceManager::Create(this,
					  m_HalMaxValues,
					  m_HalMaxValuesEx, m_pSurfaceMgr);

	if (result != CM_SUCCESS) {
		CM_ASSERT(0);
		return result;
	}

	result = CreateQueue_Internel();
	if (result != CM_SUCCESS) {
		CM_ASSERT(0);
		return result;
	}

	return result;
}

INT CmDevice_RT::CreateAuxDevice(CmDriverContext * pDriverContext)
{
	INT hr = CM_SUCCESS;
	PCM_HAL_STATE pCmHalState;
	PCM_CONTEXT pCmCtx;
	PGENOS_CONTEXT pOsContext;

	pOsContext =
	    (PGENOS_CONTEXT) GENOS_AllocAndZeroMemory(sizeof(GENOS_CONTEXT));
	CMCHK_NULL(pOsContext);

	if (pDriverContext) {
		pOsContext->wDeviceID = pDriverContext->deviceid;
		pOsContext->wRevision = pDriverContext->device_rev;
		pOsContext->bufmgr = pDriverContext->bufmgr;
	}

	m_pUmdContext = pOsContext;

	CHK_GENOSSTATUS_RETURN_CMERROR(HalCm_Create
				       (pOsContext, &m_DevCreateOption,
					&pCmHalState));

	CHK_GENOSSTATUS_RETURN_CMERROR(pCmHalState->pfnCmAllocate(pCmHalState));

	pCmCtx = (PCM_CONTEXT) GENOS_AllocAndZeroMemory(sizeof(CM_CONTEXT));
	CMCHK_NULL(pCmCtx);
	pCmCtx->GenHwDrvCtx = *pOsContext;
	pCmCtx->pCmHalState = pCmHalState;

	m_pAccelData = (PVOID) pCmCtx;

	CMCHK_HR_MESSAGE(GetMaxValueFromCaps(m_HalMaxValues, m_HalMaxValuesEx),
			 "Failed to get Max values.");
	CMCHK_HR_MESSAGE(GetGenPlatform(m_Platform), "Failed to get GPU type.");

	m_DDIVersion = VA_CM_VERSION;

 finish:
	return hr;
}

INT CmDevice_RT::DestroyAuxDevice()
{
	PCM_CONTEXT pCmData = (PCM_CONTEXT) m_pAccelData;

	if (pCmData && pCmData->pCmHalState) {
		HalCm_Destroy(pCmData->pCmHalState);
		GENOS_FreeMemory(pCmData);
	}

	if (m_pUmdContext) {
		GENOS_FreeMemAndSetNull(m_pUmdContext);
	}

	return CM_SUCCESS;
}

CM_RT_API INT CmDevice_RT::CreateBuffer(UINT size, CmBuffer * &pSurface)
{
	if ((size < CM_MIN_SURF_WIDTH) || (size > CM_MAX_1D_SURF_WIDTH)) {
		CM_ASSERT(0);
		return CM_INVALID_WIDTH;
	}

	CLock locker(m_CriticalSection_Surface);

	CmBuffer_RT *p = NULL;
	VOID *pSysMem = NULL;
	int result = m_pSurfaceMgr->CreateBuffer(size, CM_BUFFER_N, p, NULL,
						 pSysMem);
	pSurface = static_cast < CmBuffer * >(p);

	return result;
}

CM_RT_API INT
    CmDevice_RT::CreateBuffer(CmOsResource * pCmOsResource, CmBuffer * &pSurface)
{
	INT result = CM_SUCCESS;
	if (pCmOsResource == NULL) {
		return CM_INVALID_GENOS_RESOURCE_HANDLE;
	}

	CLock locker(m_CriticalSection_Surface);
	CmBuffer_RT *pBufferRT = NULL;
	VOID *pSysMem = NULL;
	result =
	    m_pSurfaceMgr->CreateBuffer(pCmOsResource->orig_width, CM_BUFFER_N,
					pBufferRT, pCmOsResource, pSysMem);

	pSurface = static_cast < CmBuffer * >(pBufferRT);

	return result;
}

CM_RT_API INT
    CmDevice_RT::CreateBufferUP(UINT size, void *pSysMem, CmBufferUP * &pSurface)
{
	if ((size < CM_MIN_SURF_WIDTH) || (size > CM_MAX_1D_SURF_WIDTH)) {
		CM_ASSERT(0);
		return CM_INVALID_WIDTH;
	}

	CLock locker(m_CriticalSection_Surface);

	CmBuffer_RT *p = NULL;
	int result = m_pSurfaceMgr->CreateBuffer(size, CM_BUFFER_UP, p, NULL,
						 pSysMem);
	pSurface = static_cast < CmBufferUP * >(p);

	return result;
}

CM_RT_API INT CmDevice_RT::DestroyBufferUP(CmBufferUP * &pSurface)
{
	CmBuffer_RT *temp = NULL;
	if (pSurface && (pSurface->Type() == CM_ENUM_CLASS_TYPE_CMBUFFER_RT)) {
		temp = static_cast < CmBuffer_RT * >(pSurface);
	} else {
		return CM_FAILURE;
	}

	CLock locker(m_CriticalSection_Surface);

	INT status = m_pSurfaceMgr->DestroySurface(temp, APP_DESTROY);

	if (status != CM_FAILURE) {
		pSurface = NULL;
		return CM_SUCCESS;
	} else {
		return CM_FAILURE;
	}
	return status;
}

CM_RT_API INT CmDevice_RT::ForceDestroyBufferUP(CmBufferUP * &pSurface)
{
	CmBuffer_RT *temp = NULL;
	if (pSurface && (pSurface->Type() == CM_ENUM_CLASS_TYPE_CMBUFFER_RT)) {
		temp = static_cast < CmBuffer_RT * >(pSurface);
	} else {
		return CM_FAILURE;
	}

	CLock locker(m_CriticalSection_Surface);

	INT status = m_pSurfaceMgr->DestroySurface(temp, FORCE_DESTROY);

	if (status == CM_SUCCESS) {
		pSurface = NULL;
	}
	return status;
}

INT CmDevice_RT::DestroyBufferUP(CmBufferUP * &pSurface, INT iIndexInPool,
			      INT iSurfaceID, SURFACE_DESTROY_KIND kind)
{
	CmBuffer_RT *temp = NULL;

	CLock locker(m_CriticalSection_Surface);
	INT currentID = m_pSurfaceMgr->GetSurfaceIdInPool(iIndexInPool);
	if (currentID > iSurfaceID) {
		return CM_SUCCESS;
	}

	if (pSurface && (pSurface->Type() == CM_ENUM_CLASS_TYPE_CMBUFFER_RT)) {
		temp = static_cast < CmBuffer_RT * >(pSurface);
	} else {
		return CM_FAILURE;
	}

	INT status = m_pSurfaceMgr->DestroySurface(temp, kind);

	if (status == CM_SUCCESS) {
		pSurface = NULL;
	}

	return status;
}

CM_RT_API INT
    CmDevice_RT::CreateSurface2DUP(UINT width, UINT height,
				CM_SURFACE_FORMAT format, void *pSysMem,
				CmSurface2DUP * &pSurface)
{
	INT result = m_pSurfaceMgr->Surface2DSanityCheck(width, height, format);
	if (result != CM_SUCCESS) {
		CM_ASSERT(0);
		return result;
	}

	CLock locker(m_CriticalSection_Surface);
	CmSurface2DUP_RT* pSurface_RT = NULL;
	result = m_pSurfaceMgr->CreateSurface2DUP(width, height, format, pSysMem, pSurface_RT);
	pSurface=(static_cast<CmSurface2DUP *>(pSurface_RT));
    return result;
}

CM_RT_API INT
    CmDevice_RT::CreateSurface2D(UINT width, UINT height, CM_SURFACE_FORMAT format,
			      CmSurface2D * &pSurface)
{       INT result;
	CLock locker(m_CriticalSection_Surface);
        CmSurface2D_RT* pSurface_RT = NULL;
	result=m_pSurfaceMgr->CreateSurface2D(width, height, 0, TRUE, format,
					      pSurface_RT);
	pSurface=(static_cast<CmSurface2D *>(pSurface_RT));
	return result;
}

CM_RT_API INT
    CmDevice_RT::CreateSurface2D(CmOsResource * pCmOsResource,
			      CmSurface2D * &pSurface)
{
	INT result;
	if (pCmOsResource == NULL) {
		return CM_INVALID_GENOS_RESOURCE_HANDLE;
	}

	CLock locker(m_CriticalSection_Surface);
        CmSurface2D_RT* pSurface_RT = NULL;
	result=m_pSurfaceMgr->CreateSurface2D(pCmOsResource, FALSE, pSurface_RT);
	pSurface=(static_cast<CmSurface2D *>(pSurface_RT));
	return result;
}

INT CmDevice_RT::DestroySurface(CmBuffer * &pSurface, INT iIndexInPool,
			     INT iSurfaceID, SURFACE_DESTROY_KIND kind)
{
	CLock locker(m_CriticalSection_Surface);
	INT currentID = m_pSurfaceMgr->GetSurfaceIdInPool(iIndexInPool);
	if (currentID > iSurfaceID) {
		return CM_SUCCESS;
	}

	CmBuffer_RT *temp = NULL;
	if (pSurface && (pSurface->Type() == CM_ENUM_CLASS_TYPE_CMBUFFER_RT)) {
		temp = static_cast < CmBuffer_RT * >(pSurface);
	}

	if (temp == NULL) {
		return CM_FAILURE;
	}

	INT status = m_pSurfaceMgr->DestroySurface(temp, kind);

	if (status == CM_SUCCESS) {
		pSurface = NULL;
	}

	return status;
}

CM_RT_API INT CmDevice_RT::DestroySurface(CmBuffer * &pSurface)
{
	CmBuffer_RT *temp = NULL;
	if (pSurface && (pSurface->Type() == CM_ENUM_CLASS_TYPE_CMBUFFER_RT)) {
		temp = static_cast < CmBuffer_RT * >(pSurface);
	} else {
		return CM_FAILURE;
	}

	CLock locker(m_CriticalSection_Surface);

	INT status = m_pSurfaceMgr->DestroySurface(temp, APP_DESTROY);

	if (status != CM_FAILURE) {
		pSurface = NULL;
		return CM_SUCCESS;
	} else {
		return CM_FAILURE;
	}
}

CM_RT_API INT CmDevice_RT::DestroySurface(CmSurface2DUP * &pSurface)
{
	CLock locker(m_CriticalSection_Surface);
	CmSurface2DUP_RT *pSurface_RT = static_cast<CmSurface2DUP_RT *>(pSurface);
	if(pSurface_RT == NULL) {
			return CM_FAILURE;
	}
	INT status = m_pSurfaceMgr->DestroySurface(pSurface_RT, APP_DESTROY);

	if (status != CM_FAILURE) {
		pSurface = NULL;
		return CM_SUCCESS;
	} else {
		return CM_FAILURE;
	}
}

INT CmDevice_RT::DestroySurface(CmSurface2DUP * &pSurface, INT iIndexInPool,
			     INT iSurfaceID, SURFACE_DESTROY_KIND kind)
{
	CLock locker(m_CriticalSection_Surface);
	INT currentID = m_pSurfaceMgr->GetSurfaceIdInPool(iIndexInPool);
	if (currentID > iSurfaceID) {
		return CM_SUCCESS;
	}
	CmSurface2DUP_RT *pSurface_RT = static_cast<CmSurface2DUP_RT *>(pSurface);
	if(pSurface_RT == NULL) {
			return CM_FAILURE;
	}
	INT status = m_pSurfaceMgr->DestroySurface(pSurface_RT, kind);

	if (status == CM_SUCCESS) {
		pSurface = NULL;
	}
	return status;
}

INT CmDevice_RT::DestroySurface(CmSurface2D * &pSurface, INT iIndexInPool,
			     INT iSurfaceID, SURFACE_DESTROY_KIND kind)
{
	CLock locker(m_CriticalSection_Surface);

	INT currentID = m_pSurfaceMgr->GetSurfaceIdInPool(iIndexInPool);
	if (currentID > iSurfaceID) {
		return CM_SUCCESS;
	}
	CmSurface2D_RT *pSurface_RT = static_cast<CmSurface2D_RT *>(pSurface);
	INT status = m_pSurfaceMgr->DestroySurface(pSurface_RT, kind);

	if (status == CM_SUCCESS) {
		pSurface = NULL;
	}

	return status;
}

CM_RT_API INT CmDevice_RT::DestroySurface(CmSurface2D * &pSurface)
{
	CLock locker(m_CriticalSection_Surface);

	CmSurface2D_RT *pSurface_RT = static_cast<CmSurface2D_RT *>(pSurface);
	INT status = m_pSurfaceMgr->DestroySurface(pSurface_RT, APP_DESTROY);

	if (status != CM_FAILURE) {
		pSurface = NULL;
		return CM_SUCCESS;
	} else {
		return CM_FAILURE;
	}

	return status;
}

CM_RT_API INT CmDevice_RT::GetRTDllVersion(CM_DLL_FILE_VERSION* pFileVersion)
{
    if (pFileVersion)
    {
        pFileVersion->wMANVERSION   = m_RTDllVersion.wMANVERSION;
        pFileVersion->wMANREVISION  = m_RTDllVersion.wMANREVISION;
        pFileVersion->wSUBREVISION  = m_RTDllVersion.wSUBREVISION;
        pFileVersion->wBUILD_NUMBER = m_RTDllVersion.wBUILD_NUMBER;
        return CM_SUCCESS;
    }
    else
    {
        return CM_QUERY_DLL_VERSION_FAILURE;
    }
}

INT CmDevice_RT::GetJITCompileFnt(pJITCompile & fJITCompile)
{
	if (m_fJITCompile) {
		fJITCompile = m_fJITCompile;
	} else {
		if (!m_hJITDll) {
			m_hJITDll = dlopen(GetJitterName(), RTLD_LAZY);
			if (NULL == m_hJITDll) {
				CM_ASSERT(0);
				return CM_JITDLL_LOAD_FAILURE;
			}
		}

		m_fJITCompile =
		    (pJITCompile) GetProcAddress(m_hJITDll,
						 JITCOMPILE_FUNCTION_STR);
		if (NULL == m_fJITCompile) {
			CM_ASSERT(0);
			return CM_JITDLL_LOAD_FAILURE;
		}
		fJITCompile = m_fJITCompile;
	}
	return CM_SUCCESS;
}

INT CmDevice_RT::GetFreeBlockFnt(pFreeBlock & fFreeBlock)
{
	if (m_fFreeBlock) {
		fFreeBlock = m_fFreeBlock;
	} else {
		if (!m_hJITDll) {
			m_hJITDll = dlopen(GetJitterName(), RTLD_LAZY);
			if (NULL == m_hJITDll) {
				CM_ASSERT(0);
				return CM_JITDLL_LOAD_FAILURE;
			}
		}

		m_fFreeBlock =
		    (pFreeBlock) GetProcAddress(m_hJITDll,
						FREEBLOCK_FUNCTION_STR);
		if (NULL == m_fFreeBlock) {
			CM_ASSERT(0);
			return CM_JITDLL_LOAD_FAILURE;
		}
		fFreeBlock = m_fFreeBlock;
	}
	return CM_SUCCESS;
}

INT CmDevice_RT::GetJITVersionFnt(pJITVersion & fJITVersion)
{
	if (m_fJITVersion) {
		fJITVersion = m_fJITVersion;
	} else {
		if (!m_hJITDll) {
			m_hJITDll = dlopen(GetJitterName(), RTLD_LAZY);
			if (NULL == m_hJITDll) {
				CM_ASSERT(0);
				return CM_JITDLL_LOAD_FAILURE;
			}
		}

		m_fJITVersion =
		    (pJITVersion) GetProcAddress(m_hJITDll,
						 JITVERSION_FUNCTION_STR);
		if (NULL == m_fJITVersion) {
			CM_ASSERT(0);
			return CM_JITDLL_LOAD_FAILURE;
		}
		fJITVersion = m_fJITVersion;
	}
	return CM_SUCCESS;
}

INT CmDevice_RT::LoadJITDll(void)
{
	int result = 0;

	if (NULL == m_hJITDll) {
		m_hJITDll = dlopen(GetJitterName(), RTLD_LAZY);
		if (NULL == m_hJITDll) {
			result = CM_JITDLL_LOAD_FAILURE;
			CM_ASSERT(0);
			return result;
		}
		if (NULL == m_fJITCompile) {
			m_fJITCompile =
			    (pJITCompile) GetProcAddress(m_hJITDll,
							 JITCOMPILE_FUNCTION_STR);
			m_fFreeBlock =
			    (pFreeBlock) GetProcAddress(m_hJITDll,
							FREEBLOCK_FUNCTION_STR);
			m_fJITVersion =
			    (pJITVersion) GetProcAddress(m_hJITDll,
							 JITVERSION_FUNCTION_STR);
		}

		if ((NULL == m_fJITCompile) || (NULL == m_fFreeBlock)
		    || (NULL == m_fJITVersion)) {
			result = CM_JITDLL_LOAD_FAILURE;
			CM_ASSERT(0);
			return result;
		}
	}

	return result;
}

CM_RT_API INT CmDevice_RT::GetGenPlatform(UINT & platform)
{
	if (m_Platform != IGFX_UNKNOWN_CORE) {
		platform = m_Platform;
		return CM_SUCCESS;
	}

	platform = IGFX_UNKNOWN_CORE;

	INT hr = 0;
	DXVA_CM_QUERY_CAPS queryCaps;
	UINT querySize = sizeof(DXVA_CM_QUERY_CAPS);

	CmSafeMemSet(&queryCaps, 0, sizeof(queryCaps));
	queryCaps.Type = DXVA_CM_QUERY_GPU;

	hr = GetCapsInternal(&queryCaps, &querySize);
	if (FAILED(hr)) {
		CM_ASSERT(0);
		return CM_FAILURE;
	}
	if (queryCaps.iVersion) {
		platform = queryCaps.iVersion;
	}

	return CM_SUCCESS;
}

CM_RT_API INT
    CmDevice_RT::GetSurface2DInfo(UINT width, UINT height,
			       CM_SURFACE_FORMAT format, UINT & pitch,
			       UINT & physicalSize)
{
	CM_RETURN_CODE hr = CM_SUCCESS;
	CM_HAL_SURFACE2D_UP_PARAM inParam;
	PCM_CONTEXT pCmData;
	PCM_HAL_STATE pCmHalState;

	CMCHK_HR(m_pSurfaceMgr->Surface2DSanityCheck(width, height, format));

	CmSafeMemSet(&inParam, 0, sizeof(CM_HAL_SURFACE2D_UP_PARAM));
	inParam.iWidth = width;
	inParam.iHeight = height;
	inParam.format = m_pSurfaceMgr->CmFmtToGenHwFmt(format);

	pCmData = (PCM_CONTEXT) GetAccelData();
	pCmHalState = pCmData->pCmHalState;
	CHK_GENOSSTATUS_RETURN_CMERROR(pCmHalState->pfnGetSurface2DPitchAndSize
				       (pCmHalState, &inParam));

	pitch = inParam.iPitch;
	physicalSize = inParam.iPhysicalSize;

 finish:
	return hr;
}

INT CmDevice_RT::CreateQueue_Internel(void)
{
	if (m_pQueue) {
		CM_ASSERTMESSAGE("Failed to create more than one queue.");
		return CM_FAILURE;
	}

	INT result = CmQueue_RT::Create(this, m_pQueue);
	if (result != CM_SUCCESS) {
		CM_ASSERTMESSAGE("Failed to create queue.");
		return CM_FAILURE;
	}

	return result;
}

INT CmDevice_RT::GetSurfaceManager(CmSurfaceManager * &pSurfaceMgr)
{
	pSurfaceMgr = m_pSurfaceMgr;
	return CM_SUCCESS;
}

CSync *CmDevice_RT::GetSurfaceLock()
{
	return &m_CriticalSection_ReadWriteSurface2D;
}

CSync *CmDevice_RT::GetSurfaceCreationLock()
{
	return &m_CriticalSection_Surface;
}

CSync *CmDevice_RT::GetProgramKernelLock()
{
	return &m_CriticalSection_Program_Kernel;
}

INT CmDevice_RT::GetQueue(CmQueue_RT * &pQueue)
{
	pQueue = m_pQueue;
	return CM_SUCCESS;
}

INT CmDevice_RT::GetHalMaxValues(CM_HAL_MAX_VALUES * &pHalMaxValues,
			      CM_HAL_MAX_VALUES_EX * &pHalMaxValuesEx)
{
	pHalMaxValues = &m_HalMaxValues;
	pHalMaxValuesEx = &m_HalMaxValuesEx;

	return CM_SUCCESS;
}

INT CmDevice_RT::GetMaxValueFromCaps(CM_HAL_MAX_VALUES & MaxValues,
				  CM_HAL_MAX_VALUES_EX & MaxValuesEx)
{
	DXVA_CM_QUERY_CAPS queryCaps;
	UINT querySize = sizeof(DXVA_CM_QUERY_CAPS);
	CmSafeMemSet(&queryCaps, 0, sizeof(DXVA_CM_QUERY_CAPS));
	queryCaps.Type = DXVA_CM_MAX_VALUES;

	INT hr = GetCapsInternal(&queryCaps, &querySize);
	if (FAILED(hr)) {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

	MaxValues = queryCaps.MaxValues;
	MaxValues.iMaxArgsPerKernel =
	    (queryCaps.MaxValues.iMaxArgsPerKernel >
	     CM_MAX_ARGS_PER_KERNEL) ? (CM_MAX_ARGS_PER_KERNEL) :
	    queryCaps.MaxValues.iMaxArgsPerKernel;

	CmSafeMemSet(&queryCaps, 0, sizeof(DXVA_CM_QUERY_CAPS));
	queryCaps.Type = DXVA_CM_MAX_VALUES_EX;

	hr = GetCapsInternal(&queryCaps, &querySize);
	if (FAILED(hr)) {
		CM_ASSERT(0);
		return CM_FAILURE;
	}
	MaxValuesEx = queryCaps.MaxValuesEx;

	return CM_SUCCESS;
}

INT CmDevice_RT::GetCapsInternal(PVOID pCaps, PUINT puSize)
{
	PDXVA_CM_QUERY_CAPS pQueryCaps;
	PCM_CONTEXT pCmData;
	PCM_HAL_STATE pCmHalState;

	CM_RETURN_CODE hr = CM_SUCCESS;

	if ((!puSize) || (!pCaps) || (*puSize < sizeof(DXVA_CM_QUERY_CAPS))) {
		CM_ASSERTMESSAGE("Invalid Arguments.");
		hr = CM_FAILURE;
		goto finish;
	}

	pQueryCaps = (PDXVA_CM_QUERY_CAPS) pCaps;
	*puSize = sizeof(DXVA_CM_QUERY_CAPS);

	if (pQueryCaps->Type == DXVA_CM_QUERY_VERSION) {
		pQueryCaps->iVersion = DXVA_CM_VERSION;
		hr = CM_SUCCESS;
		goto finish;
	}

	pCmData = (PCM_CONTEXT) GetAccelData();
	CMCHK_NULL(pCmData);

	pCmHalState = pCmData->pCmHalState;
	CMCHK_NULL(pCmHalState);

	switch (pQueryCaps->Type) {
	case DXVA_CM_QUERY_REG_HANDLE:
		pQueryCaps->hRegistration =
		    (HANDLE *) & pCmHalState->SurfaceRegTable;
		break;

	case DXVA_CM_MAX_VALUES:
		CHK_GENOSSTATUS_RETURN_CMERROR(pCmHalState->pfnGetMaxValues
					       (pCmHalState,
						&pQueryCaps->MaxValues));
		break;

	case DXVA_CM_MAX_VALUES_EX:
		CHK_GENOSSTATUS_RETURN_CMERROR(pCmHalState->pfnGetMaxValuesEx
					       (pCmHalState,
						&pQueryCaps->MaxValuesEx));
		break;

	case DXVA_CM_QUERY_GPU:
		pQueryCaps->genCore =
		    pCmHalState->pHwInterface->Platform.eRenderCoreFamily;
		break;

	case DXVA_CM_QUERY_GT:
		if (GFX_IS_PRODUCT
		    (pCmHalState->pHwInterface->Platform, IGFX_CHERRYVIEW)) {
			pQueryCaps->genGT = PLATFORM_INTEL_GTCHV;
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GT1) {
			pQueryCaps->genGT = PLATFORM_INTEL_GT1;
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GT1_5) {
			pQueryCaps->genGT = PLATFORM_INTEL_GT1_5;
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GT2) {
			pQueryCaps->genGT = PLATFORM_INTEL_GT2;
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GT3) {
			pQueryCaps->genGT = PLATFORM_INTEL_GT3;
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GT4) {
			pQueryCaps->genGT = PLATFORM_INTEL_GT4;
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GTA) {
			pQueryCaps->genGT = PLATFORM_INTEL_GTA;  //BXT-GTA
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GTC) {
			pQueryCaps->genGT = PLATFORM_INTEL_GTC; //BXT-GTC
		} else if (pCmHalState->pHwInterface->Platform.GtType ==
			   GTTYPE_GTX) {
			pQueryCaps->genGT = PLATFORM_INTEL_GTX; //BXT-GTX
		}
		break;

	case DXVA_CM_QUERY_STEP:
		pQueryCaps->genStepId = pCmHalState->Platform.usRevId;
		break;

	case DXVA_CM_QUERY_GPU_FREQ:
		CHK_GENOSSTATUS_RETURN_CMERROR
		    (pCmHalState->pfnGetGPUCurrentFrequency
		     (pCmHalState, &pQueryCaps->GPUCurrentFreq));
		break;

	case DXVA_CM_QUERY_SURFACE2D_FORMAT_COUNT:
		pQueryCaps->Surface2DCount =
		    CM_MAX_SURFACE2D_FORMAT_COUNT_INTERNAL;
		break;

	case DXVA_CM_QUERY_SURFACE2D_FORMATS:
		if (pQueryCaps->pSurface2DFormats) {
			CM_SURFACE_FORMAT
			    formats[CM_MAX_SURFACE2D_FORMAT_COUNT_INTERNAL] = {
			CM_SURFACE_FORMAT_X8R8G8B8,
				    CM_SURFACE_FORMAT_A8R8G8B8,
				    CM_SURFACE_FORMAT_R32F,
				    CM_SURFACE_FORMAT_V8U8,
				    CM_SURFACE_FORMAT_P8,
				    CM_SURFACE_FORMAT_YUY2,
				    CM_SURFACE_FORMAT_A8,
				    CM_SURFACE_FORMAT_NV12,
				    CM_SURFACE_FORMAT_UYVY,
				    CM_SURFACE_FORMAT_R8_UINT,
				    CM_SURFACE_FORMAT_R16_UINT,
				    CM_SURFACE_FORMAT_411P,
				    CM_SURFACE_FORMAT_422H,
				    CM_SURFACE_FORMAT_444P,
				    CM_SURFACE_FORMAT_IMC3,
				    CM_SURFACE_FORMAT_422V,
				    CM_SURFACE_FORMAT_YV12,};
			CmSafeMemCopy(pQueryCaps->pSurface2DFormats, formats,
				      CM_MAX_SURFACE2D_FORMAT_COUNT_INTERNAL *
				      sizeof(CM_SURFACE_FORMAT));
			break;
		} else {
			hr = CM_FAILURE;
			goto finish;
		}

	default:
		hr = CM_FAILURE;
		goto finish;
	}

 finish:
	return hr;
}

CM_RT_API INT
    CmDevice_RT::GetCaps(CM_DEVICE_CAP_NAME capName, size_t & capValueSize,
		      void *pCapValue)
{
	PCM_CONTEXT pCmData;
	PCM_HAL_STATE pCmHalState;

	if (pCapValue == NULL) {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

	pCmData = (PCM_CONTEXT) GetAccelData();
	if (pCmData == NULL) {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

	pCmHalState = pCmData->pCmHalState;
	if (pCmHalState == NULL) {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

	switch (capName) {
	case CAP_KERNEL_COUNT_PER_TASK:
		if (capValueSize >= sizeof(m_HalMaxValues.iMaxKernelsPerTask)) {
			capValueSize =
			    sizeof(m_HalMaxValues.iMaxKernelsPerTask);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMaxKernelsPerTask,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_KERNEL_BINARY_SIZE:
		if (capValueSize >= sizeof(m_HalMaxValues.iMaxKernelBinarySize)) {
			capValueSize =
			    sizeof(m_HalMaxValues.iMaxKernelBinarySize);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMaxKernelBinarySize,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_BUFFER_COUNT:
		if (capValueSize >= sizeof(m_HalMaxValues.iMaxBufferTableSize)) {
			capValueSize =
			    sizeof(m_HalMaxValues.iMaxBufferTableSize);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMaxBufferTableSize,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_SURFACE2D_COUNT:
		if (capValueSize >=
		    sizeof(m_HalMaxValues.iMax2DSurfaceTableSize)) {
			capValueSize =
			    sizeof(m_HalMaxValues.iMax2DSurfaceTableSize);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMax2DSurfaceTableSize,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_SURFACE_COUNT_PER_KERNEL:
		if (capValueSize >=
		    sizeof(m_HalMaxValues.iMaxSurfacesPerKernel)) {
			capValueSize =
			    sizeof(m_HalMaxValues.iMaxSurfacesPerKernel);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMaxSurfacesPerKernel,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_ARG_COUNT_PER_KERNEL:
		if (capValueSize >= sizeof(m_HalMaxValues.iMaxArgsPerKernel)) {
			capValueSize = sizeof(m_HalMaxValues.iMaxArgsPerKernel);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMaxArgsPerKernel,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_ARG_SIZE_PER_KERNEL:
		if (capValueSize >=
		    sizeof(m_HalMaxValues.iMaxArgByteSizePerKernel)) {
			capValueSize =
			    sizeof(m_HalMaxValues.iMaxArgByteSizePerKernel);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMaxArgByteSizePerKernel,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_USER_DEFINED_THREAD_COUNT_PER_TASK:
		if (capValueSize >=
		    sizeof(m_HalMaxValues.iMaxUserThreadsPerTask)) {
			capValueSize =
			    sizeof(m_HalMaxValues.iMaxUserThreadsPerTask);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.iMaxUserThreadsPerTask,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_USER_DEFINED_THREAD_COUNT_PER_MEDIA_WALKER:
		if (capValueSize >=
		    sizeof(m_HalMaxValuesEx.iMaxUserThreadsPerMediaWalker)) {
			capValueSize =
			    sizeof
			    (m_HalMaxValuesEx.iMaxUserThreadsPerMediaWalker);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValuesEx.
				      iMaxUserThreadsPerMediaWalker,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_USER_DEFINED_THREAD_COUNT_PER_THREAD_GROUP:
		if (capValueSize >=
		    sizeof(m_HalMaxValuesEx.iMaxUserThreadsPerThreadGroup)) {
			capValueSize =
			    sizeof
			    (m_HalMaxValuesEx.iMaxUserThreadsPerThreadGroup);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValuesEx.
				      iMaxUserThreadsPerThreadGroup,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_USER_DEFINED_THREAD_COUNT_PER_TASK_NO_THREAD_ARG:
		if (capValueSize >=
		    sizeof(m_HalMaxValues.iMaxUserThreadsPerTaskNoThreadArg)) {
			capValueSize =
			    sizeof
			    (m_HalMaxValues.iMaxUserThreadsPerTaskNoThreadArg);
			CmSafeMemCopy(pCapValue,
				      &m_HalMaxValues.
				      iMaxUserThreadsPerTaskNoThreadArg,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_HW_THREAD_COUNT:
		if (capValueSize >= sizeof(m_HalMaxValues.iMaxHwThreads)) {
			capValueSize = sizeof(m_HalMaxValues.iMaxHwThreads);
			CmSafeMemCopy(pCapValue, &m_HalMaxValues.iMaxHwThreads,
				      capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_SURFACE2D_FORMAT_COUNT:
		if (capValueSize >= sizeof(UINT)) {
			capValueSize = sizeof(UINT);
			UINT formatCount = CM_MAX_SURFACE2D_FORMAT_COUNT;
			CmSafeMemCopy(pCapValue, &formatCount, capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_SURFACE2D_FORMATS:
		if (capValueSize >=
		    CM_MAX_SURFACE2D_FORMAT_COUNT * sizeof(CM_SURFACE_FORMAT)) {
			capValueSize =
			    CM_MAX_SURFACE2D_FORMAT_COUNT *
			    sizeof(CM_SURFACE_FORMAT);
			CM_SURFACE_FORMAT formats[CM_MAX_SURFACE2D_FORMAT_COUNT]
			    = {
				CM_SURFACE_FORMAT_X8R8G8B8,
				CM_SURFACE_FORMAT_A8R8G8B8,
				CM_SURFACE_FORMAT_R32F,
				CM_SURFACE_FORMAT_V8U8,
				CM_SURFACE_FORMAT_P8,
				CM_SURFACE_FORMAT_YUY2,
				CM_SURFACE_FORMAT_A8,
				CM_SURFACE_FORMAT_NV12,
				CM_SURFACE_FORMAT_UYVY,
				CM_SURFACE_FORMAT_R8_UINT,
				CM_SURFACE_FORMAT_R16_UINT,
				CM_SURFACE_FORMAT_411P,
				CM_SURFACE_FORMAT_422H,
				CM_SURFACE_FORMAT_444P,
				CM_SURFACE_FORMAT_IMC3,
				CM_SURFACE_FORMAT_422V,
				CM_SURFACE_FORMAT_YV12,
			};
			CmSafeMemCopy(pCapValue, formats, capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_GPU_PLATFORM:
		if (capValueSize >= sizeof(UINT)) {
			UINT platform = PLATFORM_INTEL_UNKNOWN;
			capValueSize = sizeof(UINT);
			switch (m_Platform) {
			case IGFX_GEN7_5_CORE:
				platform = PLATFORM_INTEL_HSW;
				break;

			case IGFX_GEN8_CORE:
				if (GFX_IS_PRODUCT
				    (pCmHalState->pHwInterface->Platform,
				     IGFX_CHERRYVIEW)) {
					platform = PLATFORM_INTEL_CHV;
				} else {
					platform = PLATFORM_INTEL_BDW;
				}
				break;

			case IGFX_GEN9_CORE:
				if (GFX_IS_PRODUCT
				    (pCmHalState->pHwInterface->Platform,
				     IGFX_BROXTON)) {
					//change to PLATFORM_INTEL_BXT soon...
					platform = PLATFORM_INTEL_SKL;
				} else {
					platform = PLATFORM_INTEL_SKL;
				}
				break;
			}

			CmSafeMemCopy(pCapValue, &platform, capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_GT_PLATFORM:
		if (capValueSize >= sizeof(UINT)) {
			DXVA_CM_QUERY_CAPS queryCaps;
			queryCaps.Type = DXVA_CM_QUERY_GT;
			UINT queryCapsSize = sizeof(DXVA_CM_QUERY_CAPS);
			GetCapsInternal(&queryCaps, &queryCapsSize);
			capValueSize = sizeof(UINT);
			UINT gtPlatform = queryCaps.genGT;
			CmSafeMemCopy(pCapValue, &gtPlatform, capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}
	case CAP_MIN_FREQUENCY:
		if (capValueSize >= sizeof(UINT)) {
			DXVA_CM_QUERY_CAPS queryCaps;
			queryCaps.Type = DXVA_CM_MIN_RENDER_FREQ;
			UINT queryCapsSize = sizeof(DXVA_CM_QUERY_CAPS);
			GetCapsInternal(&queryCaps, &queryCapsSize);
			UINT frequency = queryCaps.MinRenderFreq;
			capValueSize = sizeof(UINT);
			CmSafeMemCopy(pCapValue, &frequency, capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_MAX_FREQUENCY:
		if (capValueSize >= sizeof(UINT)) {
			DXVA_CM_QUERY_CAPS queryCaps;
			queryCaps.Type = DXVA_CM_MAX_RENDER_FREQ;
			UINT queryCapsSize = sizeof(DXVA_CM_QUERY_CAPS);
			GetCapsInternal(&queryCaps, &queryCapsSize);
			UINT frequency = queryCaps.MaxRenderFreq;
			capValueSize = sizeof(UINT);
			CmSafeMemCopy(pCapValue, &frequency, capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	case CAP_GPU_CURRENT_FREQUENCY:
		if (capValueSize >= sizeof(UINT)) {
			DXVA_CM_QUERY_CAPS queryCaps;
			queryCaps.Type = DXVA_CM_QUERY_GPU_FREQ;
			UINT queryCapsSize = sizeof(DXVA_CM_QUERY_CAPS);
			GetCapsInternal(&queryCaps, &queryCapsSize);
			UINT frequency = queryCaps.GPUCurrentFreq;
			capValueSize = sizeof(UINT);
			CmSafeMemCopy(pCapValue, &frequency, capValueSize);
			return CM_SUCCESS;
		} else {
			return CM_FAILURE;
		}

	default:
		return CM_FAILURE;
	}
}

CM_RT_API INT
    CmDevice_RT::LoadProgram(void *pCommonISACode, const UINT size,
			  CmProgram * &pProgram, const char *options)
{
	INT result;

	if ((pCommonISACode == NULL) || (size == 0)) {
		CM_ASSERT(0);
		return CM_INVALID_COMMON_ISA;
	}

	CLock locker(m_CriticalSection_Program_Kernel);

	UINT firstfreeslot = m_ProgramArray.GetFirstFreeIndex();

	CmProgram_RT *pProgram_RT = NULL;
	result =
	    CmProgram_RT::Create(this, pCommonISACode, size, NULL, 0, pProgram_RT,
			      options, firstfreeslot);
	if (result == CM_SUCCESS) {
		m_ProgramArray.SetElement(firstfreeslot, pProgram_RT);
		m_ProgramCount++;
		pProgram = (static_cast<CmProgram *>(pProgram_RT));
	}
	return result;
}

CM_RT_API INT CmDevice_RT::DestroyProgram(CmProgram * &pProgram)
{
	if (pProgram == NULL) {
		return CM_FAILURE;
	}

	CLock locker(m_CriticalSection_Program_Kernel);
        CmProgram_RT * pProgram_RT = (static_cast<CmProgram_RT* >(pProgram));
	UINT indexInProgramArrary = pProgram_RT->GetProgramIndex();

	if (pProgram_RT == m_ProgramArray.GetElement(indexInProgramArrary)) {
		CmProgram_RT::Destroy(pProgram_RT);
		if (pProgram_RT == NULL) {
			m_ProgramArray.SetElement(indexInProgramArrary, NULL);
		}
		return CM_SUCCESS;
	} else {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

}

CM_RT_API INT
    CmDevice_RT::CreateKernel(CmProgram * pProgram, const char *kernelName,
			   CmKernel * &pKernel, const char *options)
{
	if (pProgram == NULL) {
		CM_ASSERT(0);
		return CM_INVALID_ARG_VALUE;
	}

	CLock locker(m_CriticalSection_Program_Kernel);

	UINT freeSlotInKernelArray = m_KernelArray.GetFirstFreeIndex();
	CmKernel_RT *pKernel_RT=NULL;
        CmProgram_RT* pProgram_RT = static_cast<CmProgram_RT*>(pProgram);
	INT result =
	    CmKernel_RT::Create(this, pProgram_RT, kernelName, freeSlotInKernelArray,
			     m_KernelCount, pKernel_RT, options);
	if (result == CM_SUCCESS) {
		m_KernelArray.SetElement(freeSlotInKernelArray, pKernel_RT);
		m_KernelCount++;
		pKernel = static_cast< CmKernel * >(pKernel_RT);
	}

	return result;
}

CM_RT_API INT CmDevice_RT::DestroyKernel(CmKernel * &pKernel)
{
	if (pKernel == NULL) {
		return CM_FAILURE;
	}

	CLock locker(m_CriticalSection_Program_Kernel);
    CmKernel_RT *pKernel_RT=static_cast<CmKernel_RT*>(pKernel);

	UINT indexInKernelArrary = pKernel_RT->GetKernelIndex();
    CmProgram_RT *pProgram_RT = NULL;
	if (pKernel_RT == m_KernelArray.GetElement(indexInKernelArrary)) {
	    pKernel_RT->GetCmProgram(pProgram_RT);
		if (pProgram_RT == NULL) {
			CM_ASSERT(0);
			return CM_FAILURE;
		}

		UINT indexInProgramArray = pProgram_RT->GetProgramIndex();

		if (pProgram_RT == m_ProgramArray.GetElement(indexInProgramArray)) {
			CmKernel_RT::Destroy(pKernel_RT, pProgram_RT);

			if (pKernel_RT == NULL) {
				m_KernelArray.SetElement(indexInKernelArrary,
							 NULL);
			}

			if (pProgram_RT == NULL) {
				m_ProgramArray.SetElement(indexInProgramArray,
							  NULL);
			}
			return CM_SUCCESS;
		} else {
			CM_ASSERT(0);
			return CM_FAILURE;
		}
	} else {
		CM_ASSERT(0);
		return CM_FAILURE;
	}
	return CM_SUCCESS;
}

CM_RT_API INT CmDevice_RT::CreateQueue(CmQueue * &pQueue)
{
	pQueue = m_pQueue;
	return CM_SUCCESS;
}

CM_RT_API INT CmDevice_RT::CreateTask(CmTask * &pTask)
{
	CLock locker(m_CriticalSection_Task);

	CmTask_RT* ptask_RT = NULL;
	UINT freeSlotInTaskArray = m_TaskArray.GetFirstFreeIndex();
	INT result = CmTask_RT::Create(this, freeSlotInTaskArray,
				    m_HalMaxValues.iMaxKernelsPerTask, ptask_RT);
	if (result == CM_SUCCESS) {
	    pTask = static_cast< CmTask* >(ptask_RT);
	    m_TaskArray.SetElement(freeSlotInTaskArray, pTask);
	    m_TaskCount++;
	}
	return result;
}

INT CmDevice_RT::DestroyQueue(CmQueue_RT * &pQueue)
{
	if (pQueue == NULL) {
		return CM_FAILURE;
	}

	return CmQueue_RT::Destroy(pQueue);
}

CM_RT_API INT CmDevice_RT::DestroyTask(CmTask * &pTask)
{

	CLock locker(m_CriticalSection_Task);

	if (pTask == NULL) {
		return CM_FAILURE;
	}

        CmTask_RT* pTask_RT = static_cast< CmTask_RT* >(pTask);
        if ( pTask_RT == NULL ) {
            return CM_FAILURE;
        }
	UINT index = pTask_RT->GetIndexInTaskArray();

	if (pTask == (CmTask_RT *) m_TaskArray.GetElement(index)) {
		INT status = CmTask_RT::Destroy(pTask_RT);
		if (status == CM_SUCCESS) {
			m_TaskArray.SetElement(index, NULL);
			pTask = NULL;
			return CM_SUCCESS;
		} else {
			CM_ASSERT(0);
			return status;
		}
	} else {
		CM_ASSERT(0);
		return CM_FAILURE;
	}
}

CM_RT_API INT
    CmDevice_RT::CreateThreadSpace(UINT width, UINT height, CmThreadSpace * &pTS)
{
	CLock locker(m_CriticalSection_ThreadSpace);

	UINT freeSlotInThreadSpaceArray =
	    m_ThreadSpaceArray.GetFirstFreeIndex();
	INT result =
	    CmThreadSpace::Create(this, freeSlotInThreadSpaceArray, width,
				  height,
				  pTS);
	if (result == CM_SUCCESS) {
		m_ThreadSpaceArray.SetElement(freeSlotInThreadSpaceArray, pTS);
		m_ThreadSpaceCount++;
	}

	return result;
}

CM_RT_API INT CmDevice_RT::DestroyThreadSpace(CmThreadSpace * &pTS)
{
	if (pTS == NULL) {
		return CM_FAILURE;
	}

	UINT indexTs = pTS->GetIndexInTsArray();

	CLock locker(m_CriticalSection_ThreadSpace);
	if (pTS == m_ThreadSpaceArray.GetElement(indexTs)) {
		INT status = CmThreadSpace::Destroy(pTS);
		if (status == CM_SUCCESS) {
			m_ThreadSpaceArray.SetElement(indexTs, NULL);
			pTS = NULL;
			return CM_SUCCESS;
		} else {
			CM_ASSERT(0);
			return status;
		}
	} else {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

}

INT CmDevice_RT::GetDDIVersion(UINT & DDIVersion)
{
	DDIVersion = m_DDIVersion;
	return CM_SUCCESS;
}

CM_RT_API INT
    CmDevice_RT::CreateThreadGroupSpace(UINT thrdSpaceWidth, UINT thrdSpaceHeight,
				     UINT grpSpaceWidth, UINT grpSpaceHeight,
				     CmThreadGroupSpace * &pTGS)
{
	CLock locker(m_CriticalSection_ThreadGroupSpace);

	UINT firstfreeslot = m_ThreadGroupSpaceArray.GetFirstFreeIndex();
	INT result =
	    CmThreadGroupSpace_RT::Create(this, firstfreeslot, thrdSpaceWidth,
				       thrdSpaceHeight, grpSpaceWidth,
				       grpSpaceHeight, pTGS);
	if (result == CM_SUCCESS) {
		m_ThreadGroupSpaceArray.SetElement(firstfreeslot, pTGS);
		m_ThreadGroupSpaceCount++;
	}
	return result;
}

CM_RT_API INT CmDevice_RT::DestroyThreadGroupSpace(CmThreadGroupSpace * &pTGS)
{
	if (pTGS == NULL) {
		return CM_FAILURE;
	}

	UINT indexTGs = static_cast<CmThreadGroupSpace_RT*>(pTGS)->GetIndexInTGsArray();

	CLock locker(m_CriticalSection_ThreadGroupSpace);

	if (pTGS == static_cast <
	    CmThreadGroupSpace *
	    >(m_ThreadGroupSpaceArray.GetElement(indexTGs))) {
		INT status = CmThreadGroupSpace_RT::Destroy(pTGS);
		if (status == CM_SUCCESS) {
			m_ThreadGroupSpaceArray.SetElement(indexTGs, NULL);
			pTGS = NULL;
			return CM_SUCCESS;
		}
	} else {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

	return CM_FAILURE;
}

INT CmDevice_RT::GetGenStepInfo(UINT platform, char *&stepinfostr)
{
	INT hr;

	const char *CmSteppingInfo[MAX_STEPPING_NUM] =
	{
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J"
	};

	DXVA_CM_QUERY_CAPS queryCaps;
	CmSafeMemSet(&queryCaps, 0, sizeof(queryCaps));
	queryCaps.Type = DXVA_CM_QUERY_STEP;
	UINT queryCapsSize = sizeof(queryCaps);

	if (platform < IGFX_GEN7_5_CORE) {
		stepinfostr = NULL;
		return CM_SUCCESS;
	}

	hr = GetCapsInternal(&queryCaps, &queryCapsSize);
	if (FAILED(hr)) {
		CM_ASSERT(0);
		return CM_FAILURE;
	}

	UINT stepid = queryCaps.genStepId;
	UINT ulStepId = (1 << stepid);

	if (platform < IGFX_GEN9_CORE)
	{
		switch (ulStepId) {
		case SIWA_ONLY_BDW_A0:
			stepinfostr = (char *)HW_GT_STEPPING_A0;
			break;

		case SIWA_ONLY_HSW_A1:
			stepinfostr = (char *)HW_GT_STEPPING_A1;
			break;

		case SIWA_ONLY_HSW_B0:
			stepinfostr = (char *)HW_GT_STEPPING_B0;
			break;

		case SIWA_ONLY_HSW_C0:
			stepinfostr = (char *)HW_GT_STEPPING_C0;
			break;

		default:
			stepinfostr = NULL;
		}
	}
	else if (stepid < MAX_STEPPING_NUM)
	{
		stepinfostr = (char*)CmSteppingInfo[stepid];
	}
	else
	{
		stepinfostr = NULL;
	}

	return CM_SUCCESS;
}

CM_RT_API INT CmDevice_RT::SetSuggestedL3Config( L3_SUGGEST_CONFIG l3_s_c)
{
    INT upper_bound;
    const L3_CONFIG_REGISTER_VALUES *l3_c;

    switch (m_Platform)
    {
        case IGFX_GEN6_CORE:
            return CM_FAILURE;

        case IGFX_GEN7_CORE:
            upper_bound = HSW_L3_CONFIG_NUM;
            l3_c = &IVB_L3_PLANE[0];
            break;

        case IGFX_GEN7_5_CORE:
            upper_bound = HSW_L3_CONFIG_NUM;
            l3_c = HSW_L3_PLANE;
            break;

        case IGFX_GEN8_CORE:
            upper_bound = BDW_L3_CONFIG_NUM;
            l3_c = BDW_L3_PLANE;
            break;

	case IGFX_GEN9_CORE:
            upper_bound = SKL_L3_CONFIG_NUM;
            l3_c = SKL_L3_PLANE;
            break;

	default:
            return CM_FAILURE;
    }

    if (l3_s_c>=  upper_bound || l3_s_c< 0)
        return CM_FAILURE;
;
    m_l3_c.SQCREG1_VALUE  = l3_c[l3_s_c].SQCREG1_VALUE;
    m_l3_c.CNTLREG2_VALUE = l3_c[l3_s_c].CNTLREG2_VALUE;
    m_l3_c.CNTLREG3_VALUE = l3_c[l3_s_c].CNTLREG3_VALUE;
    m_l3_c.CNTLREG_VALUE = l3_c[ l3_s_c ].CNTLREG_VALUE;
    SetCapsInternal(CAP_L3_CONFIG, sizeof(L3_CONFIG_REGISTER_VALUES), &m_l3_c);
    return CM_SUCCESS;
}

INT CmDevice_RT::SetCapsInternal(CM_DEVICE_CAP_NAME capName, size_t capValueSize,
			      void *pCapValue)
{
	CM_RETURN_CODE hr = CM_SUCCESS;

	DXVA_CM_SET_CAPS setCaps;
	UINT maxValue;
	size_t size = sizeof(maxValue);
	CmSafeMemSet(&setCaps, 0, sizeof(setCaps));

	switch (capName) {
	case CAP_HW_THREAD_COUNT:
		if (capValueSize != sizeof(UINT)) {
			CM_ASSERT(0);
			return CM_INVALID_HARDWARE_THREAD_NUMBER;
		}

		if (*(UINT *) pCapValue <= 0) {
			CM_ASSERT(0);
			return CM_INVALID_HARDWARE_THREAD_NUMBER;
		}

		GetCaps(CAP_HW_THREAD_COUNT, size, &maxValue);
		if (*(UINT *) pCapValue > maxValue) {
			CM_ASSERT(0);
			return CM_INVALID_HARDWARE_THREAD_NUMBER;
		}

		setCaps.Type = DXVA_CM_MAX_HW_THREADS;
		setCaps.MaxValue = *(UINT *) pCapValue;
		break;

	case CAP_L3_CONFIG:
            if (capValueSize != sizeof(L3_CONFIG_REGISTER_VALUES)){
                CM_ASSERT(0);
                return CM_INVALIDE_L3_CONFIGURATION;
               }
            else {
                L3_CONFIG_REGISTER_VALUES *l3_c = (L3_CONFIG_REGISTER_VALUES *)pCapValue;
                setCaps.L3_SQCREG1 = l3_c->SQCREG1_VALUE;
                setCaps.L3_CNTLREG2 = l3_c->CNTLREG2_VALUE;
                setCaps.L3_CNTLREG3 = l3_c->CNTLREG3_VALUE;
                setCaps.L3_CNTLREG = l3_c->CNTLREG_VALUE;
                setCaps.Type = DXVA_CM_MAX_HW_L3_CONFIG;
              }
        break;

	default:
		return CM_FAILURE;
	}

	PCM_CONTEXT pCmData = (PCM_CONTEXT) this->GetAccelData();
	CHK_GENOSSTATUS_RETURN_CMERROR(pCmData->pCmHalState->
				       pfnSetCaps(pCmData->pCmHalState,
						  (PCM_HAL_MAX_SET_CAPS_PARAM)
						  & setCaps));

 finish:
	return hr;
}

INT CmDevice_RT::GetSurf2DLookUpEntry(UINT index, PCMLOOKUP_ENTRY & pLookupEntry)
{
	PCM_CONTEXT pCmData = (PCM_CONTEXT) GetAccelData();
	if (pCmData) {
		pLookupEntry = &(pCmData->pCmHalState->pSurf2DTable[index]);
	} else {
		return CM_FAILURE;
	}

	return CM_SUCCESS;
}

INT CmDevice_RT::LoadProgramWithGenCode(void *pCISACode, const UINT uiCISACodeSize,
				     void *pGenCode, const UINT uiGenCodeSize,
				     CmProgram_RT * &pProgram, const char *options)
{
	INT result;
	CLock locker(m_CriticalSection_Program_Kernel);

	UINT firstfreeslot = m_ProgramArray.GetFirstFreeIndex();
	result =
	    CmProgram_RT::Create(this, pCISACode, uiCISACodeSize, pGenCode,
			      uiGenCodeSize, pProgram, options, firstfreeslot);
	if (result == CM_SUCCESS) {
		m_ProgramArray.SetElement(firstfreeslot, pProgram);
		m_ProgramCount++;
	}
	return result;
}

INT CmDevice_RT::GetSurface2DInPool(UINT width, UINT height,
				 CM_SURFACE_FORMAT format,
				 CmSurface2D * &pSurface)
{
	CLock locker(m_CriticalSection_Surface);

	CmSurface2D_RT *pSurface_RT = NULL;
	INT result =
	    m_pSurfaceMgr->GetSurface2dInPool(width, height, format, pSurface_RT);
	pSurface=static_cast<CmSurface2D *>(pSurface_RT);
	return result;
}

INT CmDevice_RT::GetSurfaceIDInPool(INT iIndex)
{
	CLock locker(m_CriticalSection_Surface);
	INT result = m_pSurfaceMgr->GetSurfaceIdInPool(iIndex);
	return result;
}

INT CmDevice_RT::DestroySurfaceInPool(UINT & freeSurfNum)
{
	CLock locker(m_CriticalSection_Surface);

	freeSurfNum = m_pSurfaceMgr->TouchSurfaceInPoolForDestroy();
	if ((INT) freeSurfNum < 0) {
		freeSurfNum = 0;
		return CM_FAILURE;
	}

	return CM_SUCCESS;
}

INT CmDevice_RT::InitDevCreateOption(CM_HAL_CREATE_PARAM & DevCreateParam,
				  UINT DevCreateOption)
{
	UINT MaxTaskNumber = 0;

	DevCreateParam.DisableScratchSpace =
	    (DevCreateOption & CM_DEVICE_CREATE_OPTION_SCRATCH_SPACE_MASK);

	DevCreateParam.EnableSurfaceReuse =
	    (DevCreateOption & CM_DEVICE_CREATE_OPTION_SURFACE_REUSE_ENABLE);

	if (DevCreateParam.DisableScratchSpace) {
		DevCreateParam.ScratchSpaceSize = 0;
	} else {
		DevCreateParam.ScratchSpaceSize =
		    (DevCreateOption & CM_DEVICE_CONFIG_SCRATCH_SPACE_SIZE_MASK)
		    >> CM_DEVICE_CONFIG_SCRATCH_SPACE_SIZE_OFFSET;
	}

	MaxTaskNumber =
	    (DevCreateOption & CM_DEVICE_CONFIG_TASK_NUM_MASK) >>
	    CM_DEVICE_CONFIG_TASK_NUM_OFFSET;

	DevCreateParam.MaxTaskNumber =
	    (MaxTaskNumber + 1) * CM_DEVICE_CONFIG_TASK_NUM_STEP;

	DevCreateParam.bMediaReset = FALSE;

	MaxTaskNumber =
	    (DevCreateOption & CM_DEVICE_CONFIG_EXTRA_TASK_NUM_MASK) >>
	    CM_DEVICE_CONFIG_EXTRA_TASK_NUM_OFFSET;

	DevCreateParam.MaxTaskNumber =
	    (MaxTaskNumber + 1) * DevCreateParam.MaxTaskNumber;

	DevCreateParam.bRequestSliceShutdown =
	    (DevCreateOption & CM_DEVICE_CONFIG_SLICESHUTDOWN_ENABLE) ? TRUE :
	    FALSE;

	DevCreateParam.bRequestCustomGpuContext =
	    (DevCreateOption & CM_DEVICE_CONFIG_GPUCONTEXT_ENABLE) ? TRUE :
	    FALSE;

	DevCreateParam.bSLMMode =
	    (DevCreateOption & CM_DEVICE_CONFIG_SLM_MODE_ENABLE) ? TRUE : FALSE;

	return CM_SUCCESS;
}

BOOL CmDevice_RT::IsScratchSpaceDisabled()
{
	return m_DevCreateOption.DisableScratchSpace ? TRUE : FALSE;
}

BOOL CmDevice_RT::IsSurfaceReuseEnabled()
{
	return m_DevCreateOption.EnableSurfaceReuse ? TRUE : FALSE;
}

UINT CmDevice_RT::ValidSurfaceIndexStart()
{
	UINT genid;
	GetGenPlatform(genid);

	if (genid >= IGFX_GEN9_CORE) {
		return (CM_GLOBAL_SURFACE_INDEX_START_GEN9_PLUS +
			CM_GLOBAL_SURFACE_NUMBER);
	} else {
		return (CM_NULL_SURFACE_BINDING_INDEX + 1);
	}
}

UINT CmDevice_RT::MaxIndirectSurfaceCount()
{
	UINT genid;
	GetGenPlatform(genid);

	if (genid >= IGFX_GEN9_CORE) {
		return (GT_RESERVED_INDEX_START_GEN9_PLUS -
			CM_GLOBAL_SURFACE_NUMBER - 1);
	} else {
		return (GT_RESERVED_INDEX_START - CM_GLOBAL_SURFACE_NUMBER - 1);
	}
}

BOOL CmDevice_RT::IsCmReservedSurfaceIndex(UINT surfBTI)
{
	UINT genid;
	GetGenPlatform(genid);

	if (genid >= IGFX_GEN9_CORE) {
		if (surfBTI >= CM_GLOBAL_SURFACE_INDEX_START_GEN9_PLUS
		    && surfBTI <
		    (CM_GLOBAL_SURFACE_INDEX_START_GEN9_PLUS +
		     CM_GLOBAL_SURFACE_NUMBER))
			return TRUE;
		else
			return FALSE;
	} else {
		if (surfBTI >= CM_GLOBAL_SURFACE_INDEX_START
		    && surfBTI <
		    (CM_GLOBAL_SURFACE_INDEX_START + CM_GLOBAL_SURFACE_NUMBER))
			return TRUE;
		else
			return FALSE;
	}
}

BOOL CmDevice_RT::IsValidSurfaceIndex(UINT surfBTI)
{
	UINT genid;
	GetGenPlatform(genid);

	if (genid >= IGFX_GEN9_CORE) {
		if (surfBTI >=
		    (CM_GLOBAL_SURFACE_INDEX_START_GEN9_PLUS +
		     CM_GLOBAL_SURFACE_NUMBER)
		    && surfBTI < GT_RESERVED_INDEX_START_GEN9_PLUS)
			return TRUE;
		else
			return FALSE;
	} else {
		if (surfBTI > CM_NULL_SURFACE_BINDING_INDEX
		    && surfBTI < CM_GLOBAL_SURFACE_INDEX_START)
			return TRUE;
		else
			return FALSE;
	}
}

EXTERN_C INT
CreateCmDevice(CmDevice_RT * &pDevice, UINT & version,
	       CmDriverContext * pDriverContext, UINT DevCreateOption)
{
	INT result = CM_SUCCESS;
	pDevice = NULL;

	result = CmDevice_RT::Create(pDriverContext, pDevice, DevCreateOption);
	if (result == CM_SUCCESS) {
		version = CURRENT_CM_VERSION;
	} else {
		version = 0;
	}

	return result;
}

EXTERN_C INT DestroyCmDevice(CmDevice_RT * &pDevice)
{
	INT result = CM_SUCCESS;

	result = CmDevice_RT::Destroy(pDevice);
	if (result == CM_SUCCESS) {
		pDevice = NULL;
	}

	return result;
}
