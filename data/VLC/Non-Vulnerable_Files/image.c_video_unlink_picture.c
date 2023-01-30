}
static void video_unlink_picture( decoder_t *p_dec, picture_t *p_pic )
{
    (void)p_dec;
    picture_Release( p_pic );
}
