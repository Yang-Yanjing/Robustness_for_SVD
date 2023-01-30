 ****************************************************************************/
static void *DecBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    int i_result;
    DMO_OUTPUT_DATA_BUFFER db;
    CMediaBuffer *p_out;
    block_t block_out;
    uint32_t i_status;
    if( !pp_block ) return NULL;
    p_block = *pp_block;
    /* Won't work with streams with B-frames, but do we have any ? */
    if( p_block && p_block->i_pts <= VLC_TS_INVALID )
        p_block->i_pts = p_block->i_dts;
    /* Date management */
    if( p_block && p_block->i_pts > VLC_TS_INVALID &&
        p_block->i_pts != date_Get( &p_sys->end_date ) )
    {
        date_Set( &p_sys->end_date, p_block->i_pts );
    }
#if 0 /* Breaks the video decoding */
    if( !date_Get( &p_sys->end_date ) )
    {
        /* We've just started the stream, wait for the first PTS. */
        if( p_block ) block_Release( p_block );
        return NULL;
    }
#endif
    /* Feed input to the DMO */
    if( p_block && p_block->i_buffer )
    {
        CMediaBuffer *p_in;
        p_in = CMediaBufferCreate( p_block, p_block->i_buffer, true );
        i_result = p_sys->p_dmo->vt->ProcessInput( p_sys->p_dmo, 0,
                       (IMediaBuffer *)p_in, DMO_INPUT_DATA_BUFFERF_SYNCPOINT,
                       0, 0 );
        p_in->vt->Release( (IUnknown *)p_in );
        if( i_result == S_FALSE )
        {
            /* No output generated */
#ifdef DMO_DEBUG
            msg_Dbg( p_dec, "ProcessInput(): no output generated" );
#endif
            return NULL;
        }
        else if( i_result == (int)DMO_E_NOTACCEPTING )
        {
            /* Need to call ProcessOutput */
            msg_Dbg( p_dec, "ProcessInput(): not accepting" );
        }
        else if( i_result != S_OK )
        {
            msg_Dbg( p_dec, "ProcessInput(): failed" );
            return NULL;
        }
        else
        {
#ifdef DMO_DEBUG
            msg_Dbg( p_dec, "ProcessInput(): successful" );
#endif
            *pp_block = NULL;
        }
    }
    else if( p_block && !p_block->i_buffer )
    {
        block_Release( p_block );
        *pp_block = NULL;
    }
    /* Get output from the DMO */
    block_out.p_buffer = p_sys->p_buffer;
    block_out.i_buffer = 0;
    p_out = CMediaBufferCreate( &block_out, p_sys->i_min_output, false );
    memset( &db, 0, sizeof(db) );
    db.pBuffer = (IMediaBuffer *)p_out;
    i_result = p_sys->p_dmo->vt->ProcessOutput( p_sys->p_dmo,
                   DMO_PROCESS_OUTPUT_DISCARD_WHEN_NO_BUFFER,
                   1, &db, &i_status );
    if( i_result != S_OK )
    {
        if( i_result != S_FALSE )
            msg_Dbg( p_dec, "ProcessOutput(): failed" );
#ifdef DMO_DEBUG
        else
            msg_Dbg( p_dec, "ProcessOutput(): no output" );
#endif
        p_out->vt->Release( (IUnknown *)p_out );
        return NULL;
    }
#ifdef DMO_DEBUG
    msg_Dbg( p_dec, "ProcessOutput(): success" );
#endif
    if( !block_out.i_buffer )
    {
#ifdef DMO_DEBUG
        msg_Dbg( p_dec, "ProcessOutput(): no output (i_buffer_out == 0)" );
#endif
        p_out->vt->Release( (IUnknown *)p_out );
        return NULL;
    }
    if( p_dec->fmt_out.i_cat == VIDEO_ES )
    {
        /* Get a new picture */
        picture_t *p_pic = decoder_NewPicture( p_dec );
        if( !p_pic ) return NULL;
        CopyPicture( p_pic, block_out.p_buffer );
        /* Date management */
        p_pic->date = date_Get( &p_sys->end_date );
        date_Increment( &p_sys->end_date, 1 );
        p_out->vt->Release( (IUnknown *)p_out );
        return p_pic;
    }
    else
    {
        block_t *p_aout_buffer;
        int i_samples = block_out.i_buffer /
            ( p_dec->fmt_out.audio.i_bitspersample *
              p_dec->fmt_out.audio.i_channels / 8 );
        p_aout_buffer = decoder_NewAudioBuffer( p_dec, i_samples );
        if( p_aout_buffer )
        {
            memcpy( p_aout_buffer->p_buffer,
                    block_out.p_buffer, block_out.i_buffer );
            /* Date management */
            p_aout_buffer->i_pts = date_Get( &p_sys->end_date );
            p_aout_buffer->i_length =
                date_Increment( &p_sys->end_date, i_samples )
                - p_aout_buffer->i_pts;
        }
        p_out->vt->Release( (IUnknown *)p_out );
        return p_aout_buffer;
    }
    return NULL;
}
