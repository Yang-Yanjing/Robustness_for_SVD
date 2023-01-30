static void matroska_metadata_creation_time(AVDictionary **metadata, int64_t date_utc)
{
    char buffer[32];
    
    time_t creation_time = date_utc / 1000000000 + 978307200;
    struct tm tmpbuf, *ptm = gmtime_r(&creation_time, &tmpbuf);
    if (!ptm) return;
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ptm))
        av_dict_set(metadata, "creation_time", buffer, 0);
}
