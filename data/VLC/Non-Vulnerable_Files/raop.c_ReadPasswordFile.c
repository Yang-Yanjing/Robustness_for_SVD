}
static char *ReadPasswordFile( vlc_object_t *p_this, const char *psz_path )
{
    FILE *p_file = NULL;
    char *psz_password = NULL;
    char *psz_newline;
    char ps_buffer[256];
    p_file = vlc_fopen( psz_path, "rt" );
    if ( p_file == NULL )
    {
        msg_Err( p_this, "Unable to open password file '%s': %s", psz_path,
                 vlc_strerror_c(errno) );
        goto error;
    }
    /* Read one line only */
    if ( fgets( ps_buffer, sizeof( ps_buffer ), p_file ) == NULL )
    {
        if ( ferror( p_file ) )
        {
            msg_Err( p_this, "Error reading '%s': %s", psz_path,
                     vlc_strerror_c(errno) );
            goto error;
        }
        /* Nothing was read, but there was no error either. Maybe the file is
         * empty. Not all implementations of fgets(3) write \0 to the output
         * buffer in this case.
         */
        ps_buffer[0] = '\0';
    } else {
        /* Replace first newline with '\0' */
        psz_newline = strchr( ps_buffer, '\n' );
        if ( psz_newline != NULL )
            *psz_newline = '\0';
    }
    if ( *ps_buffer == '\0' ) {
        msg_Err( p_this, "No password could be read from '%s'", psz_path );
        goto error;
    }
    psz_password = strdup( ps_buffer );
error:
    if ( p_file != NULL )
        fclose( p_file );
    return psz_password;
}
