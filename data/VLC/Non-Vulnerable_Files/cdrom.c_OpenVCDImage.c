 ****************************************************************************/
static int OpenVCDImage( vlc_object_t * p_this, const char *psz_dev,
                         vcddev_t *p_vcddev )
{
    int i_ret = -1;
    char *p_pos;
    char *psz_vcdfile = NULL;
    char *psz_cuefile = NULL;
    FILE *cuefile     = NULL;
    int *p_sectors    = NULL;
    char line[1024];
    bool b_found      = false;
    /* Check if we are dealing with a .cue file */
    p_pos = strrchr( psz_dev, '.' );
    if( p_pos && !strcmp( p_pos, ".cue" ) )
    {
        /* psz_dev must be the cue file. Let's assume there's a .bin
         * file with the same filename */
        if( asprintf( &psz_vcdfile, "%.*s.bin", (int)(p_pos - psz_dev),
                      psz_dev ) < 0 )
            psz_vcdfile = NULL;
        psz_cuefile = strdup( psz_dev );
    }
    else
    if( p_pos )
    {
        /* psz_dev must be the actual vcd file. Let's assume there's a .cue
         * file with the same filename */
        if( asprintf( &psz_cuefile, "%.*s.cue", (int)(p_pos - psz_dev),
                      psz_dev ) < 0 )
            psz_cuefile = NULL;
        psz_vcdfile = strdup( psz_dev );
    }
    else
    {
        if( asprintf( &psz_cuefile, "%s.cue", psz_dev ) == -1 )
            psz_cuefile = NULL;
         /* If we need to look up the .cue file, then we don't have to look
          * for the vcd */
        psz_vcdfile = strdup( psz_dev );
    }
    if( psz_cuefile == NULL || psz_vcdfile == NULL )
        goto error;
    /* Open the cue file and try to parse it */
    msg_Dbg( p_this,"trying .cue file: %s", psz_cuefile );
    cuefile = vlc_fopen( psz_cuefile, "rt" );
    if( cuefile == NULL )
    {
        msg_Dbg( p_this, "could not find .cue file" );
        goto error;
    }
    msg_Dbg( p_this,"guessing vcd image file: %s", psz_vcdfile );
    p_vcddev->i_vcdimage_handle = vlc_open( psz_vcdfile,
                                    O_RDONLY | O_NONBLOCK | O_BINARY );
    while( fgets( line, 1024, cuefile ) && !b_found )
    {
        /* We have a cue file, but no valid vcd file yet */
        char filename[1024];
        char type[16];
        int i_temp = sscanf( line, "FILE \"%1023[^\"]\" %15s", filename, type );
        switch( i_temp )
        {
            case 2:
                msg_Dbg( p_this, "the cue file says the data file is %s", type );
                if( strcasecmp( type, "BINARY" ) )
                    goto error; /* Error if not binary, otherwise treat as case 1 */
            case 1:
                if( p_vcddev->i_vcdimage_handle == -1 )
                {
                    msg_Dbg( p_this, "we could not find the data file, but we found a new path" );
                    free( psz_vcdfile);
                    if( *filename != '/' && ((p_pos = strrchr( psz_cuefile, '/' ))
                        || (p_pos = strrchr( psz_cuefile, '\\' ) )) )
                    {
                        psz_vcdfile = malloc( strlen(filename) +
                                      (p_pos - psz_cuefile + 1) + 1 );
                        strncpy( psz_vcdfile, psz_cuefile, (p_pos - psz_cuefile + 1) );
                        strcpy( psz_vcdfile + (p_pos - psz_cuefile + 1), filename );
                    } else psz_vcdfile = strdup( filename );
                    msg_Dbg( p_this,"using vcd image file: %s", psz_vcdfile );
                    p_vcddev->i_vcdimage_handle = vlc_open( psz_vcdfile,
                                        O_RDONLY | O_NONBLOCK | O_BINARY );
                }
                b_found = true;
            default:
                break;
        }
    }
    if( p_vcddev->i_vcdimage_handle == -1)
        goto error;
    /* Try to parse the i_tracks and p_sectors info so we can just forget
     * about the cuefile */
    size_t i_tracks = 0;
    while( fgets( line, 1024, cuefile ) && i_tracks < INT_MAX-1 )
    {
        /* look for a TRACK line */
        char psz_dummy[10];
        if( !sscanf( line, "%9s", psz_dummy ) || strcmp(psz_dummy, "TRACK") )
            continue;
        /* look for an INDEX line */
        while( fgets( line, 1024, cuefile ) )
        {
            int i_num, i_min, i_sec, i_frame;
            if( (sscanf( line, "%*9s %2u %2u:%2u:%2u", &i_num,
                         &i_min, &i_sec, &i_frame ) != 4) || (i_num != 1) )
                continue;
            int *buf = realloc (p_sectors, (i_tracks + 1) * sizeof (*buf));
            if (buf == NULL)
                goto error;
            p_sectors = buf;
            p_sectors[i_tracks] = MSF_TO_LBA(i_min, i_sec, i_frame);
            msg_Dbg( p_this, "vcd track %i begins at sector:%i",
                     (int)i_tracks, (int)p_sectors[i_tracks] );
            i_tracks++;
            break;
        }
    }
    /* fill in the last entry */
    int *buf = realloc (p_sectors, (i_tracks + 1) * sizeof (*buf));
    if (buf == NULL)
        goto error;
    p_sectors = buf;
    p_sectors[i_tracks] = lseek(p_vcddev->i_vcdimage_handle, 0, SEEK_END)
                                 / VCD_SECTOR_SIZE;
    msg_Dbg( p_this, "vcd track %i, begins at sector:%i",
             (int)i_tracks, (int)p_sectors[i_tracks] );
    p_vcddev->i_tracks = ++i_tracks;
    p_vcddev->p_sectors = p_sectors;
    p_sectors = NULL;
    i_ret = 0;
error:
    if( cuefile ) fclose( cuefile );
    free( p_sectors );
    free( psz_cuefile );
    free( psz_vcdfile );
    return i_ret;
}
