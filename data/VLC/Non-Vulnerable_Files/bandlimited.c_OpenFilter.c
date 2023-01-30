 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    unsigned int i_out_rate  = p_filter->fmt_out.audio.i_rate;
    if ( p_filter->fmt_in.audio.i_rate == p_filter->fmt_out.audio.i_rate
      || p_filter->fmt_in.audio.i_format != p_filter->fmt_out.audio.i_format
      || p_filter->fmt_in.audio.i_physical_channels
              != p_filter->fmt_out.audio.i_physical_channels
      || p_filter->fmt_in.audio.i_original_channels
              != p_filter->fmt_out.audio.i_original_channels
      || p_filter->fmt_in.audio.i_format != VLC_CODEC_FL32 )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the module's structure */
    p_filter->p_sys = p_sys = malloc( sizeof(struct filter_sys_t) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->p_buf = NULL;
    p_sys->i_buf_size = 0;
    p_sys->i_old_wing = 0;
    p_sys->b_first = true;
    p_filter->pf_audio_filter = Resample;
    msg_Dbg( p_this, "%4.4s/%iKHz/%i->%4.4s/%iKHz/%i",
             (char *)&p_filter->fmt_in.i_codec,
             p_filter->fmt_in.audio.i_rate,
             p_filter->fmt_in.audio.i_channels,
             (char *)&p_filter->fmt_out.i_codec,
             p_filter->fmt_out.audio.i_rate,
             p_filter->fmt_out.audio.i_channels);
    p_filter->fmt_out = p_filter->fmt_in;
    p_filter->fmt_out.audio.i_rate = i_out_rate;
    return 0;
}
