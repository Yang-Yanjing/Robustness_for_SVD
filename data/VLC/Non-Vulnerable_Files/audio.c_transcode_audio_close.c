}
void transcode_audio_close( sout_stream_id_sys_t *id )
{
    /* Close decoder */
    if( id->p_decoder->p_module )
        module_unneed( id->p_decoder, id->p_decoder->p_module );
    id->p_decoder->p_module = NULL;
    if( id->p_decoder->p_description )
        vlc_meta_Delete( id->p_decoder->p_description );
    id->p_decoder->p_description = NULL;
    /* Close encoder */
    if( id->p_encoder->p_module )
        module_unneed( id->p_encoder, id->p_encoder->p_module );
    id->p_encoder->p_module = NULL;
    /* Close filters */
    if( id->p_af_chain != NULL )
        aout_FiltersDelete( (vlc_object_t *)NULL, id->p_af_chain );
}
