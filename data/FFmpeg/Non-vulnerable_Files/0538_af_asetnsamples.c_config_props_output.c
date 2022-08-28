static int config_props_output(AVFilterLink *outlink)
{
    ASNSContext *asns = outlink->src->priv;
    asns->fifo = av_audio_fifo_alloc(outlink->format, outlink->channels, asns->nb_out_samples);
    if (!asns->fifo)
        return AVERROR(ENOMEM);
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return 0;
}
