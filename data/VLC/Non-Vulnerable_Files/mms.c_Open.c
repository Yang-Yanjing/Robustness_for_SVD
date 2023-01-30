 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    access_t *p_access = (access_t*)p_this;
    /* use specified method */
    if( *p_access->psz_access )
    {
        if( !strncmp( p_access->psz_access, "mmsu", 4 ) )
        {
            return  MMSTUOpen ( p_access );
        }
        else if( !strncmp( p_access->psz_access, "mmst", 4 ) )
        {
            return  MMSTUOpen ( p_access );
        }
        else if( !strncmp( p_access->psz_access, "mmsh", 4 ) ||
                 !strncmp( p_access->psz_access, "http", 4 ) )
        {
            return  MMSHOpen ( p_access );
        }
    }
    if( MMSTUOpen ( p_access ) )
    {
        if( !vlc_object_alive(p_access) )
            return VLC_EGENERIC;
        /* try mmsh if mmstu failed */
        return  MMSHOpen ( p_access );
    }
    return VLC_SUCCESS;
}
