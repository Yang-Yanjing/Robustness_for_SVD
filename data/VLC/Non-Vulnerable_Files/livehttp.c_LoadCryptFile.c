 ************************************************************************/
static int LoadCryptFile( sout_access_out_t *p_access )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    FILE *stream = vlc_fopen( p_sys->psz_keyfile, "rt" );
    char *key_file=NULL,*key_uri=NULL;
    if( unlikely( stream == NULL ) )
    {
        msg_Err( p_access, "Unable to open keyloadfile %s: %s",
                 p_sys->psz_keyfile, vlc_strerror_c(errno) );
        return VLC_EGENERIC;
    }
    //First read key_uri
    ssize_t len = getline( &key_uri, &(size_t){0}, stream );
    if( unlikely( len == -1 ) )
    {
        msg_Err( p_access, "Cannot read %s: %s", p_sys->psz_keyfile,
                 vlc_strerror_c(errno) );
        clearerr( stream );
        fclose( stream );
        free( key_uri );
        return VLC_EGENERIC;
    }
    //Strip the newline from uri, maybe scanf would be better?
    key_uri[len-1]='\0';
    len = getline( &key_file, &(size_t){0}, stream );
    if( unlikely( len == -1 ) )
    {
        msg_Err( p_access, "Cannot read %s: %s", p_sys->psz_keyfile,
                 vlc_strerror_c(errno) );
        clearerr( stream );
        fclose( stream );
        free( key_uri );
        free( key_file );
        return VLC_EGENERIC;
    }
    // Strip the last newline from filename
    key_file[len-1]='\0';
    fclose( stream );
    int returncode = VLC_SUCCESS;
    if( !p_sys->key_uri || strcmp( p_sys->key_uri, key_uri ) )
    {
        if( p_sys->key_uri )
        {
            free( p_sys->key_uri );
            p_sys->key_uri = NULL;
        }
        p_sys->key_uri = strdup( key_uri );
        returncode = CryptSetup( p_access, key_file );
    }
    free( key_file );
    free( key_uri );
    return returncode;
}
