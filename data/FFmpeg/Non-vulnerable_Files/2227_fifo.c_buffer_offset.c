static void buffer_offset(AVFilterLink *link, AVFrame *frame,
                          int offset)
{
    int nb_channels = av_get_channel_layout_nb_channels(link->channel_layout);
    int planar = av_sample_fmt_is_planar(link->format);
    int planes = planar ? nb_channels : 1;
    int block_align = av_get_bytes_per_sample(link->format) * (planar ? 1 : nb_channels);
    int i;
    av_assert0(frame->nb_samples > offset);
    for (i = 0; i < planes; i++)
        frame->extended_data[i] += block_align * offset;
    if (frame->data != frame->extended_data)
        memcpy(frame->data, frame->extended_data,
               FFMIN(planes, FF_ARRAY_ELEMS(frame->data)) * sizeof(*frame->data));
    frame->linesize[0] -= block_align*offset;
    frame->nb_samples -= offset;
    if (frame->pts != AV_NOPTS_VALUE) {
        frame->pts += av_rescale_q(offset, (AVRational){1, link->sample_rate},
                                   link->time_base);
    }
}
