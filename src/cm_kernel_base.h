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
 *	Igor Chebykin <igor.v.chebykin@intel.com>
 */

#ifndef CM_KERNEL_BASE_H_
#define CM_KERNEL_BASE_H_


class CmKernel {
 public:
        virtual INT SetThreadCount(UINT count) = 0;
        virtual INT SetKernelArg(UINT index, size_t size, const void *pValue) = 0;

        virtual INT SetThreadArg(UINT threadId, UINT index, size_t size,
                         const void *pValue) = 0;

        virtual INT SetStaticBuffer(UINT index, const void *pValue) = 0;
        virtual INT AssociateThreadSpace(CmThreadSpace * &pTS) = 0;
        virtual INT AssociateThreadGroupSpace(CmThreadGroupSpace * &pTGS) = 0;
        virtual INT SetSurfaceBTI(SurfaceIndex * pSurface, UINT BTIndex) = 0;
		virtual INT GetArgKind(UINT index, WORD &kind) = 0;
        virtual ~CmKernel() {};
};


#endif /* CM_KERNEL_BASE_H_ */
