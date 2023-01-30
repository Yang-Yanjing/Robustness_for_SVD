 ****************************************************************************/
static bool isHTTPLiveStreaming(stream_t *s)
{
    const uint8_t *peek;
    int size = stream_Peek(s->p_source, &peek, 7);
    if (size < 7)
        return false;
    if (memcmp(peek, "#EXTM3U", 7) != 0)
        return false;
    size = stream_Peek(s->p_source, &peek, 2048);
    if (unlikely(size < 7))
        return false;
    peek += 7;
    size -= 7;
    /* Parse stream and search for
     * EXT-X-TARGETDURATION or EXT-X-STREAM-INF tag, see
     * http://tools.ietf.org/html/draft-pantos-http-live-streaming-04#page-8 */
    while (size--)
    {
        static const char *const ext[] = {
            "TARGETDURATION",
            "MEDIA-SEQUENCE",
            "KEY",
            "ALLOW-CACHE",
            "ENDLIST",
            "STREAM-INF",
            "DISCONTINUITY",
            "VERSION"
        };
        if (*peek++ != '#')
            continue;
        if (size < 6)
            continue;
        if (memcmp(peek, "EXT-X-", 6))
            continue;
        peek += 6;
        size -= 6;
        for (size_t i = 0; i < ARRAY_SIZE(ext); i++)
        {
            size_t len = strlen(ext[i]);
            if (size < 0 || (size_t)size < len)
                continue;
            if (!memcmp(peek, ext[i], len))
                return true;
        }
    }
    return false;
}
