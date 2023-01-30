 */
static HRESULT FillBuffer( vlc_object_t *obj, aout_stream_sys_t *p_sys,
                           block_t *p_buffer )
{
    size_t towrite = (p_buffer != NULL) ? p_buffer->i_buffer : DS_BUF_SIZE;
    void *p_write_position, *p_wrap_around;
    unsigned long l_bytes1, l_bytes2;
    HRESULT dsresult;
    vlc_mutex_lock( &p_sys->lock );
    /* Before copying anything, we have to lock the buffer */
    dsresult = IDirectSoundBuffer_Lock(
           p_sys->p_dsbuffer,    /* DS buffer */
           p_sys->i_write,       /* Start offset */
           towrite,                /* Number of bytes */
           &p_write_position,    /* Address of lock start */
           &l_bytes1,            /* Count of bytes locked before wrap around */
           &p_wrap_around,       /* Buffer address (if wrap around) */
           &l_bytes2,            /* Count of bytes after wrap around */
           0 );                  /* Flags: DSBLOCK_FROMWRITECURSOR is buggy */
    if( dsresult == DSERR_BUFFERLOST )
    {
        IDirectSoundBuffer_Restore( p_sys->p_dsbuffer );
        dsresult = IDirectSoundBuffer_Lock(
                               p_sys->p_dsbuffer,
                               p_sys->i_write,
                               towrite,
                               &p_write_position,
                               &l_bytes1,
                               &p_wrap_around,
                               &l_bytes2,
                               0 );
    }
    if( dsresult != DS_OK )
    {
        msg_Warn( obj, "cannot lock buffer" );
        if( p_buffer != NULL )
            block_Release( p_buffer );
        vlc_mutex_unlock( &p_sys->lock );
        return dsresult;
    }
    if( p_buffer == NULL )
    {
        memset( p_write_position, 0, l_bytes1 );
        memset( p_wrap_around, 0, l_bytes2 );
    }
    else
    {
        if( p_sys->chans_to_reorder ) /* Do the channel reordering here */
            aout_ChannelReorder( p_buffer->p_buffer, p_buffer->i_buffer,
                                 p_sys->chans_to_reorder, p_sys->chan_table,
                                 p_sys->format );
        memcpy( p_write_position, p_buffer->p_buffer, l_bytes1 );
        if( p_wrap_around && l_bytes2 )
            memcpy( p_wrap_around, p_buffer->p_buffer + l_bytes1, l_bytes2 );
        if( unlikely( ( l_bytes1 + l_bytes2 ) < p_buffer->i_buffer ) )
            msg_Err( obj, "Buffer overrun");
        block_Release( p_buffer );
    }
    /* Now the data has been copied, unlock the buffer */
    IDirectSoundBuffer_Unlock( p_sys->p_dsbuffer, p_write_position, l_bytes1,
                               p_wrap_around, l_bytes2 );
    p_sys->i_write += towrite;
    p_sys->i_write %= DS_BUF_SIZE;
    p_sys->i_data += towrite;
    vlc_mutex_unlock( &p_sys->lock );
    return DS_OK;
}
