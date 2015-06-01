/**
 * Copyright (c) 2015 Intel Corporation
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
 * Description:
 *	 Communication with the source-level shader debugger.
 *
 * Authors:
 *	 Mircea Gherzan <mircea.gherzan@intel.com>
 */

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

#include "debugger.h"

static const unsigned int IGFX_DBG_CURRENT_VERSION = 2;

class SharedLibContainer
{
private:
	static const char soName32_[];
	static const char soName64_[];
	static const char envVar_[];
	static void *libHandle_;

public:
	static const char *libName(void)
	{
		return sizeof(void *) == 4 ? soName32_ : soName64_;
	}

	SharedLibContainer()
	{
		// Do not reload the library if is already loaded
		if (libHandle_)
			return;

		libHandle_ = dlopen(libName(), RTLD_LAZY);
	}

	bool isEnabled(void)
	{
		return (libHandle_ != NULL);
	}

	void *symAddress(const char *symName)
	{
		void *symPtr;
		char *errPtr;

		dlerror();
		symPtr = dlsym(libHandle_, symName);
		errPtr = dlerror();

		if (errPtr)
			fprintf(stderr, "Could not find symbol %s: %s\n",
				symName, errPtr);

		return symPtr;
	}
};

const char SharedLibContainer::soName32_[] = "libigfxdbgxchg32.so";
const char SharedLibContainer::soName64_[] = "libigfxdbgxchg64.so";
void *SharedLibContainer::libHandle_;

int DbgNotifyNewDevice(void *cmDevHandle)
{
	struct DbgNewDeviceData
	{
		unsigned version;
		void *cmDevHandle;
		void *umdDevHandle;
	};

	typedef int (*pfnNotifyNewDevice)(DbgNewDeviceData *);

	SharedLibContainer slc;
	if (!slc.isEnabled())
		return 0;

	pfnNotifyNewDevice pfn;
	pfn = (pfnNotifyNewDevice)slc.symAddress("notifyNewDevice");
	if (!pfn)
		return -1;

	DbgNewDeviceData ndd;
	ndd.version = IGFX_DBG_CURRENT_VERSION;
	ndd.cmDevHandle = cmDevHandle;
	ndd.umdDevHandle = NULL;

	return (*pfn)(&ndd);
}

int DbgNotifyDeviceDestruction(void *cmDevHandle)
{
	struct DbgDeviceDestructionData
	{
		unsigned version;
		void *cmDevHandle;
	};

	typedef int (*pfnNotifyDevDestruction)(DbgDeviceDestructionData *);

	SharedLibContainer slc;
	if (!slc.isEnabled())
		return 0;

	pfnNotifyDevDestruction pfn;
	pfn  = (pfnNotifyDevDestruction)slc.symAddress("notifyDeviceDestruction");
	if (!pfn)
		return -1;

	DbgDeviceDestructionData ddd;
	ddd.version = IGFX_DBG_CURRENT_VERSION;
	ddd.cmDevHandle = cmDevHandle;

	return (*pfn)(&ddd);
}

int DbgNotifyKernelBinary(void *devHandle, void *programHandle,
			     const char *kernelName, void *genBinary,
			     unsigned genBinarySize, void *genDebugInfo,
			     unsigned genDebugInfoSize,
			     const char *debugInfoFile)
{
	struct DbgKernelBinaryData
	{
		unsigned version;
		void* udh;
		void* uph;
		const char* kernelName;
		void* genBinary;
		unsigned int genBinarySize;
		void* genDebugInfo;
		unsigned genDebugInfoSize;
		const char* debugInfoFileName;
	};

	typedef int (*pfnNotifyKernelBinary)(DbgKernelBinaryData *);

	SharedLibContainer slc;
	if (!slc.isEnabled())
		return 0;

	pfnNotifyKernelBinary pfn;
	pfn  = (pfnNotifyKernelBinary)slc.symAddress("notifyKernelBinary");
	if (!pfn)
		return -1;

	DbgKernelBinaryData kbd;

	kbd.version = IGFX_DBG_CURRENT_VERSION;
	kbd.udh = devHandle;
	kbd.uph = programHandle;
	kbd.kernelName = kernelName;
	kbd.genBinary = genBinary;
	kbd.genBinarySize = genBinarySize;
	kbd.genDebugInfo = genDebugInfo;
	kbd.genDebugInfoSize = genDebugInfoSize;
	kbd.debugInfoFileName = debugInfoFile;

	return (*pfn)(&kbd);
}

int DbgGetSysRoutineBinary(PLATFORM plat, unsigned bti,
			      const unsigned char **pSip,
			      unsigned *sipSize, unsigned *resSize)
{
	int ret;
	static void *libHandle;

	struct DbgRequestSysRoutineBinary2
	{
		const unsigned version;
		const unsigned bti;
		const unsigned char *sysRoutineBinary;	// set by the interchange
		unsigned sysRoutineSize;		// set by the interchange
		unsigned debugSurfaceSize;		// set by the interchange
		PLATFORM platform;
	};

	typedef int (*pfnReqSysRoutine)(DbgRequestSysRoutineBinary2 *);
	static pfnReqSysRoutine pfn;

	static const char requestSysRoutineName[] = "requestSipBinary";

	DbgRequestSysRoutineBinary2 rsrb = {
		.version = IGFX_DBG_CURRENT_VERSION,
		.bti	 = bti,
	};
	rsrb.platform	 = plat;

	SharedLibContainer slc;
	if (slc.isEnabled()) {
		// The interchange might have already been loaded
		pfn = (pfnReqSysRoutine)slc.symAddress(requestSysRoutineName);
	} else if (!pfn) {
		// Try to load it even if debuggger notifications are disabled:
		// we want to keep the system routine independ from them.
		const char *name = SharedLibContainer::libName();

		dlerror();
		libHandle = dlopen(name, RTLD_LAZY);
		if (!libHandle) {
			fprintf(stderr, "Failed to load %s: %s\n",
				name, dlerror());
			return -1;
		}

		pfn = (pfnReqSysRoutine)dlsym(libHandle, requestSysRoutineName);
	}

	if (!pfn) {
		ret = -1;
		goto out;
	}


	ret = (*pfn)(&rsrb);
	if (ret) {
		goto out;
	}

	*pSip = rsrb.sysRoutineBinary;
	*sipSize = rsrb.sysRoutineSize;
	*resSize = rsrb.debugSurfaceSize;

	ret = 0;
out:
	return ret;
}

