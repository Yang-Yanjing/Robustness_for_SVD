 */
static void DpbUnlinkPicture( decoder_t *p_dec, picture_t *p_picture )
{
    picture_dpb_t *p = DpbFindPicture( p_dec, p_picture );
    /* XXX it is needed to workaround libmpeg2 bugs */
    if( !p || !p->b_linked )
    {
        msg_Err( p_dec, "DpbUnlinkPicture called on an invalid picture" );
        return;
    }
    assert( p && p->b_linked );
    decoder_UnlinkPicture( p_dec, p->p_picture );
    p->b_linked = false;
    if( !p->b_displayed )
        decoder_DeletePicture( p_dec, p->p_picture );
    p->p_picture = NULL;
}
