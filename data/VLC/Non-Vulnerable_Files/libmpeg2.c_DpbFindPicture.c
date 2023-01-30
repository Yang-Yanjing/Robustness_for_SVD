}
static picture_dpb_t *DpbFindPicture( decoder_t *p_dec, picture_t *p_picture )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    for( int i = 0; i < DPB_COUNT; i++ )
    {
        picture_dpb_t *p = &p_sys->p_dpb[i];
        if( p->p_picture == p_picture )
            return p;
    }
    return NULL;
}
