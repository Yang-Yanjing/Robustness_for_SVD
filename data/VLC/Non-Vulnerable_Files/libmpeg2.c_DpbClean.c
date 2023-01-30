 */
static void DpbClean( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    for( int i = 0; i < DPB_COUNT; i++ )
    {
        picture_dpb_t *p = &p_sys->p_dpb[i];
        if( !p->p_picture )
            continue;
        if( p->b_linked )
            decoder_UnlinkPicture( p_dec, p->p_picture );
        if( !p->b_displayed )
            decoder_DeletePicture( p_dec, p->p_picture );
        p->p_picture = NULL;
    }
}
