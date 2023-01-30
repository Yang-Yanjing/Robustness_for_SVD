 *****************************************************************************/
static int OpenDecoderCommon( vlc_object_t *p_this, bool b_force_dump )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    char psz_file[10 + 3 * sizeof (p_dec)];
    snprintf( psz_file, sizeof( psz_file), "stream.%p", p_dec );
    if( !b_force_dump )
        b_force_dump = var_InheritBool( p_dec, "dummy-save-es" );
    if( b_force_dump )
    {
        FILE *stream = vlc_fopen( psz_file, "wb" );
        if( stream == NULL )
        {
            msg_Err( p_dec, "cannot create `%s'", psz_file );
            return VLC_EGENERIC;
        }
        msg_Dbg( p_dec, "dumping stream to file `%s'", psz_file );
        p_dec->p_sys = (void *)stream;
    }
    else
        p_dec->p_sys = NULL;
    /* Set callbacks */
    p_dec->pf_decode_video = (picture_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    p_dec->pf_decode_audio = (block_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    p_dec->pf_decode_sub = (subpicture_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    return VLC_SUCCESS;
}
