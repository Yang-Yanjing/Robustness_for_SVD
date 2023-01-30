}
static void video_del_buffer( decoder_t *p_dec, picture_t *p_pic )
{
    (void)p_dec;
    picture_Release( p_pic );
}
