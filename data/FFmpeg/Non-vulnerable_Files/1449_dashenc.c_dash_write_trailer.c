static int dash_write_trailer(AVFormatContext *s)
{
    DASHContext *c = s->priv_data;
    if (s->nb_streams > 0) {
        OutputStream *os = &c->streams[0];
        
        
        if (!c->last_duration)
            c->last_duration = av_rescale_q(os->max_pts - os->start_pts,
                                            s->streams[0]->time_base,
                                            AV_TIME_BASE_Q);
        c->total_duration = av_rescale_q(os->max_pts - os->first_pts,
                                         s->streams[0]->time_base,
                                         AV_TIME_BASE_Q);
    }
    dash_flush(s, 1, -1);
    if (c->remove_at_exit) {
        char filename[1024];
        int i;
        for (i = 0; i < s->nb_streams; i++) {
            OutputStream *os = &c->streams[i];
            snprintf(filename, sizeof(filename), "%s%s", c->dirname, os->initfile);
            unlink(filename);
        }
        unlink(s->filename);
    }
    dash_free(s);
    return 0;
}
