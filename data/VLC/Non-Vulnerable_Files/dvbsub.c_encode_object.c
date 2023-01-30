                               bool b_top );
static void encode_object( encoder_t *p_enc, bs_t *s, subpicture_t *p_subpic )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    subpicture_region_t *p_region;
    int i_region;
    int i_length_pos, i_update_pos, i_pixel_data_pos;
    for( i_region = 0, p_region = p_subpic->p_region; p_region;
         p_region = p_region->p_next, i_region++ )
    {
        bs_write( s, 8, 0x0f ); /* Sync byte */
        bs_write( s, 8, DVBSUB_ST_OBJECT_DATA ); /* Segment type */
        bs_write( s, 16, 1 ); /* Page id */
        i_length_pos = bs_pos( s );
        bs_write( s, 16, 0 ); /* Segment length */
        bs_write( s, 16, i_region ); /* Object id */
        bs_write( s, 4, p_sys->i_region_ver++ );
        /* object coding method */
        switch( p_region->fmt.i_chroma )
        {
        case VLC_CODEC_YUVP:
            bs_write( s, 2, 0 );
            break;
        case VLC_CODEC_TEXT:
            bs_write( s, 2, 1 );
            break;
        default:
            msg_Err( p_enc, "FOURCC %d not supported by encoder.", p_region->fmt.i_chroma );
            continue;
        }
        bs_write( s, 1, 0 ); /* non modifying color flag */
        bs_write( s, 1, 0 ); /* Reserved */
        if( p_region->fmt.i_chroma == VLC_CODEC_TEXT )
        {
            int i_size, i;
            if( !p_region->psz_text ) continue;
            i_size = __MIN( strlen( p_region->psz_text ), 256 );
            bs_write( s, 8, i_size ); /* number of characters in string */
            for( i = 0; i < i_size; i++ )
            {
                bs_write( s, 16, p_region->psz_text[i] );
            }
            /* Update segment length */
            SetWBE( &s->p_start[i_length_pos/8],
                    (bs_pos(s) - i_length_pos)/8 -2 );
            continue;
        }
        /* Coding of a bitmap object */
        i_update_pos = bs_pos( s );
        bs_write( s, 16, 0 ); /* topfield data block length */
        bs_write( s, 16, 0 ); /* bottomfield data block length */
        /* Top field */
        i_pixel_data_pos = bs_pos( s );
        encode_pixel_data( p_enc, s, p_region, true );
        i_pixel_data_pos = ( bs_pos( s ) - i_pixel_data_pos ) / 8;
        SetWBE( &s->p_start[i_update_pos/8], i_pixel_data_pos );
        /* Bottom field */
        i_pixel_data_pos = bs_pos( s );
        encode_pixel_data( p_enc, s, p_region, false );
        i_pixel_data_pos = ( bs_pos( s ) - i_pixel_data_pos ) / 8;
        SetWBE( &s->p_start[i_update_pos/8+2], i_pixel_data_pos );
        /* Stuffing for word alignment */
        bs_align_0( s );
        if( bs_pos( s ) % 16 ) bs_write( s, 8, 0 );
        /* Update segment length */
        SetWBE( &s->p_start[i_length_pos/8], (bs_pos(s) - i_length_pos)/8 -2 );
    }
}
