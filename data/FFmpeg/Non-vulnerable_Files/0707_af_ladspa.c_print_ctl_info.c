AVFILTER_DEFINE_CLASS(ladspa);
static void print_ctl_info(AVFilterContext *ctx, int level,
                           LADSPAContext *s, int ctl, unsigned long *map,
                           LADSPA_Data *values, int print)
{
    const LADSPA_PortRangeHint *h = s->desc->PortRangeHints + map[ctl];
    av_log(ctx, level, "c%i: %s [", ctl, s->desc->PortNames[map[ctl]]);
    if (LADSPA_IS_HINT_TOGGLED(h->HintDescriptor)) {
        av_log(ctx, level, "toggled (1 or 0)");
        if (LADSPA_IS_HINT_HAS_DEFAULT(h->HintDescriptor))
            av_log(ctx, level, " (default %i)", (int)values[ctl]);
    } else {
        if (LADSPA_IS_HINT_INTEGER(h->HintDescriptor)) {
            av_log(ctx, level, "<int>");
            if (LADSPA_IS_HINT_BOUNDED_BELOW(h->HintDescriptor))
                av_log(ctx, level, ", min: %i", (int)h->LowerBound);
            if (LADSPA_IS_HINT_BOUNDED_ABOVE(h->HintDescriptor))
                av_log(ctx, level, ", max: %i", (int)h->UpperBound);
            if (print)
                av_log(ctx, level, " (value %d)", (int)values[ctl]);
            else if (LADSPA_IS_HINT_HAS_DEFAULT(h->HintDescriptor))
                av_log(ctx, level, " (default %d)", (int)values[ctl]);
        } else {
            av_log(ctx, level, "<float>");
            if (LADSPA_IS_HINT_BOUNDED_BELOW(h->HintDescriptor))
                av_log(ctx, level, ", min: %f", h->LowerBound);
            if (LADSPA_IS_HINT_BOUNDED_ABOVE(h->HintDescriptor))
                av_log(ctx, level, ", max: %f", h->UpperBound);
            if (print)
                av_log(ctx, level, " (value %f)", values[ctl]);
            else if (LADSPA_IS_HINT_HAS_DEFAULT(h->HintDescriptor))
                av_log(ctx, level, " (default %f)", values[ctl]);
        }
        if (LADSPA_IS_HINT_SAMPLE_RATE(h->HintDescriptor))
            av_log(ctx, level, ", multiple of sample rate");
        if (LADSPA_IS_HINT_LOGARITHMIC(h->HintDescriptor))
            av_log(ctx, level, ", logarithmic scale");
    }
    av_log(ctx, level, "]\n");
}
AVFILTER_DEFINE_CLASS(ladspa);
static void print_ctl_info(AVFilterContext *ctx, int level,
                           LADSPAContext *s, int ctl, unsigned long *map,
                           LADSPA_Data *values, int print)
{
    const LADSPA_PortRangeHint *h = s->desc->PortRangeHints + map[ctl];
    av_log(ctx, level, "c%i: %s [", ctl, s->desc->PortNames[map[ctl]]);
    if (LADSPA_IS_HINT_TOGGLED(h->HintDescriptor)) {
        av_log(ctx, level, "toggled (1 or 0)");
        if (LADSPA_IS_HINT_HAS_DEFAULT(h->HintDescriptor))
            av_log(ctx, level, " (default %i)", (int)values[ctl]);
    } else {
        if (LADSPA_IS_HINT_INTEGER(h->HintDescriptor)) {
            av_log(ctx, level, "<int>");
            if (LADSPA_IS_HINT_BOUNDED_BELOW(h->HintDescriptor))
                av_log(ctx, level, ", min: %i", (int)h->LowerBound);
            if (LADSPA_IS_HINT_BOUNDED_ABOVE(h->HintDescriptor))
                av_log(ctx, level, ", max: %i", (int)h->UpperBound);
            if (print)
                av_log(ctx, level, " (value %d)", (int)values[ctl]);
            else if (LADSPA_IS_HINT_HAS_DEFAULT(h->HintDescriptor))
                av_log(ctx, level, " (default %d)", (int)values[ctl]);
        } else {
            av_log(ctx, level, "<float>");
            if (LADSPA_IS_HINT_BOUNDED_BELOW(h->HintDescriptor))
                av_log(ctx, level, ", min: %f", h->LowerBound);
            if (LADSPA_IS_HINT_BOUNDED_ABOVE(h->HintDescriptor))
                av_log(ctx, level, ", max: %f", h->UpperBound);
            if (print)
                av_log(ctx, level, " (value %f)", values[ctl]);
            else if (LADSPA_IS_HINT_HAS_DEFAULT(h->HintDescriptor))
                av_log(ctx, level, " (default %f)", values[ctl]);
        }
        if (LADSPA_IS_HINT_SAMPLE_RATE(h->HintDescriptor))
            av_log(ctx, level, ", multiple of sample rate");
        if (LADSPA_IS_HINT_LOGARITHMIC(h->HintDescriptor))
            av_log(ctx, level, ", logarithmic scale");
    }
