    return( i_err );
static int avi_HeaderAdd_avih( sout_mux_t *p_mux,
                               buffer_out_t *p_bo )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    avi_stream_t    *p_video = NULL;
    int         i_stream;
    uint32_t    i_microsecperframe;
    int         i_maxbytespersec;
    int         i_totalframes;
    AVI_BOX_ENTER( "avih" );
    if( p_sys->i_stream_video >= 0 )
    {
        p_video = &p_sys->stream[p_sys->i_stream_video];
        if( p_video->i_frames <= 0 )
        {
        //    p_video = NULL;
        }
    }
    if( p_video )
    {
        i_microsecperframe =
            (uint32_t)( (float)1000000 /
                        (float)p_sys->stream[p_sys->i_stream_video].f_fps );
        i_totalframes = p_sys->stream[p_sys->i_stream_video].i_frames;
    }
    else
    {
        msg_Warn( p_mux, "avi file without video track isn't a good idea..." );
        i_microsecperframe = 0;
        i_totalframes = 0;
    }
    for( i_stream = 0,i_maxbytespersec = 0; i_stream < p_sys->i_streams; i_stream++ )
    {
        if( p_sys->stream[i_stream].i_duration > 0 )
        {
            i_maxbytespersec +=
                p_sys->stream[i_stream].i_totalsize /
                p_sys->stream[i_stream].i_duration;
        }
    }
    bo_AddDWordLE( p_bo, i_microsecperframe );
    bo_AddDWordLE( p_bo, i_maxbytespersec );
    bo_AddDWordLE( p_bo, 0 );                   /* padding */
    bo_AddDWordLE( p_bo, AVIF_TRUSTCKTYPE |
                         AVIF_HASINDEX |
                         AVIF_ISINTERLEAVED );  /* flags */
    bo_AddDWordLE( p_bo, i_totalframes );
    bo_AddDWordLE( p_bo, 0 );                   /* initial frame */
    bo_AddDWordLE( p_bo, p_sys->i_streams );    /* streams count */
    bo_AddDWordLE( p_bo, 1024 * 1024 );         /* suggested buffer size */
    if( p_video )
    {
        bo_AddDWordLE( p_bo, p_video->p_bih->biWidth );
        bo_AddDWordLE( p_bo, p_video->p_bih->biHeight );
    }
    else
    {
        bo_AddDWordLE( p_bo, 0 );
        bo_AddDWordLE( p_bo, 0 );
    }
    bo_AddDWordLE( p_bo, 0 );                   /* ???? */
    bo_AddDWordLE( p_bo, 0 );                   /* ???? */
    bo_AddDWordLE( p_bo, 0 );                   /* ???? */
    bo_AddDWordLE( p_bo, 0 );                   /* ???? */
    AVI_BOX_EXIT( 0 );
}
