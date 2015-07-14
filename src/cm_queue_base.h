/*
 * cm_queue_base.h
 *
 *  Created on: Jun 26, 2015
 *      Author: ichebyki
 */

#ifndef CM_QUEUE_BASE_H_
#define CM_QUEUE_BASE_H_

class CmTask;
class CmEvent;
class CmThreadGroupSpace;
class CmThreadSpace;

class CmQueue {
 public:
        virtual INT Enqueue(CmTask * pTask, CmEvent * &pEvent,
                    const CmThreadSpace * pTS = NULL) = 0;
        virtual INT DestroyEvent(CmEvent * &pEvent) = 0;

 public:
        virtual INT EnqueueWithGroup(CmTask * pTask, CmEvent * &pEvent,
                        const CmThreadGroupSpace * pTGS = NULL) = 0;
        virtual INT EnqueueWithHints(CmTask * pTask, CmEvent * &pEvent, UINT hints) = 0;
};


#endif /* CM_QUEUE_BASE_H_ */
