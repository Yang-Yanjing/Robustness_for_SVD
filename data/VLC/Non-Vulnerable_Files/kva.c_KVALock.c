}
static int KVALock( picture_t *picture )
{
    picture_sys_t *picsys = picture->p_sys;
    PVOID kva_buffer;
    ULONG kva_bpl;
    if( kvaLockBuffer( &kva_buffer, &kva_bpl ))
        return VLC_EGENERIC;
    /* Packed or Y plane */
    picture->p->p_pixels = ( uint8_t * )kva_buffer;
    picture->p->i_pitch  = kva_bpl;
    picture->p->i_lines  = picture->format.i_height;
    /* Other planes */
    for( int n = 1; n < picture->i_planes; n++ )
    {
        const plane_t *o = &picture->p[n-1];
        plane_t *p = &picture->p[n];
        p->p_pixels = o->p_pixels + o->i_lines * o->i_pitch;
        p->i_pitch  = kva_bpl >> picsys->i_chroma_shift;
        p->i_lines  = picture->format.i_height >> picsys->i_chroma_shift;
    }
    return VLC_SUCCESS;
}
