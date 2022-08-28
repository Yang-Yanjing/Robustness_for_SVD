int main(void)
{
    printf("Testing av_parse_video_rate()\n");
    {
        int i;
        static const char *const rates[] = {
            "-inf",
            "inf",
            "nan",
            "123/0",
            "-123 / 0",
            "",
            "/",
            " 123  /  321",
            "foo/foo",
            "foo/1",
            "1/foo",
            "0/0",
            "/0",
            "1/",
            "1",
            "0",
            "-123/123",
            "-foo",
            "123.23",
            ".23",
            "-.23",
            "-0.234",
            "-0.0000001",
            "  21332.2324   ",
            " -21332.2324   ",
        };
        for (i = 0; i < FF_ARRAY_ELEMS(rates); i++) {
            int ret;
            AVRational q = { 0, 0 };
            ret = av_parse_video_rate(&q, rates[i]);
            printf("'%s' -> %d/%d %s\n",
                   rates[i], q.num, q.den, ret ? "ERROR" : "OK");
        }
    }
    printf("\nTesting av_parse_color()\n");
    {
        int i;
        uint8_t rgba[4];
        static const char *const color_names[] = {
            "bikeshed",
            "RaNdOm",
            "foo",
            "red",
            "Red ",
            "RED",
            "Violet",
            "Yellow",
            "Red",
            "0x000000",
            "0x0000000",
            "0xff000000",
            "0x3e34ff",
            "0x3e34ffaa",
            "0xffXXee",
            "0xfoobar",
            "0xffffeeeeeeee",
            "#ff0000",
            "#ffXX00",
            "ff0000",
            "ffXX00",
            "red@foo",
            "random@10",
            "0xff0000@1.0",
            "red@",
            "red@0xfff",
            "red@0xf",
            "red@2",
            "red@0.1",
            "red@-1",
            "red@0.5",
            "red@1.0",
            "red@256",
            "red@10foo",
            "red@-1.0",
            "red@-0.0",
        };
        av_log_set_level(AV_LOG_DEBUG);
        for (i = 0;  i < FF_ARRAY_ELEMS(color_names); i++) {
            if (av_parse_color(rgba, color_names[i], -1, NULL) >= 0)
                printf("%s -> R(%d) G(%d) B(%d) A(%d)\n",
                       color_names[i], rgba[0], rgba[1], rgba[2], rgba[3]);
            else
                printf("%s -> error\n", color_names[i]);
        }
    }
    printf("\nTesting av_small_strptime()\n");
    {
        int i;
        struct tm tm = { 0 };
        struct fmt_timespec_entry {
            const char *fmt, *timespec;
        } fmt_timespec_entries[] = {
            { "%Y-%m-%d",                    "2012-12-21" },
            { "%Y - %m - %d",                "2012-12-21" },
            { "%Y-%m-%d %H:%M:%S",           "2012-12-21 20:12:21" },
            { "  %Y - %m - %d %H : %M : %S", "   2012 - 12 -  21   20 : 12 : 21" },
        };
        av_log_set_level(AV_LOG_DEBUG);
        for (i = 0;  i < FF_ARRAY_ELEMS(fmt_timespec_entries); i++) {
            char *p;
            struct fmt_timespec_entry *e = &fmt_timespec_entries[i];
            printf("fmt:'%s' spec:'%s' -> ", e->fmt, e->timespec);
            p = av_small_strptime(e->timespec, e->fmt, &tm);
            if (p) {
                printf("%04d-%02d-%2d %02d:%02d:%02d\n",
                       1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
                       tm.tm_hour, tm.tm_min, tm.tm_sec);
            } else {
                printf("error\n");
            }
        }
    }
    printf("\nTesting av_parse_time()\n");
    {
        int i;
        int64_t tv;
        time_t tvi;
        struct tm *tm;
        static char tzstr[] = "TZ=CET-1";
        static const char * const time_string[] = {
            "now",
            "12:35:46",
            "2000-12-20 0:02:47.5z",
            "2000-12-20T010247.6",
        };
        static const char * const duration_string[] = {
            "2:34:56.79",
            "-1:23:45.67",
            "42.1729",
            "-1729.42",
            "12:34",
        };
        av_log_set_level(AV_LOG_DEBUG);
        putenv(tzstr);
        printf("(now is 2012-03-17 09:14:13 +0100, local time is UTC+1)\n");
        for (i = 0;  i < FF_ARRAY_ELEMS(time_string); i++) {
            printf("%-24s -> ", time_string[i]);
            if (av_parse_time(&tv, time_string[i], 0)) {
                printf("error\n");
            } else {
                tvi = tv / 1000000;
                tm = gmtime(&tvi);
                printf("%14"PRIi64".%06d = %04d-%02d-%02dT%02d:%02d:%02dZ\n",
                       tv / 1000000, (int)(tv % 1000000),
                       tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                       tm->tm_hour, tm->tm_min, tm->tm_sec);
            }
        }
        for (i = 0;  i < FF_ARRAY_ELEMS(duration_string); i++) {
            printf("%-24s -> ", duration_string[i]);
            if (av_parse_time(&tv, duration_string[i], 1)) {
                printf("error\n");
            } else {
                printf("%+21"PRIi64"\n", tv);
            }
        }
    }
    return 0;
}
