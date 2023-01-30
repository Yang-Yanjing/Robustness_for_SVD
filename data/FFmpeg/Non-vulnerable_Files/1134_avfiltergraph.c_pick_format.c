static int pick_format(AVFilterLink *link, AVFilterLink *ref)
{
    if (!link || !link->in_formats)
        return 0;
    if (link->type == AVMEDIA_TYPE_VIDEO) {
        if(ref && ref->type == AVMEDIA_TYPE_VIDEO){
            int has_alpha= av_pix_fmt_desc_get(ref->format)->nb_components % 2 == 0;
            enum AVPixelFormat best= AV_PIX_FMT_NONE;
            int i;
            for (i=0; i<link->in_formats->nb_formats; i++) {
                enum AVPixelFormat p = link->in_formats->formats[i];
                best= av_find_best_pix_fmt_of_2(best, p, ref->format, has_alpha, NULL);
            }
            av_log(link->src,AV_LOG_DEBUG, "picking %s out of %d ref:%s alpha:%d\n",
                   av_get_pix_fmt_name(best), link->in_formats->nb_formats,
                   av_get_pix_fmt_name(ref->format), has_alpha);
            link->in_formats->formats[0] = best;
        }
    } else if (link->type == AVMEDIA_TYPE_AUDIO) {
        if(ref && ref->type == AVMEDIA_TYPE_AUDIO){
            enum AVSampleFormat best= AV_SAMPLE_FMT_NONE;
            int i;
            for (i=0; i<link->in_formats->nb_formats; i++) {
                enum AVSampleFormat p = link->in_formats->formats[i];
                best = find_best_sample_fmt_of_2(best, p, ref->format);
            }
            av_log(link->src,AV_LOG_DEBUG, "picking %s out of %d ref:%s\n",
                   av_get_sample_fmt_name(best), link->in_formats->nb_formats,
                   av_get_sample_fmt_name(ref->format));
            link->in_formats->formats[0] = best;
        }
    }
    link->in_formats->nb_formats = 1;
    link->format = link->in_formats->formats[0];
    if (link->type == AVMEDIA_TYPE_AUDIO) {
        if (!link->in_samplerates->nb_formats) {
            av_log(link->src, AV_LOG_ERROR, "Cannot select sample rate for"
                   " the link between filters %s and %s.\n", link->src->name,
                   link->dst->name);
            return AVERROR(EINVAL);
        }
        link->in_samplerates->nb_formats = 1;
        link->sample_rate = link->in_samplerates->formats[0];
        if (link->in_channel_layouts->all_layouts) {
            av_log(link->src, AV_LOG_ERROR, "Cannot select channel layout for"
                   " the link between filters %s and %s.\n", link->src->name,
                   link->dst->name);
            if (!link->in_channel_layouts->all_counts)
                av_log(link->src, AV_LOG_ERROR, "Unknown channel layouts not "
                       "supported, try specifying a channel layout using "
                       "'aformat=channel_layouts=something'.\n");
            return AVERROR(EINVAL);
        }
        link->in_channel_layouts->nb_channel_layouts = 1;
        link->channel_layout = link->in_channel_layouts->channel_layouts[0];
        if ((link->channels = FF_LAYOUT2COUNT(link->channel_layout)))
            link->channel_layout = 0;
        else
            link->channels = av_get_channel_layout_nb_channels(link->channel_layout);
    }
    ff_formats_unref(&link->in_formats);
    ff_formats_unref(&link->out_formats);
    ff_formats_unref(&link->in_samplerates);
    ff_formats_unref(&link->out_samplerates);
    ff_channel_layouts_unref(&link->in_channel_layouts);
    ff_channel_layouts_unref(&link->out_channel_layouts);
    return 0;
}
