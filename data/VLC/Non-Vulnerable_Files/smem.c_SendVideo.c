}
static int SendVideo( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                      block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    size_t i_size = p_buffer->i_buffer;
    uint8_t* p_pixels = NULL;
    /* Calling the prerender callback to get user buffer */
    p_sys->pf_video_prerender_callback( id->p_data, &p_pixels, i_size );
    if (!p_pixels)
    {
        msg_Err( p_stream, "No buffer given!" );
        block_ChainRelease( p_buffer );
        return VLC_EGENERIC;
    }
    /* Copying data into user buffer */
    memcpy( p_pixels, p_buffer->p_buffer, i_size );
    /* Calling the postrender callback to tell the user his buffer is ready */
    p_sys->pf_video_postrender_callback( id->p_data, p_pixels,
                                         id->format->video.i_width, id->format->video.i_height,
                                         id->format->video.i_bits_per_pixel, i_size, p_buffer->i_pts );
    block_ChainRelease( p_buffer );
    return VLC_SUCCESS;
}
