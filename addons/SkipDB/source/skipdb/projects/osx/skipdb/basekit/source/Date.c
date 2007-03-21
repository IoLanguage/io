/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#define DATE_C
#include "Date.h"
#undef DATE_C
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "PortableStrptime.h"
#include "ByteArray.h"

Date *Date_new(void)
{
	Date *self = (Date *)calloc(1, sizeof(Date));
	self->ut = UTinstant_new();
	Date_now(self);
	return self;
}

void Date_copy_(Date *self, Date *other)
{ 
	UTinstant_setSeconds_(self->ut, UTinstant_asSeconds(other->ut));
}

void Date_free(Date *self)
{ 
	UTinstant_free(self->ut);
	free(self); 
}

int Date_compare(Date *self, Date *other)
{
	double s1 = Date_asSeconds(self);
	double s2 = Date_asSeconds(other);
	
	if (s1 == s2) 
	{
		return 0;
	}
	
	return s1 > s2 ? 1 : -1;
}

double Date_SecondsFrom1970ToNow(void)
{
	double s, us;
	struct timeval timeval;
	struct timezone timezone;
	
	gettimeofday(&timeval, &timezone);
	s = timeval.tv_sec;
	s -= timezone.tz_minuteswest * 60;
	us = timeval.tv_usec;
	
	return s + (us/1000000.0); /* + (60*60);*/
}

void Date_now(Date *self)
{
	struct timeval timeval;
	struct timezone timezone;
	//struct tm *t;
	
	gettimeofday(&timeval, &timezone);
	
	Date_setTimevalue_(self, timeval);
	
	//t = localtime((const time_t *)&(timeval.tv_sec));
	/*
	 UTinstant_setSecond_(self->ut, t->tm_sec);
	 UTinstant_setMinute_(self->ut, t->tm_min);
	 UTinstant_setHour_(self->ut, t->tm_hour);
	 UTinstant_setDay_(self->ut, t->tm_mday);
	 UTinstant_setMonth_(self->ut, t->tm_mon);
	 UTinstant_setYear_(self->ut, t->tm_year);
	 */
	/*UTinstant_setDST_(self->ut, t->tm_isdst);*/
	//UTinstant_setTM_(self->ut, t);
	//UTinstant_setSecond_(self->ut, UTinstant_second(self->ut) + (((double)timeval.tv_usec)/(double)1000000.0));
	
}

void Date_setTimevalue_(Date *self, struct timeval tv)
{
	struct tm *t;
	
	t = localtime((const time_t *)&(tv.tv_sec));
	UTinstant_setTM_(self->ut, t);
	UTinstant_setSecond_(self->ut, 
					 UTinstant_second(self->ut) + 
					 (((double)tv.tv_usec)/(double)1000000.0));
}

double Date_Clock(void)
{
	return ((double)clock())/((double)CLOCKS_PER_SEC); 
}


// zone -------------------------------------------------------- 

void Date_setToLocalTimeZone(Date *self)
{ 
	struct timeval timeval;
	gettimeofday(&timeval, &(self->tz));
}

struct timezone Date_timeZone(Date *self)
{ 
	return self->tz; 
}

void Date_setTimeZone_(Date *self, struct timezone tz)
{ 
	self->tz = tz; 
}

void Date_convertToTimeZone_(Date *self, struct timezone tz)
{
	double s = Date_asSeconds(self) + 
	((self->tz.tz_minuteswest - tz.tz_minuteswest) * 60);
	Date_fromSeconds_(self, s);
	Date_setTimeZone_(self, tz);
}

// time -------------------------------------------------------- 

/*
 void Date_fromTimespec(Date *self, double t);
 {
	 UTinstant_setSecond_(self->ut, UTinstant_second(self->ut) + (((double)timeval.tv_usec)/(double)1000000.0));
 }
 */

void Date_fromLocalTime_(Date *self, struct tm *t)
{
	UTinstant_setTM_(self->ut, t);
}

void Date_fromTime_(Date *self, time_t t)
{ 
	Date_fromSeconds_(self, (double)t); 
}

time_t Date_asTime(Date *self)
{ 
	return (time_t)UTinstant_secondsSince1970(self->ut); 
}

// sconds -------------------------------------------------------- 

double Date_asSeconds(Date *self)
{
	return UTinstant_secondsSince1970(self->ut);
}

void Date_fromSeconds_(Date *self, double s)
{
	UTinstant_setSecondsSince1970_(self->ut, s);
}

void Date_addSeconds_(Date *self, double s)
{
	//Date_fromSeconds_(self, Date_asSeconds(self) + s); 
	UTinstant_setSeconds_(self->ut, UTinstant_asSeconds(self->ut) + s);
}

double Date_secondsSince_(Date *self, Date *other)
{ 
	return Date_asSeconds(self) - Date_asSeconds(other); 
}

// components -------------------------------------------------------- 

long Date_year(Date *self) 
{ 
	return UTinstant_year(self->ut); 
}

void Date_setYear_(Date *self, long v) 
{ 
	UTinstant_setYear_(self->ut, v); 
}

int Date_month(Date *self) 
{ 
	return UTinstant_month(self->ut); 
}

void Date_setMonth_(Date *self, int v) 
{ 
	UTinstant_setMonth_(self->ut, v); 
}

int Date_day(Date *self) 
{ 
	return UTinstant_day(self->ut); 
}

void Date_setDay_(Date *self, int v) 
{ 
	UTinstant_setDay_(self->ut, v); 
}

int Date_hour(Date *self) 
{ 
	return UTinstant_hour(self->ut); 
}

void Date_setHour_(Date *self, int v) 
{ 
	UTinstant_setHour_(self->ut, v); 
}

int Date_minute(Date *self) 
{ 
	return UTinstant_minute(self->ut); 
}

void Date_setMinute_(Date *self, int v) 
{ 
	UTinstant_setMinute_(self->ut, v); 
}

double Date_second(Date *self) 
{ 
	return UTinstant_second(self->ut); 
}

void Date_setSecond_(Date *self, double v) 
{ 
	UTinstant_setSecond_(self->ut, v); 
}

unsigned char Date_isDaylightSavingsTime(Date *self)
{ 
	time_t t = (time_t)UTinstant_secondsSince1970(self->ut);
	struct tm *tp = localtime(&t);
	return (unsigned char)tp->tm_isdst; 
}

int Date_isLeapYear(Date *self)
{
	int year = UTinstant_year(self->ut);
	
	if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// format -------------------------------------------------------- 

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

void Date_fromString_format_(Date *self, const char *s, const char *format)
{
	struct tm t = EmptyTM();
	io_strptime((char *)s, (char *)format, &t);
	/*
	 printf("year  = %i\n", t.tm_year);
	 printf("month = %i\n", t.tm_mon);
	 printf("day   = %i\n", t.tm_mday);
	 printf("hour  = %i\n", t.tm_hour);
	 printf("min   = %i\n", t.tm_min);
	 printf("sec   = %i\n", t.tm_sec);
	 */
	UTinstant_setTM_(self->ut, &t);
}

// durations -------------------------------------------------------- 

Duration *Date_newDurationBySubtractingDate_(Date *self, Date *other)
{
	double d = Date_secondsSince_(self, other);
	return Duration_newWithSeconds_(d);
}

void Date_addDuration_(Date *self, Duration *d)
{ 
	Date_addSeconds_(self, Duration_asSeconds(d)); 
}

void Date_subtractDuration_(Date *self, Duration *d)
{ 
	Date_addSeconds_(self, -Duration_asSeconds(d)); 
}

// ----------------------------------------------------------- 

double Date_secondsSinceNow(Date *self)
{
	double n = Date_SecondsFrom1970ToNow();
	double s = UTinstant_secondsSince1970(self->ut);
	return n - s;
}

// format -------------------------------------------------------- 

ByteArray *Date_asString(Date *self, const char *format)
{
	return UTinstant_asString(self->ut, format);
}
