}
static int avi_HeaderAdd_meta( buffer_out_t *p_bo, const char psz_meta[4],
                               const char *psz_data )
{
    if ( psz_data == NULL ) return 1;
    const char *psz = psz_data;
    AVI_BOX_ENTER( psz_meta );
    while (*psz) bo_AddByte( p_bo, *psz++ );
    bo_AddByte( p_bo, 0 );
    AVI_BOX_EXIT( 0 );
}
