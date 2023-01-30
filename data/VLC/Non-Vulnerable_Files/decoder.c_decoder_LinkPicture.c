}
void decoder_LinkPicture( decoder_t *p_decoder, picture_t *p_picture )
{
    p_decoder->pf_picture_link( p_decoder, p_picture );
}
