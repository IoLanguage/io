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
 *	CalUTinstant computes the day of the week, the day of the year
 *	the gregorian (or julian) calendar date and the universal
 *	time from the julian decimal date.
 *	for astronomical purposes, The Gregorian calendar reform occurred
 *	on 15 Oct. 1582.  This is 05 Oct 1582 by the julian calendar.
 
 *	Input:	a ut_instant structure pointer, where the j_date element
 *		has been set. ( = 0 for 01 Jan -4712 12 HR UT )
 *
 *	output:  will set all the other elements of the structure.
 *		As a convienence, the function will also return the year.
 *
 *	Reference: Astronomial formulae for calculators, meeus, p 23
 *	from fortran program by F. Espenak - April 1982 Page 277,
 *	50 Year canon of solar eclipses: 1986-2035
 *
 */

#include "UTinstant.h"	/*	time structures	*/

long CalUTinstant( struct ut_instant *date )
{
    double frac;
    long jd;
    long ka;
    long kb;
    long kc;
    long kd;
    long ke;
    long ialp;
    
    jd = (long) (date->j_date + 0.5);	/* integer julian date */
    frac = date->j_date + 0.5 - (double) jd + 1.0e-10; /* day fraction */
    ka = (long) jd;
    if ( jd >= 2299161L )
    {
	ialp = (long int)(( (double) jd - 1867216.25 ) / ( 36524.25 ));
	ka = jd + 1L + ialp - ( ialp >> 2 );
    }
    kb = ka + 1524L;
    kc =  (long int)(( (double) kb - 122.1 ) / 365.25);
    kd = (long int)((double) kc * 365.25);
    ke = (long int)((double) ( kb - kd ) / 30.6001);
    date->day = kb - kd - ((long) ( (double) ke * 30.6001 ));
    if ( ke > 13L )
	date->month = ke - 13L;
    else
	date->month = ke - 1L;
    if ( (date->month == 2) && (date->day > 28) )
	date->day = 29;
    if ( (date->month == 2) && (date->day == 29) && (ke == 3L) )
	date->year = kc - 4716L;
    else if ( date->month > 2 )
	date->year = kc - 4716L;
    else
	date->year = kc - 4715L;
    date->i_hour = (long int)(date->d_hour = frac * 24.0);	/* hour */
    date->d_minute = (
		      ( date->d_hour - (double) date->i_hour ) * 60.0); /* minute */
		      date->i_minute = (int)date->d_minute;
		      date->second = (long int)(
						( date->d_minute - (double) date->i_minute ) * 60.0);/* second */
						date->weekday = (jd + 1L) % 7L;	/* day of week */
						if ( date->year == ((date->year >> 2) << 2) )
						date->day_of_year =
						( ( 275 * date->month ) / 9)
						- ((date->month + 9) / 12)
						+ date->day - 30;
						else
						date->day_of_year =
						( ( 275 * date->month ) / 9)
						- (((date->month + 9) / 12) << 1)
						+ date->day - 30;
						return( date->year );
}	/*	end of	 long CalUTinstant( date )	*/
						
						/*
						 *	JulUTinstant computes the julian decimal date (j_date) from
						 *	the gregorian (or Julian) calendar date.
						 *	for astronomical purposes, The Gregorian calendar reform occurred
						 *	on 15 Oct. 1582.  This is 05 Oct 1582 by the julian calendar.
						 *	Input:  a ut_instant structure pointer where Day, Month, Year and
						 *		i_hour, i_minute, and second have been set for the date
						 *		in question.
						 *
						 *	Output: the j_date and weekday elements of the structure will be set.
						 *		Also, the return value of the function will be the j_date too.
						 *
						 *	Reference: Astronomial formulae for calculators, meeus, p 23
						 *	from fortran program by F. Espenak - April 1982 Page 276,
						 *	50 Year canon of solar eclipses: 1986-2035
						 */
						
						double JulUTinstant(struct ut_instant *date)
						{
						    double frac, gyr;
						    long iy0, im0;
						    long ia, ib;
						    long jd;
						    
						    /* decimal day fraction	*/
						    frac = (( double)date->i_hour/ 24.0)
							+ ((double) date->i_minute / 1440.0)
							+ (date->second / 86400.0);
						    /* convert date to format YYYY.MMDDdd	*/
						    gyr = (double) date->year
							+ (0.01 * (double) date->month)
							+ (0.0001 * (double) date->day)
							+ (0.0001 * frac) + 1.0e-9;
						    /* conversion factors */
						    if ( date->month <= 2 )
						    {
							iy0 = date->year - 1L;
							im0 = date->month + 12;
						    }
						    else
						    {
							iy0 = date->year;
							im0 = date->month;
						    }
						    ia = iy0 / 100L;
						    ib = 2L - ia + (ia >> 2);
						    /* calculate julian date	*/
						    if ( date->year <= 0L )
							jd = (long) ((365.25 * (double) iy0) - 0.75)
							    + (long) (30.6001 * (im0 + 1L) )
							    + (long) date->day + 1720994L;
						    else
							jd = (long) (365.25 * (double) iy0)
							    + (long) (30.6001 * (double) (im0 + 1L))
							    + (long) date->day + 1720994L;
						    if ( gyr >= 1582.1015 )	/* on or after 15 October 1582	*/
							jd += ib;
						    date->j_date = (double) jd + frac + 0.5;
						    jd = (long) (date->j_date + 0.5);
						    date->weekday = (jd + 1L) % 7L;
						    return( date->j_date );
						}	/*	end of	double JulUTinstant( date )	*/


/*------------------------------------------------------------ 
  OO extensions by Steve Dekorte 2004 
  ------------------------------------------------------------- */
  
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECONDS_PER_DAY   (24.0 * 60.0 * 60.0)

void UTinstant_setToStartOfYear_(UTinstant *self, int year)
{  
    memset(self, 0, sizeof(UTinstant));
    
    self->year        = year; 
    self->month       = 1;  /* [1-12] */
    self->day         = 1;   
    self->i_hour      = 0;  
    self->i_minute    = 0;	
    self->second      = 0;
    
    JulUTinstant(self);
    /*CalUTinstant(self);*/
}

double SecondsFromJulian0ToYear(int year)
{
    UTinstant ut;
    double j, s;
    
    UTinstant_setToStartOfYear_(&ut, year);
    
    j = JulUTinstant(&ut) * SECONDS_PER_DAY;
    
    modf(j, &s);
    return s;
}


UTinstant *UTinstant_new(void)
{
    UTinstant *self = (UTinstant *)calloc(1, sizeof(UTinstant));
    UTinstant_setSecondsSince1970_(self, 0.0);
    return self;
}

void UTinstant_free(UTinstant *self)
{
    free(self);
}

void UTinstant_updateCalendar(UTinstant *self)
{
    CalUTinstant(self);
}

void UTinstant_updateJUTinstant(UTinstant *self)
{
    JulUTinstant(self);
}

double UTinstant_asSeconds(UTinstant *self)
{
    return self->j_date * SECONDS_PER_DAY;
}

void UTinstant_setSeconds_(UTinstant *self, double s)
{
    self->j_date = s / SECONDS_PER_DAY;
    UTinstant_updateCalendar(self);
}

void UTinstant_setSecondsSince1970_(UTinstant *self, double s)
{    
    UTinstant_setYear_month_day_hour_minute_second_(self, 1970, 0, 0, 0, 0, 0);
    
    self->j_date += s / SECONDS_PER_DAY;
    UTinstant_updateCalendar(self);
}

double UTinstant_secondsSince1970(UTinstant *self)
{
    return UTinstant_asSeconds(self) - SecondsFromJulian0ToYear(1970);
}

void UTinstant_setSecondsSince1900_(UTinstant *self, double s)
{
    UTinstant_setSeconds_(self, SecondsFromJulian0ToYear(1900) + s);
}

double UTinstant_secondsSince1900(UTinstant *self)
{
    return UTinstant_asSeconds(self) - SecondsFromJulian0ToYear(1900);
}

/* month, day and hour begin at 0 */
void UTinstant_setYear_month_day_hour_minute_second_(UTinstant *self,
    int year, 
    int month, 
    int day, 
    int hour, 
    int minute, 
    double second)
{
    self->year        = year; 
    self->month       = month + 1; /* [1-12] */
    self->day         = day + 1; /* [1-31] */
    self->i_hour      = hour;    /* [0-23] */
    self->i_minute    = minute;  /* [0-59] */
    self->second      = second;  /* [0-59.9999] */
    UTinstant_updateJUTinstant(self);
}

void UTinstant_setTM_(UTinstant *self, struct tm *tm)
{
    UTinstant_setToStartOfYear_(self, 1900);
    self->year        = tm->tm_year + 1900; 
    self->month       = tm->tm_mon  + 1; /* [1-12] */
    self->day         = tm->tm_mday; /* [1-31] */
    self->day_of_year = tm->tm_yday;    /* [1-365] */
    self->i_hour      = tm->tm_hour;    /* [0-23] */
    self->i_minute    = tm->tm_min;  /* [0-59] */
    self->second      = tm->tm_sec;  /* [0-59.9999] */
    UTinstant_updateJUTinstant(self);
}

/* --- year ------------------------------------------------------ */

void UTinstant_setYear_(UTinstant *self, long v)
{
    self->year = v;
    UTinstant_updateJUTinstant(self);
}

long UTinstant_year(UTinstant *self) { return self->year; }

/* --- month ------------------------------------------------------ */

void UTinstant_setMonth_(UTinstant *self, int v)
{
    self->month = v;
    UTinstant_updateJUTinstant(self);
}

int UTinstant_month(UTinstant *self) { return self->month; }

/* --- day ------------------------------------------------------ */

void UTinstant_setDay_(UTinstant *self, int v)
{
    self->day = v;
    UTinstant_updateJUTinstant(self);
}

int UTinstant_day(UTinstant *self) { return self->day; }

/* --- hour ------------------------------------------------------ */

void UTinstant_setHour_(UTinstant *self, int v)
{
    self->i_hour = v;
    UTinstant_updateJUTinstant(self);
}

int UTinstant_hour(UTinstant *self) { return self->i_hour; }

/* --- minute ------------------------------------------------------ */

void UTinstant_setMinute_(UTinstant *self, int v)
{
    self->i_minute = v;
    UTinstant_updateJUTinstant(self);
}

int UTinstant_minute(UTinstant *self) 
{ 
return self->i_minute; 
}

/* --- second ------------------------------------------------------ */

void UTinstant_setSecond_(UTinstant *self, double v)
{
    self->second = v;
    UTinstant_updateJUTinstant(self);
}

double UTinstant_second(UTinstant *self) 
{ 
return self->second; 
}

double UTinstant_dayOfYear(UTinstant *self) 
{ 
return self->day_of_year - 1; 
}

/* --- format -------------------------------------------------------- */

#define MAX_FORMAT_SIZE 1024

static struct tm EmptyTM(void)
{
    time_t tmp = 0;
    struct tm *tt = localtime(&tmp);
    struct tm t;
    memcpy(&t, tt, sizeof(struct tm));
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = 0;
    t.tm_mon = 0;
    t.tm_year = 0;
    t.tm_wday = 0;
    t.tm_yday = 0;
    return t;  
}

char *UTinstant_shortWeekdayName(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_wday = self->weekday;
    strftime(s, 128, "%a", &t);
    return s;
}

char *UTinstant_weekdayName(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_wday = self->weekday;
    strftime(s, 128, "%A", &t);
    return s;
}

char *UTinstant_shortMonthName(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_mon = UTinstant_month(self) - 1;
    strftime(s, 128, "%b", &t);
    return s;
}

char *UTinstant_monthName(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_mon = UTinstant_month(self) - 1;
    strftime(s, 128, "%B", &t);
    return s;
}

char *UTinstant_dayOfMonthString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_mday = UTinstant_day(self);
    strftime(s, 128, "%d", &t);
    return s;
}

char *UTinstant_hour24String(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_hour = UTinstant_hour(self);
    strftime(s, 128, "%H", &t);
    return s;
}

char *UTinstant_hour12String(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_hour = UTinstant_hour(self);
    strftime(s, 128, "%I", &t);
    return s;
}

char *UTinstant_dayOfYearString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_yday = (int)UTinstant_dayOfYear(self);
    strftime(s, 128, "%j", &t);
    return s;
}

char *UTinstant_monthNumberString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_mon = (int)UTinstant_month(self) - 1;
    strftime(s, 128, "%m", &t);
    return s;
}

char *UTinstant_minuteString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_min = (int)UTinstant_minute(self);
    strftime(s, 128, "%M", &t);
    return s;
}

char *UTinstant_amPmString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_hour = (int)UTinstant_hour(self);
    strftime(s, 128, "%p", &t);
    return s;
}

char *UTinstant_secondString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_sec = (int)UTinstant_second(self);
    strftime(s, 128, "%S", &t);
    return s;
}

char *UTinstant_weekOfYearSunFirstString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_mon = UTinstant_month(self) - 1;
    t.tm_mday = (int)UTinstant_day(self);
    strftime(s, 128, "%U", &t);
    return s;
}

char *UTinstant_weekdayNumberString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_year = (int)UTinstant_year(self);
    t.tm_mon = (int)UTinstant_month(self) - 1;
    t.tm_mday = (int)UTinstant_day(self);
    strftime(s, 128, "%w", &t);
    return s;
}

char *UTinstant_weekOfYearMonFirstString(UTinstant *self, char *s)
{
    struct tm t = EmptyTM();
    t.tm_mon = (int)UTinstant_month(self) - 1;
    t.tm_mday = (int)UTinstant_day(self);
    strftime(s, 128, "%W", &t);
    return s;
}

char *UTinstant_shortYearString(UTinstant *self, char *s)
{
    int y = UTinstant_year(self);
    y -= (y/100)*100;
    
    if (y == 0) 
    { 
	snprintf(s, MAX_FORMAT_SIZE, "00"); 
	return s; 
    }
    
    if (y < 10) 
    { 
	snprintf(s, MAX_FORMAT_SIZE, "0%i", y); 
	return s; 
    }
    
    snprintf(s, MAX_FORMAT_SIZE, "%i", y);
    return s;
}

char *UTinstant_yearString(UTinstant *self, char *s)
{
    snprintf(s, MAX_FORMAT_SIZE, "%i", (int)UTinstant_year(self));
    return s;
}

char *UTinstant_zoneString(UTinstant *self, char *s)
{
    time_t t = time(NULL);
    struct tm *tp = localtime(&t);
    strftime(s, 128, "%Z", tp);
    return s;
}


ByteArray *UTinstant_asString(UTinstant *self, const char *format)
{
    /* this is not perfect, but it strftime can't deal with years < 1970 */
    char *s = (char *)calloc(1, MAX_FORMAT_SIZE + strlen(format));
    ByteArray *b = ByteArray_newWithCString_(format);
    ByteArray_replaceCString_withCString_(b, "%c", "%a %b %d %H:%M:%S %Y");
    ByteArray_replaceCString_withCString_(b, "%x", "%m/%d/%y");
    ByteArray_replaceCString_withCString_(b, "%X", "%H:%M:%S");
    
    ByteArray_replaceCString_withCString_(b, "%a", UTinstant_shortWeekdayName(self, s));
    ByteArray_replaceCString_withCString_(b, "%A", UTinstant_weekdayName(self, s));
    ByteArray_replaceCString_withCString_(b, "%b", UTinstant_shortMonthName(self, s));
    ByteArray_replaceCString_withCString_(b, "%B", UTinstant_monthName(self, s));
    
    ByteArray_replaceCString_withCString_(b, "%d", UTinstant_dayOfMonthString(self, s));
    ByteArray_replaceCString_withCString_(b, "%H", UTinstant_hour24String(self, s));
    ByteArray_replaceCString_withCString_(b, "%I", UTinstant_hour12String(self, s));
    ByteArray_replaceCString_withCString_(b, "%j", UTinstant_dayOfYearString(self, s));
    ByteArray_replaceCString_withCString_(b, "%m", UTinstant_monthNumberString(self, s));
    ByteArray_replaceCString_withCString_(b, "%M", UTinstant_minuteString(self, s));
    ByteArray_replaceCString_withCString_(b, "%p", UTinstant_amPmString(self, s));
    ByteArray_replaceCString_withCString_(b, "%S", UTinstant_secondString(self, s));
    ByteArray_replaceCString_withCString_(b, "%U", UTinstant_weekOfYearSunFirstString(self, s));
    ByteArray_replaceCString_withCString_(b, "%w", UTinstant_weekdayNumberString(self, s));
    ByteArray_replaceCString_withCString_(b, "%W", UTinstant_weekOfYearMonFirstString(self, s));
    
    ByteArray_replaceCString_withCString_(b, "%y", UTinstant_shortYearString(self, s));
    ByteArray_replaceCString_withCString_(b, "%Y", UTinstant_yearString(self, s));
    ByteArray_replaceCString_withCString_(b, "%Z", UTinstant_zoneString(self, s));
    ByteArray_replaceCString_withCString_(b, "%%", "%");
    free(s);
    return b;
}
