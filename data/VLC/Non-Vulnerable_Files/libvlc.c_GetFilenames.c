 *****************************************************************************/
static void GetFilenames( libvlc_int_t *p_vlc, unsigned n,
                          const char *const args[] )
{
    while( n > 0 )
    {
        /* Count the input options */
        unsigned i_options = 0;
        while( args[--n][0] == ':' )
        {
            i_options++;
            if( n == 0 )
            {
                msg_Warn( p_vlc, "options %s without item", args[n] );
                return; /* syntax!? */
            }
        }
        char *mrl = NULL;
        if( strstr( args[n], "://" ) == NULL )
        {
            mrl = vlc_path2uri( args[n], NULL );
            if( !mrl )
                continue;
        }
        intf_InsertItem( p_vlc, (mrl != NULL) ? mrl : args[n], i_options,
                         ( i_options ? &args[n + 1] : NULL ),
                         VLC_INPUT_OPTION_TRUSTED );
        free( mrl );
    }
}
