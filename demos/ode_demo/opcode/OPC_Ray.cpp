///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for rays.
 *	\file		IceRay.cpp
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Ray class.
 *	A ray is a half-line P(t) = mOrig + mDir * t, with 0 <= t <= +infinity
 *	\class		Ray
 *	\author		Pierre Terdiman
 *	\version	1.0
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Segment class.
 *	A segment is defined by S(t) = mP0 * (1 - t) + mP1 * t, with 0 <= t <= 1
 *	Alternatively, a segment is S(t) = Origin + t * Direction for 0 <= t <= 1.
 *	Direction is not necessarily unit length. The end points are Origin = mP0 and Origin + Direction = mP1.
 *
 *	\class		Segment
 *	\author		Pierre Terdiman
 *	\version	1.0
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include	"Stdafx.h"

using namespace IceMaths;
