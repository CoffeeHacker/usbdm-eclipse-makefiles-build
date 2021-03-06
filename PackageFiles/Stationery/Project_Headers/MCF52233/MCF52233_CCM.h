/********************************************************************/
/* Coldfire C Header File
 *
 *     Date      : 2009/04/30
 *     Revision  : 0.97
 *
 *     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 *     http      : www.freescale.com
 *     mail      : support@freescale.com
 */

#ifndef __MCF52233_CCM_H__
#define __MCF52233_CCM_H__


/*********************************************************************
*
* Chip Configuration Module (CCM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CCM_CCR                          (*(vuint16*)(0x40110004))
#define MCF_CCM_RCON                         (*(vuint16*)(0x40110008))
#define MCF_CCM_CIR                          (*(vuint16*)(0x4011000A))


/* Bit definitions and macros for MCF_CCM_CCR */
#define MCF_CCM_CCR_BMT(x)                   (((x)&0x7)<<0)
#define MCF_CCM_CCR_BMT_65536                (0)
#define MCF_CCM_CCR_BMT_32768                (0x1)
#define MCF_CCM_CCR_BMT_16384                (0x2)
#define MCF_CCM_CCR_BMT_8192                 (0x3)
#define MCF_CCM_CCR_BMT_4096                 (0x4)
#define MCF_CCM_CCR_BMT_2048                 (0x5)
#define MCF_CCM_CCR_BMT_1024                 (0x6)
#define MCF_CCM_CCR_BMT_512                  (0x7)
#define MCF_CCM_CCR_BME                      (0x8)
#define MCF_CCM_CCR_PSTEN                    (0x20)
#define MCF_CCM_CCR_SZEN                     (0x40)

/* Bit definitions and macros for MCF_CCM_RCON */
#define MCF_CCM_RCON_MODE                    (0x1)
#define MCF_CCM_RCON_RLOAD                   (0x20)

/* Bit definitions and macros for MCF_CCM_CIR */
#define MCF_CCM_CIR_PRN(x)                   (((x)&0x3F)<<0)
#define MCF_CCM_CIR_PIN(x)                   (((x)&0x3FF)<<0x6)


#endif /* __MCF52233_CCM_H__ */
