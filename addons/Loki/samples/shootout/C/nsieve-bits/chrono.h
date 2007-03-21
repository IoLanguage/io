/* 
 *  Misc Stuff (profiling) for MPEG2Dec3
 *
 *	Copyright (C) 2002-2003 Marc Fauconneau <marc.fd@liberysurf.fr>
 *
 *  This file is part of MPEG2Dec3, a free MPEG-2 decoder
 *	
 *  MPEG2Dec3 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  MPEG2Dec3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include <windows.h>
#include <time.h>

#define ui64 unsigned __int64

ui64 inline read_counter(void)
{
	ui64 ts;
	UINT32 ts1, ts2;
	__asm {
		rdtsc
		mov ts1, eax
		mov ts2, edx
	}
	ts = ((ui64) ts2 << 32) | ((ui64) ts1);
	return ts;
}

// get CPU frequency in Hz (1 Mz precision)
ui64 get_freq(void)
{
	unsigned __int64  x = 0;
	long i;
	i = time(NULL);
	while (i == time(NULL));
	x -= read_counter();
	i++;
	while (i == time(NULL));
	x += read_counter();
	return x;
}

#include <stdio.h>

void inline render(ui64 delta, double div)
{
	__asm emms
//	ui64 delta = read_counter()-timer;
//	char buffer[256];
//	printf(buffer,"cycles count = %I64d, %f µs",delta/div,(double)delta/((double)1412*div));
	printf("cycles count = %I64d, %f seconds\n",delta/div,(double)delta/((double)(2079.*div*1000*1000)));
//	OutputDebugString(buffer);
}