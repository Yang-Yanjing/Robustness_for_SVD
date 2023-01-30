 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys;
    vcddev_t     *vcddev;
    char         *psz_name;
    if( !p_access->psz_filepath || !*p_access->psz_filepath )
    {
        /* Only when selected */
        if( !p_access->psz_access || !*p_access->psz_access )
            return VLC_EGENERIC;
        psz_name = var_InheritString( p_this, "cd-audio" );
        if( !psz_name )
            return VLC_EGENERIC;
    }
    else psz_name = ToLocaleDup( p_access->psz_filepath );
#if defined( _WIN32 ) || defined( __OS2__ )
    if( psz_name[0] && psz_name[1] == ':' &&
        psz_name[2] == '\\' && psz_name[3] == '\0' ) psz_name[2] = '\0';
#endif
    /* Set up p_access */
    STANDARD_BLOCK_ACCESS_INIT
    /* Open CDDA */
    if( (vcddev = ioctl_Open( VLC_OBJECT(p_access), psz_name ) ) == NULL )
    {
        msg_Warn( p_access, "could not open %s", psz_name );
        free( psz_name );
        free( p_sys );
        return VLC_EGENERIC;
    }
    free( psz_name );
    p_sys->vcddev = vcddev;
    /* Do we play a single track ? */
    p_sys->i_track = var_InheritInteger( p_access, "cdda-track" ) - 1;
    if( p_sys->i_track < 0 )
    {
        /* We only do separate items if the whole disc is requested */
        input_thread_t *p_input = access_GetParentInput( p_access );
        int i_ret = -1;
        if( p_input )
        {
            input_item_t *p_current = input_GetItem( p_input );
            if( p_current )
                i_ret = GetTracks( p_access, p_current );
            vlc_object_release( p_input );
        }
        if( i_ret < 0 )
            goto error;
    }
    else
    {
        /* Build a WAV header for the output data */
        memset( &p_sys->waveheader, 0, sizeof(WAVEHEADER) );
        SetWLE( &p_sys->waveheader.Format, 1 ); /*WAVE_FORMAT_PCM*/
        SetWLE( &p_sys->waveheader.BitsPerSample, 16);
        p_sys->waveheader.MainChunkID = VLC_FOURCC('R', 'I', 'F', 'F');
        p_sys->waveheader.Length = 0;               /* we just don't know */
        p_sys->waveheader.ChunkTypeID = VLC_FOURCC('W', 'A', 'V', 'E');
        p_sys->waveheader.SubChunkID = VLC_FOURCC('f', 'm', 't', ' ');
        SetDWLE( &p_sys->waveheader.SubChunkLength, 16);
        SetWLE( &p_sys->waveheader.Modus, 2);
        SetDWLE( &p_sys->waveheader.SampleFreq, 44100);
        SetWLE( &p_sys->waveheader.BytesPerSample,
                    2 /*Modus*/ * 16 /*BitsPerSample*/ / 8 );
        SetDWLE( &p_sys->waveheader.BytesPerSec,
                    2*16/8 /*BytesPerSample*/ * 44100 /*SampleFreq*/ );
        p_sys->waveheader.DataChunkID = VLC_FOURCC('d', 'a', 't', 'a');
        p_sys->waveheader.DataLength = 0;           /* we just don't know */
        p_sys->i_first_sector = var_InheritInteger( p_access,
                                                    "cdda-first-sector" );
        p_sys->i_last_sector  = var_InheritInteger( p_access,
                                                    "cdda-last-sector" );
        /* Tracknumber in MRL */
        if( p_sys->i_first_sector < 0 || p_sys->i_last_sector < 0 )
        {
            const int i_titles = ioctl_GetTracksMap( VLC_OBJECT(p_access),
                                                     p_sys->vcddev, &p_sys->p_sectors );
            if( p_sys->i_track >= i_titles )
            {
                msg_Err( p_access, "invalid track number" );
                goto error;
            }
            p_sys->i_first_sector = p_sys->p_sectors[p_sys->i_track];
            p_sys->i_last_sector = p_sys->p_sectors[p_sys->i_track+1];
        }
        p_sys->i_sector = p_sys->i_first_sector;
        p_sys->size = (p_sys->i_last_sector - p_sys->i_first_sector)
                                     * (int64_t)CDDA_DATA_SIZE;
    }
    return VLC_SUCCESS;
error:
    free( p_sys->p_sectors );
    ioctl_Close( VLC_OBJECT(p_access), p_sys->vcddev );
    free( p_sys );
    return VLC_EGENERIC;
}
