}
subpicture_t *decoder_NewSubpicture( decoder_t *p_decoder,
                                     const subpicture_updater_t *p_dyn )
{
    subpicture_t *p_subpicture = p_decoder->pf_spu_buffer_new( p_decoder, p_dyn );
    if( !p_subpicture )
        msg_Warn( p_decoder, "can't get output subpicture" );
    return p_subpicture;
}
