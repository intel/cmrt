/*
 * cm_kernel_base.h
 *
 *  Created on: Jun 26, 2015
 *      Author: ichebyki
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

};


#endif /* CM_KERNEL_BASE_H_ */
