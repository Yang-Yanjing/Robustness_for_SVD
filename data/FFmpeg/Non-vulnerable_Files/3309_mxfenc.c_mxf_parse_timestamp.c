static uint64_t mxf_parse_timestamp(time_t timestamp)
{
    struct tm tmbuf;
    struct tm *time = gmtime_r(&timestamp, &tmbuf);
    if (!time)
        return 0;
    return (uint64_t)(time->tm_year+1900) << 48 |
           (uint64_t)(time->tm_mon+1)     << 40 |
           (uint64_t) time->tm_mday       << 32 |
                      time->tm_hour       << 24 |
                      time->tm_min        << 16 |
                      time->tm_sec        << 8;
}
