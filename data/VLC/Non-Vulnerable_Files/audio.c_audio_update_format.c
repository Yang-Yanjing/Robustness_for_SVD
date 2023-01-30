};
static int audio_update_format( decoder_t *p_dec )
{
    aout_FormatPrepare( &p_dec->fmt_out.audio );
    return 0;
}
