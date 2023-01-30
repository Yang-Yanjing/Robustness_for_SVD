 */
static int DpbDisplayPicture( decoder_t *p_dec, picture_t *p_picture )
{
    picture_dpb_t *p = DpbFindPicture( p_dec, p_picture );
    /* XXX it is needed to workaround libmpeg2 bugs */
    if( !p || p->b_displayed || !p->b_linked )
    {
        msg_Err( p_dec, "DpbDisplayPicture called on an invalid picture" );
        return VLC_EGENERIC;
    }
    assert( p && !p->b_displayed && p->b_linked );
    p->b_displayed = true;
    return VLC_SUCCESS;
}
