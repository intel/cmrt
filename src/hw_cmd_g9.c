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

#include "hw_cmd_g9.h"

CONST MEDIA_VFE_STATE_CMD_G9 g_cInit_MEDIA_VFE_STATE_CMD_G9 = {
	// DWORD 0
	{
	 OP_LENGTH(SIZE32(MEDIA_VFE_STATE_CMD_G9)),
	 MEDIASUBOP_MEDIA_VFE_STATE,
	 GFXOP_PIPELINED,
	 PIPE_MEDIA,
	 INSTRUCTION_GFX},

	// DWORD 1
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 2
	{
	 0,
	 0},

	// DWORD 3
	{
	 0,
	 0,
	 FALSE,
	 FALSE,
	 0,
	 0},

	// DWORD 4
	{
	 0,
	 0,
	 0},

	// DWORD 5
	{
	 0,
	 0},

	// DWORD 6
	{
	 0xFF,
	 0,
	 0,
	 FALSE},

	// DWORD 7
	{
	 0xF,
	 0,
	 0,
	 0xF,
	 1,
	 0xF,
	 0xF,
	 0xF},

	// DWORD 8
	{
	 0xF,
	 1,
	 0,
	 0xE,
	 1,
	 0xE,
	 0xF,
	 0xE}
};

CONST MEDIA_OBJECT_WALKER_CMD_G9 g_cInit_MEDIA_OBJECT_WALKER_CMD_G9 = {
	// DWORD 0
	{
	 OP_LENGTH(SIZE32(MEDIA_OBJECT_WALKER_CMD_G9)),
	 MEDIASUBOP_MEDIA_OBJECT_WALKER,
	 GFXOP_NONPIPELINED,
	 PIPE_MEDIA,
	 INSTRUCTION_GFX},

	// DWORD 1
	{
	 0,
	 0,
	 0},

	// DWORD 2
	{
	 0,
	 0,
	 0,
	 0,
	 0,
	 0},

	// DWORD 3
	{
	 0},

	// DWORD 4
	{
	 0},

	// DWORD 5
	{
	 0,
	 0},

	// DWORD 6
	{
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0},

	// DWORD 7
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 8
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 9
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 10
	0,

	// DWORD 11
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 12
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 13
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 14
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 15
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 16
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 17
	{
	 0}

};

CONST PIPELINE_SELECT_CMD_G9 g_cInit_PIPELINE_SELECT_CMD_MEDIA_G9 = {
	// DWORD 0
	{
	 GFXPIPELINE_MEDIA,
	 0,
	 0,
	 0,
	 0,
	 0x3,
	 GFXSUBOP_PIPELINE_SELECT,
	 GFXOP_NONPIPELINED,
	 PIPE_SINGLE_DWORD,
	 INSTRUCTION_GFX}
};

CONST SURFACE_STATE_G9 g_cInit_SURFACE_STATE_G9 = {
	// DWORD 0
	{
	 FALSE,
	 FALSE,
	 FALSE,
	 FALSE,
	 FALSE,
	 FALSE,
	 GFX3DSTATE_BOUNDARY_NORMAL,
	 GFX3DSTATE_WRITE_ONLY_ON_MISS,
	 0,
	 0,
	 0,
	 0,
	 1,
	 1,
	 GFX3DSTATE_SURFACEFORMAT_R8_UNORM,
	 0,
	 0,
	 GFX3DSTATE_SURFACETYPE_2D},

	// DWORD 1
	{
	 0,
	 0,
	 0,
	 0,
	 0},

	// DWORD 2
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 3
	{
	 0,
	 0,
	 0},

	// DWORD 4
	{
	 0},

	// DWORD 5
	{
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0,
	 0},

	// DWORD 6
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 7
	{
	 0,
	 0,
	 SCS_ALPHA,
	 SCS_BLUE,
	 SCS_GREEN,
	 SCS_RED,
	 0,
	 0,
	 0},

	// DWORD 8
	{
	 0},

	// DWORD 9
	{
	 0,
	 0},

	// DWORD 10
	{
	 0,
	 0,
	 0,
	 0},

	// DWORD 11
	{
	 0,
	 0},

	// DWORD 12
	{
	 0},

	// DWORD 13
	{
	 0},

	// DWORD 14
	{
	 0},

	// DWORD 15
	{
	 0}
};
