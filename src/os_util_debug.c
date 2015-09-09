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

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "os_util_debug.h"

#if GENOS_MESSAGES_ENABLED
#include "os_utilities.h"

#define __GENOS_USER_FEATURE_KEY_MESSAGE_DEFAULT_VALUE	1
#define __GENOS_USER_FEATURE_KEY_MESSAGE_PRINT_ENABLED	"Message Print Enabled"

#define __MEDIA_REGISTRY_SUBKEY_INTERNAL		"LibVa"
#define __MEDIA_REGISTRY_SUBKEY_PERFORMANCE		""
#define __MEDIA_REGISTRY_SUBKEY_REPORT                  "LibVa/Report"

extern INT32 GenOsMemAllocCounter;

const PCCHAR GenOsLogPathTemplate = "%s/igd_%u.%s";

const PCCHAR DDILogPathTemplate = "%s\\ddi_dump_%d.%s";

const PCCHAR GENOS_LogLevelName[GENOS_MESSAGE_LVL_COUNT] = {
	"",
	"CRITICAL",
	"NORMAL  ",
	"VERBOSE ",
	"ENTER   ",
	"EXIT    ",
	"ENTER   ",
	"EXIT    ",
};

const PCCHAR GENOS_ComponentName[GENOS_COMPONENT_COUNT] = {
	"[GENOS]:  ",
	"[GENHW]:  ",
	"[LIBVA]:",
	"[CM]:   "
};

GENOS_MESSAGE_PARAMS g_GenOsMsgParams;
GENOS_MESSAGE_PARAMS g_GenOsMsgParams_DDI_Dump;

GENOS_USER_FEATURE_VALUE_ID pcComponentRegKeys[GENOS_COMPONENT_COUNT][3] = {
	{
	 __GENOS_USER_FEATURE_KEY_MESSAGE_OS_TAG,
	 __GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_OS,
	 __GENOS_USER_FEATURE_KEY_SUB_COMPONENT_OS_TAG},

	{
	 __GENOS_USER_FEATURE_KEY_MESSAGE_HW_TAG,
	 __GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_HW,
	 __GENOS_USER_FEATURE_KEY_SUB_COMPONENT_HW_TAG},

	{
	 __GENOS_USER_FEATURE_KEY_MESSAGE_DDI_TAG,
	 __GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_DDI,
	 __GENOS_USER_FEATURE_KEY_SUB_COMPONENT_DDI_TAG},

	{
	 __GENOS_USER_FEATURE_KEY_MESSAGE_CM_TAG,
	 __GENOS_USER_FEATURE_KEY_BY_SUB_COMPONENT_CM,
	 __GENOS_USER_FEATURE_KEY_SUB_COMPONENT_CM_TAG}
};



UINT8 subComponentCount[GENOS_COMPONENT_COUNT] = {
	GENOS_SUBCOMP_COUNT,
	GENOS_HW_SUBCOMP_COUNT,
	GENOS_DDI_SUBCOMP_COUNT,
	GENOS_CM_SUBCOMP_COUNT
};

void _GENOS_Assert(GENOS_COMPONENT_ID comp,
		   uint8_t subComp)
{
	abort();
}


VOID GENOS_SetSubCompMessageLevel(GENOS_COMPONENT_ID compID, UINT8 subCompID,
				  GENOS_MESSAGE_LEVEL msgLevel)
{
	if (compID >= GENOS_COMPONENT_COUNT) {
		GENOS_OS_ASSERTMESSAGE("Invalid component %d.", compID);
		return;
	}

	if (subCompID >= GENOS_MAX_SUBCOMPONENT_COUNT) {
		GENOS_OS_ASSERTMESSAGE("Invalid sub-component %d.", subCompID);
		return;
	}

	g_GenOsMsgParams.components[compID].subComponents[subCompID].
	    uiMessageLevel = msgLevel;
}

VOID GENOS_SetCompMessageLevel(GENOS_COMPONENT_ID compID,
			       GENOS_MESSAGE_LEVEL msgLevel)
{
	if (compID >= GENOS_COMPONENT_COUNT) {
		GENOS_OS_ASSERTMESSAGE("Invalid component %d.", compID);
		return;
	}

	g_GenOsMsgParams.components[compID].component.uiMessageLevel = msgLevel;
}

VOID GENOS_SetCompMessageLevelAll(GENOS_MESSAGE_LEVEL msgLevel)
{
	UINT32 i;

	for (i = 0; i < GENOS_COMPONENT_COUNT; i++) {
		GENOS_SetCompMessageLevel((GENOS_COMPONENT_ID) i, msgLevel);
	}
}

VOID GENOS_SubCompAssertEnableDisable(GENOS_COMPONENT_ID compID,
				      UINT8 subCompID, BOOL bEnable)
{
	if (compID >= GENOS_COMPONENT_COUNT) {
		GENOS_OS_ASSERTMESSAGE("Invalid component %d.", compID);
		return;
	}

	if (subCompID >= GENOS_MAX_SUBCOMPONENT_COUNT) {
		GENOS_OS_ASSERTMESSAGE("Invalid sub-component %d.", subCompID);
		return;
	}

	g_GenOsMsgParams.components[compID].subComponents[subCompID].
	    bAssertEnabled = bEnable;
}

VOID GENOS_CompAssertEnableDisable(GENOS_COMPONENT_ID compID, BOOL bEnable)
{
	if (compID >= GENOS_COMPONENT_COUNT) {
		GENOS_OS_ASSERTMESSAGE("Invalid component %d.", compID);
		return;
	}

	g_GenOsMsgParams.components[compID].component.bAssertEnabled = bEnable;
}

static GENOS_STATUS GENOS_UserFeature_WriteValues(PGENOS_USER_FEATURE_INTERFACE fi,
						  PGENOS_USER_FEATURE f)
{
	// TODO
	return GENOS_STATUS_SUCCESS;
}

static GENOS_STATUS GENOS_UserFeature_ReadValue(PGENOS_USER_FEATURE_INTERFACE fi,
						PGENOS_USER_FEATURE f,
						PCCHAR p,
						GENOS_USER_FEATURE_VALUE_TYPE vt)
{
	// TODO
	return GENOS_STATUS_SUCCESS;
}

static GENOS_STATUS GENOS_UserFeature_ReadValue_ID(PGENOS_USER_FEATURE_INTERFACE fi,
						   PGENOS_USER_FEATURE f,
						   GENOS_USER_FEATURE_VALUE_ID vID,
						   GENOS_USER_FEATURE_VALUE_TYPE vt)
{
	// TODO
	return GENOS_STATUS_SUCCESS;
}

VOID GENOS_MessageInitComponent(GENOS_COMPONENT_ID compID)
{
	GENOS_STATUS eStatus = GENOS_STATUS_SUCCESS;
	GENOS_USER_FEATURE UserFeature;
	GENOS_USER_FEATURE_VALUE UserFeatureValue =
	    { __GENOS_USER_FEATURE_KEY_INVALID_ID,
	    0,
	    0,
	    0,
	    0,
	    GENOS_USER_FEATURE_TYPE_INVALID,
	    GENOS_USER_FEATURE_VALUE_TYPE_INVALID,
	    0
	};
	UINT32 uiCompRegSetting;
	UINT64 uiSubCompRegSetting;
	UINT8 i;
	GENOS_USER_FEATURE_VALUE_ID pcMessageKey = __GENOS_USER_FEATURE_KEY_INVALID_ID;
	GENOS_USER_FEATURE_VALUE_ID pcBySubComponentsKey = __GENOS_USER_FEATURE_KEY_INVALID_ID;
	GENOS_USER_FEATURE_VALUE_ID pcSubComponentsKey = __GENOS_USER_FEATURE_KEY_INVALID_ID;

	if (compID >= GENOS_COMPONENT_COUNT) {
		GENOS_OS_ASSERTMESSAGE("Invalid component %d.", compID);
		return;
	}

	pcMessageKey = pcComponentRegKeys[compID][0];
	pcBySubComponentsKey = pcComponentRegKeys[compID][1];
	pcSubComponentsKey = pcComponentRegKeys[compID][2];

	UserFeatureValue.u32Data =
	    __GENOS_USER_FEATURE_KEY_MESSAGE_DEFAULT_VALUE;
	UserFeature.Type = GENOS_USER_FEATURE_TYPE_USER;
	UserFeature.pPath = __MEDIA_REGISTRY_SUBKEY_INTERNAL;
	UserFeature.pValues = &UserFeatureValue;
	UserFeature.uiNumValues = 1;

	eStatus = GENOS_UserFeature_ReadValue_ID(NULL,
						 &UserFeature,
						 pcMessageKey,
						 GENOS_USER_FEATURE_VALUE_TYPE_UINT32);

	if (eStatus == GENOS_STATUS_READ_REGISTRY_FAILED) {
		GENOS_UserFeature_WriteValues(NULL, &UserFeature);
	}

	uiCompRegSetting = UserFeatureValue.u32Data;

	GENOS_SetCompMessageLevel(compID,
				  (GENOS_MESSAGE_LEVEL) (uiCompRegSetting &
							 0x7));
	GENOS_CompAssertEnableDisable(compID, (uiCompRegSetting >> 3) & 0x1);

	UserFeatureValue.bData = FALSE;

	eStatus = GENOS_UserFeature_ReadValue_ID(NULL,
						 &UserFeature,
						 pcBySubComponentsKey,
						 GENOS_USER_FEATURE_VALUE_TYPE_UINT32);
	if (eStatus == GENOS_STATUS_READ_REGISTRY_FAILED) {
		GENOS_UserFeature_WriteValues(NULL, &UserFeature);
	}

	g_GenOsMsgParams.components[compID].bBySubComponent =
	    UserFeatureValue.bData;

	if (g_GenOsMsgParams.components[compID].bBySubComponent) {
		UserFeatureValue.u64Data = 0;

		eStatus = GENOS_UserFeature_ReadValue_ID(NULL,
							 &UserFeature,
							 pcSubComponentsKey,
							 GENOS_USER_FEATURE_VALUE_TYPE_UINT64);

		if (eStatus == GENOS_STATUS_READ_REGISTRY_FAILED) {
			GENOS_UserFeature_WriteValues(NULL, &UserFeature);
		}

		uiSubCompRegSetting = UserFeatureValue.u64Data;

		for (i = 0; i < subComponentCount[compID]; i++) {
			GENOS_SetSubCompMessageLevel(compID, i,
						     (GENOS_MESSAGE_LEVEL)
						     (uiSubCompRegSetting &
						      0x7));
			GENOS_SubCompAssertEnableDisable(compID, i,
							 (uiSubCompRegSetting >>
							  3) & 0x1);

			uiSubCompRegSetting = (uiSubCompRegSetting >> 4);
		}
	}
}

VOID GENOS_MessageInit()
{
	UINT8 i;
	GENOS_USER_FEATURE UserFeature;
	GENOS_USER_FEATURE_VALUE UserFeatureValue =
	    { __GENOS_USER_FEATURE_KEY_INVALID_ID,
	    0,
	    0,
	    0,
	    0,
	    GENOS_USER_FEATURE_TYPE_INVALID,
	    GENOS_USER_FEATURE_VALUE_TYPE_INVALID,
	    0
	};
	GENOS_STATUS eStatus = GENOS_STATUS_SUCCESS;

	if (g_GenOsMsgParams.uiCounter == 0) {
		GenOsMemAllocCounter = 0;

		GENOS_SetCompMessageLevelAll(GENOS_MESSAGE_LVL_CRITICAL);

		for (i = 0; i < GENOS_COMPONENT_COUNT; i++) {
			GENOS_MessageInitComponent((GENOS_COMPONENT_ID) i);
		}

		GENOS_ZeroMemory(&UserFeatureValue, sizeof(UserFeatureValue));
		UserFeatureValue.bData = TRUE;
		UserFeature.Type = GENOS_USER_FEATURE_TYPE_USER;
		UserFeature.pPath = __MEDIA_REGISTRY_SUBKEY_INTERNAL;
		UserFeature.pValues = &UserFeatureValue;
		UserFeature.uiNumValues = 1;

		eStatus = GENOS_UserFeature_ReadValue(NULL,
						      &UserFeature,
						      __GENOS_USER_FEATURE_KEY_MESSAGE_PRINT_ENABLED,
						      GENOS_USER_FEATURE_VALUE_TYPE_INT32);

		if (eStatus == GENOS_STATUS_READ_REGISTRY_FAILED) {
			GENOS_UserFeature_WriteValues(NULL, &UserFeature);
		}

		g_GenOsMsgParams.bUseOutputDebugString = UserFeatureValue.bData;
	}
	g_GenOsMsgParams.uiCounter++;

}

VOID GENOS_MessageClose()
{
	if (g_GenOsMsgParams.uiCounter == 1) {
		GENOS_ZeroMemory(&g_GenOsMsgParams,
				 sizeof(GENOS_MESSAGE_PARAMS));
	} else {
		g_GenOsMsgParams.uiCounter--;
	}
}

static BOOL GENOS_ShouldPrintMessage(GENOS_MESSAGE_LEVEL level,
				     GENOS_COMPONENT_ID compID,
				     UINT8 subCompID, const PCCHAR message)
{
	if (message == NULL) {
		return FALSE;
	}

	if (compID >= GENOS_COMPONENT_COUNT ||
	    subCompID >= GENOS_MAX_SUBCOMPONENT_COUNT) {
		return FALSE;
	}
	if (g_GenOsMsgParams.components[compID].component.uiMessageLevel <
	    level) {
		return FALSE;
	}

	if (g_GenOsMsgParams.components[compID].bBySubComponent &&
	    g_GenOsMsgParams.components[compID].subComponents[subCompID].
	    uiMessageLevel < level) {
		return FALSE;
	}

	return TRUE;
}

void GENOS_Message(GENOS_MESSAGE_LEVEL level,
		   const char *tag,
		   GENOS_COMPONENT_ID comp,
		   uint8_t subComp,
		   const char *msg, ...)
{
	FILE *stream = stderr;
	va_list args;

	if (!GENOS_ShouldPrintMessage(level, comp, subComp, msg)) {
		return;
	}

	va_start(args, msg);
	vfprintf(stream, msg, args);
	fflush(stream);
}

#if GENOS_ASSERT_ENABLED
BOOL GENOS_ShouldAssert(GENOS_COMPONENT_ID compID, UINT8 subCompID)
{
	if (compID >= GENOS_COMPONENT_COUNT ||
	    subCompID >= GENOS_MAX_SUBCOMPONENT_COUNT) {
		return FALSE;
	}

	if (!g_GenOsMsgParams.components[compID].component.bAssertEnabled) {
		return FALSE;
	}

	if (g_GenOsMsgParams.components[compID].bBySubComponent &&
	    !g_GenOsMsgParams.components[compID].subComponents[subCompID].
	    bAssertEnabled) {
		return FALSE;
	}

	return TRUE;

}
#endif

#endif
