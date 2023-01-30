 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    video_splitter_t *p_splitter = (video_splitter_t*)p_this;
    video_splitter_sys_t *p_sys;
    const panoramix_chroma_t *p_chroma;
    for( int i = 0; ; i++ )
    {
        vlc_fourcc_t i_chroma = p_chroma_array[i].i_chroma;
        if( !i_chroma )
        {
            msg_Err( p_splitter, "colorspace not supported by plug-in !" );
            return VLC_EGENERIC;
        }
        if( i_chroma == p_splitter->fmt.i_chroma )
        {
            p_chroma = &p_chroma_array[i];
            break;
        }
    }
    /* Allocate structure */
    p_splitter->p_sys = p_sys = malloc( sizeof( *p_sys ) );
    if( !p_sys )
        return VLC_ENOMEM;
    /* */
    p_sys->p_chroma = p_chroma;
    /* */
    config_ChainParse( p_splitter, CFG_PREFIX, ppsz_filter_options,
                       p_splitter->p_cfg );
    /* */
    p_sys->i_col = var_InheritInteger( p_splitter, CFG_PREFIX "cols" );
    p_sys->i_row = var_InheritInteger( p_splitter, CFG_PREFIX "rows" );
    /* Autodetect number of displays */
    if( p_sys->i_col < 0 || p_sys->i_row < 0 )
    {
#ifdef _WIN32
        const int i_monitor_count = GetSystemMetrics(SM_CMONITORS);
        if( i_monitor_count > 1 )
        {
            p_sys->i_col = GetSystemMetrics( SM_CXVIRTUALSCREEN ) / GetSystemMetrics( SM_CXSCREEN );
            p_sys->i_row = GetSystemMetrics( SM_CYVIRTUALSCREEN ) / GetSystemMetrics( SM_CYSCREEN );
            if( p_sys->i_col * p_sys->i_row != i_monitor_count )
            {
                p_sys->i_col = i_monitor_count;
                p_sys->i_row = 1;
            }
        }
#else
        const unsigned i_monitors = CountMonitors( p_this );
        if( i_monitors > 1 ) /* Find closest to square */
            for( unsigned w = 1; (i_monitors / w) >= w ; w++ )
            {
                if( i_monitors % w )
                    continue;
                p_sys->i_row = w;
                p_sys->i_col = i_monitors / w;
            }
#endif
        /* By default do 2x1 */
        if( p_sys->i_row < 0 )
            p_sys->i_row = 1;
        if( p_sys->i_col < 0 )
            p_sys->i_col = 2;
        var_SetInteger( p_splitter, CFG_PREFIX "cols", p_sys->i_col);
        var_SetInteger( p_splitter, CFG_PREFIX "rows", p_sys->i_row);
    }
    /* */
    p_sys->b_attenuate = var_InheritBool( p_splitter, CFG_PREFIX "attenuate");
    p_sys->bz_length = var_InheritInteger( p_splitter, CFG_PREFIX "bz-length" );
    p_sys->bz_height = var_InheritInteger( p_splitter, CFG_PREFIX "bz-height" );
    p_sys->bz_begin = var_InheritInteger( p_splitter, CFG_PREFIX "bz-begin" );
    p_sys->bz_middle = var_InheritInteger( p_splitter, CFG_PREFIX "bz-middle" );
    p_sys->bz_end = var_InheritInteger( p_splitter, CFG_PREFIX "bz-end" );
    p_sys->bz_middle_pos = var_InheritInteger( p_splitter, CFG_PREFIX "bz-middle-pos" );
    double d_p = 100.0 / p_sys->bz_middle_pos;
    p_sys->a_2 = d_p * p_sys->bz_begin - (double)(d_p * d_p / (d_p - 1)) * p_sys->bz_middle + (double)(d_p / (d_p - 1)) * p_sys->bz_end;
    p_sys->a_1 = -(d_p + 1) * p_sys->bz_begin + (double)(d_p * d_p / (d_p - 1)) * p_sys->bz_middle - (double)(1 / (d_p - 1)) * p_sys->bz_end;
    p_sys->a_0 =  p_sys->bz_begin;
    /* */
    p_sys->i_col = VLC_CLIP( p_sys->i_col, 1, COL_MAX );
    p_sys->i_row = VLC_CLIP( p_sys->i_row, 1, ROW_MAX );
    msg_Dbg( p_splitter, "opening a %i x %i wall",
             p_sys->i_col, p_sys->i_row );
    if( p_sys->bz_length > 0 && ( p_sys->i_row > 1 || p_sys->i_col > 1 ) )
    {
        const int i_overlap_w2_max = p_splitter->fmt.i_width  / p_sys->i_col / 2;
        const int i_overlap_h2_max = p_splitter->fmt.i_height / p_sys->i_row / 2;
        const int i_overlap2_max = __MIN( i_overlap_w2_max, i_overlap_h2_max );
        if( p_sys->i_col > 1 )
            p_sys->i_overlap_w2 = i_overlap2_max * p_sys->bz_length / 100;
        else
            p_sys->i_overlap_w2 = 0;
        if( p_sys->i_row > 1 )
            p_sys->i_overlap_h2 = i_overlap2_max * p_sys->bz_height / 100;
        else
            p_sys->i_overlap_h2 = 0;
        /* */
        int i_div_max_w = 1;
        int i_div_max_h = 1;
        for( int i = 0; i < VOUT_MAX_PLANES; i++ )
        {
            i_div_max_w = __MAX( i_div_max_w, p_chroma->pi_div_w[i] );
            i_div_max_h = __MAX( i_div_max_h, p_chroma->pi_div_h[i] );
        }
        p_sys->i_overlap_w2 = i_div_max_w * (p_sys->i_overlap_w2 / i_div_max_w);
        p_sys->i_overlap_h2 = i_div_max_h * (p_sys->i_overlap_h2 / i_div_max_h);
    }
    else
    {
        p_sys->i_overlap_w2 = 0;
        p_sys->i_overlap_h2 = 0;
    }
    /* Compute attenuate parameters */
    if( p_sys->b_attenuate )
    {
        panoramix_gamma_t p_gamma[VOUT_MAX_PLANES];
        p_gamma[0].f_gamma = var_InheritFloat( p_splitter, CFG_PREFIX "bz-gamma-red" );
        p_gamma[1].f_gamma = var_InheritFloat( p_splitter, CFG_PREFIX "bz-gamma-green" );
        p_gamma[2].f_gamma = var_InheritFloat( p_splitter, CFG_PREFIX "bz-gamma-blue" );
        p_gamma[0].f_black_crush = var_InheritInteger( p_splitter, CFG_PREFIX "bz-blackcrush-red" ) / 255.0;
        p_gamma[1].f_black_crush = var_InheritInteger( p_splitter, CFG_PREFIX "bz-blackcrush-green" ) / 255.0;
        p_gamma[2].f_black_crush = var_InheritInteger( p_splitter, CFG_PREFIX "bz-blackcrush-blue" ) / 255.0;
        p_gamma[0].f_white_crush = var_InheritInteger( p_splitter, CFG_PREFIX "bz-whitecrush-red" ) / 255.0;
        p_gamma[1].f_white_crush = var_InheritInteger( p_splitter, CFG_PREFIX "bz-whitecrush-green" ) / 255.0;
        p_gamma[2].f_white_crush = var_InheritInteger( p_splitter, CFG_PREFIX "bz-whitecrush-blue" ) / 255.0;
        p_gamma[0].f_black_level = var_InheritInteger( p_splitter, CFG_PREFIX "bz-blacklevel-red" ) / 255.0;
        p_gamma[1].f_black_level = var_InheritInteger( p_splitter, CFG_PREFIX "bz-blacklevel-green" ) / 255.0;
        p_gamma[2].f_black_level = var_InheritInteger( p_splitter, CFG_PREFIX "bz-blacklevel-blue" ) / 255.0;
        p_gamma[0].f_white_level = var_InheritInteger( p_splitter, CFG_PREFIX "bz-whitelevel-red" ) / 255.0;
        p_gamma[1].f_white_level = var_InheritInteger( p_splitter, CFG_PREFIX "bz-whitelevel-green" ) / 255.0;
        p_gamma[2].f_white_level = var_InheritInteger( p_splitter, CFG_PREFIX "bz-whitelevel-blue" ) / 255.0;
        for( int i = 3; i < VOUT_MAX_PLANES; i++ )
        {
            /* Initialize unsupported planes */
            p_gamma[i].f_gamma = 1.0;
            p_gamma[i].f_black_crush = 140.0/255.0;
            p_gamma[i].f_white_crush = 200.0/255.0;
            p_gamma[i].f_black_level = 150.0/255.0;
            p_gamma[i].f_white_level = 0.0/255.0;
        }
        if( p_chroma->i_chroma == VLC_CODEC_YV12 )
        {
            /* Exchange U and V */
            panoramix_gamma_t t = p_gamma[1];
            p_gamma[1] = p_gamma[2];
            p_gamma[2] = t;
        }
        for( int i_index = 0; i_index < 256; i_index++ )
        {
            for( int i_index2 = 0; i_index2 <= ACCURACY; i_index2++ )
            {
                for( int i_plane = 0; i_plane < VOUT_MAX_PLANES; i_plane++ )
                {
                    double f_factor = GammaFactor( &p_gamma[i_plane], (float)i_index / 255.0 );
                    float f_lut = clip_unit( 1.0 - ((ACCURACY - (float)i_index2) * f_factor / (ACCURACY - 1)) );
                    p_sys->p_lut[i_plane][i_index2][i_index] = f_lut * i_index + (int)( (1.0 - f_lut) * (float)p_chroma->pi_black[i_plane] );
                }
            }
        }
        for( int i_plane = 0; i_plane < VOUT_MAX_PLANES; i_plane++ )
        {
            if( !p_chroma->pi_div_w[i_plane] || !p_chroma->pi_div_h[i_plane] )
                continue;
            const int i_length_w = (2 * p_sys->i_overlap_w2) / p_chroma->pi_div_w[i_plane];
            const int i_length_h = (2 * p_sys->i_overlap_h2) / p_chroma->pi_div_h[i_plane];
            for( int i_dir = 0; i_dir < 2; i_dir++ )
            {
                const int i_length = i_dir == 0 ? i_length_w : i_length_h;
                const int i_den = i_length * i_length;
                const int a_2 = p_sys->a_2 * (ACCURACY / 100);
                const int a_1 = p_sys->a_1 * i_length * (ACCURACY / 100);
                const int a_0 = p_sys->a_0 * i_den * (ACCURACY / 100);
                for( int i_position = 0; i_position < 2; i_position++ )
                {
                    for( int i_index = 0; i_index < i_length; i_index++ )
                    {
                        const int v = i_position == 1 ? i_index : (i_length - i_index);
                        const int i_lambda = clip_accuracy( ACCURACY - (a_2 * v*v + a_1 * v + a_0) / i_den );
                        if( i_dir == 0 )
                            p_sys->lambdav[i_plane][i_position][i_index] = i_lambda;
                        else
                            p_sys->lambdah[i_plane][i_position][i_index] = i_lambda;
                    }
                }
            }
        }
    }
    /* */
    char *psz_state = var_InheritString( p_splitter, CFG_PREFIX "active" );
    /* */
    bool pb_active[COL_MAX*ROW_MAX];
    for( int i = 0; i < COL_MAX*ROW_MAX; i++ )
        pb_active[i] = psz_state == NULL;
    /* Parse active list if provided */
    char *psz_tmp = psz_state;
    while( psz_tmp && *psz_tmp )
    {
        char *psz_next = strchr( psz_tmp, ',' );
        if( psz_next )
            *psz_next++ = '\0';
        const int i_index = atoi( psz_tmp );
        if( i_index >= 0 && i_index < COL_MAX*ROW_MAX )
            pb_active[i_index] = true;
        psz_tmp = psz_next;
    }
    free( psz_state );
    /* */
    p_splitter->i_output =
        Configuration( p_sys->pp_output,
                       p_sys->i_col, p_sys->i_row,
                       p_splitter->fmt.i_width, p_splitter->fmt.i_height,
                       p_sys->i_overlap_w2, p_sys->i_overlap_h2,
                       p_sys->b_attenuate,
                       pb_active );
    p_splitter->p_output = calloc( p_splitter->i_output,
                                   sizeof(*p_splitter->p_output) );
    if( !p_splitter->p_output )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    for( int y = 0; y < p_sys->i_row; y++ )
    {
        for( int x = 0; x < p_sys->i_col; x++ )
        {
            panoramix_output_t *p_output = &p_sys->pp_output[x][y];
            if( !p_output->b_active )
                continue;
            video_splitter_output_t *p_cfg = &p_splitter->p_output[p_output->i_output];
            /* */
            video_format_Copy( &p_cfg->fmt, &p_splitter->fmt );
            p_cfg->fmt.i_visible_width  =
            p_cfg->fmt.i_width          = p_output->i_width;
            p_cfg->fmt.i_visible_height =
            p_cfg->fmt.i_height         = p_output->i_height;
            p_cfg->window.i_x     = p_output->i_x;
            p_cfg->window.i_y     = p_output->i_y;
            p_cfg->window.i_align = p_output->i_align;
            p_cfg->psz_module = NULL;
        }
    }
    /* */
    p_splitter->pf_filter = Filter;
    p_splitter->pf_mouse  = Mouse;
    return VLC_SUCCESS;
}
