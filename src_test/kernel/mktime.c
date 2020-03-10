#include "common.h"
#include "time.h"

#define MINUTE 60
#define HOUR (60 * MINUTE)
#define DAY (24 * HOUR)
#define YEAR (365 * DAY)

static int month[12] = 
                        {
                            	0,
	                            DAY * (31),
	                            DAY * (31 + 29),
	                            DAY * (31 + 29 + 31),
	                            DAY * (31 + 29 + 31 + 30),
	                            DAY * (31 + 29 + 31 + 30 + 31),
	                            DAY * (31 + 29 + 31 + 30 + 31 + 30),
	                            DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31),
	                            DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
	                            DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
	                            DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
	                            DAY * (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)
                        };

int64_t kernel_mktime(struct tm *tm)
{
    int64_t res;
    int year;

    year = tm -> tm_year - 70;
    res = YEAR * year + DAY * ((year + 1) >> 2);
    res += month[tm -> tm_mon];

    if((tm -> tm_mon > 1) && ((year + 2) & 0x03))
    {
        res -= DAY;
    }

    res += DAY * (tm -> tm_mday + 1);
    res += HOUR * tm -> tm_hour;
    res += MINUTE * tm -> tm_min;
    res += tm -> tm_sec;
    return res;
}