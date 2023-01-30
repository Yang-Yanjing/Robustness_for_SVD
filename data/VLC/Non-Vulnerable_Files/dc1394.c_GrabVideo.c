 *****************************************************************************/
static block_t *GrabVideo( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_block = NULL;
    if( dc1394_capture_dequeue( p_sys->camera,
                DC1394_CAPTURE_POLICY_WAIT,
                &p_sys->frame ) != DC1394_SUCCESS )
    {
        msg_Err( p_demux, "Unable to capture a frame" );
        return NULL;
    }
    p_block = block_Alloc( p_sys->frame->size[0] * p_sys->frame->size[1] * 2 );
    if( !p_block )
    {
        msg_Err( p_demux, "Can not get block" );
        return NULL;
    }
    if( !p_sys->frame->image )
    {
        msg_Err (p_demux, "Capture buffer empty");
        block_Release( p_block );
        return NULL;
    }
    memcpy( p_block->p_buffer, (const char *)p_sys->frame->image,
            p_sys->width * p_sys->height * 2 );
    p_block->i_pts = p_block->i_dts = mdate();
    dc1394_capture_enqueue( p_sys->camera, p_sys->frame );
    return p_block;
}
