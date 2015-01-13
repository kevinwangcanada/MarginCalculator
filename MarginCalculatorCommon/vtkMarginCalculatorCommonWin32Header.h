///  vtkMarginCalculatorCommonWin32Header - manage Windows system differences
/// 
/// The vtkMarginCalculatorCommonWin32Header captures some system differences between Unix
/// and Windows operating systems. 


#ifndef __vtkMarginCalculatorCommonWin32Header_h
#define __vtkMarginCalculatorCommonWin32Header_h

#include "vtkMarginCalculatorCommonConfigure.h"

#if defined(WIN32) && !defined(vtkMarginCalculatorCommon_STATIC)
#if defined(vtkMarginCalculatorCommon_EXPORTS)
#define VTK_MARGINCALCULATORCOMMON_EXPORT __declspec( dllexport ) 
#else
#define VTK_MARGINCALCULATORCOMMON_EXPORT __declspec( dllimport )
#endif
#else
#define VTK_MARGINCALCULATORCOMMON_EXPORT
#endif

#endif
