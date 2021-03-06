/*
 * gdbHandler.h
 *
 *  Created on: 22/05/2011
 *      Author: podonoghue
 */

#ifndef GDBHANDLER_H_
#define GDBHANDLER_H_
#include "GdbInOut.h"

enum GdbTargetStatus {
   T_NOCONNECTION   = -1,
   T_UNKNOWN =  0, // Unknown - error
   T_RUNNING,      // Executing
   T_HALT,         // Debug halt
   T_SLEEPING,     // Low power sleep
};

enum GdbMessageLevel {
   M_BORINGINFO = 0,
   M_INFO,
   M_WARN,
   M_ERROR,
   M_FATAL,
};

typedef USBDM_ErrorCode (*GdbCallback)(const char *msg, GdbMessageLevel level, USBDM_ErrorCode rc);

USBDM_ErrorCode gdbHandlerInit(GdbInOut *gdbInOut, DeviceData &deviceData, GdbCallback callback = NULL);
void gdbReportError(USBDM_ErrorCode rc);
USBDM_ErrorCode doGdbCommand(const GdbPacket *pkt);
GdbTargetStatus getTargetStatus();
GdbTargetStatus gdbPollTarget(void);

#endif /* GDBHANDLER_H_ */
