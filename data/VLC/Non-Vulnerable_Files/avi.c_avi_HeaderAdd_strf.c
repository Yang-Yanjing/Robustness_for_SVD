}
static int avi_HeaderAdd_strf( buffer_out_t *p_bo, avi_stream_t *p_stream )
{
    AVI_BOX_ENTER( "strf" );
    switch( p_stream->i_cat )
    {
        case AUDIO_ES:
            bo_AddWordLE( p_bo, p_stream->p_wf->wFormatTag );
            bo_AddWordLE( p_bo, p_stream->p_wf->nChannels );
            bo_AddDWordLE( p_bo, p_stream->p_wf->nSamplesPerSec );
            bo_AddDWordLE( p_bo, p_stream->p_wf->nAvgBytesPerSec );
            bo_AddWordLE( p_bo, p_stream->p_wf->nBlockAlign );
            bo_AddWordLE( p_bo, p_stream->p_wf->wBitsPerSample );
            bo_AddWordLE( p_bo, p_stream->p_wf->cbSize );
            bo_AddMem( p_bo, p_stream->p_wf->cbSize, (uint8_t*)&p_stream->p_wf[1] );
            break;
        case VIDEO_ES:
            bo_AddDWordLE( p_bo, p_stream->p_bih->biSize );
            bo_AddDWordLE( p_bo, p_stream->p_bih->biWidth );
            bo_AddDWordLE( p_bo, p_stream->p_bih->biHeight );
            bo_AddWordLE( p_bo, p_stream->p_bih->biPlanes );
            bo_AddWordLE( p_bo, p_stream->p_bih->biBitCount );
            if( VLC_FOURCC( 0, 0, 0, 1 ) == 0x00000001 )
            {
                bo_AddDWordBE( p_bo, p_stream->p_bih->biCompression );
            }
            else
            {
                bo_AddDWordLE( p_bo, p_stream->p_bih->biCompression );
            }
            bo_AddDWordLE( p_bo, p_stream->p_bih->biSizeImage );
            bo_AddDWordLE( p_bo, p_stream->p_bih->biXPelsPerMeter );
            bo_AddDWordLE( p_bo, p_stream->p_bih->biYPelsPerMeter );
            bo_AddDWordLE( p_bo, p_stream->p_bih->biClrUsed );
            bo_AddDWordLE( p_bo, p_stream->p_bih->biClrImportant );
            bo_AddMem( p_bo,
                       p_stream->p_bih->biSize - sizeof( VLC_BITMAPINFOHEADER ),
                       (uint8_t*)&p_stream->p_bih[1] );
            break;
    }
    AVI_BOX_EXIT( 0 );
}
