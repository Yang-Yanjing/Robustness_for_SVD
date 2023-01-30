 *****************************************************************************/
static int Mux( sout_mux_t *p_mux )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    if( p_sys->b_error ) return VLC_EGENERIC;
    if( p_sys->b_write_header )
    {
        int error;
        msg_Dbg( p_mux, "writing header" );
        char *psz_opts = var_GetNonEmptyString( p_mux, "sout-avformat-options" );
        AVDictionary *options = NULL;
        if (psz_opts && *psz_opts)
            options = vlc_av_get_options(psz_opts);
        free(psz_opts);
        error = avformat_write_header( p_sys->oc, options ? &options : NULL);
        AVDictionaryEntry *t = NULL;
        while ((t = av_dict_get(options, "", t, AV_DICT_IGNORE_SUFFIX))) {
            msg_Err( p_mux, "Unknown option \"%s\"", t->key );
        }
        av_dict_free(&options);
        if( error < 0 )
        {
            msg_Err( p_mux, "could not write header: %s",
                     vlc_strerror_c(AVUNERROR(error)) );
            p_sys->b_write_header = false;
            p_sys->b_error = true;
            return VLC_EGENERIC;
        }
        avio_flush( p_sys->oc->pb );
        p_sys->b_write_header = false;
    }
    for( ;; )
    {
        mtime_t i_dts;
        int i_stream = sout_MuxGetStream( p_mux, 1, &i_dts );
        if( i_stream < 0 )
            return VLC_SUCCESS;
        MuxBlock( p_mux, p_mux->pp_inputs[i_stream] );
    }
    return VLC_SUCCESS;
}
