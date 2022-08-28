av_cold int ff_hq_init_vlcs(HQContext *c)
{
    int ret = init_vlc(&c->hqa_cbp_vlc, 5, FF_ARRAY_ELEMS(cbp_vlc_lens),
                       cbp_vlc_lens, 1, 1, cbp_vlc_bits, 1, 1, 0);
    if (ret < 0)
        return ret;
    return init_vlc(&c->hq_ac_vlc, 9, NUM_HQ_AC_ENTRIES,
                    ff_hq_ac_bits, 1, 1, ff_hq_ac_codes, 2, 2, 0);
}
