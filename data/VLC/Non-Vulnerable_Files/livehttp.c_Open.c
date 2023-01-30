 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_access_out_t   *p_access = (sout_access_out_t*)p_this;
    sout_access_out_sys_t *p_sys;
    char *psz_idx;
    config_ChainParse( p_access, SOUT_CFG_PREFIX, ppsz_sout_options, p_access->p_cfg );
    if( !p_access->psz_path )
    {
        msg_Err( p_access, "no file name specified" );
        return VLC_EGENERIC;
    }
    if( unlikely( !( p_sys = calloc ( 1, sizeof( *p_sys ) ) ) ) )
        return VLC_ENOMEM;
    p_sys->i_seglen = var_GetInteger( p_access, SOUT_CFG_PREFIX "seglen" );
    /* Try to get within asked segment length */
    p_sys->i_seglenm = CLOCK_FREQ * p_sys->i_seglen;
    p_sys->block_buffer = NULL;
    p_sys->last_block_buffer = &p_sys->block_buffer;
    p_sys->i_numsegs = var_GetInteger( p_access, SOUT_CFG_PREFIX "numsegs" );
    p_sys->i_initial_segment = var_GetInteger( p_access, SOUT_CFG_PREFIX "initial-segment-number" );
    p_sys->b_splitanywhere = var_GetBool( p_access, SOUT_CFG_PREFIX "splitanywhere" );
    p_sys->b_delsegs = var_GetBool( p_access, SOUT_CFG_PREFIX "delsegs" );
    p_sys->b_ratecontrol = var_GetBool( p_access, SOUT_CFG_PREFIX "ratecontrol") ;
    p_sys->b_caching = var_GetBool( p_access, SOUT_CFG_PREFIX "caching") ;
    p_sys->b_generate_iv = var_GetBool( p_access, SOUT_CFG_PREFIX "generate-iv") ;
    p_sys->b_segment_has_data = false;
    p_sys->segments_t = vlc_array_new();
    p_sys->stuffing_size = 0;
    p_sys->i_opendts = VLC_TS_INVALID;
    p_sys->i_dts_offset  = 0;
    p_sys->psz_indexPath = NULL;
    psz_idx = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "index" );
    if ( psz_idx )
    {
        char *psz_tmp;
        psz_tmp = str_format_time( psz_idx );
        free( psz_idx );
        if ( !psz_tmp )
        {
            free( p_sys );
            return VLC_ENOMEM;
        }
        path_sanitize( psz_tmp );
        p_sys->psz_indexPath = psz_tmp;
        vlc_unlink( p_sys->psz_indexPath );
    }
    p_sys->psz_indexUrl = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "index-url" );
    p_sys->psz_keyfile  = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "key-loadfile" );
    p_sys->key_uri      = var_GetNonEmptyString( p_access, SOUT_CFG_PREFIX "key-uri" );
    p_access->p_sys = p_sys;
    if( p_sys->psz_keyfile && ( LoadCryptFile( p_access ) < 0 ) )
    {
        free( p_sys->psz_indexUrl );
        free( p_sys->psz_indexPath );
        free( p_sys );
        msg_Err( p_access, "Encryption init failed" );
        return VLC_EGENERIC;
    }
    else if( !p_sys->psz_keyfile && ( CryptSetup( p_access, NULL ) < 0 ) )
    {
        free( p_sys->psz_indexUrl );
        free( p_sys->psz_indexPath );
        free( p_sys );
        msg_Err( p_access, "Encryption init failed" );
        return VLC_EGENERIC;
    }
    p_sys->i_handle = -1;
    p_sys->i_segment = p_sys->i_initial_segment > 0 ? p_sys->i_initial_segment -1 : 0;
    p_sys->psz_cursegPath = NULL;
    p_access->pf_write = Write;
    p_access->pf_seek  = Seek;
    p_access->pf_control = Control;
    return VLC_SUCCESS;
}
