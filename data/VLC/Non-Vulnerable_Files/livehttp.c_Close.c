 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_access_out_t *p_access = (sout_access_out_t*)p_this;
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    block_t *output_block = p_sys->block_buffer;
    p_sys->block_buffer = NULL;
    p_sys->last_block_buffer = &p_sys->block_buffer;
    while( output_block )
    {
        block_t *p_next = output_block->p_next;
        output_block->p_next = NULL;
        /* Since we are flushing, check the segment change by hand and don't wait
         * possible keyframe*/
        if( p_sys->b_segment_has_data &&  (float)(output_block->i_length + p_sys->i_dts_offset +
                     output_block->i_dts - p_sys->i_opendts) >= p_sys->i_seglenm )
        {
            closeCurrentSegment( p_access, p_sys, false );
            p_sys->i_dts_offset = 0;
            if( unlikely(openNextFile( p_access, p_sys ) < 0 ) )
            {
                block_ChainRelease( output_block );
                output_block = NULL;
                block_ChainRelease( p_next );
                /* Jump out of the loop so we can close rest of the stuff*/
                continue;
            }
            p_sys->i_opendts = p_sys->block_buffer ? p_sys->block_buffer->i_dts : output_block->i_dts;
        }
        Write( p_access, output_block );
        output_block = p_next;
    }
    ssize_t writevalue = writeSegment( p_access );
    msg_Dbg( p_access, "Writing.. %zd", writevalue );
    if( unlikely( writevalue < 0 ) )
    {
        block_ChainRelease( p_sys->block_buffer );
        p_sys->block_buffer = NULL;
        p_sys->last_block_buffer = &p_sys->block_buffer;
    }
    closeCurrentSegment( p_access, p_sys, true );
    if( p_sys->key_uri )
    {
        gcry_cipher_close( p_sys->aes_ctx );
        free( p_sys->key_uri );
    }
    while( vlc_array_count( p_sys->segments_t ) > 0 )
    {
        output_segment_t *segment = vlc_array_item_at_index( p_sys->segments_t, 0 );
        vlc_array_remove( p_sys->segments_t, 0 );
        if( p_sys->b_delsegs && p_sys->i_numsegs && segment->psz_filename )
        {
            msg_Dbg( p_access, "Removing segment number %d name %s", segment->i_segment_number, segment->psz_filename );
            vlc_unlink( segment->psz_filename );
        }
        destroySegment( segment );
    }
    vlc_array_destroy( p_sys->segments_t );
    free( p_sys->psz_indexUrl );
    free( p_sys->psz_indexPath );
    free( p_sys );
    msg_Dbg( p_access, "livehttp access output closed" );
}
