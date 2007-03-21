/*
 *	Copyright (c) 1986-2002, Hiram Clawson - curator@hiram.ws.NoSpam
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or
 *	without modification, are permitted provided that the following
 *	conditions are met:
 *
 *		Redistributions of source code must retain the above
 *		copyright notice, this list of conditions and the
 *		following disclaimer.
 *
 *		Redistributions in binary form must reproduce the
 *		above copyright notice, this list of conditions and
 *		the following disclaimer in the documentation and/or
 *		other materials provided with the distribution.
 *
 *		Neither name of The Museum of Hiram nor the names of
 *		its contributors may be used to endorse or promote products
 *		derived from this software without specific prior
 *		written permission. 
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *	CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *	IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *	OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 *	STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *	IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *	THE POSSIBILITY OF SUCH DAMAGE. 
 */
/*
 *	jday.h: contains definitions of structures used for time
 *		calculations.
 *
 */
#ifndef	_jday_h_
#define _jday_h_

#ifdef __cplusplus
extern "C" {
#endif

/* j_date = days since julian date */
typedef struct ut_instant {
	double	j_date;		/* julian decimal date, 0 = 01 Jan -4712 12 HR UT */
	long	year;		/* year, valid range [-4,712, +2,147,483,647] */
	int	month;		/* [1-12]	*/ /* C: 0-11 */
	int	day;		/* [1-31]	*/ /* C: same */
	int	i_hour;		/* [0-23]	*/ /* C: same */
	int	i_minute;	/* [0-59]	*/ /* C: same */
	double	second;		/* [0-59.9999]	*/ /* C: same */
	double	d_hour;		/* [0.0-23.9999] includes minute and second */
	double	d_minute;	/* [0.0-59.9999] includes second	*/
	int	weekday;	/* [0-6]	*/ /* C: same */
	int	day_of_year;	/* [1-366]	*/ /* C: 0-365 */
} UTinstant, * UTinstantPtr;

/*	Functions in caldate.c	*/
long CalDate( UTinstantPtr );	/* converts julian date to year,mo,da */
double JulDate( UTinstantPtr );	/* returns julian day from year,mo,da */

#include <time.h>
#include "ByteArray.h"

/*------------------------------------------------------------ */

UTinstant *UTinstant_new(void);
void UTinstant_free(UTinstant *self);

void UTinstant_updateCalendar(UTinstant *self);
void UTinstant_updateJDate(UTinstant *self);

void UTinstant_setSeconds_(UTinstant *self, double s); /* Julian seconds */
double UTinstant_asSeconds(UTinstant *self);

void UTinstant_setSecondsSince1970_(UTinstant *self, double s);
double UTinstant_secondsSince1970(UTinstant *self);

void UTinstant_setSecondsSince1900_(UTinstant *self, double s);
double UTinstant_secondsSince1900(UTinstant *self);

/* month, day and hour begin at 0 */
void UTinstant_setYear_month_day_hour_minute_second_(UTinstant *self,
   int year, int month, int day, int hour, int minute, double second);

void UTinstant_setTM_(UTinstant *self, struct tm *tm);

/* --- year ------------------------------------------------------ */
void UTinstant_setYear_(UTinstant *self, long v);
long UTinstant_year(UTinstant *self);

/* --- month ------------------------------------------------------ */
void UTinstant_setMonth_(UTinstant *self, int v);
int UTinstant_month(UTinstant *self);

/* --- day ------------------------------------------------------ */
void UTinstant_setDay_(UTinstant *self, int v);
int UTinstant_day(UTinstant *self);

/* --- hour ------------------------------------------------------ */
void UTinstant_setHour_(UTinstant *self, int v);
int UTinstant_hour(UTinstant *self);

/* --- minute ------------------------------------------------------ */
void UTinstant_setMinute_(UTinstant *self, int v);
int UTinstant_minute(UTinstant *self);

/* --- second ------------------------------------------------------ */
void UTinstant_setSecond_(UTinstant *self, double v);
double UTinstant_second(UTinstant *self);

ByteArray *UTinstant_asString(UTinstant *self, const char *format);

#ifdef __cplusplus
}
#endif
#endif	/*	_jday_h_	*/
