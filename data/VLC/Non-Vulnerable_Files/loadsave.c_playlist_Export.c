#include <vlc_modules.h>
int playlist_Export( playlist_t * p_playlist, const char *psz_filename,
                     playlist_item_t *p_export_root, const char *psz_type )
{
    if( p_export_root == NULL ) return VLC_EGENERIC;
    playlist_export_t *p_export =
        vlc_custom_create( p_playlist, sizeof( *p_export ), "playlist export" );
    if( unlikely(p_export == NULL) )
        return VLC_ENOMEM;
    msg_Dbg( p_export, "saving %s to file %s",
             p_export_root->p_input->psz_name, psz_filename );
    int ret = VLC_EGENERIC;
    /* Prepare the playlist_export_t structure */
    p_export->p_root = p_export_root;
    p_export->psz_filename = psz_filename;
    p_export->p_file = vlc_fopen( psz_filename, "wt" );
    if( p_export->p_file == NULL )
    {
        msg_Err( p_export, "could not create playlist file %s: %s",
                 psz_filename, vlc_strerror_c(errno) );
        goto out;
    }
    module_t *p_module;
    /* And call the module ! All work is done now */
    playlist_Lock( p_playlist );
    p_module = module_need( p_export, "playlist export", psz_type, true );
    playlist_Unlock( p_playlist );
    if( p_module != NULL )
    {
        module_unneed( p_export, p_module );
        if( !ferror( p_export->p_file ) )
            ret = VLC_SUCCESS;
        else
            msg_Err( p_playlist, "could not write playlist file: %s",
                     vlc_strerror_c(errno) );
    }
    else
        msg_Err( p_playlist, "could not export playlist" );
   fclose( p_export->p_file );
out:
   vlc_object_release( p_export );
   return ret;
}
