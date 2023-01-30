static void framesync_advance(FFFrameSync *fs)
{
    int latest;
    unsigned i;
    int64_t pts;
    if (fs->eof)
        return;
    while (!fs->frame_ready) {
        latest = -1;
        for (i = 0; i < fs->nb_in; i++) {
            if (!fs->in[i].have_next) {
                if (latest < 0 || fs->in[i].pts < fs->in[latest].pts)
                    latest = i;
            }
        }
        if (latest >= 0) {
            fs->in_request = latest;
            break;
        }
        pts = fs->in[0].pts_next;
        for (i = 1; i < fs->nb_in; i++)
            if (fs->in[i].pts_next < pts)
                pts = fs->in[i].pts_next;
        if (pts == INT64_MAX) {
            fs->eof = 1;
            break;
        }
        for (i = 0; i < fs->nb_in; i++) {
            if (fs->in[i].pts_next == pts ||
                (fs->in[i].before == EXT_INFINITY &&
                 fs->in[i].state == STATE_BOF)) {
                av_frame_free(&fs->in[i].frame);
                fs->in[i].frame      = fs->in[i].frame_next;
                fs->in[i].pts        = fs->in[i].pts_next;
                fs->in[i].frame_next = NULL;
                fs->in[i].pts_next   = AV_NOPTS_VALUE;
                fs->in[i].have_next  = 0;
                fs->in[i].state      = fs->in[i].frame ? STATE_RUN : STATE_EOF;
                if (fs->in[i].sync == fs->sync_level && fs->in[i].frame)
                    fs->frame_ready = 1;
                if (fs->in[i].state == STATE_EOF &&
                    fs->in[i].after == EXT_STOP)
                    fs->eof = 1;
            }
        }
        if (fs->eof)
            fs->frame_ready = 0;
        if (fs->frame_ready)
            for (i = 0; i < fs->nb_in; i++)
                if ((fs->in[i].state == STATE_BOF &&
                     fs->in[i].before == EXT_STOP))
                    fs->frame_ready = 0;
        fs->pts = pts;
    }
}
