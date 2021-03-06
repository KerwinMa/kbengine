/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __TIMESTAMP_HPP__
#define __TIMESTAMP_HPP__

#include "cstdkbe/platform.hpp"
#include "helper/debug_helper.hpp"
namespace KBEngine{
// Indicates whether or not to use a call to RDTSC (Read Time Stamp Counter)
// to calculate timestamp. The benefit of using this is that it is fast and
// accurate, returning actual clock ticks. The downside is that this does not
// work well with CPUs that use Speedstep technology to vary their clock speeds.
#ifndef _XBOX360
#ifdef unix
// #define KBE_USE_RDTSC
#else // unix
# define KBE_USE_RDTSC
#endif // unix
#endif // _XBOX360

/**
 * This function returns the processor's (real-time) clock cycle counter.
 */
#ifdef unix

enum KBETimingMethod
{
	RDTSC_TIMING_METHOD,
	GET_TIME_OF_DAY_TIMING_METHOD,
	GET_TIME_TIMING_METHOD,
	NO_TIMING_METHOD,
};

extern KBETimingMethod g_timingMethod;

inline uint64 timestamp_rdtsc()
{
	uint32 rethi, retlo;
	__asm__ __volatile__ (
		// Read Time-Stamp Counter
		// loads current value of processor's timestamp counter into EDX:EAX
		"rdtsc":
		"=d"    (rethi),
		"=a"    (retlo)
						  );
	return uint64(rethi) << 32 | retlo; // 自CPU上电以来所经过的时钟周期数,达到纳秒级的计时精度
}

// Alternate Linux implementation uses gettimeofday. In rough tests, this can
// be between 20 and 600 times slower than using RDTSC. Also, there is a problem
// under 2.4 kernels where two consecutive calls to gettimeofday may actually
// return a result that goes backwards.
#include <sys/time.h>

inline uint64 timestamp_gettimeofday()
{
	timeval tv;
	gettimeofday( &tv, NULL );
	return 1000000ULL * uint64( tv.tv_sec ) + uint64( tv.tv_usec );
}

#include <time.h>
#include <asm/unistd.h>

inline uint64 timestamp_gettime()
{
	timespec tv;
	// Using a syscall here so we don't have to link with -lrt
	KBE_VERIFY(syscall( __NR_clock_gettime, CLOCK_MONOTONIC, &tv ) == 0);

	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

inline uint64 timestamp()
{
#ifdef KBE_USE_RDTSC
	return timestamp_rdtsc();
#else // KBE_USE_RDTSC
	if (g_timingMethod == RDTSC_TIMING_METHOD)
		return timestamp_rdtsc();
	else if (g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return timestamp_gettimeofday();
	else //if (g_timingMethod == GET_TIME_TIMING_METHOD)
		return timestamp_gettime();

#endif // KBE_USE_RDTSC
}

#elif defined(_WIN32)

#ifdef KBE_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
inline uint64 timestamp()
{
	__asm rdtsc // 自CPU上电以来所经过的时钟周期数,达到纳秒级的计时精度
}
#pragma warning (pop)
#else // KBE_USE_RDTSC

#ifdef _XBOX360
#include <xtl.h>
#else // _XBOX360
#include <windows.h>
#endif // _XBOX360

inline uint64 timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	return counter.QuadPart;
}

#endif // KBE_USE_RDTSC

#elif defined( PLAYSTATION3 )

inline uint64 timestamp()
{
	uint64 ts;
	SYS_TIMEBASE_GET( ts );
	return ts;
}

#else
	#error Unsupported platform!
#endif

uint64 stampsPerSecond();
double stampsPerSecondD();

uint64 stampsPerSecond_rdtsc();
double stampsPerSecondD_rdtsc();

uint64 stampsPerSecond_gettimeofday();
double stampsPerSecondD_gettimeofday();

inline double stampsToSeconds( uint64 stamps )
{
	return double( stamps )/stampsPerSecondD();
}

// -----------------------------------------------------------------------------
// Section: TimeStamp
// -----------------------------------------------------------------------------

/**
 *	This class stores a value in stamps but has access functions in seconds.
 */
class TimeStamp
{
public:
	TimeStamp( uint64 stamps = 0 ) : stamp_( stamps ) {}

	operator uint64 &()				{ return stamp_; }
	operator uint64() const			{ return stamp_; }

	inline double inSeconds() const;
	inline void setInSeconds( double seconds );

	inline TimeStamp ageInStamps() const;
	inline double ageInSeconds() const;

	// Utility methods
	inline static double toSeconds( uint64 stamps );
	inline static TimeStamp fromSeconds( double seconds );

	uint64	stamp_;
};


inline double TimeStamp::toSeconds( uint64 stamps )
{
	return double( stamps )/stampsPerSecondD();
}

inline TimeStamp TimeStamp::fromSeconds( double seconds )
{
	return uint64( seconds * stampsPerSecondD() );
}

inline double TimeStamp::inSeconds() const
{
	return toSeconds( stamp_ );
}

inline void TimeStamp::setInSeconds( double seconds )
{
	stamp_ = fromSeconds( seconds );
}

inline TimeStamp TimeStamp::ageInStamps() const
{
	return timestamp() - stamp_;
}

inline double TimeStamp::ageInSeconds() const
{
	return toSeconds( this->ageInStamps() );
}

}

#endif // __TIMESTAMP_HPP__
