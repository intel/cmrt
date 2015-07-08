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

#ifndef __HW_CMD_G9_H__
#define __HW_CMD_G9_H__

#include "oscl_platform_def.h"
#include "hw_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _NAME_MERGE_(x, y)                      x ## y
#define _NAME_LABEL_(name, id)                  _NAME_MERGE_(name, id)
#define __CODEGEN_UNIQUE(name)                  _NAME_LABEL_(name, __LINE__)

	typedef struct _MEDIA_VFE_STATE_CMD_G9 {
		// DWORD 0
		union {
			struct {
				DWORD Length:BITFIELD_RANGE(0, 15);
				DWORD InstructionSubOpcode:BITFIELD_RANGE(16,
									  23);
				DWORD InstructionOpcode:BITFIELD_RANGE(24, 26);
				DWORD InstructionPipeline:BITFIELD_RANGE(27,
									 28);
				DWORD InstructionType:BITFIELD_RANGE(29, 31);
			};
			struct {
				DWORD Value;
			};
		} DW0;

		// DWORD 1
		union {
			struct {
				DWORD PerThreadScratchSpace:BITFIELD_RANGE(0,
									   3);
				DWORD StackSize:BITFIELD_RANGE(4, 7);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE(8,
									      9);
				DWORD ScratchSpaceBasePointer:BITFIELD_RANGE(10,
									     31);
			};
			struct {
				DWORD Value;
			};
		} DW1;

		// DWORD 2
		union {
			struct {
				DWORD
				    ScratchSpaceBasePointer64:BITFIELD_RANGE(0,
									     15);
				DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (16, 31);
			};
			struct {
				DWORD Value;
			};
		} DW2;

		// DWORD 3
		union {
			struct {
				DWORD DebugCounterControl:BITFIELD_RANGE(0, 1);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE(2,
									      5);
				DWORD BypassGatewayControl:BITFIELD_BIT(6);
				DWORD ResetGatewayTimer:BITFIELD_BIT(7);
				DWORD NumberofURBEntries:BITFIELD_RANGE(8, 15);
				DWORD MaximumNumberofThreads:BITFIELD_RANGE(16,
									    31);
			};
			struct {
				DWORD Value;
			};
		} DW3;

		// DWORD 4
		union {
			struct {
				DWORD SliceDisable:BITFIELD_RANGE(0, 1);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE(2,
									      7);
				DWORD DebugObjectID:BITFIELD_RANGE(8, 31);
			};
			struct {
				DWORD Value;
			};
		} DW4;

		// DWORD 5
		union {
			struct {
				DWORD CURBEAllocationSize:BITFIELD_RANGE(0, 15);
				DWORD URBEntryAllocationSize:BITFIELD_RANGE(16,
									    31);
			};
			struct {
				DWORD Value;
			};
		} DW5;

		// DWORD 6
		union {
			struct {
				DWORD ScoreboardMask:BITFIELD_RANGE(0, 7);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE(8,
									      29);
				DWORD ScoreboardType:BITFIELD_BIT(30);
				DWORD ScoreboardEnable:BITFIELD_BIT(31);
			};
			struct {
				DWORD Value;
			};
		} DW6;

		// DWORD 7
		union {
			struct {
				DWORD Scoreboard0DeltaX:BITFIELD_RANGE(0, 3);
				DWORD Scoreboard0DeltaY:BITFIELD_RANGE(4, 7);
				DWORD Scoreboard1DeltaX:BITFIELD_RANGE(8, 11);
				DWORD Scoreboard1DeltaY:BITFIELD_RANGE(12, 15);
				DWORD Scoreboard2DeltaX:BITFIELD_RANGE(16, 19);
				DWORD Scoreboard2DeltaY:BITFIELD_RANGE(20, 23);
				DWORD Scoreboard3DeltaX:BITFIELD_RANGE(24, 27);
				DWORD Scoreboard3DeltaY:BITFIELD_RANGE(28, 31);
			};
			struct {
				DWORD Value;
			};
		} DW7;

		// DWORD 8
		union {
			struct {
				DWORD Scoreboard4DeltaX:BITFIELD_RANGE(0, 3);
				DWORD Scoreboard4DeltaY:BITFIELD_RANGE(4, 7);
				DWORD Scoreboard5DeltaX:BITFIELD_RANGE(8, 11);
				DWORD Scoreboard5DeltaY:BITFIELD_RANGE(12, 15);
				DWORD Scoreboard6DeltaX:BITFIELD_RANGE(16, 19);
				DWORD Scoreboard6DeltaY:BITFIELD_RANGE(20, 23);
				DWORD Scoreboard7DeltaX:BITFIELD_RANGE(24, 27);
				DWORD Scoreboard7DeltaY:BITFIELD_RANGE(28, 31);
			};
			struct {
				DWORD Value;
			};
		} DW8;

	} MEDIA_VFE_STATE_CMD_G9, *PMEDIA_VFE_STATE_CMD_G9;

	 C_ASSERT(SIZE32(MEDIA_VFE_STATE_CMD_G9) == 9);

	typedef struct _MEDIA_OBJECT_WALKER_CMD_G9 {
		// DWORD 0
		union {
			struct {
				DWORD Length:16;
				DWORD InstructionSubOpcode:8;
				DWORD InstructionOpcode:3;
				DWORD InstructionPipeline:2;
				DWORD InstructionType:3;
			};
			struct {
				DWORD Value;
			};
		} DW0;

		// DWORD 1
		union {
			struct {
				DWORD InterfaceDescriptorOffset:6;
				DWORD __CODEGEN_UNIQUE(Reserved):2;
				DWORD ObjectID:24;
			};
			struct {
				DWORD Value;
			};
		} DW1;

		// DWORD 2
		union {
			struct {
				DWORD IndirectDataLength:17;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
				DWORD UseScoreboard:1;
				DWORD MarkedDispatch:2;
				DWORD ThreadSynchronization:1;
				DWORD __CODEGEN_UNIQUE(Reserved):7;
			};
			struct {
				DWORD Value;
			};
		} DW2;

		// DWORD 3
		union {
			struct {
				DWORD IndirectDataStartAddress;
			};
			struct {
				DWORD Value;
			};
		} DW3;

		// DWORD 4
		union {
			struct {
				DWORD __CODEGEN_UNIQUE(Reserved):32;
			};
			struct {
				DWORD Value;
			};
		} DW4;

		// DWORD 5
		union {
			struct {
				DWORD ScoreboardMask:8;
				DWORD GroupIdLoopSelect:24;
			};
			struct {
				DWORD Value;
			};
		} DW5;

		// DWORD 6
		union {
			struct {
				DWORD __CODEGEN_UNIQUE(Reserved):8;
				DWORD MidLoopUnitX:2;
				DWORD __CODEGEN_UNIQUE(Reserved):2;
				DWORD MidLoopUnitY:2;
				DWORD __CODEGEN_UNIQUE(Reserved):2;
				DWORD MidLoopExtraSteps:5;
				DWORD __CODEGEN_UNIQUE(Reserved):3;
				DWORD ColorCountMinusOne:4;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
			};
			struct {
				DWORD Value;
			};
		} DW6;

		// DWORD 7
		union {
			struct {
				DWORD LocalLoopExecCount:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
				DWORD GlobalLoopExecCount:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
			};
			struct {
				DWORD Value;
			};
		} DW7;

		// DWORD 8
		union {
			struct {
				DWORD BlockResolutionX:11;
				DWORD __CODEGEN_UNIQUE(Reserved):5;
				DWORD BlockResolutionY:11;
				DWORD __CODEGEN_UNIQUE(Reserved):5;
			};
			struct {
				DWORD Value;
			};
		} DW8;

		// DWORD 9
		union {
			struct {
				DWORD LocalStartX:11;
				DWORD __CODEGEN_UNIQUE(Reserved):5;
				DWORD LocalStartY:11;
				DWORD __CODEGEN_UNIQUE(Reserved):5;
			};
			struct {
				DWORD Value;
			};
		} DW9;

		// DWORD 10
		union {
			struct {
				DWORD Reserved:32;
			};
			struct {
				DWORD Value;
			};
		} DW10;

		// DWORD 11
		union {
			struct {
				DWORD LocalOuterLoopStrideX:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
				DWORD LocalOuterLoopStrideY:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
			};
			struct {
				DWORD Value;
			};
		} DW11;

		// DWORD 12
		union {
			struct {
				DWORD LocalInnerLoopUnitX:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
				DWORD LocalInnerLoopUnitY:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
			};
			struct {
				DWORD Value;
			};
		} DW12;

		// DWORD 13
		union {
			struct {
				DWORD GlobalResolutionX:11;
				DWORD __CODEGEN_UNIQUE(Reserved):5;
				DWORD GlobalResolutionY:11;
				DWORD __CODEGEN_UNIQUE(Reserved):5;
			};
			struct {
				DWORD Value;
			};
		} DW13;

		// DWORD 14
		union {
			struct {
				DWORD GlobalStartX:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
				DWORD GlobalStartY:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
			};
			struct {
				DWORD Value;
			};
		} DW14;

		// DWORD 15
		union {
			struct {
				DWORD GlobalOuterLoopStrideX:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
				DWORD GlobalOuterLoopStrideY:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
			};
			struct {
				DWORD Value;
			};
		} DW15;

		// DWORD 16
		union {
			struct {
				DWORD GlobalInnerLoopUnitX:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
				DWORD GlobalInnerLoopUnitY:12;
				DWORD __CODEGEN_UNIQUE(Reserved):4;
			};
			struct {
				DWORD Value;
			};
		} DW16;

		// DWORD 17
		union {
			struct {
				DWORD InlineData:32;
			};
			struct {
				DWORD Value;
			};
		} DW17;
	} MEDIA_OBJECT_WALKER_CMD_G9, *PMEDIA_OBJECT_WALKER_CMD_G9;

	 C_ASSERT(SIZE32(MEDIA_OBJECT_WALKER_CMD_G9) == 18);

	typedef struct _PIPELINE_SELECT_CMD_G9 {
		// DWORD 0
		union {
			struct {
				DWORD PipelineSelect:BITFIELD_RANGE(0, 1);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE(2,
									      3);
				DWORD SamplerDOPClockGateEnable:BITFIELD_BIT(4);
				DWORD ForceMediaAwake:BITFIELD_BIT(5);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE(6,
									      7);
				DWORD MaskBits:BITFIELD_RANGE(8, 15);
				DWORD InstructionSubOpcode:BITFIELD_RANGE(16,
									  23);
				DWORD InstructionOpcode:BITFIELD_RANGE(24, 26);
				DWORD CommandSubType:BITFIELD_RANGE(27, 28);
				DWORD InstructionType:BITFIELD_RANGE(29, 31);
			};
			struct {
				DWORD Value;
			};
		} DW0;
	} PIPELINE_SELECT_CMD_G9, *PPIPELINE_SELECT_CMD_G9;

	 C_ASSERT(SIZE32(PIPELINE_SELECT_CMD_G9) == 1);

	typedef struct _SURFACE_STATE_G9 {
		// DWORD 0
		union {
			struct {
				DWORD CubeFaceEnablesPositiveZ:BITFIELD_BIT(0);
				DWORD CubeFaceEnablesNegativeZ:BITFIELD_BIT(1);
				DWORD CubeFaceEnablesPositiveY:BITFIELD_BIT(2);
				DWORD CubeFaceEnablesNegativeY:BITFIELD_BIT(3);
				DWORD CubeFaceEnablesPositiveX:BITFIELD_BIT(4);
				DWORD CubeFaceEnablesNegativeX:BITFIELD_BIT(5);
				DWORD MediaBoundaryPixelMode:BITFIELD_RANGE(6,
									    7);
				DWORD RenderCacheReadWriteMode:BITFIELD_BIT(8);
				 DWORD
				    SamplerL2BypassModeDisable:BITFIELD_BIT(9);
				DWORD VerticalLineStrideOffset:BITFIELD_BIT(10);
				DWORD VerticalLineStride:BITFIELD_BIT(11);
				DWORD TileMode:BITFIELD_RANGE(12, 13);
				 DWORD
				    SurfaceHorizontalAlignment:BITFIELD_RANGE
				    (14, 15);
				 DWORD
				    SurfaceVerticalAlignment:BITFIELD_RANGE(16,
									    17);
				DWORD SurfaceFormat:BITFIELD_RANGE(18, 26);
				DWORD ASTC_Enable:BITFIELD_BIT(27);
				DWORD SurfaceArray:BITFIELD_BIT(28);
				DWORD SurfaceType:BITFIELD_RANGE(29, 31);
			};
			struct {
				DWORD Value;
			};
		} DW0;

		// DWORD 1
		union {
			struct {
				DWORD SurfaceQPitch:BITFIELD_RANGE(0, 14);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (15, 18);
				DWORD BaseMipLevel:BITFIELD_RANGE(19, 23);
				DWORD SurfaceMemObjCtrlState:BITFIELD_RANGE(24,
									    30);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_BIT(31);
			};
			struct {
				DWORD Value;
			};
		} DW1;

		// DWORD 2
		union {
			struct {
				DWORD Width:BITFIELD_RANGE(0, 13);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (14, 15);
				DWORD Height:BITFIELD_RANGE(16, 29);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (30, 31);
			};
			struct {
				DWORD Value;
			};
		} DW2;

		// DWORD 3
		union {
			struct {
				DWORD SurfacePitch:BITFIELD_RANGE(0, 17);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (18, 20);
				DWORD Depth:BITFIELD_RANGE(21, 31);
			};
			struct {
				DWORD Value;
			};
		} DW3;

		// DWORD 4
		union {
			struct {
				DWORD
				    MultiSamplePositionPaletteIndex:BITFIELD_RANGE
				    (0, 2);
				DWORD NumberofMultiSamples:BITFIELD_RANGE(3, 5);
				 DWORD
				    MultiSampledSurfaceStorageFormat:BITFIELD_BIT
				    (6);
				DWORD RenderTargetViewExtent:BITFIELD_RANGE(7,
									    17);
				DWORD MinimumArrayElement:BITFIELD_RANGE(18,
									 28);
				DWORD RenderTargetRotation:BITFIELD_RANGE(29,
									  30);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_BIT(31);
			};
			struct {
				DWORD MinArrrayElement:BITFIELD_RANGE(0, 26);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (27, 31);
			};
			struct {
				DWORD Value;
			};
		} DW4;

		// DWORD 5
		union {
			struct {
				DWORD MipCount:BITFIELD_RANGE(0, 3);
				DWORD SurfaceMinLOD:BITFIELD_RANGE(4, 7);
				DWORD MipTailStartLOD:BITFIELD_RANGE(8, 11);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (12, 13);
				DWORD CoherencyType:BITFIELD_BIT(14);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (15, 17);
				DWORD TiledResourceMode:BITFIELD_RANGE(18, 19);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_BIT(20);
				DWORD YOffset:BITFIELD_RANGE(21, 23);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_BIT(24);
				DWORD XOffset:BITFIELD_RANGE(25, 31);
			};
			struct {
				DWORD Value;
			};
		} DW5;

		// DWORD 6
		union {
			struct {
				DWORD AuxSurfaceMode:BITFIELD_RANGE(0, 1);
				DWORD RTCompressionEnable:BITFIELD_BIT(2);
				DWORD AuxSurfacePitch:BITFIELD_RANGE(3, 11);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (12, 15);
				DWORD AuxSurfaceQPitch:BITFIELD_RANGE(16, 30);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_BIT(31);
			};
			struct {
				DWORD YOffsetUOrUVPlane:BITFIELD_RANGE(0, 13);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (14, 15);
				DWORD XOffsetUOrUVPlane:BITFIELD_RANGE(16, 29);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_BIT(30);
				DWORD SeperateUVPlaneEnable:BITFIELD_BIT(31);
			};
			struct {
				DWORD Value;
			};
		} DW6;

		// DWORD 7
		union {
			struct {
				DWORD ResourceMinLOD:BITFIELD_RANGE(0, 11);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (12, 15);
				DWORD ShaderChannelSelectA:BITFIELD_RANGE(16,
									  18);
				DWORD ShaderChannelSelectB:BITFIELD_RANGE(19,
									  21);
				DWORD ShaderChannelSelectG:BITFIELD_RANGE(22,
									  24);
				DWORD ShaderChannelSelectR:BITFIELD_RANGE(25,
									  27);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (28, 29);
				DWORD MemCompressionEnable:BITFIELD_BIT(30);
				DWORD MemCompressionMode:BITFIELD_BIT(31);
			};
			struct {
				DWORD Value;
			};
		} DW7;

		// DWORD 8
		union {
			struct {
				DWORD SurfaceBaseAddress;
			};
			struct {
				DWORD Value;
			};
		} DW8;

		// DWORD 9
		union {
			struct {
				DWORD SurfaceBaseAddress64:BITFIELD_RANGE(0,
									  15);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (16, 31);
			};
			struct {
				DWORD Value;
			};
		} DW9;

		// DWORD 10
		union {
			struct {
				DWORD QuiltWidth:BITFIELD_RANGE(0, 4);
				DWORD QuiltHeight:BITFIELD_RANGE(5, 9);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (10, 11);
				DWORD AuxSurfaceBaseAddress:BITFIELD_RANGE(12,
									   31);
			};
			struct {
				DWORD Value;
			};
		} DW10;

		// DWORD 11
		union {
			struct {
				DWORD AuxSurfaceBaseAddress64:BITFIELD_RANGE(0,
									     15);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (16, 31);
			};
			struct {
				DWORD YOffsetVPlane:BITFIELD_RANGE(0, 13);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (14, 15);
				DWORD XOffsetVPlane:BITFIELD_RANGE(16, 29);
				 DWORD
				    __CODEGEN_UNIQUE(Reserved):BITFIELD_RANGE
				    (30, 31);
			};
			struct {
				DWORD Value;
			};
		} DW11;

		// DWORD 12
		union {
			struct {
				DWORD HierarchicalDepthClear;
			};
			struct {
				DWORD RedClearColor;
			};
			struct {
				DWORD Value;
			};
		} DW12;

		// DWORD 13
		union {
			struct {
				DWORD Reserved;
			};
			struct {
				DWORD GreenClearColor;
			};
			struct {
				DWORD Value;
			};
		} DW13;

		// DWORD 14
		union {
			struct {
				DWORD Reserved;
			};
			struct {
				DWORD BlueClearColor;
			};
			struct {
				DWORD Value;
			};
		} DW14;

		// DWORD 15
		union {
			struct {
				DWORD Reserved;
			};
			struct {
				DWORD AlphaClearColor;
			};
			struct {
				DWORD Value;
			};
		} DW15;
	} SURFACE_STATE_G9, *PSURFACE_STATE_G9;

	 C_ASSERT(SIZE32(SURFACE_STATE_G9) == 16);

	typedef struct _PACKET_SURFACE_STATE_G9 {
		SURFACE_STATE_TOKEN_G75 Token;
		SURFACE_STATE_G9 cmdSurfaceState_g9;
	} PACKET_SURFACE_STATE_G9, *PPACKET_SURFACE_STATE_G9;

	extern CONST PIPELINE_SELECT_CMD_G9
	    g_cInit_PIPELINE_SELECT_CMD_MEDIA_G9;
	extern CONST SURFACE_STATE_G9 g_cInit_SURFACE_STATE_G9;
	extern CONST MEDIA_OBJECT_WALKER_CMD_G9
	    g_cInit_MEDIA_OBJECT_WALKER_CMD_G9;
	extern CONST MEDIA_VFE_STATE_CMD_G9 g_cInit_MEDIA_VFE_STATE_CMD_G9;

#ifdef __cplusplus
}
#endif
#endif
