 *****************************************************************************/
static void PutPicture( decoder_t *p_dec, picture_t *p_picture )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* */
    uint8_t *pp_buf[3];
    for( int j = 0; j < 3; j++ )
        pp_buf[j] = p_picture ? p_picture->p[j].p_pixels : NULL;
    mpeg2_set_buf( p_sys->p_mpeg2dec, pp_buf, p_picture );
    /* Completly broken API, why the hell does it suppose
     * the stride of the chroma planes ! */
    if( p_picture )
        mpeg2_stride( p_sys->p_mpeg2dec, p_picture->p[Y_PLANE].i_pitch );
}
