#define HLS_READ_SIZE 65536
static int hls_Download(stream_t *s, segment_t *segment)
{
    stream_sys_t *p_sys = s->p_sys;
    assert(segment);
    stream_t *p_ts = stream_UrlNew(s, segment->url);
    if (p_ts == NULL)
        return VLC_EGENERIC;
    int64_t size = stream_Size(p_ts);
    if (size < 0)
        size = 0;
    unsigned i_total_read = 0;
    int i_return = VLC_SUCCESS;
    block_t *p_segment_data = block_Alloc(size ? size : HLS_READ_SIZE);
    if (!p_segment_data)
    {
        i_return = VLC_ENOMEM;
        goto end;
    }
    for( ;; )
    {
        /* NOTE: Beware the size reported for a segment by the HLS server may not
         * be correct, when downloading the segment data. Therefore check the size
         * and enlarge the segment data block if necessary.
         */
        uint64_t i_toread = (size > 0) ? (uint64_t) size - i_total_read: HLS_READ_SIZE;
        if (i_total_read + i_toread > p_segment_data->i_buffer)
        {
            msg_Dbg(s, "size changed to %"PRIu64, i_total_read + i_toread);
            block_t *p_realloc_block = block_Realloc(p_segment_data, 0, i_total_read + i_toread);
            if (p_realloc_block == NULL)
            {
                if(p_segment_data)
                    block_Release(p_segment_data);
                i_return = VLC_ENOMEM;
                goto end;
            }
            p_segment_data = p_realloc_block;
        }
        int i_canc = vlc_savecancel();
        int i_length = stream_Read(p_ts, &p_segment_data->p_buffer[i_total_read],
                                   (i_toread >= HLS_READ_SIZE) ? HLS_READ_SIZE : i_toread);
        vlc_restorecancel(i_canc);
        if (i_length <= 0)
        {
            if(size > 0 && i_total_read < size)
                msg_Warn(s, "segment read %"PRIu64"/%"PRIu64, size - i_total_read, size );
            p_segment_data->i_buffer = i_total_read;
            break;
        }
        i_total_read += i_length;
        if (atomic_load(&p_sys->closing))
            break;
    };
    segment->data = p_segment_data;
    segment->size = p_segment_data->i_buffer;
end:
    stream_Delete(p_ts);
    return i_return;
}
