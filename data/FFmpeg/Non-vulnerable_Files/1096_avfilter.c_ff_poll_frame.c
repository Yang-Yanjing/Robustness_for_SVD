int ff_poll_frame(AVFilterLink *link)
{
    int i, min = INT_MAX;
    if (link->srcpad->poll_frame)
        return link->srcpad->poll_frame(link);
    for (i = 0; i < link->src->nb_inputs; i++) {
        int val;
        if (!link->src->inputs[i])
            return AVERROR(EINVAL);
        val = ff_poll_frame(link->src->inputs[i]);
        min = FFMIN(min, val);
    }
    return min;
}
