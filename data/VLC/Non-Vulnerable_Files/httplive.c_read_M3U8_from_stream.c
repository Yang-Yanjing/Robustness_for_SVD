/* Read M3U8 file */
static ssize_t read_M3U8_from_stream(stream_t *s, uint8_t **buffer)
{
    uint8_t *p = NULL;
    int64_t size = stream_Size(s);
    size = VLC_CLIP(size, 0, INT64_MAX - 1);
    int64_t i_alloc_size = 0;
    ssize_t i_total_read = 0;
    unsigned i_chunk_size = HLS_READ_SIZE;
    for( ;; )
    {
        int i_toread = (size) ? size - i_total_read : i_chunk_size;
        if(i_toread + i_total_read > INT64_MAX - 1)
            break;
        if(i_alloc_size < i_toread)
        {
            i_alloc_size += i_toread;
            p = realloc_or_free(p, 1 + i_alloc_size);
            if (p == NULL)
                return VLC_ENOMEM;
            if (i_chunk_size < (1 << 26))
                i_chunk_size <<= 1;
        }
        int i_read = stream_Read(s, & p[i_total_read], i_toread);
        if (i_read == 0)
        {
            break;      /* EOF ? */
        }
        else if (i_read < 0)
        {
            free (p);
            return i_read;
        }
        else
        {
            i_total_read += i_read;
        }
    }
    p[i_total_read] = '\0';
    *buffer = p;
    return i_total_read;
}
