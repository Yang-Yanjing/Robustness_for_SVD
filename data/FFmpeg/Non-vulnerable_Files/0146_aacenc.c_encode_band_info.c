static void encode_band_info(AACEncContext *s, SingleChannelElement *sce)
{
    int w;
    if (s->coder->set_special_band_scalefactors)
        s->coder->set_special_band_scalefactors(s, sce);
    for (w = 0; w < sce->ics.num_windows; w += sce->ics.group_len[w])
        s->coder->encode_window_bands_info(s, sce, w, sce->ics.group_len[w], s->lambda);
}
