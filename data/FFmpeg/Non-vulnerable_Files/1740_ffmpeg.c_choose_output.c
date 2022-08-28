static OutputStream *choose_output(void)
{
    int i;
    int64_t opts_min = INT64_MAX;
    OutputStream *ost_min = NULL;
    for (i = 0; i < nb_output_streams; i++) {
        OutputStream *ost = output_streams[i];
        int64_t opts = ost->st->cur_dts == AV_NOPTS_VALUE ? INT64_MIN :
                       av_rescale_q(ost->st->cur_dts, ost->st->time_base,
                                    AV_TIME_BASE_Q);
        if (ost->st->cur_dts == AV_NOPTS_VALUE)
            av_log(NULL, AV_LOG_DEBUG, "cur_dts is invalid (this is harmless if it occurs once at the start per stream)\n");
        if (!ost->initialized && !ost->inputs_done)
            return ost;
        if (!ost->finished && opts < opts_min) {
            opts_min = opts;
            ost_min  = ost->unavailable ? NULL : ost;
        }
    }
    return ost_min;
}
