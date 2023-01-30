 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    audio_format_t fmt_in  = p_filter->fmt_in.audio;
    audio_format_t fmt_out = p_filter->fmt_out.audio;
    fmt_in.i_format = p_filter->fmt_in.i_codec;
    fmt_out.i_format = p_filter->fmt_out.i_codec;
    if( !IsSupported( &fmt_in, &fmt_out ) )
        return VLC_EGENERIC;
    p_filter->p_sys = malloc( sizeof(*p_sys) );
    if( unlikely(!p_filter->p_sys) )
        return VLC_ENOMEM;
    p_filter->pf_audio_filter = Filter;
    const unsigned i_input_physical = p_filter->fmt_in.audio.i_physical_channels;
    const bool b_input_7_0 = (i_input_physical & ~AOUT_CHAN_LFE) == AOUT_CHANS_7_0;
    const bool b_input_6_1 = !b_input_7_0 &&
                             i_input_physical == AOUT_CHANS_6_1_MIDDLE;
    const bool b_input_5_0 = !b_input_7_0 && !b_input_6_1 &&
                             ( (i_input_physical & AOUT_CHANS_5_0) == AOUT_CHANS_5_0 ||
                               (i_input_physical & AOUT_CHANS_5_0_MIDDLE) == AOUT_CHANS_5_0_MIDDLE );
    const bool b_input_4_center_rear =  !b_input_7_0 && !b_input_5_0 &&
                             (i_input_physical & ~AOUT_CHAN_LFE) == AOUT_CHANS_4_CENTER_REAR;
    const bool b_input_3_0 = !b_input_7_0 && !b_input_5_0 && !b_input_4_center_rear &&
                             (i_input_physical & ~AOUT_CHAN_LFE) == AOUT_CHANS_3_0;
    if( p_filter->fmt_out.audio.i_physical_channels == AOUT_CHANS_2_0 )
    {
        if( b_input_7_0 )
            p_filter->p_sys->pf_dowork = DoWork_7_x_to_2_0;
        else if( b_input_6_1 )
            p_filter->p_sys->pf_dowork = DoWork_6_1_to_2_0;
        else if( b_input_5_0 )
            p_filter->p_sys->pf_dowork = DoWork_5_x_to_2_0;
        else if( b_input_4_center_rear )
            p_filter->p_sys->pf_dowork = DoWork_4_0_to_2_0;
        else if( b_input_3_0 )
            p_filter->p_sys->pf_dowork = DoWork_3_x_to_2_0;
    }
    else if( p_filter->fmt_out.audio.i_physical_channels == AOUT_CHAN_CENTER )
    {
        if( b_input_7_0 )
            p_filter->p_sys->pf_dowork = DoWork_7_x_to_1_0;
        else if( b_input_5_0 )
            p_filter->p_sys->pf_dowork = DoWork_5_x_to_1_0;
        else if( b_input_4_center_rear )
            p_filter->p_sys->pf_dowork = DoWork_4_0_to_1_0;
        else if( b_input_3_0 )
            p_filter->p_sys->pf_dowork = DoWork_3_x_to_1_0;
        else
            p_filter->p_sys->pf_dowork = DoWork_2_x_to_1_0;
    }
    else if(p_filter->fmt_out.audio.i_physical_channels == AOUT_CHANS_4_0)
    {
        if( b_input_7_0 )
            p_filter->p_sys->pf_dowork = DoWork_7_x_to_4_0;
        else
            p_filter->p_sys->pf_dowork = DoWork_5_x_to_4_0;
    }
    else
    {
        assert( b_input_7_0 || b_input_6_1 );
        if( b_input_7_0 )
            p_filter->p_sys->pf_dowork = DoWork_7_x_to_5_x;
        else
            p_filter->p_sys->pf_dowork = DoWork_6_1_to_5_x;
    }
    return VLC_SUCCESS;
}
