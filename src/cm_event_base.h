/*
 * cm_event_base.h
 *
 *  Created on: Jun 26, 2015
 *      Author: ichebyki
 */

#ifndef CM_EVENT_BASE_H_
#define CM_EVENT_BASE_H_


class CmEvent {
 public:
        virtual INT GetStatus(CM_STATUS & status) = 0;
        virtual INT GetExecutionTime(UINT64 & time) = 0;
        virtual INT WaitForTaskFinished(DWORD dwTimeOutMs = CM_MAX_TIMEOUT_MS) = 0;

};


#endif /* CM_EVENT_BASE_H_ */
