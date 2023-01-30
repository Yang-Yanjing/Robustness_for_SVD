 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    unsigned i_channels;
    filter_sys_t *p_sys;
    i_channels = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    p_sys = p_filter->p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_nb = var_CreateGetInteger( p_filter->p_parent, "norm-buff-size" );
    p_sys->f_max = var_CreateGetFloat( p_filter->p_parent, "norm-max-level" );
    if( p_sys->f_max <= 0 ) p_sys->f_max = 0.01;
    /* We need to store (nb_buffers+1)*nb_channels floats */
    p_sys->p_last = calloc( i_channels * (p_filter->p_sys->i_nb + 2), sizeof(float) );
    if( !p_sys->p_last )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    p_filter->fmt_in.audio.i_format = VLC_CODEC_FL32;
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}
