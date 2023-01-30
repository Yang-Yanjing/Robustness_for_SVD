}
static void GetHeader( access_t *p_access, int i_content_length )
{
    access_sys_t *p_sys = p_access->p_sys;
    int i_read_content = 0;
    /* Read the asf header */
    p_sys->i_header = 0;
    free( p_sys->p_header  );
    p_sys->p_header = NULL;
    for( ;; )
    {
        chunk_t ck;
        if( (i_content_length >= 0 && i_read_content >= i_content_length) || GetPacket( p_access, &ck ) || ck.i_type != 0x4824 )
            break;
        i_read_content += (4+ck.i_size);
        if( ck.i_data > 0 )
        {
            p_sys->i_header += ck.i_data;
            p_sys->p_header = xrealloc( p_sys->p_header, p_sys->i_header );
            memcpy( &p_sys->p_header[p_sys->i_header - ck.i_data],
                    ck.p_data, ck.i_data );
        }
    }
    msg_Dbg( p_access, "complete header size=%d", p_sys->i_header );
}
