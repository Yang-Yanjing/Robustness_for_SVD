 *****************************************************************************/
static int CheckSegmentChange( sout_access_out_t *p_access, block_t *p_buffer )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    block_t *output = p_sys->block_buffer;
    /* let's check if we need to store offset to keep
     * better count of actual duration */
    if( unlikely( p_buffer->i_dts < p_sys->i_opendts ) )
    {
        block_t *last_buffer = p_sys->block_buffer;
        while( last_buffer->p_next )
            last_buffer = last_buffer->p_next;
        p_sys->i_dts_offset += last_buffer->i_dts - p_sys->i_opendts;
        p_sys->i_opendts    = p_buffer->i_dts;
        msg_Dbg( p_access, "dts offset %"PRId64, p_sys->i_dts_offset );
    }
    if( p_sys->i_handle > 0 && p_sys->b_segment_has_data &&
       (( p_buffer->i_length + p_buffer->i_dts - p_sys->i_opendts +
          p_sys->i_dts_offset ) >= p_sys->i_seglenm ) )
    {
        closeCurrentSegment( p_access, p_sys, false );
    }
    if ( unlikely( p_sys->i_handle < 0 ) )
    {
        p_sys->i_dts_offset = 0;
        p_sys->i_opendts = output ? output->i_dts : p_buffer->i_dts;
        //For first segment we can get negative duration otherwise...?
        if( ( p_sys->i_opendts != VLC_TS_INVALID ) &&
            ( p_buffer->i_dts < p_sys->i_opendts ) )
            p_sys->i_opendts = p_buffer->i_dts;
        if ( openNextFile( p_access, p_sys ) < 0 )
           return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
