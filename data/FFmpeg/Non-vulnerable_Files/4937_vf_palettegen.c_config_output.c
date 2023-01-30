static int config_output(AVFilterLink *outlink)
{
    outlink->w = outlink->h = 16;
    outlink->sample_aspect_ratio = av_make_q(1, 1);
    outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    return 0;
}
