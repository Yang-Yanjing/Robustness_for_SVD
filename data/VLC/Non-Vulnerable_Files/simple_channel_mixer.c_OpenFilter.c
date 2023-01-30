 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    if (!vlc_CPU_ARM_NEON())
        return VLC_EGENERIC;
    audio_format_t fmt_in  = p_filter->fmt_in.audio;
    audio_format_t fmt_out = p_filter->fmt_out.audio;
    fmt_in.i_format = p_filter->fmt_in.i_codec;
    fmt_out.i_format = p_filter->fmt_out.i_codec;
    if( fmt_in.i_format != VLC_CODEC_FL32 ||
        fmt_in.i_format != fmt_out.i_format ||
        fmt_in.i_rate != fmt_out.i_rate )
    {
        return VLC_EGENERIC;
    }
    if( fmt_in.i_physical_channels == fmt_out.i_physical_channels &&
        fmt_in.i_original_channels == fmt_out.i_original_channels )
    {
        return VLC_EGENERIC;
    }
    const bool b_input_7 = (fmt_in.i_physical_channels & ~AOUT_CHAN_LFE) == AOUT_CHANS_7_0;
    const bool b_input_5 = ( (fmt_in.i_physical_channels & AOUT_CHANS_5_0) == AOUT_CHANS_5_0 ||
                             (fmt_in.i_physical_channels & AOUT_CHANS_5_0_MIDDLE) == AOUT_CHANS_5_0_MIDDLE );
    const bool b_input_4 =  (fmt_in.i_physical_channels & ~AOUT_CHAN_LFE) == AOUT_CHANS_4_CENTER_REAR;
    const bool b_input_3 = (fmt_in.i_physical_channels & ~AOUT_CHAN_LFE) == AOUT_CHANS_3_0;
    const bool b_output_1 = fmt_out.i_physical_channels == AOUT_CHAN_CENTER;
    const bool b_output_2 = fmt_out.i_physical_channels == AOUT_CHANS_2_0;
    const bool b_output_4 = fmt_out.i_physical_channels == AOUT_CHANS_4_0;
    /* Only conversion to Mono, Stereo and 4.0 right now */
    /* Only from 7/7.1/5/5.1/3/3.1/2.0
     * XXX 5.X rear and middle are handled the same way */
    TRY_FILTER(7,2)
    TRY_FILTER(5,2)
    TRY_FILTER(4,2)
    TRY_FILTER(3,2)
    TRY_FILTER(7,1)
    TRY_FILTER(5,1)
    TRY_FILTER(7,4)
    TRY_FILTER(5,4)
    return VLC_EGENERIC;
}
