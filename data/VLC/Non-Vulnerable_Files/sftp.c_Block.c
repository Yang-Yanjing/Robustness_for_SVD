}
static block_t* Block( access_t* p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    if( p_access->info.b_eof )
        return NULL;
    /* Allocate the buffer we need */
    size_t i_len = __MIN( p_sys->i_read_size,
                          p_sys->filesize - p_access->info.i_pos );
    block_t* p_block = block_Alloc( i_len );
    if( !p_block )
        return NULL;
    /* Read the specified size */
    ssize_t i_ret = libssh2_sftp_read( p_access->p_sys->file, (char*)p_block->p_buffer, i_len );
    if( i_ret < 0 )
    {
        block_Release( p_block );
        msg_Err( p_access, "read failed" );
        return NULL;
    }
    else if( i_ret == 0 )
    {
        p_access->info.b_eof = true;
        block_Release( p_block );
        return NULL;
    }
    else
    {
        p_block->i_buffer = i_ret;
        p_access->info.i_pos += i_ret;
        return p_block;
    }
}
