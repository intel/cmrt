/*
 * cm_surface_2d_up_base.h
 *
 *  Created on: Jun 28, 2015
 *      Author: ichebyki
 */

#ifndef CM_SURFACE_2D_UP_BASE_H_
#define CM_SURFACE_2D_UP_BASE_H_


class CmSurface2DUP {
 public:
        virtual INT GetIndex(SurfaceIndex * &pIndex) = 0;

        virtual INT GetSurfaceDesc(UINT & width, UINT & height,
                           CM_SURFACE_FORMAT & format, UINT & sizeperpixel) = 0;

};


#endif /* CM_SURFACE_2D_UP_BASE_H_ */
