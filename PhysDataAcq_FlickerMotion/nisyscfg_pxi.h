/*============================================================================*/
/*             National Instruments / System Configuration API                */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 2010-2016.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:   nisyscfg_pxi.h                                                    */
/* Purpose: Include file for PXI specific attributes to be used with the      */
/*          nisyscfg interface defined in nisyscfg.h                          */
/*                                                                            */
/*============================================================================*/

#ifndef ___nisysconfig_pxi_h___
#define ___nisysconfig_pxi_h___

typedef enum
{
   NISysCfgPxiFanModesAuto       = 1,
   NISysCfgPxiFanModesSafeManual = 2
} NISysCfgPxiFanModes;

typedef enum
{
   NISysCfgPxiPropertyFanMode    = 185597952,  // NISysCfgPxiFanModes
   NISysCfgPxiPropertyFanUserRpm = 185602048   // unsigned int
} NISysCfgPxiProperty;

#endif
