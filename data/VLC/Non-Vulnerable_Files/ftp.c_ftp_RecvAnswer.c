 where the request contains one \377. */
static int ftp_RecvAnswer( vlc_object_t *obj, access_sys_t *sys,
                           int *restrict codep, char **restrict strp,
                           void (*cb)(void *, const char *), void *opaque )
{
    if( codep != NULL )
        *codep = 500;
    if( strp != NULL )
        *strp = NULL;
    char *resp = net_Gets( obj, sys->cmd.fd, sys->cmd.p_vs );
    if( resp == NULL )
    {
        msg_Err( obj, "response failure" );
        goto error;
    }
    char *end;
    unsigned code = strtoul( resp, &end, 10 );
    if( (end - resp) != 3 || (*end != '-' && *end != ' ') )
    {
        msg_Err( obj, "malformatted response" );
        goto error;
    }
    msg_Dbg( obj, "received response: \"%s\"", resp );
    if( *end == '-' ) /* Multi-line response */
    {
        bool done;
        *end = ' ';
        do
        {
            char *line = net_Gets( obj, sys->cmd.fd, sys->cmd.p_vs );
            if( line == NULL )
            {
                msg_Err( obj, "response failure" );
                goto error;
            }
            done = !strncmp( resp, line, 4 );
            if( !done )
                cb( opaque, line );
            free( line );
        }
        while( !done );
    }
    if( codep != NULL )
        *codep = code;
    if( strp != NULL )
        *strp = resp;
    else
        free( resp );
    return code / 100;
error:
    free( resp );
    return -1;
}
