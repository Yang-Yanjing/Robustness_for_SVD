 *****************************************************************************/
static int OpenFilter( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys;
    int i_ret;
    if( p_filter->fmt_in.i_codec != VLC_CODEC_DTS
     || p_filter->fmt_out.audio.i_format != VLC_CODEC_FL32 )
        return VLC_EGENERIC;
    /* Allocate the memory needed to store the module's structure */
    p_sys = p_filter->p_sys = malloc( sizeof(filter_sys_t) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    i_ret = Open( VLC_OBJECT(p_filter), p_sys,
                  &p_filter->fmt_in.audio, &p_filter->fmt_out.audio );
    p_filter->pf_audio_filter = Convert;
    p_filter->fmt_out.audio.i_rate = p_filter->fmt_in.audio.i_rate;
    return i_ret;
}
