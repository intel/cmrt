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
#pragma once
#include "cm_def.h"

class CmKernel;
class CmDevice_RT;

#include "cm_task_base.h"

class CmTask_RT : public CmTask {
 public:
	static INT Create(CmDevice_RT * pCmDevice, UINT index,
			  UINT max_kernel_count, CmTask_RT * &pKernelArray);
	static INT Destroy(CmTask_RT * &pKernelArray);

	CM_RT_API INT AddKernel(CmKernel * pKernel);
	CM_RT_API INT Reset(void);
	CM_RT_API INT AddSync(void);
	CM_RT_API INT SetPowerOption(PCM_HAL_POWER_OPTION_PARAM pPowerOption);
	CM_RT_API INT SetPreemptionMode(CM_HAL_PREEMPTION_MODE mode);
	CM_RT_API CM_HAL_PREEMPTION_MODE GetPreemptionMode();

	UINT GetKernelCount();
	CmKernel *GetKernelPointer(UINT index);
	UINT GetIndexInTaskArray();
	BOOLEAN IntegrityCheckKernelThreadspace(void);
	UINT64 GetSyncBitmap();
	PCM_HAL_POWER_OPTION_PARAM GetPowerOption();

 protected:
	 CmTask_RT(CmDevice_RT * pCmDevice, UINT index, UINT max_kernel_count);
	~CmTask_RT(void);

	INT Initialize();

	CmKernel **m_pKernelArray;

	UINT m_KernelCount;
	UINT m_MaxKernelCount;

	UINT m_IndexTaskArray;

	UINT64 m_ui64SyncBitmap;
	CmDevice_RT *m_pCmDev;

	CM_HAL_POWER_OPTION_PARAM m_PowerOption;
	CM_HAL_PREEMPTION_MODE m_PreemptionMode;

 private:
	 CmTask_RT(const CmTask_RT & other);
	 CmTask_RT & operator=(const CmTask_RT & other);
};
