#ifndef DVC11XX_H__
#define DVC11XX_H__
//------------------------------------------------------------------------------

//#define  DVC1120//DVC1124 DVC1120 DVC1117	DVC1114	DVC1110

#define DVC1124
//#define DVC1114
//#define DVC1110

#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_tim.h"
#include "basic_data_type.h" 

#ifdef	 DVC1124
#include "DVC1124.h"
#endif

#ifdef	 DVC1120
#include "DVC1120.h"
#endif

#ifdef	 DVC1117
#include "DVC1117.h"
#endif

#ifdef	 DVC1114
#include "DVC1114.h"
#endif
#ifdef	 DVC1110
#include "DVC1110.h"
#endif

#endif
