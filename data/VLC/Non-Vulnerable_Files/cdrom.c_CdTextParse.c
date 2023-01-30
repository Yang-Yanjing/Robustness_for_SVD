/* */
static int CdTextParse( vlc_meta_t ***ppp_tracks, int *pi_tracks,
                        const uint8_t *p_buffer, int i_buffer )
{
    char *pppsz_info[128][0x10];
    int i_track_last = -1;
    if( i_buffer < 4 )
        return -1;
    memset( pppsz_info, 0, sizeof(pppsz_info) );
    for( int i = 0; i < (i_buffer-4)/18; i++ )
    {
        const uint8_t *p_block = &p_buffer[4 + 18*i];
        char psz_text[12+1];
        const int i_pack_type = p_block[0];
        if( i_pack_type < 0x80 || i_pack_type > 0x8f )
            continue;
        const int i_track_number = (p_block[1] >> 0)&0x7f;
        const int i_extension_flag = ( p_block[1] >> 7)& 0x01;
        if( i_extension_flag )
            continue;
        //const int i_sequence_number = p_block[2];
        //const int i_charater_position = (p_block[3] >> 0) &0x0f;
        //const int i_block_number = (p_block[3] >> 4) &0x07;
        /* TODO unicode support
         * I need a sample */
        //const int i_unicode = ( p_block[3] >> 7)&0x01;
        //const int i_crc = (p_block[4+12] << 8) | (p_block[4+13] << 0);
        /* */
        memcpy( psz_text, &p_block[4], 12 );
        psz_text[12] = '\0';
        /* */
        int i_track =  i_track_number;
        char *psz_track = &psz_text[0];
        while( i_track <= 127 && psz_track < &psz_text[12] )
        {
            //fprintf( stderr, "t=%d psz_track=%p end=%p", i_track, psz_track, &psz_text[12] );
            if( *psz_track )
            {
                astrcat( &pppsz_info[i_track][i_pack_type-0x80], psz_track );
                i_track_last = __MAX( i_track_last, i_track );
            }
            i_track++;
            psz_track += 1 + strlen(psz_track);
        }
    }
    if( i_track_last < 0 )
        return -1;
    vlc_meta_t **pp_tracks = calloc( i_track_last+1, sizeof(*pp_tracks) );
    if( !pp_tracks )
        goto exit;
    for( int j = 0; j < 0x10; j++ )
    {
        for( int i = 0; i <= i_track_last; i++ )
        {
            /* */
            if( pppsz_info[i][j] )
                EnsureUTF8( pppsz_info[i][j] );
            /* */
            const char *psz_default = pppsz_info[0][j];
            const char *psz_value = pppsz_info[i][j];
            if( !psz_value && !psz_default )
                continue;
            vlc_meta_t *p_track = pp_tracks[i];
            if( !p_track )
            {
                p_track = pp_tracks[i] = vlc_meta_New();
                if( !p_track )
                    continue;
            }
            switch( j )
            {
            case 0x00: /* Album/Title */
                if( i == 0 )
                {
                    vlc_meta_SetAlbum( p_track, psz_value );
                }
                else
                {
                    if( psz_value )
                        vlc_meta_SetTitle( p_track, psz_value );
                    if( psz_default )
                        vlc_meta_SetAlbum( p_track, psz_default );
                }
                break;
            case 0x01: /* Performer */
                vlc_meta_SetArtist( p_track,
                                    psz_value ? psz_value : psz_default );
                break;
            case 0x05: /* Messages */
                vlc_meta_SetDescription( p_track,
                                         psz_value ? psz_value : psz_default );
                break;
            case 0x07: /* Genre */
                vlc_meta_SetGenre( p_track,
                                   psz_value ? psz_value : psz_default );
                break;
            /* FIXME unsupported:
             * 0x02: songwriter
             * 0x03: composer
             * 0x04: arrenger
             * 0x06: disc id */
            }
        }
    }
    /* */
exit:
    for( int j = 0; j < 0x10; j++ )
        for( int i = 0; i <= i_track_last; i++ )
            free( pppsz_info[i][j] );
    *ppp_tracks = pp_tracks;
    *pi_tracks = i_track_last+1;
    return pp_tracks ? 0 : -1;
}
