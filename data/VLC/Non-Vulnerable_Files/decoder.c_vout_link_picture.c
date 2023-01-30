}
static void vout_link_picture( decoder_t *p_dec, picture_t *p_pic )
{
    vout_HoldPicture( p_dec->p_owner->p_vout, p_pic );
}
