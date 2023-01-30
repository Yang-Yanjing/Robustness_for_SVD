}
void decoder_DeletePicture( decoder_t *p_decoder, picture_t *p_picture )
{
    p_decoder->pf_vout_buffer_del( p_decoder, p_picture );
}
