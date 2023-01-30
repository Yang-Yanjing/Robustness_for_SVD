 */
static int parse_M3U8(stream_t *s, vlc_array_t *streams, uint8_t *buffer,
                      const ssize_t len, bool b_parent_is_meta)
{
    stream_sys_t *p_sys = s->p_sys;
    uint8_t *p_read, *p_begin, *p_end;
    assert(streams);
    assert(buffer);
    msg_Dbg(s, "parse_M3U8\n%s", buffer);
    p_begin = buffer;
    p_end = p_begin + len;
    char *line = ReadLine(p_begin, &p_read, p_end - p_begin);
    if (line == NULL)
        return VLC_ENOMEM;
    p_begin = p_read;
    if (strncmp(line, "#EXTM3U", 7) != 0)
    {
        msg_Err(s, "missing #EXTM3U tag .. aborting");
        free(line);
        return VLC_EGENERIC;
    }
    free(line);
    line = NULL;
    /* What is the version ? */
    int version = 1;
    uint8_t *p = (uint8_t *)strstr((const char *)buffer, "#EXT-X-VERSION:");
    if (p != NULL)
    {
        uint8_t *tmp = NULL;
        char *psz_version = ReadLine(p, &tmp, p_end - p);
        if (psz_version == NULL)
            return VLC_ENOMEM;
        int ret = sscanf((const char*)psz_version, "#EXT-X-VERSION:%d", &version);
        if (ret != 1)
        {
            msg_Warn(s, "#EXT-X-VERSION: no protocol version found, assuming version 1.");
            version = 1;
        }
        free(psz_version);
        p = NULL;
    }
    /* Is it a live stream ? */
    p_sys->b_live = (strstr((const char *)buffer, "#EXT-X-ENDLIST") == NULL) ? true : false;
    /* Is it a meta index file ? */
    bool b_meta = (strstr((const char *)buffer, "#EXT-X-STREAM-INF") == NULL) ? false : true;
    if(b_meta && b_parent_is_meta) /* Nested meta playlists ?? */
        return VLC_EGENERIC;
    int err = VLC_SUCCESS;
    if (b_meta)
    {
        msg_Dbg(s, "Meta playlist");
        /* M3U8 Meta Index file */
        do {
            /* Next line */
            line = ReadLine(p_begin, &p_read, p_end - p_begin);
            if (line == NULL)
                break;
            p_begin = p_read;
            /* */
            if (strncmp(line, "#EXT-X-STREAM-INF", 17) == 0)
            {
                p_sys->b_meta = true;
                char *uri = ReadLine(p_begin, &p_read, p_end - p_begin);
                if (uri == NULL)
                    err = VLC_ENOMEM;
                else
                {
                    if (*uri == '#')
                    {
                        msg_Warn(s, "Skipping invalid stream-inf: %s", uri);
                        free(uri);
                    }
                    else
                    {
                        bool new_stream_added = false;
                        hls_stream_t *hls = NULL;
                        err = parse_StreamInformation(s, &streams, &hls, line, uri);
                        if (err == VLC_SUCCESS)
                            new_stream_added = true;
                        free(uri);
                        if (hls)
                        {
                            /* Download playlist file from server */
                            uint8_t *buf = NULL;
                            ssize_t len = read_M3U8_from_url(s, hls->url, &buf);
                            if (len < 0)
                            {
                                msg_Warn(s, "failed to read %s, continue for other streams", hls->url);
                                /* remove stream just added */
                                if (new_stream_added)
                                    vlc_array_remove(streams, vlc_array_count(streams) - 1);
                                /* ignore download error, so we have chance to try other streams */
                                err = VLC_SUCCESS;
                            }
                            else
                            {
                                /* Parse HLS m3u8 content. */
                                err = parse_M3U8(s, streams, buf, len, b_meta);
                                free(buf);
                            }
                            hls->version = version;
                            if (!p_sys->b_live)
                                hls->size = hls_GetStreamSize(hls); /* Stream size (approximate) */
                        }
                    }
                }
                p_begin = p_read;
            }
            free(line);
            line = NULL;
            if (p_begin >= p_end)
                break;
        } while (err == VLC_SUCCESS);
        size_t stream_count = vlc_array_count(streams);
        msg_Dbg(s, "%d streams loaded in Meta playlist", (int)stream_count);
        if (stream_count == 0)
        {
            msg_Err(s, "No playable streams found in Meta playlist");
            err = VLC_EGENERIC;
        }
    }
    else
    {
        msg_Dbg(s, "%s Playlist HLS protocol version: %d", p_sys->b_live ? "Live": "VOD", version);
        hls_stream_t *hls = NULL;
        if (p_sys->b_meta)
            hls = hls_GetLast(streams);
        else
        {
            /* No Meta playlist used */
            hls = hls_New(streams, 0, 0, p_sys->m3u8);
            if (hls)
            {
                /* Get TARGET-DURATION first */
                p = (uint8_t *)strstr((const char *)buffer, "#EXT-X-TARGETDURATION:");
                if (p)
                {
                    uint8_t *p_rest = NULL;
                    char *psz_duration = ReadLine(p, &p_rest,  p_end - p);
                    if (psz_duration == NULL)
                        return VLC_EGENERIC;
                    err = parse_TargetDuration(s, hls, psz_duration);
                    free(psz_duration);
                    p = NULL;
                }
                /* Store version */
                hls->version = version;
            }
            else return VLC_ENOMEM;
        }
        assert(hls);
        /* */
        bool media_sequence_loaded = false;
        int segment_duration = -1;
        do
        {
            /* Next line */
            line = ReadLine(p_begin, &p_read, p_end - p_begin);
            if (line == NULL)
                break;
            p_begin = p_read;
            if (strncmp(line, "#EXTINF", 7) == 0)
                err = parse_SegmentInformation(hls, line, &segment_duration);
            else if (strncmp(line, "#EXT-X-TARGETDURATION", 21) == 0)
                err = parse_TargetDuration(s, hls, line);
            else if (strncmp(line, "#EXT-X-MEDIA-SEQUENCE", 21) == 0)
            {
                /* A Playlist file MUST NOT contain more than one EXT-X-MEDIA-SEQUENCE tag. */
                /* We only care about first one */
                if (!media_sequence_loaded)
                {
                    err = parse_MediaSequence(s, hls, line);
                    media_sequence_loaded = true;
                }
            }
            else if (strncmp(line, "#EXT-X-KEY", 10) == 0)
                err = parse_Key(s, hls, line);
            else if (strncmp(line, "#EXT-X-PROGRAM-DATE-TIME", 24) == 0)
                err = parse_ProgramDateTime(s, hls, line);
            else if (strncmp(line, "#EXT-X-ALLOW-CACHE", 18) == 0)
                err = parse_AllowCache(s, hls, line);
            else if (strncmp(line, "#EXT-X-DISCONTINUITY", 20) == 0)
                err = parse_Discontinuity(s, hls, line);
            else if (strncmp(line, "#EXT-X-VERSION", 14) == 0)
                err = parse_Version(s, hls, line);
            else if (strncmp(line, "#EXT-X-ENDLIST", 14) == 0)
                err = parse_EndList(s, hls);
            else if ((strncmp(line, "#", 1) != 0) && (*line != '\0') )
            {
                err = parse_AddSegment(hls, segment_duration, line);
                segment_duration = -1; /* reset duration */
            }
            free(line);
            line = NULL;
            if (p_begin >= p_end)
                break;
        } while (err == VLC_SUCCESS);
        free(line);
    }
    return err;
}
