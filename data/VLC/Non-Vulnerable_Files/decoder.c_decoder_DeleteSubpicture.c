}
void decoder_DeleteSubpicture( decoder_t *p_decoder, subpicture_t *p_subpicture )
{
    p_decoder->pf_spu_buffer_del( p_decoder, p_subpicture );
}
