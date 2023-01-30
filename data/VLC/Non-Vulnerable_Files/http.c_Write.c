 *****************************************************************************/
static ssize_t Write( sout_access_out_t *p_access, block_t *p_buffer )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    int i_err = 0;
    int i_len = 0;
    while( p_buffer )
    {
        block_t *p_next;
        if( p_buffer->i_flags & BLOCK_FLAG_HEADER )
        {
            /* gather header */
            if( p_sys->b_header_complete )
            {
                /* free previously gathered header */
                p_sys->i_header_size = 0;
                p_sys->b_header_complete = false;
            }
            if( (int)(p_buffer->i_buffer + p_sys->i_header_size) >
                p_sys->i_header_allocated )
            {
                p_sys->i_header_allocated =
                    p_buffer->i_buffer + p_sys->i_header_size + 1024;
                p_sys->p_header = xrealloc( p_sys->p_header,
                                                  p_sys->i_header_allocated );
            }
            memcpy( &p_sys->p_header[p_sys->i_header_size],
                    p_buffer->p_buffer,
                    p_buffer->i_buffer );
            p_sys->i_header_size += p_buffer->i_buffer;
        }
        else if( !p_sys->b_header_complete )
        {
            p_sys->b_header_complete = true;
            if ( p_sys->b_metacube )
            {
                struct metacube2_block_header hdr;
                memcpy( hdr.sync, METACUBE2_SYNC, sizeof( METACUBE2_SYNC ) );
                hdr.size = hton32( p_sys->i_header_size );
                hdr.flags = hton16( METACUBE_FLAGS_HEADER );
                hdr.csum = hton16( metacube2_compute_crc( &hdr ) );
                int i_header_size = p_sys->i_header_size + sizeof( hdr );
                uint8_t *p_hdr_block = xmalloc( i_header_size );
                memcpy( p_hdr_block, &hdr, sizeof( hdr ) );
                memcpy( p_hdr_block + sizeof( hdr ), p_sys->p_header, p_sys->i_header_size );
                httpd_StreamHeader( p_sys->p_httpd_stream, p_hdr_block, i_header_size );
                free( p_hdr_block );
            }
            else
            {
                httpd_StreamHeader( p_sys->p_httpd_stream, p_sys->p_header,
                                    p_sys->i_header_size );
            }
        }
        i_len += p_buffer->i_buffer;
        if( p_buffer->i_flags & BLOCK_FLAG_TYPE_I )
        {
            p_sys->b_has_keyframes = true;
        }
        p_next = p_buffer->p_next;
        if( p_sys->b_metacube )
        {
            /* prepend Metacube header */
            struct metacube2_block_header hdr;
            memcpy( hdr.sync, METACUBE2_SYNC, sizeof( METACUBE2_SYNC ) );
            hdr.size = hton32( p_buffer->i_buffer );
            hdr.flags = hton16( 0 );
            if( p_buffer->i_flags & BLOCK_FLAG_HEADER )
                hdr.flags |= hton16( METACUBE_FLAGS_HEADER );
            if( p_sys->b_has_keyframes && !( p_buffer->i_flags & BLOCK_FLAG_TYPE_I ) )
                hdr.flags |= hton16( METACUBE_FLAGS_NOT_SUITABLE_FOR_STREAM_START );
            hdr.csum = hton16( metacube2_compute_crc( &hdr ) );
            p_buffer = block_Realloc( p_buffer, sizeof( hdr ), p_buffer->i_buffer );
            if( p_buffer == NULL ) {
                block_ChainRelease( p_next );
                return VLC_ENOMEM;
            }
            memcpy( p_buffer->p_buffer, &hdr, sizeof( hdr ) );
        }
        /* send data */
        i_err = httpd_StreamSend( p_sys->p_httpd_stream, p_buffer );
        block_Release( p_buffer );
        p_buffer = p_next;
        if( i_err < 0 )
        {
            break;
        }
    }
    if( i_err < 0 )
    {
        block_ChainRelease( p_buffer );
    }
    return( i_err < 0 ? VLC_EGENERIC : i_len );
}
