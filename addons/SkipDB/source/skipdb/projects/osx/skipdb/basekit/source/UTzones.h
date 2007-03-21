
#ifdef __cplusplus
extern "C" {
#endif

float UTzone_HoursFromGMTFor_(const char *zoneSymbol); /* return 24 for error */
float UTzone_HoursBetweenZone_and_(char *fromZone, const char *toZone);

#ifdef __cplusplus
}
#endif
