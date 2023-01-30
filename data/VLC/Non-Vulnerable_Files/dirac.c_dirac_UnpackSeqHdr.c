/* read in useful bits from sequence header */
static bool dirac_UnpackSeqHdr( struct seq_hdr_t *p_sh, block_t *p_block )
{
    bs_t bs;
    bs_init( &bs, p_block->p_buffer, p_block->i_buffer );
    bs_skip( &bs, 13*8 ); /* parse_info_header */
    dirac_uint( &bs ); /* major_version */
    dirac_uint( &bs ); /* minor_version */
    dirac_uint( &bs ); /* profile */
    dirac_uint( &bs ); /* level */
    uint32_t u_video_format = dirac_uint( &bs ); /* index */
    if( u_video_format > 20 )
    {
        /* don't know how to parse this header */
        return false;
    }
    static const struct {
        uint32_t u_w, u_h;
    } dirac_size_tbl[] = {
        {640,480}, {176,120}, {176,144}, {352,240}, {352,288}, {704,480},
        {704,576}, {720,480}, {720,576}, {1280,720}, {1280,720}, {1920,1080},
        {1920,1080}, {1920,1080}, {1920,1080}, {2048,1080}, {4096,2160},
        {3840,2160}, {3840,2160}, {7680,4320}, {7680,4320},
    };
    p_sh->u_width = dirac_size_tbl[u_video_format].u_w;
    p_sh->u_height = dirac_size_tbl[u_video_format].u_h;
    if( dirac_bool( &bs ) )
    {
        p_sh->u_width = dirac_uint( &bs ); /* frame_width */
        p_sh->u_height = dirac_uint( &bs ); /* frame_height */
    }
    if( dirac_bool( &bs ) )
    {
        dirac_uint( &bs ); /* chroma_format */
    }
    if( dirac_bool( &bs ) )
    {
        dirac_uint( &bs ); /* scan_format */
    }
    static const struct {
        uint32_t u_n /* numerator */, u_d /* denominator */;
    } dirac_frate_tbl[] = { /* table 10.3 */
        {1, 1}, /* this value is not used */
        {24000,1001}, {24,1}, {25,1}, {30000,1001}, {30,1},
        {50,1}, {60000,1001}, {60,1}, {15000,1001}, {25,2},
    };
    const unsigned dirac_frate_tbl_size =
        sizeof( dirac_frate_tbl ) / sizeof( *dirac_frate_tbl );
    static const uint32_t dirac_vidfmt_frate[] = { /* table C.1 */
        1, 9, 10, 9, 10, 9, 10, 4, 3, 7, 6, 4, 3, 7, 6, 2, 2, 7, 6, 7, 6,
    };
    p_sh->u_fps_num = dirac_frate_tbl[dirac_vidfmt_frate[u_video_format]].u_n;
    p_sh->u_fps_den = dirac_frate_tbl[dirac_vidfmt_frate[u_video_format]].u_d;
    if( dirac_bool( &bs ) )
    {
        uint32_t frame_rate_index = dirac_uint( &bs );
        if( frame_rate_index >= dirac_frate_tbl_size )
        {
            /* invalid header */
            return false;
        }
        p_sh->u_fps_num = dirac_frate_tbl[frame_rate_index].u_n;
        p_sh->u_fps_den = dirac_frate_tbl[frame_rate_index].u_d;
        if( frame_rate_index == 0 )
        {
            p_sh->u_fps_num = dirac_uint( &bs ); /* frame_rate_numerator */
            p_sh->u_fps_den = dirac_uint( &bs ); /* frame_rate_denominator */
        }
    }
    /* must have a valid framerate */
    if( !p_sh->u_fps_num || !p_sh->u_fps_den )
        return false;
    if( dirac_bool( &bs ) )
    {
        uint32_t par_index = dirac_uint( &bs );
        if( !par_index )
        {
            dirac_uint( &bs ); /* par_num */
            dirac_uint( &bs ); /* par_den */
        }
    }
    if( dirac_bool( &bs ) )
    {
        dirac_uint( &bs ); /* clean_width */
        dirac_uint( &bs ); /* clean_height */
        dirac_uint( &bs ); /* clean_left_offset */
        dirac_uint( &bs ); /* clean_top_offset */
    }
    if( dirac_bool( &bs ) )
    {
        uint32_t signal_range_index = dirac_uint( &bs );
        if( !signal_range_index )
        {
            dirac_uint( &bs ); /* luma_offset */
            dirac_uint( &bs ); /* luma_excursion */
            dirac_uint( &bs ); /* chroma_offset */
            dirac_uint( &bs ); /* chroma_excursion */
        }
    }
    if( dirac_bool( &bs ) )
    {
        uint32_t colour_spec_index = dirac_uint( &bs );
        if( !colour_spec_index )
        {
            if( dirac_bool( &bs ) )
            {
                dirac_uint( &bs ); /* colour_primaries_index */
            }
            if( dirac_bool( &bs ) )
            {
                dirac_uint( &bs ); /* colour_matrix_index */
            }
            if( dirac_bool( &bs ) )
            {
                dirac_uint( &bs ); /* transfer_function_index */
            }
        }
    }
    p_sh->u_picture_coding_mode = dirac_uint( &bs );
    return true;
}
