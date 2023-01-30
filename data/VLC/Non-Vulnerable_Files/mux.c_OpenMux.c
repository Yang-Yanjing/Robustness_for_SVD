 *****************************************************************************/
int OpenMux( vlc_object_t *p_this )
{
    AVOutputFormat *file_oformat;
    sout_mux_t *p_mux = (sout_mux_t*)p_this;
    bool dummy = !strcmp( p_mux->p_access->psz_access, "dummy");
    if( dummy && strlen(p_mux->p_access->psz_path)
                              >= sizeof (((AVFormatContext *)NULL)->filename) )
        return VLC_EGENERIC;
    vlc_init_avformat(p_this);
    config_ChainParse( p_mux, "sout-avformat-", ppsz_mux_options, p_mux->p_cfg );
    /* Find the requested muxer */
    char *psz_mux = var_InheritString( p_mux, "sout-avformat-mux" );
    if( psz_mux )
    {
        file_oformat = av_guess_format( psz_mux, NULL, NULL );
        free( psz_mux );
    }
    else
    {
        file_oformat =
            av_guess_format( NULL, p_mux->p_access->psz_path, NULL);
    }
    if (!file_oformat)
    {
      msg_Err( p_mux, "unable for find a suitable output format" );
      return VLC_EGENERIC;
    }
    sout_mux_sys_t *p_sys = malloc( sizeof( sout_mux_sys_t ) );
    if( unlikely(p_sys == NULL) )
        return VLC_ENOMEM;
    p_mux->p_sys = p_sys;
    p_sys->oc = avformat_alloc_context();
    p_sys->oc->oformat = file_oformat;
    /* If we use dummy access, let avformat write output */
    if( dummy )
        strcpy( p_sys->oc->filename, p_mux->p_access->psz_path );
    /* Create I/O wrapper */
    p_sys->io_buffer_size = 10 * 1024 * 1024;  /* FIXME */
    p_sys->io_buffer = malloc( p_sys->io_buffer_size );
    bool b_can_seek;
    if( sout_AccessOutControl( p_mux->p_access, ACCESS_OUT_CAN_SEEK, &b_can_seek ) )
        b_can_seek = false;
    p_sys->io = avio_alloc_context(
        p_sys->io_buffer, p_sys->io_buffer_size,
        1, p_mux, NULL, IOWrite, b_can_seek ? IOSeek : NULL );
    p_sys->oc->pb = p_sys->io;
    p_sys->oc->nb_streams = 0;
    p_sys->b_write_header = true;
    p_sys->b_write_keyframe = false;
    p_sys->b_error = false;
    /* Fill p_mux fields */
    p_mux->pf_control   = Control;
    p_mux->pf_addstream = AddStream;
    p_mux->pf_delstream = DelStream;
    p_mux->pf_mux       = Mux;
    return VLC_SUCCESS;
}
