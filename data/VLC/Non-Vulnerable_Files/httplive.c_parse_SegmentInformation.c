}
static int parse_SegmentInformation(hls_stream_t *hls, char *p_read, int *duration)
{
    assert(hls);
    assert(p_read);
    /* strip of #EXTINF: */
    char *p_next = NULL;
    char *token = strtok_r(p_read, ":", &p_next);
    if (token == NULL)
        return VLC_EGENERIC;
    /* read duration */
    token = strtok_r(NULL, ",", &p_next);
    if (token == NULL)
        return VLC_EGENERIC;
    int value;
    char *endptr;
    if (hls->version < 3)
    {
        errno = 0;
        value = strtol(token, &endptr, 10);
        if (token == endptr || errno == ERANGE)
        {
            *duration = -1;
            return VLC_EGENERIC;
        }
        *duration = value;
    }
    else
    {
        errno = 0;
        double d = strtof(token, &endptr);
        if (token == endptr || errno == ERANGE)
        {
            *duration = -1;
            return VLC_EGENERIC;
        }
        if ((d) - ((int)d) >= 0.5)
            value = ((int)d) + 1;
        else
            value = ((int)d);
        *duration = value;
    }
    if( *duration > hls->max_segment_length)
        hls->max_segment_length = *duration;
    /* Ignore the rest of the line */
    return VLC_SUCCESS;
}
