 *****************************************************************************/
static ssize_t openNextFile( sout_access_out_t *p_access, sout_access_out_sys_t *p_sys )
{
    int fd;
    uint32_t i_newseg = p_sys->i_segment + 1;
    /* Create segment and fill it info that we can (everything excluding duration */
    output_segment_t *segment = (output_segment_t*)calloc(1, sizeof(output_segment_t));
    if( unlikely( !segment ) )
        return -1;
    segment->i_segment_number = i_newseg;
    segment->psz_filename = formatSegmentPath( p_access->psz_path, i_newseg, true );
    char *psz_idxFormat = p_sys->psz_indexUrl ? p_sys->psz_indexUrl : p_access->psz_path;
    segment->psz_uri = formatSegmentPath( psz_idxFormat , i_newseg, false );
    if ( unlikely( !segment->psz_filename ) )
    {
        msg_Err( p_access, "Format segmentpath failed");
        destroySegment( segment );
        return -1;
    }
    fd = vlc_open( segment->psz_filename, O_WRONLY | O_CREAT | O_LARGEFILE |
                     O_TRUNC, 0666 );
    if ( fd == -1 )
    {
        msg_Err( p_access, "cannot open `%s' (%s)", segment->psz_filename,
                 vlc_strerror_c(errno) );
        destroySegment( segment );
        return -1;
    }
    vlc_array_append( p_sys->segments_t, segment);
    if( p_sys->psz_keyfile )
    {
        LoadCryptFile( p_access );
    }
    if( p_sys->key_uri )
    {
        segment->psz_key_uri = strdup( p_sys->key_uri );
        CryptKey( p_access, i_newseg );
        if( p_sys->b_generate_iv )
            memcpy( segment->aes_ivs, p_sys->aes_ivs, sizeof(uint8_t)*16 );
    }
    msg_Dbg( p_access, "Successfully opened livehttp file: %s (%"PRIu32")" , segment->psz_filename, i_newseg );
    p_sys->psz_cursegPath = strdup(segment->psz_filename);
    p_sys->i_handle = fd;
    p_sys->i_segment = i_newseg;
    p_sys->b_segment_has_data = false;
    return fd;
}
