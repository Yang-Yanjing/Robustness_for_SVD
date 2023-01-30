 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = vlc_object_create( p_this, sizeof( *p_sys ) );
    if( unlikely( p_sys == NULL ) )
        return VLC_ENOMEM;
    p_filter->p_sys = p_sys;
    p_sys->volume.format = p_filter->fmt_in.audio.i_format;
    p_sys->module = module_need( &p_sys->volume, "audio volume", NULL, false );
    if( p_sys->module == NULL )
    {
        msg_Warn( p_filter, "unsupported format" );
        vlc_object_release( &p_sys->volume );
        return VLC_EGENERIC;
    }
    p_sys->f_gain = var_InheritFloat( p_filter->p_parent, "gain-value" );
    msg_Dbg( p_filter, "gain multiplier sets to %.2fx", p_sys->f_gain );
    p_filter->fmt_out.audio = p_filter->fmt_in.audio;
    p_filter->pf_audio_filter = Process;
    return VLC_SUCCESS;
}
