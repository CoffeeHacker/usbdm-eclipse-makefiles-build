/*! \file
    \brief Version information
*/

//! Program versions e.g. Flash programmers
//#define USBDM_VERSION	     0x4A    //!< Version of the DLL in Hex
#define USBDM_VERSION_MAJOR  4       //!< DLL Major version #
#define USBDM_VERSION_MINOR  10      //!< DLL Minor version #
#define USBDM_VERSION_MICRO  6       //!< DLL Micro version #
#define USBDM_VERSION_NANO   00      //!< DLL Nano version #

#ifdef LOG
#define USBDM_VERSION_STRING "4.10.6.0-Debug" //!< Version as string
#else
#define USBDM_VERSION_STRING "4.10.6.0"       //!< Version as string
#endif

// The version of the GDI standard implemented
#define USBDM_GDI_INTERFACE_VERSION (0x126)

//! Name of directory to create in %APPDATA% for usbdm settings
#define CONFIGURATION_DIRECTORY_NAME "usbdm"

//----------------------------------------------------------------------------
// Revision History
//-----------+----------------------------------------------------------------
//    May 13 | 4.10.5 Release                                            - pgo
//-----------+----------------------------------------------------------------
//    Oct 12 | 4.10.3 Improved USB Open messages                         - pgo
//-----------+----------------------------------------------------------------
//    Oct 12 | 4.10.2 Codewarrior 10.3 release                           - pgo
//-----------+----------------------------------------------------------------
//    Sep 12 | 4.10.0 Major changes for ARM-JTAG and added ARM-SWD       - pgo
//-----------+----------------------------------------------------------------
//    Jun 12 | 4.9.5 Various Fixes, new flash handling etc               - pgo
//-----------+----------------------------------------------------------------
//    Mar 12 | 4.9.4 Various Fixes, BDM configuration changes            - pgo
//-----------+----------------------------------------------------------------
//    Mar 12 | 4.9.3 Various minor fixes                                 - pgo
//-----------+----------------------------------------------------------------
// 26 Feb 12 | 4.9.2 HCS12 Fixes for alignment error (split phrase)      - pgo
//-----------+----------------------------------------------------------------
// 30 Jan 12 | 4.9.0 HCS12 Improvements, firmware checks                 - pgo
//-----------+----------------------------------------------------------------
// 01 Jan 12 | 4.8.1                                                     - pgo
//-----------+----------------------------------------------------------------
// 28 Oct 11 | 4.8.0 Changes for Tower boards, BDM timer cleanup         - pgo
//-----------+----------------------------------------------------------------
// 15 Oct 11 | 4.7.2 RS08 Flash programmer modified                      - pgo
//    Oct 11 | 4.7.1 Updated progress dialogue for programmers           - pgo
//-----------+----------------------------------------------------------------
//    Jul 11 | 4.6.2 GDB, Low power connection fixes                     - pgo
//           |       CFVx connection improvement                         - pgo
//-----------+----------------------------------------------------------------
//    Jun 11 | 4.6.0 Kinetis release                                     - pgo
//-----------+----------------------------------------------------------------
//    Feb 11 | 4.5.0 Codewarrior Eclipse V10.1 release                   - pgo
//-----------+----------------------------------------------------------------
// 21 Jan 11 | 4.4.1 Bug fixes, don't display, connect after reset       - pgo
//-----------+----------------------------------------------------------------
// 26 Dec 10 | 4.4.0 Minor change to Flash programming & trim in GDI     - pgo
//-----------+----------------------------------------------------------------
//  1 Dec 10 | 4.3.0 Incorporated Codewarrior Eclipse plug-in            - pgo
//-----------+----------------------------------------------------------------
// 21 Oct 10 | 4.2.0 Bug fixes, USB busy strategy - break compatibility! - pgo
//-----------+----------------------------------------------------------------
// 21 Oct 10 | 4.1.0 Changes to USB Toggle initialisation                - pgo
//-----------+----------------------------------------------------------------
// 27 Sep 10 | 4.0.2 Fixed 2 bugs in updateBdmInfo() (JB16 & Bootmode)   - pgo
//-----------+----------------------------------------------------------------
//  1 Aug 10 | 4.0.0 Linux Version                                       - pgo
//-----------+----------------------------------------------------------------
// 20 May 10 | 3.2.1 Added JTAG read/write                               - pgo
//-----------+----------------------------------------------------------------
//  4 May 10 | 3.2.0 Significant changes to RS08 flash handling          - pgo
//-----------+----------------------------------------------------------------
// 28 Mar 10 | 3.1.5 Added USBDM_RS08FlashProgramCommence (Coldfire V10) - pgo
//-----------+----------------------------------------------------------------
// ?? Dec 09 | 3.1.1 Added USBDM_GetSpeedHz                              - pgo
//-----------+----------------------------------------------------------------
// 22 Nov 09 | 3.0.8 Added USBDM_GetErrorString                          - pgo
//-----------+----------------------------------------------------------------
//  7 Nov 09 | 3.0.7 Added configuration save & restore                  - pgo
//           |       Relocated log file
//-----------+----------------------------------------------------------------
//           | 3.0.6 Fixed /0 error in setSpeed() & NoIce mods           - pgo
//-----------+----------------------------------------------------------------
//           | 3.0.0 Completely changed structure!                       - pgo
//-----------+----------------------------------------------------------------
// 20 May 09 | 2.3.1 Release                                             - pgo
//-----------+----------------------------------------------------------------
// 20 May 09 | Added CFVx & JTAG settings                                - pgo
//-----------+----------------------------------------------------------------
// 20 May 09 | Restored CFV1 CSR2 etc entry points                       - pgo
//-----------+----------------------------------------------------------------
//
