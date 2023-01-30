                                   int i_line );
static void encode_pixel_data( encoder_t *p_enc, bs_t *s,
                               subpicture_region_t *p_region,
                               bool b_top )
{
    unsigned int i_line;
    /* Sanity check */
    if( p_region->fmt.i_chroma != VLC_CODEC_YUVP ) return;
    /* Encode line by line */
    for( i_line = !b_top; i_line < p_region->fmt.i_visible_height;
         i_line += 2 )
    {
        switch( p_region->fmt.p_palette->i_entries )
        {
        case 0:
            break;
        case 4:
            bs_write( s, 8, 0x10 ); /* 2 bit/pixel code string */
            encode_pixel_line_2bp( s, p_region, i_line );
            break;
        case 16:
            bs_write( s, 8, 0x11 ); /* 4 bit/pixel code string */
            encode_pixel_line_4bp( s, p_region, i_line );
            break;
        case 256:
            bs_write( s, 8, 0x12 ); /* 8 bit/pixel code string */
            encode_pixel_line_8bp( s, p_region, i_line );
            break;
        default:
            msg_Err( p_enc, "subpicture palette (%i) not handled",
                     p_region->fmt.p_palette->i_entries );
            break;
        }
        bs_write( s, 8, 0xf0 ); /* End of object line code */
    }
}
