}
void decoder_UnlinkPicture( decoder_t *p_decoder, picture_t *p_picture )
{
    p_decoder->pf_picture_unlink( p_decoder, p_picture );
}
