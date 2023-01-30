}
static int recursive_mkdir( vlc_object_t *p_this, const char *psz_dirname )
{/* stolen from config_CreateDir() */
    if( !psz_dirname || !*psz_dirname ) return -1;
    if( vlc_mkdir( psz_dirname, 0700 ) == 0 )
        return 0;
    switch( errno )
    {
        case EEXIST:
            return 0;
        case ENOENT:
        {
            /* Let's try to create the parent directory */
            char psz_parent[strlen( psz_dirname ) + 1], *psz_end;
            strcpy( psz_parent, psz_dirname );
            psz_end = strrchr( psz_parent, DIR_SEP_CHAR );
            if( psz_end && psz_end != psz_parent )
            {
                *psz_end = '\0';
                if( recursive_mkdir( p_this, psz_parent ) == 0 )
                {
                    if( !vlc_mkdir( psz_dirname, 0700 ) )
                        return 0;
                }
            }
        }
    }
    msg_Warn( p_this, "could not create %s: %m", psz_dirname );
    return -1;
}
