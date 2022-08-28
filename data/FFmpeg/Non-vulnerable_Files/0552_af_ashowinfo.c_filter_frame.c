static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext *ctx = inlink->dst;
    AShowInfoContext *s  = ctx->priv;
    char chlayout_str[128];
    uint32_t checksum = 0;
    int channels    = inlink->channels;
    int planar      = av_sample_fmt_is_planar(buf->format);
    int block_align = av_get_bytes_per_sample(buf->format) * (planar ? 1 : channels);
    int data_size   = buf->nb_samples * block_align;
    int planes      = planar ? channels : 1;
    int i;
    void *tmp_ptr = av_realloc_array(s->plane_checksums, channels, sizeof(*s->plane_checksums));
    if (!tmp_ptr)
        return AVERROR(ENOMEM);
    s->plane_checksums = tmp_ptr;
    for (i = 0; i < planes; i++) {
        uint8_t *data = buf->extended_data[i];
        s->plane_checksums[i] = av_adler32_update(0, data, data_size);
        checksum = i ? av_adler32_update(checksum, data, data_size) :
                       s->plane_checksums[0];
    }
    av_get_channel_layout_string(chlayout_str, sizeof(chlayout_str), -1,
                                 buf->channel_layout);
    av_log(ctx, AV_LOG_INFO,
           "n:%"PRId64" pts:%s pts_time:%s pos:%"PRId64" "
           "fmt:%s channels:%d chlayout:%s rate:%d nb_samples:%d "
           "checksum:%08"PRIX32" ",
           inlink->frame_count,
           av_ts2str(buf->pts), av_ts2timestr(buf->pts, &inlink->time_base),
           av_frame_get_pkt_pos(buf),
           av_get_sample_fmt_name(buf->format), av_frame_get_channels(buf), chlayout_str,
           buf->sample_rate, buf->nb_samples,
           checksum);
    av_log(ctx, AV_LOG_INFO, "plane_checksums: [ ");
    for (i = 0; i < planes; i++)
        av_log(ctx, AV_LOG_INFO, "%08"PRIX32" ", s->plane_checksums[i]);
    av_log(ctx, AV_LOG_INFO, "]\n");
    for (i = 0; i < buf->nb_side_data; i++) {
        AVFrameSideData *sd = buf->side_data[i];
        av_log(ctx, AV_LOG_INFO, "  side data - ");
        switch (sd->type) {
        case AV_FRAME_DATA_MATRIXENCODING: dump_matrixenc (ctx, sd); break;
        case AV_FRAME_DATA_DOWNMIX_INFO:   dump_downmix   (ctx, sd); break;
        case AV_FRAME_DATA_REPLAYGAIN:     dump_replaygain(ctx, sd); break;
        case AV_FRAME_DATA_AUDIO_SERVICE_TYPE: dump_audio_service_type(ctx, sd); break;
        default:                           dump_unknown   (ctx, sd); break;
        }
        av_log(ctx, AV_LOG_INFO, "\n");
    }
    return ff_filter_frame(inlink->dst->outputs[0], buf);
}
