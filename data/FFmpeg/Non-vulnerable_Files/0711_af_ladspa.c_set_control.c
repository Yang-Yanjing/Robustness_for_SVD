static int set_control(AVFilterContext *ctx, unsigned long port, LADSPA_Data value)
{
    LADSPAContext *s = ctx->priv;
    const char *label = s->desc->Label;
    LADSPA_PortRangeHint *h = (LADSPA_PortRangeHint *)s->desc->PortRangeHints +
                              s->icmap[port];
    if (port >= s->nb_inputcontrols) {
        av_log(ctx, AV_LOG_ERROR, "Control c%ld is out of range [0 - %lu].\n",
               port, s->nb_inputcontrols);
        return AVERROR(EINVAL);
    }
    if (LADSPA_IS_HINT_BOUNDED_BELOW(h->HintDescriptor) &&
            value < h->LowerBound) {
        av_log(ctx, AV_LOG_ERROR,
                "%s: input control c%ld is below lower boundary of %0.4f.\n",
                label, port, h->LowerBound);
        return AVERROR(EINVAL);
    }
    if (LADSPA_IS_HINT_BOUNDED_ABOVE(h->HintDescriptor) &&
            value > h->UpperBound) {
        av_log(ctx, AV_LOG_ERROR,
                "%s: input control c%ld is above upper boundary of %0.4f.\n",
                label, port, h->UpperBound);
        return AVERROR(EINVAL);
    }
    s->ictlv[port] = value;
    return 0;
}
