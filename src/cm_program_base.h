/*
 * cm_program_base.h
 *
 *  Created on: Jun 26, 2015
 *      Author: ichebyki
 */

#ifndef CM_PROGRAM_BASE_H_
#define CM_PROGRAM_BASE_H_


class CmProgram {
 public:
        virtual INT GetKernelCount(UINT & kernelCount) = 0;
};


#endif /* CM_PROGRAM_BASE_H_ */
