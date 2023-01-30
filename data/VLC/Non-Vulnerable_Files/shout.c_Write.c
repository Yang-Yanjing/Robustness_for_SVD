 *****************************************************************************/
static ssize_t Write( sout_access_out_t *p_access, block_t *p_buffer )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    size_t i_write = 0;
    shout_sync( p_sys->p_shout );
    while( p_buffer )
    {
        block_t *p_next = p_buffer->p_next;
        if( shout_send( p_sys->p_shout, p_buffer->p_buffer, p_buffer->i_buffer )
             == SHOUTERR_SUCCESS )
        {
            i_write += p_buffer->i_buffer;
        }
        else
        {
            msg_Err( p_access, "cannot write to stream: %s",
                     shout_get_error( p_sys->p_shout ) );
            /* The most common cause seems to be a server disconnect, resulting in a
               Socket Error which can only be fixed by closing and reconnecting.
               Since we already began with a working connection, the most feasable
               approach to get out of this error status is a (timed) reconnect approach. */
            shout_close( p_sys->p_shout );
            msg_Warn( p_access, "server unavailable? trying to reconnect..." );
            /* Re-open the connection (protocol params have already been set) and re-sync */
            if( shout_open( p_sys->p_shout ) == SHOUTERR_SUCCESS )
            {
                shout_sync( p_sys->p_shout );
                msg_Warn( p_access, "reconnected to server" );
            }
            else
            {
                msg_Err( p_access, "failed to reconnect to server" );
                block_ChainRelease( p_buffer );
                return VLC_EGENERIC;
            }
        }
        block_Release( p_buffer );
        /* XXX: Unsure if that's the cause for some audio trouble... */
        p_buffer = p_next;
    }
    return i_write;
}
