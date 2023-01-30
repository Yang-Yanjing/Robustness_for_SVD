static int connect_ports(AVFilterContext *ctx, AVFilterLink *link)
{
    LADSPAContext *s = ctx->priv;
    int i, j;
    s->nb_handles = s->nb_inputs == 1 && s->nb_outputs == 1 ? link->channels : 1;
    s->handles    = av_calloc(s->nb_handles, sizeof(*s->handles));
    if (!s->handles)
        return AVERROR(ENOMEM);
    for (i = 0; i < s->nb_handles; i++) {
        s->handles[i] = s->desc->instantiate(s->desc, link->sample_rate);
        if (!s->handles[i]) {
            av_log(ctx, AV_LOG_ERROR, "Could not instantiate plugin.\n");
            return AVERROR_EXTERNAL;
        }
        
        for (j = 0; j < s->nb_inputcontrols; j++)
            s->desc->connect_port(s->handles[i], s->icmap[j], s->ictlv + j);
        
        for (j = 0; j < s->nb_outputcontrols; j++)
            s->desc->connect_port(s->handles[i], s->ocmap[j], &s->octlv[j]);
        if (s->desc->activate)
            s->desc->activate(s->handles[i]);
    }
    av_log(ctx, AV_LOG_DEBUG, "handles: %d\n", s->nb_handles);
    return 0;
}
