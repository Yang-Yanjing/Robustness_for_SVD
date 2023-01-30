static void psy_3gpp_analyze(FFPsyContext *ctx, int channel,
                                   const float **coeffs, const FFPsyWindowInfo *wi)
{
    int ch;
    FFPsyChannelGroup *group = ff_psy_find_group(ctx, channel);
    for (ch = 0; ch < group->num_ch; ch++)
        psy_3gpp_analyze_channel(ctx, channel + ch, coeffs[ch], &wi[ch]);
}
