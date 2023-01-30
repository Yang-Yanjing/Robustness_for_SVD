#define CACHE_STRING "cache "PACKAGE_NAME" "PACKAGE_VERSION
void CacheDelete( vlc_object_t *obj, const char *dir )
{
    char *path;
    assert( dir != NULL );
    if( asprintf( &path, "%s"DIR_SEP CACHE_NAME, dir ) == -1 )
        return;
    msg_Dbg( obj, "removing plugins cache file %s", path );
    vlc_unlink( path );
    free( path );
}
