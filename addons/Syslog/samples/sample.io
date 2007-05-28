#!/usr/bin/env io

# Test by Jeremy Tregunna <jtregunna@blurgle.ca>
# Not all of these options are needed to log. Most basic you only need to call:
# open(fac, opt, ident), log(pri, msg), close

logger := Syslog clone do (
	identity("SyslogTest")
	facility(facilityMap at("LOG_USER"))
	options(List add(optionsMap at("LOG_PID"), optionsMap at("LOG_CONS")))
	priority(priorityMap at("LOG_INFO"))
	open(facility, options)
	mask(List add(maskMap at("LOG_PRIMASK")))
	log(priority, "*** Merely a test ***")
	close
)

