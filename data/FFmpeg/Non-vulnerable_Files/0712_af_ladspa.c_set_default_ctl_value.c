static void set_default_ctl_value(LADSPAContext *s, int ctl,
                                  unsigned long *map, LADSPA_Data *values)
{
    const LADSPA_PortRangeHint *h = s->desc->PortRangeHints + map[ctl];
    const LADSPA_Data lower = h->LowerBound;
    const LADSPA_Data upper = h->UpperBound;
    if (LADSPA_IS_HINT_DEFAULT_MINIMUM(h->HintDescriptor)) {
        values[ctl] = lower;
    } else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(h->HintDescriptor)) {
        values[ctl] = upper;
    } else if (LADSPA_IS_HINT_DEFAULT_0(h->HintDescriptor)) {
        values[ctl] = 0.0;
    } else if (LADSPA_IS_HINT_DEFAULT_1(h->HintDescriptor)) {
        values[ctl] = 1.0;
    } else if (LADSPA_IS_HINT_DEFAULT_100(h->HintDescriptor)) {
        values[ctl] = 100.0;
    } else if (LADSPA_IS_HINT_DEFAULT_440(h->HintDescriptor)) {
        values[ctl] = 440.0;
    } else if (LADSPA_IS_HINT_DEFAULT_LOW(h->HintDescriptor)) {
        if (LADSPA_IS_HINT_LOGARITHMIC(h->HintDescriptor))
            values[ctl] = exp(log(lower) * 0.75 + log(upper) * 0.25);
        else
            values[ctl] = lower * 0.75 + upper * 0.25;
    } else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(h->HintDescriptor)) {
        if (LADSPA_IS_HINT_LOGARITHMIC(h->HintDescriptor))
            values[ctl] = exp(log(lower) * 0.5 + log(upper) * 0.5);
        else
            values[ctl] = lower * 0.5 + upper * 0.5;
    } else if (LADSPA_IS_HINT_DEFAULT_HIGH(h->HintDescriptor)) {
        if (LADSPA_IS_HINT_LOGARITHMIC(h->HintDescriptor))
            values[ctl] = exp(log(lower) * 0.25 + log(upper) * 0.75);
        else
            values[ctl] = lower * 0.25 + upper * 0.75;
    }
}
