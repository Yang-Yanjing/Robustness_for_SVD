}
static void DeleteEncoder( encoder_t * p_enc )
{
    if( p_enc->p_module ) module_unneed( p_enc, p_enc->p_module );
    es_format_Clean( &p_enc->fmt_in );
    es_format_Clean( &p_enc->fmt_out );
    vlc_object_release( p_enc );
    p_enc = NULL;
}
