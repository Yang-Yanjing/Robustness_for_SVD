 *****************************************************************************/
static void closeCurrentSegment( sout_access_out_t *p_access, sout_access_out_sys_t *p_sys, bool b_isend )
{
    if ( p_sys->i_handle >= 0 )
    {
        output_segment_t *segment = (output_segment_t *)vlc_array_item_at_index( p_sys->segments_t, vlc_array_count( p_sys->segments_t ) - 1 );
        if( p_sys->key_uri )
        {
            size_t pad = 16 - p_sys->stuffing_size;
            memset(&p_sys->stuffing_bytes[p_sys->stuffing_size], pad, pad);
            gcry_error_t err = gcry_cipher_encrypt( p_sys->aes_ctx, p_sys->stuffing_bytes, 16, NULL, 0 );
            if( err ) {
               msg_Err( p_access, "Couldn't encrypt 16 bytes: %s", gpg_strerror(err) );
            } else {
            int ret = write( p_sys->i_handle, p_sys->stuffing_bytes, 16 );
            if( ret != 16 )
                msg_Err( p_access, "Couldn't write 16 bytes" );
            }
            p_sys->stuffing_size = 0;
        }
        close( p_sys->i_handle );
        p_sys->i_handle = -1;
        if( ! ( us_asprintf( &segment->psz_duration, "%.2f", p_sys->f_seglen ) ) )
        {
            msg_Err( p_access, "Couldn't set duration on closed segment");
            return;
        }
        segment->f_seglength = p_sys->f_seglen;
        segment->i_segment_number = p_sys->i_segment;
        if ( p_sys->psz_cursegPath )
        {
            msg_Dbg( p_access, "LiveHttpSegmentComplete: %s (%"PRIu32")" , p_sys->psz_cursegPath, p_sys->i_segment );
            free( p_sys->psz_cursegPath );
            p_sys->psz_cursegPath = 0;
            updateIndexAndDel( p_access, p_sys, b_isend );
        }
    }
}
