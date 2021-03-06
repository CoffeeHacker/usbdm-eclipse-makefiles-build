/*! \file
   \brief Entry points for USBDM library

   USBDM_GDI.cpp

   \verbatim
   USBDM GDI interface DLL
   Copyright (C) 2008  Peter O'Donoghue

    This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
\endverbatim
\verbatim
Change History
-============================================================================================
|  Dec 21 2012 | Fixed Reset PC Hack in HCS08 & CFV1                               - pgo V4.10.4
|  Dec 21 2012 | Removed Mass Erase as separate operation                          - pgo V4.10.4
|  Nov 30 2012 | Changed logging                                                   - pgo V4.10.4
|  Oct  8 2012 | Modified ARM accesses to use maximum aligned sizes                - pgo V4.10.2
|  May  7 2012 | Changed to using RESET_DEFAULT for ARM in DiExecResetChild()      - pgo V4.9.5
|  Mar 30 2012 | Fixed block address error in DIMemoryWrite() when flashing        - pgo V4.9.4
|  Mar 30 2012 | Changed to USBDM_SetExtendedOptions() etc.                        - pgo V4.9.4
|  Mar 24 2012 | Added actions to PROCESS_DETACH (for DSC programmer)              - pgo V4.9.4
|  Jan 14 2012 | Ported to DSC                                                     - pgo V4.9
|  Jul 16 2011 | Extended TargetConnect strategies & Messages HCSxx                - pgo V4.7
|  Jul  3 2011 | Changes to DSC code in DLL                                        - pgo V4.7
|  Jul  3 2011 | Changes related to CFVx recovery from sleeping                    - pgo V4.7
|  Jul  2 2011 | Added halt/connect in DiExecResetChild() - HCS08                  - pgo V4.7
|  Jun 30 2011 | Added checks for WAIT/WFE states                                  - pgo V4.7
|  Jun 30 2011 | Re-arranged connection code to avoid spurious secured error       - pgo V4.7
|  Jun    2011 | Added autoconnect on DiGdiGetStatus                               - pgo
+============================================================================================
\endverbatim
*/

#include "Common.h"
#include "Log.h"
#include "Version.h"
#include "USBDM_AUX.h"
#include "USBDM_API.h"
#include "USBDM_GDI.h"
#include "Metrowerks.h"
#include "MetrowerksInterface.h"
#include "Conversion.h"
#include "TargetDefines.h"
#include "Names.h"

#include "osbdm_cfv234.h"

const TargetType_t targetType = T_CFVx;

const DiFeaturesT diFeatures = {
  /* .szIdentification                 = */ (DiStringT)"USBDM CFVx",
  /* .szVersion                        = */ (DiStringT)"1.2.6",
  /* .dnVersion                        = */ (1<<16)+(2<<8)+6,
  /* .pszConfig                        = */ NULL,
  /* .dnConfigArrayItems               = */ 0,
  /* .dccIOChannel                     = */ DI_COMM_NONE,
  /* .fMemorySetMapAvailable           = */ FALSE,
  /* .fMemorySetCpuMapAvailable        = */ FALSE,
  /* .pszMemoryType                    = */ NULL,
  /* .dnMemTypeArrayItems              = */ 0,
  /* .fEnableReadaheadCache            = */ FALSE,
  /* .fTimerInCycles                   = */ FALSE,
  /* .dnTimerResolutionMantissa        = */ 128,
  /* .dnTimerResolutionExponent        = */ 0,
  /* .ddfDownloadFormat = */    {
                  /* .dafAbsFileFormat = */ DI_ABSF_BINARY,
                  /* .dnBufferSize     = */ 1000,
                  /* .daAddress        = */ {0},
                 },
  /* .ddfAuxDownloadFormat = */ {
                  /* .dafAbsFileFormat = */ DI_ABSF_NONE,
                  /* .dnBufferSize     = */ 0,
                  /* .daAddress        = */ {0,{{{0}}}},
                 },
  /* .fAuxiliaryDownloadPathAvailable  = */ FALSE,
  /* .dcCallback                       = */ DI_CB_MTWKS_EXTENSION|DI_CB_DEBUG|DI_CB_LOG,
  /* .fRegisterClassSupport            = */ FALSE,
  /* .fSingleStepSupport               = */ TRUE,
  /* .fContinueUntilSupport            = */ FALSE,
  /* .fContinueBackgroundSupport       = */ FALSE,
  /* .dnNrCodeBpAvailable              = */ 0, // Code breakpoints
  /* .dnNrDataBpAvailable              = */ 0, // Data read/write breakpoints
  /* .fExecFromCodeBp                  = */ FALSE,
  /* .fExecFromDataBp                  = */ FALSE,
  /* .fUnifiedBpLogic                  = */ TRUE,
  /* .fExecCycleCounterAvailable       = */ FALSE,
  /* .fExecTimeAvailable               = */ FALSE,
  /* .fInstrTraceAvailable             = */ FALSE,
  /* .fRawTraceAvailable               = */ FALSE,
  /* .fCoverageAvailable               = */ FALSE,
  /* .fProfilingAvailable              = */ FALSE,
  /* .fStateSaveRestoreAvailable       = */ FALSE,
  /* .dnStateStoreMaxIndex             = */ 0,
  /* .pdbgBackground                   = */ NULL,
  /* .dnBackgroundArrayItems           = */ 0,
  /* .fDirectDiAccessAvailable         = */ FALSE,  // Direct commands not available
  /* .fApplicationIOAvailable          = */ FALSE,
  /* .fKernelAware                     = */ FALSE,
#ifdef USE_MEE
  /* .fMeeAvailable                    = */ TRUE,
#else
  /* .fMeeAvailable                    = */ FALSE,
#endif
  /* .dnNrCpusAvailable                = */ 1,
  /* .deWordEndianness                 = */ DI_BIG_ENDIAN,
  /* .dnNrHardWareCodeBpAvailable      = */ 0, // No hardware code breakpoints!
  /* .fCodeHardWareBpSkids             = */ FALSE,
  /* .pReserved                        = */ NULL,
};

#if TARGET == ARM
// ARM Register numbers used by Codewarrior
//
#define arm_RegPRIMASK 19
#define arm_RegFAULTMASK  20
#define arm_RegBASEPRI 21
#define arm_RegCONTROL 22
#endif

//! 2.2.6.1 Write Value to Register
//!
//! @param dnRegNumber
//! @param drvValue
//!
USBDM_GDI_API
DiReturnT DiRegisterWrite ( DiUInt32T        dnRegNumber,
                            DiRegisterValueT drvValue ) {
   LOGGING;
   U32c  value(drvValue);
   USBDM_ErrorCode rc = BDM_RC_OK;

//   Logging::print("DiRegisterWrite(0x%X(%d) <= 0x%08X)\n", dnRegNumber, dnRegNumber, (uint32_t)value);

   CHECK_ERROR_STATE();

   if (dnRegNumber>cfv234regID_FIRST_DEBUG_REG) {
      dnRegNumber -= cfv234regID_FIRST_DEBUG_REG;
      Logging::print("DiRegisterWriteD(0x%X(%s) <= 0x%08X)\n", dnRegNumber, getCFVxDebugRegName(dnRegNumber), (uint32_t)value);
      rc = USBDM_WriteDReg(dnRegNumber,value);
   }
   else if (dnRegNumber > cfv234regID_FIRST_CONTROL_REG) {
      dnRegNumber -= cfv234regID_FIRST_CONTROL_REG;
      Logging::print("DiRegisterWriteC(0x%X(%s) <= 0x%08X)\n", dnRegNumber, getCFVxControlRegName(dnRegNumber), (uint32_t)value);
      rc = USBDM_WriteCReg(dnRegNumber,value);
   }
   else {
      switch (dnRegNumber) {
         case cfv234regID_pc :
            Logging::print("DiRegisterWrite(0x%X(%s) <= 0x%08X)\n", CFVx_CRegPC, getCFVxControlRegName(CFVx_CRegPC), (uint32_t)value);
            rc = USBDM_WriteCReg(CFVx_CRegPC,value);
            break;
         case cfv234regID_sr :
            Logging::print("DiRegisterWrite(0x%X(%s) <= 0x%08X)\n", CFVx_CRegSR, getCFVxControlRegName(CFVx_CRegSR), (uint32_t)value);
            rc = USBDM_WriteCReg(CFVx_CRegSR,value);
            break;
         default : // D0-7, A0-7
            if (dnRegNumber<=cfv234regID_a7) {
               Logging::print("DiRegisterWrite(0x%X(%s) <= 0x%08X)\n", dnRegNumber, getCFVxRegName(dnRegNumber), (uint32_t)value);
               rc = USBDM_WriteReg(dnRegNumber,value);
            }
            else {
               Logging::print("DiRegisterWrite(illegal reg# = 0x%X (%d)\n", dnRegNumber, dnRegNumber);
               rc = BDM_RC_ILLEGAL_PARAMS;
            }
            break;
      }
   }
   if (rc != BDM_RC_OK) {
//      Logging::error("DiRegisterWrite(0x%X,%s) Failed, reason= %s\n",
//            dnRegNumber, DSC_GetRegisterName(regNum), USBDM_GetErrorString(rc));
      return setErrorState(DI_ERR_NONFATAL, rc);
   }
   return setErrorState(DI_OK);
}

//! 2.2.6.2 Read Value from Register
//!
//! @param dnRegNumber
//! @param drvValue
//!
USBDM_GDI_API
DiReturnT DiRegisterRead ( DiUInt32T         dnRegNumber,
                           pDiRegisterValueT drvValue ) {
unsigned long dataValue = 0xDEADBEEF;
USBDM_ErrorCode rc = BDM_RC_OK;

   Logging::print("DiRegisterRead(0x%X(%d))\n", dnRegNumber, dnRegNumber);

   CHECK_ERROR_STATE();

   if (dnRegNumber>=cfv234regID_FIRST_DEBUG_REG) {
      dnRegNumber -= cfv234regID_FIRST_DEBUG_REG;
      rc = USBDM_ReadDReg(dnRegNumber,&dataValue);
      Logging::print("DiRegisterRead(0x%X(%s) => 0x%08X)\n", dnRegNumber, getCFVxDebugRegName(dnRegNumber), (uint32_t)dataValue);
   }
   else if (dnRegNumber >= cfv234regID_FIRST_CONTROL_REG) {
      dnRegNumber -= cfv234regID_FIRST_CONTROL_REG;
      rc = USBDM_ReadCReg(dnRegNumber,&dataValue);
      Logging::print("DiRegisterRead(0x%X(%s) => 0x%08X)\n", dnRegNumber, getCFVxControlRegName(dnRegNumber), (uint32_t)dataValue);
   }
   else {
      switch (dnRegNumber) {
         case cfv234regID_pc :
            rc = USBDM_ReadCReg(CFVx_CRegPC,&dataValue);
            Logging::print("DiRegisterRead(0x%X(%s) => 0x%08X)\n", CFVx_CRegPC, getCFVxControlRegName(CFVx_CRegPC), (uint32_t)dataValue);
            break;
         case cfv234regID_sr :
            rc = USBDM_ReadCReg(CFVx_CRegSR,&dataValue);
            Logging::print("DiRegisterRead(0x%X(%s) => 0x%08X)\n", CFVx_CRegSR, getCFVxControlRegName(CFVx_CRegSR), (uint32_t)dataValue);
            break;
         default : // D0-7, A0-7
            if (dnRegNumber<=cfv234regID_a7) {
               rc = USBDM_ReadReg(dnRegNumber,&dataValue);
               Logging::print("DiRegisterRead(0x%X(%s) => 0x%08X)\n", dnRegNumber, getCFVxRegName(dnRegNumber), (uint32_t)dataValue);
            }
            else {
               Logging::print("DiRegisterRead(Illegal reg# = 0x%X (%d)\n", dnRegNumber, dnRegNumber);
               rc = BDM_RC_ILLEGAL_PARAMS;
            }
            break;
      }
   }
   if (rc != BDM_RC_OK) {
      Logging::print("DiRegisterRead(0x%X) => error\n", dnRegNumber);
      return setErrorState(DI_ERR_NONFATAL, rc);
   }
   *drvValue = (U32c)dataValue;
   Logging::print("0x%X(%d) => 0x%08X\n", dnRegNumber, dnRegNumber, dataValue);
   return setErrorState(DI_OK);
}

//!  2.2.7.1 Set Breakpoint
//!
//! @param pdnBreakpointId
//! @param dbBreakpoint
//!
USBDM_GDI_API
DiReturnT DiBreakpointSet ( DiBpResultT *pdnBreakpointId,
                             DiBpT        dbBreakpoint ) {

   Logging::print("DiBreakpointSet() - not implemented\n");
   return setErrorState(DI_ERR_NOTSUPPORTED);
}

//!  2.2.7.2 Clear a Breakpoint
//!
//! @param dnBreakpointId
//!
USBDM_GDI_API
DiReturnT DiBreakpointClear ( DiUInt32T dnBreakpointId ) {

   Logging::print("DiBreakpointClear() - not implemented\n");
   return setErrorState(DI_ERR_NOTSUPPORTED);
}

//!  2.2.7.3 Clear All Breakpoints
//!
USBDM_GDI_API
DiReturnT DiBreakpointClearAll ( void ) {

   Logging::print("DiBreakpointClearAll() - not implemented\n");
   return setErrorState(DI_ERR_NOTSUPPORTED);
}

//!  2.2.8.2 Execute a Single Step
//!
//! @param dnNrInstructions
//!
USBDM_GDI_API
DiReturnT DiExecSingleStep ( DiUInt32T dnNrInstructions ) {
USBDM_ErrorCode BDMrc;

   Logging::print("DiExecSingleStep(%d)\n", dnNrInstructions);

   CHECK_ERROR_STATE();

#if (TARGET == MC56F80xx)
   BDMrc = DSC_TargetStepN(dnNrInstructions);
#else
   if (dnNrInstructions>1) {
      Logging::print("DiExecSingleStep() - Only a single step is supported!\n");
      return setErrorState(DI_ERR_PARAM, ("Only a single step is allowed"));
   }
   BDMrc = USBDM_TargetStep();
#endif

   if (BDMrc != BDM_RC_OK) {
      return setErrorState(DI_ERR_NONFATAL, BDMrc);
   }
   return setErrorState(DI_OK);
}

//!  2.2.8.6 Get DI Execution/Exit Status
//!
//! @param pdesExitStatus
//!
//! @return \n
//!     DI_OK              => OK \n
//!     DI_ERR_FATAL       => Error see \ref currentErrorString
//!
USBDM_GDI_API
DiReturnT DiExecGetStatus ( pDiExitStatusT pdesExitStatus ) {
   LOGGING;
   USBDM_ErrorCode BDMrc;
   static DiExitCauseT lastStatus = DI_WAIT_USER;

//   Logging::print("DiExecGetStatus()\n");

   // Defaults
   pdesExitStatus->dscCause = DI_WAIT_UNKNOWN;
   pdesExitStatus->dwBpId   = 0x1000400; // bkpt ID?
   pdesExitStatus->szReason = (DiStringT)"unknown state";

// Removed as prevents CW retry strategy
//   CHECK_ERROR_STATE();

   if (bdmOptions.autoReconnect) {
      USBDM_ErrorCode bdmRc = targetConnect(softConnectOptions);

      if (bdmRc != BDM_RC_OK) {
         Logging::print("DiExecGetStatus()=> connect failed\n");
         return setErrorState(DI_ERR_COMMUNICATION, bdmRc);
      }
   }
   else {
      USBDM_GetBDMStatus(&USBDMStatus);
   }
//   pdesExitStatus->szReason = (DiStringT)getBDMStatusName(&USBDMStatus);

   if (USBDMStatus.reset_recent == RESET_DETECTED) {
      Logging::print("DiExecGetStatus()=>Target has been reset\n");
      mtwksDisplayLine("Target RESET detected\n");
   }
   if (bdmOptions.usePSTSignals) {
      // Check processor state using PST signals
      if (USBDMStatus.halt_state) {
         // Processor halted
         pdesExitStatus->dscCause = DI_WAIT_UNKNOWN;
         pdesExitStatus->szReason = (DiStringT)"Debug Halted";
         if (lastStatus != pdesExitStatus->dscCause) {
            Logging::print("DiExecGetStatus(PST) status change => DI_WAIT_UNKNOWN, (%s)\n",
                  pdesExitStatus->szReason);
            }
      }
      else {
         // Processor executing
         pdesExitStatus->dscCause = DI_WAIT_RUNNING;
         pdesExitStatus->szReason = (DiStringT)"Running";
         if (lastStatus != pdesExitStatus->dscCause) {
            Logging::print("DiExecGetStatus(PST) status change => DI_WAIT_RUNNING, (%s)\n",
                  pdesExitStatus->szReason);
            }
      }
   }
   else {
      // Probe D0 register - if fail assume processor running!
      // BUG - If stopped D0 can still be read so CW thinks the target is halted
      //       but is still running.
      unsigned long int dummy;
      BDMrc = USBDM_ReadReg(CFVx_RegD0, &dummy);
      if (BDMrc == BDM_RC_OK) {
         // Processor halted
         pdesExitStatus->dscCause = DI_WAIT_UNKNOWN;
         pdesExitStatus->szReason = (DiStringT)"Debug Halted";
         if (lastStatus != pdesExitStatus->dscCause) {
            Logging::print("DiExecGetStatus() status change => DI_WAIT_UNKNOWN, (%s)\n",
                  pdesExitStatus->szReason);
         }
      }
      else {
         // Processor executing
         pdesExitStatus->dscCause = DI_WAIT_RUNNING;
         pdesExitStatus->szReason = (DiStringT)"Running";
         if (lastStatus != pdesExitStatus->dscCause) {
            Logging::print("DiExecGetStatus() status change => DI_WAIT_RUNNING, (%s)\n",
                  pdesExitStatus->szReason);
         }
      }
   }
   lastStatus = pdesExitStatus->dscCause;
   return setErrorState(DI_OK);
}
