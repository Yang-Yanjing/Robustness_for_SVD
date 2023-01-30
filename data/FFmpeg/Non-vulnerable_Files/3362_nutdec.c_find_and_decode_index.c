static int find_and_decode_index(NUTContext *nut)
{
    AVFormatContext *s = nut->avf;
    AVIOContext *bc    = s->pb;
    uint64_t tmp, end;
    int i, j, syncpoint_count;
    int64_t filesize = avio_size(bc);
    int64_t *syncpoints = NULL;
    uint64_t max_pts;
    int8_t *has_keyframe = NULL;
    int ret = AVERROR_INVALIDDATA;
    if(filesize <= 0)
        return -1;
    avio_seek(bc, filesize - 12, SEEK_SET);
    avio_seek(bc, filesize - avio_rb64(bc), SEEK_SET);
    if (avio_rb64(bc) != INDEX_STARTCODE) {
        av_log(s, AV_LOG_ERROR, "no index at the end\n");
        if(s->duration<=0)
            s->duration = find_duration(nut, filesize);
        return ret;
    }
    end  = get_packetheader(nut, bc, 1, INDEX_STARTCODE);
    end += avio_tell(bc);
    max_pts = ffio_read_varlen(bc);
    s->duration = av_rescale_q(max_pts / nut->time_base_count,
                               nut->time_base[max_pts % nut->time_base_count],
                               AV_TIME_BASE_Q);
    s->duration_estimation_method = AVFMT_DURATION_FROM_PTS;
    GET_V(syncpoint_count, tmp < INT_MAX / 8 && tmp > 0);
    syncpoints   = av_malloc_array(syncpoint_count, sizeof(int64_t));
    has_keyframe = av_malloc_array(syncpoint_count + 1, sizeof(int8_t));
    if (!syncpoints || !has_keyframe) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    for (i = 0; i < syncpoint_count; i++) {
        syncpoints[i] = ffio_read_varlen(bc);
        if (syncpoints[i] <= 0)
            goto fail;
        if (i)
            syncpoints[i] += syncpoints[i - 1];
    }
    for (i = 0; i < s->nb_streams; i++) {
        int64_t last_pts = -1;
        for (j = 0; j < syncpoint_count;) {
            uint64_t x = ffio_read_varlen(bc);
            int type   = x & 1;
            int n      = j;
            x >>= 1;
            if (type) {
                int flag = x & 1;
                x >>= 1;
                if (n + x >= syncpoint_count + 1) {
                    av_log(s, AV_LOG_ERROR, "index overflow A %d + %"PRIu64" >= %d\n", n, x, syncpoint_count + 1);
                    goto fail;
                }
                while (x--)
                    has_keyframe[n++] = flag;
                has_keyframe[n++] = !flag;
            } else {
                if (x <= 1) {
                    av_log(s, AV_LOG_ERROR, "index: x %"PRIu64" is invalid\n", x);
                    goto fail;
                }
                while (x != 1) {
                    if (n >= syncpoint_count + 1) {
                        av_log(s, AV_LOG_ERROR, "index overflow B\n");
                        goto fail;
                    }
                    has_keyframe[n++] = x & 1;
                    x >>= 1;
                }
            }
            if (has_keyframe[0]) {
                av_log(s, AV_LOG_ERROR, "keyframe before first syncpoint in index\n");
                goto fail;
            }
            av_assert0(n <= syncpoint_count + 1);
            for (; j < n && j < syncpoint_count; j++) {
                if (has_keyframe[j]) {
                    uint64_t B, A = ffio_read_varlen(bc);
                    if (!A) {
                        A = ffio_read_varlen(bc);
                        B = ffio_read_varlen(bc);
                        
                    } else
                        B = 0;
                    av_add_index_entry(s->streams[i], 16 * syncpoints[j - 1],
                                       last_pts + A, 0, 0, AVINDEX_KEYFRAME);
                    last_pts += A + B;
                }
            }
        }
    }
    if (skip_reserved(bc, end) || ffio_get_checksum(bc)) {
        av_log(s, AV_LOG_ERROR, "index checksum mismatch\n");
        goto fail;
    }
    ret = 0;
fail:
    av_free(syncpoints);
    av_free(has_keyframe);
    return ret;
}
