 *****************************************************************************/
static ssize_t Read( access_t *p_access, uint8_t *p_buffer, size_t i_len )
{
    access_sys_t *p_sys = p_access->p_sys;
    assert( p_sys->data.fd != -1 );
    assert( !p_sys->out );
    if( p_access->info.b_eof )
        return 0;
    if( p_sys->directory )
    {
        char *psz_line = net_Gets( p_access, p_sys->data.fd, p_sys->data.p_vs );
        if( !psz_line )
        {
            p_access->info.b_eof = true;
            return 0;
        }
        else
        {
            snprintf( (char*)p_buffer, i_len, "%s://%s:%d/%s/%s\n",
                      ( p_sys->tlsmode == NONE ) ? "ftp" :
                      ( ( p_sys->tlsmode == IMPLICIT ) ? "ftps" : "ftpes" ),
                      p_sys->url.psz_host, p_sys->url.i_port,
                      p_sys->url.psz_path, psz_line );
            free( psz_line );
            return strlen( (const char *)p_buffer );
        }
    }
    else
    {
        int i_read = net_Read( p_access, p_sys->data.fd, p_sys->data.p_vs,
                               p_buffer, i_len, false );
        if( i_read == 0 )
            p_access->info.b_eof = true;
        else if( i_read > 0 )
            p_access->info.i_pos += i_read;
        return i_read;
    }
}
