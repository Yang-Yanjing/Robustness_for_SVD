 */
static picture_t *DpbNewPicture( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    picture_dpb_t *p;
    int i;
    for( i = 0; i < DPB_COUNT; i++ )
    {
        p = &p_sys->p_dpb[i];
        if( !p->p_picture )
            break;
    }
    if( i >= DPB_COUNT )
    {
        msg_Err( p_dec, "Leaking picture" );
        return NULL;
    }
    p->p_picture = GetNewPicture( p_dec );
    if( p->p_picture )
    {
        decoder_LinkPicture( p_dec, p->p_picture );
        p->b_linked = true;
        p->b_displayed = false;
        p->p_picture->date = 0;
    }
    return p->p_picture;
}
