#include "stdafx.h"
#include <time.h>
#include <strstrea.h>
#include <iomanip.h>

static char BASED_CODE weekday[7][10] = {
	"Lunedi",
	"Martedi",
	"Mercoledi",
	"Giovedi",
	"Venerdi",
	"Sabato",
	"Domenica"
};
static char BASED_CODE month[12][10] = {
	"Gennaio",
	"Febbraio",
	"Marzo",
	"Aprile",
	"Maggio",
	"Giugno",
	"Luglio",
	"Agosto",
	"Settembre",
	"Ottobre",
	"Novembre",
	"Dicembre"
};

size_t strftime(char *buf, size_t dim, const char *fmt, const struct tm *t)
{
	ostrstream os(buf, dim);
	os.fill('0');

	char c;
	while ((c = *fmt++) != 0)
		if (c == '%')
			switch (c = *fmt++)
			{
			case 'Z': // Time zone name or abbreviation; no characters if time zone is unknown
				os << "ITA";
				break;

			case 'p': // Current locale's AM/PM indicator for a 12-hour clock
				os << (t->tm_hour < 12? 'A' : 'P');
				break;

			case 'x': // Date representation for current locale
				os <<	setw(2) << t->tm_mday << '/' <<
						setw(2) << t->tm_mon + 1 << '/' <<
						setw(2) << t->tm_year;
				break;

			case 'X': // Time representation for current locale
				os <<	setw(2) << t->tm_hour << ':' <<
						setw(2) << t->tm_min;
				break;

			case 'U': // Week of the year as a decimal number, with Sunday as the first day of the week (00-51)
			case 'W': // Week of the year as a decimal number, with Monday as the first day of the week (00-51)
				os << setw(2) << t->tm_yday % 7;
				break;

			case 'c': // Date and time representation appropriate for the locale
				os <<	setw(2) << t->tm_hour << ':' <<
						setw(2) << t->tm_min << ' ' <<

						setw(2) << t->tm_mday << '/' <<
						setw(2) << t->tm_mon + 1 << '/' <<
						setw(2) << t->tm_year;
				break;

			case 'a': // Abbreviated weekday name
				if (t->tm_wday >= 0 && t->tm_wday < 7)
					os.write(weekday[t->tm_wday], 3);
				break;
			case 'A': // Full weekday name
				if (t->tm_wday >= 0 && t->tm_wday < 7)
					os << weekday[t->tm_wday];
				break;
			case 'b': // Abbreviated month name
				if (t->tm_mon >= 0 && t->tm_mon < 12)
					os.write(month[t->tm_mon], 3);
				break;
			case 'B': // Full month name
				if (t->tm_mon >= 0 && t->tm_mon < 12)
					os << month[t->tm_mon];
				break;
			case 'd': // Day of the month as a decimal number (01-31)
				os << setw(2) << t->tm_mday;
				break;
			case 'H': // Hour in 24-hour format (00-23)
				os << setw(2) << t->tm_hour;
				break;
			case 'I': // Hour in 12-hour format (01-12)
				os << setw(2) << t->tm_hour / 2 + 1;
				break;
			case 'j': // Day of the year as a decimal number (001-366)
				os << setw(3) << t->tm_yday + 1;
				break;
			case 'm': // Month as a decimal number (01-12)
				os << setw(2) << t->tm_mon + 1;
				break;
			case 'M': // Minute as a decimal number (00-59)
				os << setw(2) << t->tm_min;
				break;
			case 'S': // Second as a decimal number (00-59)
				os << setw(2) << t->tm_sec;
				break;
			case 'w': // Weekday as a decimal number (0-6; Sunday is 0)
				os << t->tm_wday;
				break;
			case 'y': // Year without the century as a decimal number (00-99)
				os << setw(2) << t->tm_year % 100;
				break;
			case 'Y': // Year with the century as a decimal number
				os << 1900 + t->tm_year;
				break;

			case '%': // Percent sign
				os << c;
				break;

			default:
				break;
			}
		else
			os << c;

	os << ends;
	return os.pcount();
}
