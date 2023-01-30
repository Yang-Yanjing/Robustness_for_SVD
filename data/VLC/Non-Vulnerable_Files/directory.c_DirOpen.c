 *****************************************************************************/
int DirOpen( vlc_object_t *p_this )
{
    access_t *p_access = (access_t*)p_this;
    if( !p_access->psz_filepath )
        return VLC_EGENERIC;
    DIR *handle = vlc_opendir (p_access->psz_filepath);
    if (handle == NULL)
        return VLC_EGENERIC;
    return DirInit (p_access, handle);
}
