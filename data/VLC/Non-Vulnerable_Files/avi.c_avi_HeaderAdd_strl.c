}
static int avi_HeaderAdd_strl( buffer_out_t *p_bo, avi_stream_t *p_stream )
{
    AVI_BOX_ENTER_LIST( "strl" );
    avi_HeaderAdd_strh( p_bo, p_stream );
    avi_HeaderAdd_strf( p_bo, p_stream );
    AVI_BOX_EXIT( 0 );
}
