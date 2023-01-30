}
static void DeleteDecoder( decoder_t * p_dec )
{
    if( p_dec->p_module ) module_unneed( p_dec, p_dec->p_module );
    es_format_Clean( &p_dec->fmt_in );
    es_format_Clean( &p_dec->fmt_out );
    if( p_dec->p_description )
        vlc_meta_Delete( p_dec->p_description );
    vlc_object_release( p_dec );
    p_dec = NULL;
}
