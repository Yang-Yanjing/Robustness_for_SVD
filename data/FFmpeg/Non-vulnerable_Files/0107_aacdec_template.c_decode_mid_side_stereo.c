static void decode_mid_side_stereo(ChannelElement *cpe, GetBitContext *gb,
                                   int ms_present)
{
    int idx;
    int max_idx = cpe->ch[0].ics.num_window_groups * cpe->ch[0].ics.max_sfb;
    if (ms_present == 1) {
        for (idx = 0; idx < max_idx; idx++)
            cpe->ms_mask[idx] = get_bits1(gb);
    } else if (ms_present == 2) {
        memset(cpe->ms_mask, 1, max_idx * sizeof(cpe->ms_mask[0]));
    }
}
