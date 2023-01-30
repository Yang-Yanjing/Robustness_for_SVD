                            (buf[base]&0xff))
static void ParseSpeexComments( decoder_t *p_dec, ogg_packet *p_oggpacket )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    const SpeexMode *p_mode;
    assert( p_sys->p_header->mode < SPEEX_NB_MODES );
    p_mode = speex_mode_list[p_sys->p_header->mode];
    assert( p_mode != NULL );
    if( !p_dec->p_description )
    {
        p_dec->p_description = vlc_meta_New();
        if( !p_dec->p_description )
            return;
    }
    /* */
    char *psz_mode;
    if( asprintf( &psz_mode, "%s%s", p_mode->modeName, p_sys->p_header->vbr ? " VBR" : "" ) >= 0 )
    {
        vlc_meta_AddExtra( p_dec->p_description, _("Mode"), psz_mode );
        free( psz_mode );
    }
    /* TODO: finish comments parsing */
    VLC_UNUSED( p_oggpacket );
}
