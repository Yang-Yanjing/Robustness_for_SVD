 *****************************************************************************/
picture_t *decoder_NewPicture( decoder_t *p_decoder )
{
    picture_t *p_picture = p_decoder->pf_vout_buffer_new( p_decoder );
    if( !p_picture )
        msg_Warn( p_decoder, "can't get output picture" );
    return p_picture;
}
