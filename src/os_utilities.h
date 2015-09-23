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

#ifndef __GENOS_UTILITIES_H__
#define __GENOS_UTILITIES_H__

#include "os_defs.h"

#include <va/va.h>
typedef VAStatus GENOS_OSRESULT;
#include <stdarg.h>

#define GENOS_USER_CONTROL_MIN_DATA_SIZE	128
#define GENOS_USER_CONTROL_MAX_DATA_SIZE	2048
#define REG_FILE				"/etc/igfx_registry.txt"

typedef enum _GENOS_USER_FEATURE_VALUE_ID {
	__GENOS_USER_FEATURE_KEY_INVALID_ID,
	__GENOS_USER_FEATURE_KEY_MESSAGE_OS_TAG,
	__GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_OS,
	__GENOS_USER_FEATURE_KEY_SUB_COMPONENT_OS_TAG,
	__GENOS_USER_FEATURE_KEY_MESSAGE_HW_TAG,
	__GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_HW,
	__GENOS_USER_FEATURE_KEY_SUB_COMPONENT_HW_TAG,
	__GENOS_USER_FEATURE_KEY_MESSAGE_DDI_TAG,
	__GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_DDI,
	__GENOS_USER_FEATURE_KEY_SUB_COMPONENT_DDI_TAG,
	__GENOS_USER_FEATURE_KEY_MESSAGE_CM_TAG,
	__GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_CM,
	__GENOS_USER_FEATURE_KEY_SUB_COMPONENT_CM_TAG,
} GENOS_USER_FEATURE_VALUE_ID;

typedef enum
{
	GENOS_USER_FEATURE_TYPE_INVALID,
	GENOS_USER_FEATURE_TYPE_USER,
	GENOS_USER_FEATURE_TYPE_SYSTEM,
} GENOS_USER_FEATURE_TYPE, *PGENOS_USER_FEATURE_TYPE;

typedef enum
{
	GENOS_USER_FEATURE_VALUE_TYPE_INVALID,
	GENOS_USER_FEATURE_VALUE_TYPE_BINARY,
	GENOS_USER_FEATURE_VALUE_TYPE_BOOL,
	GENOS_USER_FEATURE_VALUE_TYPE_INT32,
	GENOS_USER_FEATURE_VALUE_TYPE_INT64,
	GENOS_USER_FEATURE_VALUE_TYPE_UINT32,
	GENOS_USER_FEATURE_VALUE_TYPE_UINT64,
	GENOS_USER_FEATURE_VALUE_TYPE_FLOAT,
	GENOS_USER_FEATURE_VALUE_TYPE_STRING,
	GENOS_USER_FEATURE_VALUE_TYPE_MULTI_STRING,
} GENOS_USER_FEATURE_VALUE_TYPE, *PGENOS_USER_FEATURE_VALUE_TYPE;

typedef enum
{
	GENOS_USER_FEATURE_NOTIFY_TYPE_INVALID,
	GENOS_USER_FEATURE_NOTIFY_TYPE_VALUE_CHANGE,
} GENOS_USER_FEATURE_NOTIFY_TYPE, *PGENOS_USER_FEATURE_NOTIFY_TYPE;

typedef struct
{
	PCHAR   pStringData;
	DWORD   uMaxSize;
	DWORD   uSize;
} GENOS_USER_FEATURE_VALUE_STRING, *PGENOS_USER_FEATURE_VALUE_STRING;

typedef struct
{
	PCHAR					pMultStringData;
	DWORD					uMaxSize;
	DWORD					uSize;
	PGENOS_USER_FEATURE_VALUE_STRING	pStrings;
	DWORD					uCount;
} GENOS_USER_FEATURE_VALUE_MULTI_STRING, *PGENOS_USER_FEATURE_VALUE_MULTI_STRING;

typedef struct
{
	PUCHAR  pBinaryData;
	DWORD   uMaxSize;
	DWORD   uSize;
} GENOS_USER_FEATURE_VALUE_BINARY, *PGENOS_USER_FEATURE_VALUE_BINARY;

typedef struct _GENOS_USER_FEATURE_VALUE_DATA
{
	union
	{
		BOOL					bData;
		DWORD					u32Data;
		QWORD					u64Data;
		INT32					i32Data;
		INT64					i64Data;
		FLOAT					fData;
		GENOS_USER_FEATURE_VALUE_STRING		StringData;
		GENOS_USER_FEATURE_VALUE_MULTI_STRING	MultiStringData;
		GENOS_USER_FEATURE_VALUE_BINARY		BinaryData;
	};
	INT32						i32DataFlag;
} GENOS_USER_FEATURE_VALUE_DATA, *PGENOS_USER_FEATURE_VALUE_DATA;

typedef struct _GENOS_USER_FEATURE_VALUE_INFO
{
	PCHAR   pcName;  //store name for the bitmask/enum values
	DWORD   Value;
} GENOS_USER_FEATURE_VALUE_INFO, *PGENOS_USER_FEATURE_VALUE_INFO;

typedef struct
{
	GENOS_USER_FEATURE_VALUE_ID	ValueID;
	PCCHAR				pValueName;
	PCCHAR				pcGroup;	//!< Reg key group - eg: MediaSolo, GENOS, Codec
	PCCHAR				pcPath;		//!< Reg Key Read Path
	PCCHAR				pcWritePath;	//!< Reg Key Write Path
	GENOS_USER_FEATURE_TYPE		Type;		//!< Reg Key User Feature type - eg: System, User
	GENOS_USER_FEATURE_VALUE_TYPE	ValueType;	//!< Reg key type - eg: bool,dword
	PCCHAR				DefaultValue;	//!< Reg value
	PCCHAR				pcDescription;	//<! Reg key description
	BOOL				bExists;	//<! Set if the reg key is defined in the windows reg key manager
	UINT				uiNumOfValues;	//<! Number of valid Reg Key values. Useful for reg keys of type bitmask and enum
	PGENOS_USER_FEATURE_VALUE_INFO	pValueInfo;	//<! Store information of all valid enum/bit mask values and names
	GENOS_STATUS (*pfnSetDefaultValueData)(PGENOS_USER_FEATURE_VALUE_DATA pValueData);
	union
	{
		BOOL					bData;
		DWORD					u32Data;
		QWORD					u64Data;
		INT32					i32Data;
		INT64					i64Data;
		FLOAT					fData;
		GENOS_USER_FEATURE_VALUE_STRING		StringData;
		GENOS_USER_FEATURE_VALUE_MULTI_STRING	MultiStringData;
		GENOS_USER_FEATURE_VALUE_BINARY		BinaryData;
	};
	GENOS_USER_FEATURE_VALUE_DATA			Value;			   //!< Reg value
} GENOS_USER_FEATURE_VALUE, *PGENOS_USER_FEATURE_VALUE;

typedef struct
{
	PGENOS_USER_FEATURE_VALUE pUserFeatureValue;
} GENOS_USER_FEATURE_VALUE_MAP, *PGENOS_USER_FEATURE_VALUE_MAP;

typedef struct
{
	GENOS_USER_FEATURE_TYPE		Type;		//!< User Feature Type
	PCHAR				pPath;		//!< User Feature Path
	GENOS_USER_FEATURE_NOTIFY_TYPE	NotifyType;	//!< Notification Type
	BOOL				bTriggered;	//!< Notification is triggered or not
	PVOID				pHandle;	//!< OS Specific Handle
} GENOS_USER_FEATURE_NOTIFY_DATA, *PGENOS_USER_FEATURE_NOTIFY_DATA;


//!
//! \brief User Feature Interface
//!
typedef struct
{
	GENOS_USER_FEATURE_TYPE		Type;		//!< User Feature Type
	PCCHAR				pPath;		//!< User Feature Path
	PGENOS_USER_FEATURE_VALUE	pValues;	//!< Array of User Feature Values
	DWORD				uiNumValues;	//!< Number of User Feature Values
} GENOS_USER_FEATURE, *PGENOS_USER_FEATURE;

typedef struct _GENOS_USER_FEATURE_INTERFACE *PGENOS_USER_FEATURE_INTERFACE;
typedef struct _GENOS_USER_FEATURE_INTERFACE
{
	PVOID pOsInterface;				//!< Pointer to OS Interface
	BOOL bIsNotificationSupported;			//!< Whether Notification feature is supported
	const GENOS_USER_FEATURE *pUserFeatureInit;	//!< Initializer for Os User Feature structure

	GENOS_STATUS (* pfnReadValue) (
		PGENOS_USER_FEATURE_INTERFACE pOsUserFeatureInterface,
		PGENOS_USER_FEATURE pUserFeature,
		PCCHAR pValueName,
		GENOS_USER_FEATURE_VALUE_TYPE ValueType);

	GENOS_STATUS (* pfnWriteValues) (
		PGENOS_USER_FEATURE_INTERFACE pOsUserFeatureInterface,
		PGENOS_USER_FEATURE pUserFeature);

	GENOS_STATUS (* pfnEnableNotification) (
		PGENOS_USER_FEATURE_INTERFACE pOsUserFeatureInterface,
		PGENOS_USER_FEATURE_NOTIFY_DATA pNotification);

	GENOS_STATUS (* pfnDisableNotification) (
		PGENOS_USER_FEATURE_INTERFACE pOsUserFeatureInterface,
		PGENOS_USER_FEATURE_NOTIFY_DATA pNotification);

	GENOS_STATUS (* pfnParsePath) (
		PGENOS_USER_FEATURE_INTERFACE pOsUserFeatureInterface,
		const PCHAR pInputPath,
		PGENOS_USER_FEATURE_TYPE pUserFeatureType,
		PPCHAR ppSubPath);

} GENOS_USER_FEATURE_INTERFACE;


#ifdef __cplusplus
extern "C" {
#endif

	PVOID GENOS_AlignedAllocMemory(SIZE_T size, SIZE_T alignment);

	VOID GENOS_AlignedFreeMemory(PVOID ptr);

	PVOID GENOS_AllocMemory(SIZE_T size);

	PVOID GENOS_AllocAndZeroMemory(SIZE_T size);

	VOID GENOS_FreeMemory(PVOID ptr);

#define GENOS_FreeMemAndSetNull(ptr)            \
    GENOS_FreeMemory(ptr);                      \
    ptr = NULL;

#define GENOS_SafeFreeMemory(ptr)               \
    if (ptr) GENOS_FreeMemory(ptr);             \

	VOID GENOS_ZeroMemory(PVOID pDestination, SIZE_T stLength);

	VOID GENOS_FillMemory(PVOID pDestination, SIZE_T stLength, UINT8 bFill);

	GENOS_STATUS GENOS_GetFileSize(HANDLE hFile,
				       PUINT32 lpFileSizeLow,
				       PUINT32 lpFileSizeHigh);

	GENOS_STATUS GENOS_CreateDirectory(const PCHAR lpPathName);

	GENOS_STATUS GENOS_CreateFile(PHANDLE pHandle,
				      const PCHAR lpFileName, UINT32 iOpenFlag);

	GENOS_STATUS GENOS_ReadFile(HANDLE hFile,
				    PVOID lpBuffer,
				    UINT32 bytesToRead,
				    PUINT32 pbytesRead, PVOID lpOverlapped);

	GENOS_STATUS GENOS_WriteFile(HANDLE hFile,
				     PVOID lpBuffer,
				     UINT32 bytesToWrite,
				     PUINT32 pbytesWritten, PVOID lpOverlapped);

	GENOS_STATUS GENOS_SetFilePointer(HANDLE hFile,
					  INT32 lDistanceToMove,
					  PINT32 lpDistanceToMoveHigh,
					  INT32 dwMoveMethod);

	BOOL GENOS_CloseHandle(HANDLE hObject);

	GENOS_STATUS GENOS_AppendFileFromPtr(PCCHAR pFilename,
					     PVOID pData, DWORD dwSize);

	GENOS_STATUS GENOS_SecureStrcat(PCHAR strDestination,
					SIZE_T numberOfElements,
					const PCCHAR strSource);

	PCHAR GENOS_SecureStrtok(PCHAR strToken,
				 PCCHAR strDelimit, PCHAR * contex);

	GENOS_STATUS GENOS_SecureStrcpy(PCHAR strDestination,
					SIZE_T numberOfElements,
					const PCCHAR strSource);

	GENOS_STATUS GENOS_SecureMemcpy(PVOID pDestination,
					SIZE_T dstLength,
					PCVOID pSource, SIZE_T srcLength);

	GENOS_STATUS GENOS_SecureFileOpen(PPFILE ppFile,
					  PCCHAR filename, PCCHAR mode);

	INT32 GENOS_SecureStringPrint(PCHAR buffer,
				      SIZE_T bufSize,
				      SIZE_T length, const PCCHAR format, ...);

	GENOS_STATUS GENOS_SecureVStringPrint(PCHAR buffer,
					      SIZE_T bufSize,
					      SIZE_T length,
					      const PCCHAR format,
					      va_list var_args);
	GENOS_STATUS GENOS_LoadLibrary(const PCCHAR lpLibFileName,
				       PHMODULE phModule);

	BOOL GENOS_FreeLibrary(HMODULE hLibModule);

	PVOID GENOS_GetProcAddress(HMODULE hModule, PCCHAR lpProcName);

	INT32 GENOS_GetPid();

	BOOL GENOS_QueryPerformanceFrequency(PUINT64 pFrequency);

	BOOL GENOS_QueryPerformanceCounter(PUINT64 pPerformanceCount);

	VOID GENOS_Sleep(UINT32 mSec);

	HANDLE GENOS_CreateEventEx(PVOID lpEventAttributes,
				   PCHAR lpName, UINT32 dwFlags);

	BOOL GENOS_RegisterWaitForSingleObject(PHANDLE phNewWaitObject,
					       HANDLE hObject,
					       PVOID Callback, PVOID Context);

	BOOL GENOS_UnregisterWaitEx(HANDLE hWaitHandle);

	DWORD GENOS_GetLogicalCoreNumber();

	GENOS_THREADHANDLE GENOS_CreateThread(PVOID ThreadFunction,
					      PVOID ThreadData);

	UINT GENOS_GetThreadId(GENOS_THREADHANDLE hThread);

	UINT GENOS_GetCurrentThreadId();

	PGENOS_MUTEX GENOS_CreateMutex();

	HRESULT GENOS_DestroyMutex(PGENOS_MUTEX pMutex);

	HRESULT GENOS_LockMutex(PGENOS_MUTEX pMutex);

	HRESULT GENOS_UnlockMutex(PGENOS_MUTEX pMutex);

	PGENOS_SEMAPHORE GENOS_CreateSemaphore(UINT uiInitialCount,
					       UINT uiMaximumCount);

	HRESULT GENOS_DestroySemaphore(PGENOS_SEMAPHORE pSemaphore);

	HRESULT GENOS_WaitSemaphore(PGENOS_SEMAPHORE pSemaphore,
				    UINT uiMilliseconds);

	HRESULT GENOS_PostSemaphore(PGENOS_SEMAPHORE pSemaphore,
				    UINT uiPostCount);

	UINT GENOS_WaitForSingleObject(PVOID pObject, UINT uiMilliseconds);

	UINT GENOS_WaitForMultipleObjects(UINT uiThreadCount,
					  VOID ** ppObjects,
					  UINT bWaitAll, UINT uiMilliseconds);

	GENOS_OSRESULT GENOS_StatusToOsResult(GENOS_STATUS eStatus);

	GENOS_STATUS OsResultToGENOS_Status(GENOS_OSRESULT eResult);

#ifdef __cplusplus
}
#endif
#endif
