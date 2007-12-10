#include "UTzones.h"
#include <string.h>

typedef struct UTzoneEntry 
{
	const char *symbol;
	const char *name;
	const char *location;
	float hoursFromGMT;
} UTzoneEntry;

float UTzone_HoursFromGMTFor_(const char *zoneSymbol)
{
    UTzoneEntry zones[] =  {
	{"A", "Alpha Time Zone", "Military",  1}, 
	{"ACDT", "Australian Central Daylight Time", "Australia",  10.5}, 
	{"ACST", "Australian Central Standard Time", "Australia",  9.5}, 
	{"ADT", "Atlantic Daylight Time", "North America", -3}, 
	{"AEDT", "Australian Eastern Daylight Time", "Australia",  11}, 
	{"AEST", "Australian Eastern Standard Time", "Australia",  10}, 
	{"AKDT", "Alaska Daylight Time", "North America", -8}, 
	{"AKST", "Alaska Standard Time", "North America", -9}, 
	{"AST", "Atlantic Standard Time", "North America", -4}, 
	{"AWST", "Australian Western Standard Time", "Australia",  8}, 
	{"B", "Bravo Time Zone", "Military",  2}, 
	{"BST", "British Summer Time", "Europe",  1}, 
	{"C", "Charlie Time Zone", "Military",  3}, 
	{"CDT", "Central Daylight Time", "Australia",  10.5}, 
	{"CDT", "Central Daylight Time", "North America", -5}, 
	{"CEST", "Central European Summer Time", "Europe",  2}, 
	{"CET", "Central European Time", "Europe",  1}, 
	{"CST", "Central Standard Time", "Australia",  9.5}, 
	{"CST", "Central Standard Time", "North America", -6}, 
	{"CXT", "Christmas Island Time", "Australia",  7}, 
	{"D", "Delta Time Zone", "Military",  4}, 
	{"E", "Echo Time Zone", "Military",  5}, 
	{"EDT", "Eastern Daylight Time", "Australia",  11}, 
	{"EDT", "Eastern Daylight Time", "North America", -4}, 
	{"EEST", "Eastern European Summer Time", "Europe",  3}, 
	{"EET", "Eastern European Time", "Europe",  2}, 
	{"EST", "Eastern Standard Time", "Australia",  10}, 
	{"EST", "Eastern Standard Time", "North America", -5}, 
	{"F", "Foxtrot Time Zone", "Military",  6}, 
	{"G", "Golf Time Zone", "Military",  7}, 
	{"GMT", "Greenwich Mean Time", "Europe", 0}, 
	{"H", "Hotel Time Zone", "Military",  8}, 
	{"HAA", "Heure AvancŽe de l'Atlantique", "North America", -3}, 
	{"HAC", "Heure AvancŽe du Centre", "North America", -5}, 
	{"HADT", "Hawaii-Aleutian Daylight Time", "North America", -9}, 
	{"HAE", "Heure AvancŽe de l'Est", "North America", -4}, 
	{"HAP", "Heure AvancŽe du Pacifique", "North America", -7}, 
	{"HAR", "Heure AvancŽe des Rocheuses", "North America", -6}, 
	{"HAST", "Hawaii-Aleutian Standard Time", "North America", -10}, 
	{"HST", "Hawaii-Aleutian Standard Time", "North America", -10}, 
	{"HAT", "Heure AvancŽe de Terre-Neuve", "North America", -2.5}, 
	{"HAY", "Heure AvancŽe du Yukon", "North America", -8}, 
	{"HNA", "Heure Normale de l'Atlantique", "North America", -4}, 
	{"HNC", "Heure Normale du Centre", "North America", -6}, 
	{"HNE", "Heure Normale de l'Est", "North America", -5}, 
	{"HNP", "Heure Normale du Pacifique", "North America", -8}, 
	{"HNR", "Heure Normale des Rocheuses", "North America", -7}, 
	{"HNT", "Heure Normale de Terre-Neuve", "North America", -3.5}, 
	{"HNY", "Heure Normale du Yukon", "North America", -9}, 
	{"I", "India Time Zone", "Military",  9}, 
	{"IST", "Irish Summer Time", "Europe",  1}, 
	{"K", "Kilo Time Zone", "Military",  10}, 
	{"L", "Lima Time Zone", "Military",  11}, 
	{"M", "Mike Time Zone", "Military",  12}, 
	{"MDT", "Mountain Daylight Time", "North America", -6}, 
	{"MESZ", "MitteleuropŠische Sommerzeit", "Europe",  2}, 
	{"MEZ", "MitteleuropŠische Zeit", "Europe",  1}, 
	{"MST", "Mountain Standard Time", "North America", -7}, 
	{"N", "November Time Zone", "Military", -1}, 
	{"NDT", "Newfoundland Daylight Time", "North America", -2.5}, 
	{"NFT", "Norfolk (Island) Time", "Australia",  11.5}, 
	{"NST", "Newfoundland Standard Time", "North America", -3.5}, 
	{"O", "Oscar Time Zone", "Military", -2}, 
	{"P", "Papa Time Zone", "Military", -3}, 
	{"PDT", "Pacific Daylight Time", "North America", -7}, 
	{"PST", "Pacific Standard Time", "North America", -8}, 
	{"Q", "Quebec Time Zone", "Military", -4}, 
	{"R", "Romeo Time Zone", "Military", -5}, 
	{"S", "Sierra Time Zone", "Military", -6}, 
	{"T", "Tango Time Zone", "Military", -7}, 
	{"U", "Uniform Time Zone", "Military", -8}, 
	{"UTC", "Coordinated Universal Time", "Europe", 0}, 
	{"V", "Victor Time Zone", "Military", -9}, 
	{"W", "Whiskey Time Zone", "Military", -10}, 
	{"WEST", "Western European Summer Time", "Europe",  1}, 
	{"WET", "Western European Time", "Europe", 0}, 
	{"WST", "Western Standard Time", "Australia",  8}, 
	{"X", "X-ray Time Zone", "Military", -11}, 
	{"Y", "Yankee Time Zone", "Military", -12}, 
	{"Z", "Zulu Time Zone", "Military", 0},
	{NULL, NULL, NULL, 0},
	};
	
	UTzoneEntry *zone = zones;
	
	while (zone->symbol) 
	{
		if (strcmp(zone->symbol, zoneSymbol) == 0 ||
			strcmp(zone->name, zoneSymbol) == 0) return zone->hoursFromGMT;
		zone ++;
	}
	return 24;
}

float UTzone_HoursBetweenZone_and_(char *fromZone, const char *toZone)
{
    return UTzone_HoursFromGMTFor_(toZone) - UTzone_HoursFromGMTFor_(fromZone);
}

