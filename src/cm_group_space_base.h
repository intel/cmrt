/*
 * cm_group_space_base.h
 *
 *  Created on: Jun 26, 2015
 *      Author: ichebyki
 */

#ifndef CM_GROUP_SPACE_BASE_H_
#define CM_GROUP_SPACE_BASE_H_


class CmThreadGroupSpace {
 public:
        virtual INT GetThreadGroupSpaceSize(UINT & threadSpaceWidth,
                                    UINT & threadSpaceHeight,
                                    UINT & groupSpaceWidth,
                                    UINT & groupSpaceHeight) const = 0;

        virtual ~CmThreadGroupSpace() {};
};


#endif /* CM_GROUP_SPACE_BASE_H_ */
