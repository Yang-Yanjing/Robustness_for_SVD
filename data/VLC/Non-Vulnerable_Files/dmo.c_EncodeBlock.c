 ****************************************************************************/
static block_t *EncodeBlock( encoder_t *p_enc, void *p_data )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    CMediaBuffer *p_in;
    block_t *p_chain = NULL;
    block_t *p_block_in;
    uint32_t i_status;
    int i_result;
    mtime_t i_pts;
    if( !p_data ) return NULL;
    if( p_enc->fmt_out.i_cat == VIDEO_ES )
    {
        /* Get picture data */
        int i_plane, i_line, i_width, i_src_stride;
        picture_t *p_pic = (picture_t *)p_data;
        uint8_t *p_dst;
        int i_buffer = p_enc->fmt_in.video.i_visible_width *
            p_enc->fmt_in.video.i_visible_height *
            p_enc->fmt_in.video.i_bits_per_pixel / 8;
        p_block_in = block_Alloc( i_buffer );
        /* Copy picture stride by stride */
        p_dst = p_block_in->p_buffer;
        for( i_plane = 0; i_plane < p_pic->i_planes; i_plane++ )
        {
            uint8_t *p_src = p_pic->p[i_plane].p_pixels;
            i_width = p_pic->p[i_plane].i_visible_pitch;
            i_src_stride = p_pic->p[i_plane].i_pitch;
            for( i_line = 0; i_line < p_pic->p[i_plane].i_visible_lines;
                 i_line++ )
            {
                memcpy( p_dst, p_src, i_width );
                p_dst += i_width;
                p_src += i_src_stride;
            }
        }
        i_pts = p_pic->date;
    }
    else
    {
        block_t *p_aout_buffer = (block_t *)p_data;
        p_block_in = block_Alloc( p_aout_buffer->i_buffer );
        memcpy( p_block_in->p_buffer, p_aout_buffer->p_buffer,
                p_block_in->i_buffer );
        i_pts = p_aout_buffer->i_pts;
    }
    /* Feed input to the DMO */
    p_in = CMediaBufferCreate( p_block_in, p_block_in->i_buffer, true );
    i_result = p_sys->p_dmo->vt->ProcessInput( p_sys->p_dmo, 0,
       (IMediaBuffer *)p_in, DMO_INPUT_DATA_BUFFERF_TIME, i_pts * 10, 0 );
    p_in->vt->Release( (IUnknown *)p_in );
    if( i_result == S_FALSE )
    {
        /* No output generated */
#ifdef DMO_DEBUG
        msg_Dbg( p_enc, "ProcessInput(): no output generated %"PRId64, i_pts );
#endif
        return NULL;
    }
    else if( i_result == (int)DMO_E_NOTACCEPTING )
    {
        /* Need to call ProcessOutput */
        msg_Dbg( p_enc, "ProcessInput(): not accepting" );
    }
    else if( i_result != S_OK )
    {
        msg_Dbg( p_enc, "ProcessInput(): failed: %x", i_result );
        return NULL;
    }
#ifdef DMO_DEBUG
    msg_Dbg( p_enc, "ProcessInput(): success" );
#endif
    /* Get output from the DMO */
    while( 1 )
    {
        DMO_OUTPUT_DATA_BUFFER db;
        block_t *p_block_out;
        CMediaBuffer *p_out;
        p_block_out = block_Alloc( p_sys->i_min_output );
        p_block_out->i_buffer = 0;
        p_out = CMediaBufferCreate(p_block_out, p_sys->i_min_output, false);
        memset( &db, 0, sizeof(db) );
        db.pBuffer = (IMediaBuffer *)p_out;
        i_result = p_sys->p_dmo->vt->ProcessOutput( p_sys->p_dmo,
                                                    0, 1, &db, &i_status );
        if( i_result != S_OK )
        {
            if( i_result != S_FALSE )
                msg_Dbg( p_enc, "ProcessOutput(): failed: %x", i_result );
#ifdef DMO_DEBUG
            else
                msg_Dbg( p_enc, "ProcessOutput(): no output" );
#endif
            p_out->vt->Release( (IUnknown *)p_out );
            block_Release( p_block_out );
            return p_chain;
        }
        if( !p_block_out->i_buffer )
        {
#ifdef DMO_DEBUG
            msg_Dbg( p_enc, "ProcessOutput(): no output (i_buffer_out == 0)" );
#endif
            p_out->vt->Release( (IUnknown *)p_out );
            block_Release( p_block_out );
            return p_chain;
        }
        if( db.dwStatus & DMO_OUTPUT_DATA_BUFFERF_TIME )
        {
#ifdef DMO_DEBUG
            msg_Dbg( p_enc, "ProcessOutput(): pts: %"PRId64", %"PRId64,
                     i_pts, db.rtTimestamp / 10 );
#endif
            i_pts = db.rtTimestamp / 10;
        }
        if( db.dwStatus & DMO_OUTPUT_DATA_BUFFERF_TIMELENGTH )
        {
            p_block_out->i_length = db.rtTimelength / 10;
#ifdef DMO_DEBUG
            msg_Dbg( p_enc, "ProcessOutput(): length: %"PRId64,
                     p_block_out->i_length );
#endif
        }
        if( p_enc->fmt_out.i_cat == VIDEO_ES )
        {
            if( db.dwStatus & DMO_OUTPUT_DATA_BUFFERF_SYNCPOINT )
                p_block_out->i_flags |= BLOCK_FLAG_TYPE_I;
            else
                p_block_out->i_flags |= BLOCK_FLAG_TYPE_P;
        }
        p_block_out->i_dts = p_block_out->i_pts = i_pts;
        block_ChainAppend( &p_chain, p_block_out );
    }
}
