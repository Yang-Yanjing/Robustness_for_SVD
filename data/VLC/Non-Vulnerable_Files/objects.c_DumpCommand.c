 *****************************************************************************/
static int DumpCommand( vlc_object_t *p_this, char const *psz_cmd,
                        vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    (void)oldval;
    vlc_object_t *p_object = NULL;
    if( *newval.psz_string )
    {
        /* try using the object's name to find it */
        p_object = vlc_object_find_name( p_data, newval.psz_string );
        if( !p_object )
            return VLC_ENOOBJ;
    }
    libvlc_lock (p_this->p_libvlc);
    if( *psz_cmd == 't' )
    {
        char psz_foo[2 * MAX_DUMPSTRUCTURE_DEPTH + 1];
        if( !p_object )
            p_object = VLC_OBJECT(p_this->p_libvlc);
        psz_foo[0] = '|';
        DumpStructure( vlc_internals(p_object), 0, psz_foo );
    }
    else if( *psz_cmd == 'v' )
    {
        if( !p_object )
            p_object = p_this->p_libvlc ? VLC_OBJECT(p_this->p_libvlc) : p_this;
        PrintObject( vlc_internals(p_object), "" );
        vlc_mutex_lock( &vlc_internals( p_object )->var_lock );
        if( vlc_internals( p_object )->var_root == NULL )
            puts( " `-o No variables" );
        else
            twalk( vlc_internals( p_object )->var_root, DumpVariable );
        vlc_mutex_unlock( &vlc_internals( p_object )->var_lock );
    }
    libvlc_unlock (p_this->p_libvlc);
    if( *newval.psz_string )
    {
        vlc_object_release( p_object );
    }
    return VLC_SUCCESS;
}
