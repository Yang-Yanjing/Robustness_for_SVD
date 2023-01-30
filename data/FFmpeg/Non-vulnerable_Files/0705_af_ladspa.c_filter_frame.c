static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    LADSPAContext *s = ctx->priv;
    AVFrame *out;
    int i, h;
    if (!s->nb_outputs ||
        (av_frame_is_writable(in) && s->nb_inputs == s->nb_outputs &&
        !(s->desc->Properties & LADSPA_PROPERTY_INPLACE_BROKEN))) {
        out = in;
    } else {
        out = ff_get_audio_buffer(ctx->outputs[0], in->nb_samples);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    for (h = 0; h < s->nb_handles; h++) {
        for (i = 0; i < s->nb_inputs; i++) {
            s->desc->connect_port(s->handles[h], s->ipmap[i],
                                  (LADSPA_Data*)in->extended_data[i]);
        }
        for (i = 0; i < s->nb_outputs; i++) {
            s->desc->connect_port(s->handles[h], s->opmap[i],
                                  (LADSPA_Data*)out->extended_data[i]);
        }
        s->desc->run(s->handles[h], in->nb_samples);
    }
    for (i = 0; i < s->nb_outputcontrols; i++)
        print_ctl_info(ctx, AV_LOG_VERBOSE, s, i, s->ocmap, s->octlv, 1);
    if (out != in)
        av_frame_free(&in);
    return ff_filter_frame(ctx->outputs[0], out);
}
