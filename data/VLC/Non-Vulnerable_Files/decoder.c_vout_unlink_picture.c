}
static void vout_unlink_picture( decoder_t *p_dec, picture_t *p_pic )
{
    vout_ReleasePicture( p_dec->p_owner->p_vout, p_pic );
}
