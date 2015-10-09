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

#ifndef _Cm_Device_Base_H_
#define _Cm_Device_Base_H_

typedef struct _CM_DLL_FILE_VERSION_ CM_DLL_FILE_VERSION;

class CmDevice {
 public:

	virtual INT CreateBuffer(UINT size, CmBuffer * &pSurface) { return 0; };
	virtual INT CreateSurface2D(UINT width, UINT height, CM_SURFACE_FORMAT format,
			    CmSurface2D * &pSurface) { return 0; };

	virtual INT CreateSurface2D(CmOsResource * pOsResource,
			    CmSurface2D * &pSurface) { return 0; };
	virtual INT CreateBuffer(CmOsResource * pOsResource, CmBuffer * &pSurface) { return 0; };

	virtual INT CreateBufferUP(UINT size, void *pSystMem, CmBufferUP * &pSurface) { return 0; };
	virtual INT DestroyBufferUP(CmBufferUP * &pSurface) { return 0; };

	virtual INT CreateSurface2DUP(UINT width, UINT height,
			      CM_SURFACE_FORMAT format, void *pSysMem,
			      CmSurface2DUP * &pSurface) { return 0; };
	virtual INT DestroySurface(CmSurface2DUP * &pSurface) { return 0; };

	virtual INT DestroySurface(CmBuffer * &pSurface) { return 0; };
	virtual INT DestroySurface(CmSurface2D * &pSurface) { return 0; };

	virtual INT GetSurface2DInfo(UINT width, UINT height, CM_SURFACE_FORMAT format,
			     UINT & pitch, UINT & physicalSize) { return 0; };

	virtual INT CreateQueue(CmQueue * &pQueue) { return 0; };
	virtual INT LoadProgram(void *pCommonISACode, const UINT size,
			CmProgram * &pProgram, const char *options = NULL) { return 0; };
	virtual INT CreateKernel(CmProgram * pProgram, const char *kernelName,
			 CmKernel * &pKernel, const char *options = NULL) { return 0; };
	virtual INT DestroyKernel(CmKernel * &pKernel) { return 0; };
	virtual INT DestroyProgram(CmProgram * &pProgram) { return 0; };

	virtual INT CreateTask(CmTask * &pTask) { return 0; };
	virtual INT DestroyTask(CmTask * &pTask) { return 0; };

	virtual INT CreateThreadGroupSpace(UINT thrdSpaceWidth, UINT thrdSpaceHeight,
				   UINT grpSpaceWidth, UINT grpSpaceHeight,
				   CmThreadGroupSpace * &pTGS) { return 0; };
	virtual INT DestroyThreadGroupSpace(CmThreadGroupSpace * &pTGS) { return 0; };

	virtual INT CreateThreadSpace(UINT width, UINT height, CmThreadSpace * &pTS) { return 0; };
	virtual INT DestroyThreadSpace(CmThreadSpace * &pTS) { return 0; };

	virtual INT SetSuggestedL3Config( L3_SUGGEST_CONFIG l3_s_c) { return 0; };
/*
	virtual INT GetRTDllVersion(CM_DLL_FILE_VERSION * pFileVersion) { return 0; };
*/
	virtual INT GetRTDllVersion(CM_DLL_FILE_VERSION * pFileVersion) {return 0;};
	virtual INT GetCaps(CM_DEVICE_CAP_NAME capName, size_t & capValueSize,
		    void *pCapValue) { return 0; };
        virtual ~CmDevice(){};
};

#endif /* _Cm_Device_Base_H_ */
