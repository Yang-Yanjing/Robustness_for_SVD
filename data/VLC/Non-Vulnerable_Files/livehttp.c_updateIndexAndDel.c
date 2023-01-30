 ************************************************************************/
static int updateIndexAndDel( sout_access_out_t *p_access, sout_access_out_sys_t *p_sys, bool b_isend )
{
    uint32_t i_firstseg;
    unsigned i_index_offset = 0;
    if ( p_sys->i_numsegs == 0 ||
         p_sys->i_segment < ( p_sys->i_numsegs + p_sys->i_initial_segment ) )
    {
        i_firstseg = p_sys->i_initial_segment == 0 ? 1 : p_sys->i_initial_segment;
    }
    else
    {
        unsigned numsegs = segmentAmountNeeded( p_sys );
        i_firstseg = ( p_sys->i_segment - numsegs ) + 1;
        i_index_offset = vlc_array_count( p_sys->segments_t ) - numsegs;
    }
    // First update index
    if ( p_sys->psz_indexPath )
    {
        int val;
        FILE *fp;
        char *psz_idxTmp;
        if ( asprintf( &psz_idxTmp, "%s.tmp", p_sys->psz_indexPath ) < 0)
            return -1;
        fp = vlc_fopen( psz_idxTmp, "wt");
        if ( !fp )
        {
            msg_Err( p_access, "cannot open index file `%s'", psz_idxTmp );
            free( psz_idxTmp );
            return -1;
        }
        if ( fprintf( fp, "#EXTM3U\n#EXT-X-TARGETDURATION:%zu\n#EXT-X-VERSION:3\n#EXT-X-ALLOW-CACHE:%s"
                          "%s\n#EXT-X-MEDIA-SEQUENCE:%"PRIu32"\n", p_sys->i_seglen,
                          p_sys->b_caching ? "YES" : "NO",
                          p_sys->i_numsegs > 0 ? "" : b_isend ? "\n#EXT-X-PLAYLIST-TYPE:VOD" : "\n#EXT-X-PLAYLIST-TYPE:EVENT",
                          i_firstseg ) < 0 )
        {
            free( psz_idxTmp );
            fclose( fp );
            return -1;
        }
        char *psz_current_uri=NULL;
        for ( uint32_t i = i_firstseg; i <= p_sys->i_segment; i++ )
        {
            //scale to i_index_offset..numsegs + i_index_offset
            uint32_t index = i - i_firstseg + i_index_offset;
            output_segment_t *segment = (output_segment_t *)vlc_array_item_at_index( p_sys->segments_t, index );
            if( p_sys->key_uri &&
                ( !psz_current_uri ||  strcmp( psz_current_uri, segment->psz_key_uri ) )
              )
            {
                int ret = 0;
                free( psz_current_uri );
                psz_current_uri = strdup( segment->psz_key_uri );
                if( p_sys->b_generate_iv )
                {
                    unsigned long long iv_hi = segment->aes_ivs[0];
                    unsigned long long iv_lo = segment->aes_ivs[8];
                    for( unsigned short i = 1; i < 8; i++ )
                    {
                        iv_hi <<= 8;
                        iv_hi |= segment->aes_ivs[i] & 0xff;
                        iv_lo <<= 8;
                        iv_lo |= segment->aes_ivs[8+i] & 0xff;
                    }
                    ret = fprintf( fp, "#EXT-X-KEY:METHOD=AES-128,URI=\"%s\",IV=0X%16.16llx%16.16llx\n",
                                   segment->psz_key_uri, iv_hi, iv_lo );
                } else {
                    ret = fprintf( fp, "#EXT-X-KEY:METHOD=AES-128,URI=\"%s\"\n", segment->psz_key_uri );
                }
                if( ret < 0 )
                {
                    free( psz_current_uri );
                    free( psz_idxTmp );
                    fclose( fp );
                    return -1;
                }
            }
            val = fprintf( fp, "#EXTINF:%s,\n%s\n", segment->psz_duration, segment->psz_uri);
            if ( val < 0 )
            {
                free( psz_current_uri );
                free( psz_idxTmp );
                fclose( fp );
                return -1;
            }
        }
        free( psz_current_uri );
        if ( b_isend )
        {
            if ( fputs ( STR_ENDLIST, fp ) < 0)
            {
                free( psz_idxTmp );
                fclose( fp ) ;
                return -1;
            }
        }
        fclose( fp );
        val = vlc_rename ( psz_idxTmp, p_sys->psz_indexPath);
        if ( val < 0 )
        {
            vlc_unlink( psz_idxTmp );
            msg_Err( p_access, "Error moving LiveHttp index file" );
        }
        else
            msg_Dbg( p_access, "LiveHttpIndexComplete: %s" , p_sys->psz_indexPath );
        free( psz_idxTmp );
    }
    // Then take care of deletion
    // Try to follow pantos draft 11 section 6.2.2
    while( p_sys->b_delsegs && p_sys->i_numsegs &&
           isFirstItemRemovable( p_sys, i_firstseg, i_index_offset )
         )
    {
         output_segment_t *segment = vlc_array_item_at_index( p_sys->segments_t, 0 );
         msg_Dbg( p_access, "Removing segment number %d", segment->i_segment_number );
         vlc_array_remove( p_sys->segments_t, 0 );
         if ( segment->psz_filename )
         {
             vlc_unlink( segment->psz_filename );
         }
         destroySegment( segment );
         i_index_offset -=1;
    }
    return 0;
}
