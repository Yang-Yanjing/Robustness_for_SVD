static int request_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    MixContext      *s = ctx->priv;
    int ret;
    int wanted_samples, available_samples;
    ret = calc_active_inputs(s);
    if (ret < 0)
        return ret;
    if (s->input_state[0] == INPUT_OFF) {
        ret = request_samples(ctx, 1);
        if (ret < 0)
            return ret;
        ret = calc_active_inputs(s);
        if (ret < 0)
            return ret;
        available_samples = get_available_samples(s);
        if (!available_samples)
            return AVERROR(EAGAIN);
        return output_frame(outlink, available_samples);
    }
    if (s->frame_list->nb_frames == 0) {
        ret = ff_request_frame(ctx->inputs[0]);
        if (ret == AVERROR_EOF) {
            s->input_state[0] = INPUT_OFF;
            if (s->nb_inputs == 1)
                return AVERROR_EOF;
            else
                return AVERROR(EAGAIN);
        } else if (ret < 0)
            return ret;
    }
    av_assert0(s->frame_list->nb_frames > 0);
    wanted_samples = frame_list_next_frame_size(s->frame_list);
    if (s->active_inputs > 1) {
        ret = request_samples(ctx, wanted_samples);
        if (ret < 0)
            return ret;
        ret = calc_active_inputs(s);
        if (ret < 0)
            return ret;
    }
    if (s->active_inputs > 1) {
        available_samples = get_available_samples(s);
        if (!available_samples)
            return AVERROR(EAGAIN);
        available_samples = FFMIN(available_samples, wanted_samples);
    } else {
        available_samples = wanted_samples;
    }
    s->next_pts = frame_list_next_pts(s->frame_list);
    frame_list_remove_samples(s->frame_list, available_samples);
    return output_frame(outlink, available_samples);
}
