}
static int avi_HeaderAdd_strh( buffer_out_t *p_bo, avi_stream_t *p_stream )
{
    AVI_BOX_ENTER( "strh" );
    switch( p_stream->i_cat )
    {
        case VIDEO_ES:
            {
                bo_AddFCC( p_bo, "vids" );
                bo_AddDWordBE( p_bo, p_stream->p_bih->biCompression );
                bo_AddDWordLE( p_bo, 0 );   /* flags */
                bo_AddWordLE(  p_bo, 0 );   /* priority */
                bo_AddWordLE(  p_bo, 0 );   /* langage */
                bo_AddDWordLE( p_bo, 0 );   /* initial frame */
                bo_AddDWordLE( p_bo, 1000 );/* scale */
                bo_AddDWordLE( p_bo, (uint32_t)( 1000 * p_stream->f_fps ));
                bo_AddDWordLE( p_bo, 0 );   /* start */
                bo_AddDWordLE( p_bo, p_stream->i_frames );
                bo_AddDWordLE( p_bo, 1024 * 1024 );
                bo_AddDWordLE( p_bo, -1 );  /* quality */
                bo_AddDWordLE( p_bo, 0 );   /* samplesize */
                bo_AddWordLE(  p_bo, 0 );   /* ??? */
                bo_AddWordLE(  p_bo, 0 );   /* ??? */
                bo_AddWordLE(  p_bo, p_stream->p_bih->biWidth );
                bo_AddWordLE(  p_bo, p_stream->p_bih->biHeight );
            }
            break;
        case AUDIO_ES:
            {
                int i_rate, i_scale, i_samplesize;
                i_samplesize = p_stream->p_wf->nBlockAlign;
                if( i_samplesize > 1 )
                {
                    i_scale = i_samplesize;
                    i_rate = /*i_scale **/ p_stream->i_bitrate / 8;
                }
                else
                {
                    i_samplesize = 1;
                    i_scale = 1000;
                    i_rate = 1000 * p_stream->i_bitrate / 8;
                }
                bo_AddFCC( p_bo, "auds" );
                bo_AddDWordLE( p_bo, 0 );   /* tag */
                bo_AddDWordLE( p_bo, 0 );   /* flags */
                bo_AddWordLE(  p_bo, 0 );   /* priority */
                bo_AddWordLE(  p_bo, 0 );   /* langage */
                bo_AddDWordLE( p_bo, 0 );   /* initial frame */
                bo_AddDWordLE( p_bo, i_scale );/* scale */
                bo_AddDWordLE( p_bo, i_rate );
                bo_AddDWordLE( p_bo, 0 );   /* start */
                bo_AddDWordLE( p_bo, p_stream->i_frames );
                bo_AddDWordLE( p_bo, 10 * 1024 );
                bo_AddDWordLE( p_bo, -1 );  /* quality */
                bo_AddDWordLE( p_bo, i_samplesize );
                bo_AddWordLE(  p_bo, 0 );   /* ??? */
                bo_AddWordLE(  p_bo, 0 );   /* ??? */
                bo_AddWordLE(  p_bo, 0 );
                bo_AddWordLE(  p_bo, 0 );
            }
            break;
    }
    AVI_BOX_EXIT( 0 );
}
