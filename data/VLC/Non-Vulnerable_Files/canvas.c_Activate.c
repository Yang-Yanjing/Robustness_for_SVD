 *****************************************************************************/
static int Activate( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    unsigned i_canvas_width; /* visible width of output canvas */
    unsigned i_canvas_height; /* visible height of output canvas */
    unsigned i_canvas_aspect; /* canvas PictureAspectRatio */
    es_format_t fmt; /* target format after up/down conversion */
    char psz_croppadd[100];
    int i_padd,i_offset;
    char *psz_aspect, *psz_parser;
    bool b_padd;
    unsigned i_fmt_in_aspect;
    if( !p_filter->b_allow_fmt_out_change )
    {
        msg_Err( p_filter, "Picture format change isn't allowed" );
        return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.i_chroma != p_filter->fmt_out.video.i_chroma )
    {
        msg_Err( p_filter, "Input and output chromas don't match" );
        return VLC_EGENERIC;
    }
    config_ChainParse( p_filter, CFG_PREFIX, ppsz_filter_options,
                       p_filter->p_cfg );
    i_canvas_width = var_CreateGetInteger( p_filter, CFG_PREFIX "width" );
    i_canvas_height = var_CreateGetInteger( p_filter, CFG_PREFIX "height" );
    if( i_canvas_width == 0 || i_canvas_height == 0 )
    {
        msg_Err( p_filter, "Width and height options must be set" );
        return VLC_EGENERIC;
    }
    if( i_canvas_width & 1 || i_canvas_height & 1 )
    {
        /* If this restriction were ever relaxed, it is very important to
         * get the field polatiry correct */
        msg_Err( p_filter, "Width and height options must be even integers" );
        return VLC_EGENERIC;
    }
    if( p_filter->fmt_in.video.i_sar_num )
        i_fmt_in_aspect = (int64_t)p_filter->fmt_in.video.i_sar_num *
                      p_filter->fmt_in.video.i_visible_width *
                      VOUT_ASPECT_FACTOR /
                      p_filter->fmt_in.video.i_sar_den /
                      p_filter->fmt_in.video.i_visible_height;
    else
        i_fmt_in_aspect = (int64_t)p_filter->fmt_in.video.i_visible_width *
                      VOUT_ASPECT_FACTOR /
                      p_filter->fmt_in.video.i_visible_height;
    psz_aspect = var_CreateGetNonEmptyString( p_filter, CFG_PREFIX "aspect" );
    if( psz_aspect )
    {
        psz_parser = strchr( psz_aspect, ':' );
        int numerator = atoi( psz_aspect );
        int denominator = psz_parser ? atoi( psz_parser+1 ) : 0;
        denominator = denominator == 0 ? 1 : denominator;
        i_canvas_aspect = numerator * VOUT_ASPECT_FACTOR / denominator;
        free( psz_aspect );
        if( numerator <= 0 || denominator < 0 )
        {
            msg_Err( p_filter, "Aspect ratio must be strictly positive" );
            return VLC_EGENERIC;
        }
    }
    else
    {
        /* if there is no user supplied aspect ratio, assume the canvas
         * has the same sample aspect ratio as the subpicture */
        /* aspect = subpic_sar * canvas_width / canvas_height
         *  where subpic_sar = subpic_ph * subpic_par / subpic_pw */
        i_canvas_aspect = (uint64_t) p_filter->fmt_in.video.i_visible_height
                        * i_fmt_in_aspect
                        * i_canvas_width
                        / (i_canvas_height * p_filter->fmt_in.video.i_visible_width);
    }
    b_padd = var_CreateGetBool( p_filter, CFG_PREFIX "padd" );
    filter_sys_t *p_sys = (filter_sys_t *)malloc( sizeof( filter_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_filter->p_sys = p_sys;
    p_sys->p_chain = filter_chain_New( p_filter, "video filter2", true,
                                       alloc_init, NULL, p_filter );
    if( !p_sys->p_chain )
    {
        msg_Err( p_filter, "Could not allocate filter chain" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    es_format_Copy( &fmt, &p_filter->fmt_in );
    /* one dimension will end up with one of the following: */
    fmt.video.i_visible_width = i_canvas_width;
    fmt.video.i_visible_height = i_canvas_height;
    if( b_padd )
    {
        /* Padd */
        if( i_canvas_aspect > i_fmt_in_aspect )
        {
            /* The canvas has a wider aspect than the subpicture:
             *  ie, pillarbox the [scaled] subpicture */
            /* The following is derived form:
             * width = upconverted_subpic_height * subpic_par / canvas_sar
             *  where canvas_sar = canvas_width / (canvas_height * canvas_par)
             * then simplify */
            fmt.video.i_visible_width = i_canvas_width
                              * i_fmt_in_aspect
                              / i_canvas_aspect;
            if( fmt.video.i_visible_width & 1 ) fmt.video.i_visible_width -= 1;
            i_padd = (i_canvas_width - fmt.video.i_visible_width) / 2;
            i_offset = (i_padd & 1);
            snprintf( psz_croppadd, 100, "croppadd{paddleft=%d,paddright=%d}",
                      i_padd - i_offset, i_padd + i_offset );
        }
        else
        {
            /* The canvas has a taller aspect than the subpicture:
             *  ie, letterbox the [scaled] subpicture */
            fmt.video.i_visible_height = i_canvas_height
                               * i_canvas_aspect
                               / i_fmt_in_aspect;
            if( fmt.video.i_visible_height & 1 ) fmt.video.i_visible_height -= 1;
            i_padd = (i_canvas_height - fmt.video.i_visible_height ) / 2;
            i_offset = (i_padd & 1);
            snprintf( psz_croppadd, 100, "croppadd{paddtop=%d,paddbottom=%d}",
                      i_padd - i_offset, i_padd + i_offset );
        }
    }
    else
    {
        /* Crop */
        if( i_canvas_aspect < i_fmt_in_aspect )
        {
            /* The canvas has a narrower aspect than the subpicture:
             *  ie, crop the [scaled] subpicture horizontally */
            fmt.video.i_visible_width = i_canvas_width
                              * i_fmt_in_aspect
                              / i_canvas_aspect;
            if( fmt.video.i_visible_width & 1 ) fmt.video.i_visible_width -= 1;
            i_padd = (fmt.video.i_visible_width - i_canvas_width) / 2;
            i_offset = (i_padd & 1);
            snprintf( psz_croppadd, 100, "croppadd{cropleft=%d,cropright=%d}",
                      i_padd - i_offset, i_padd + i_offset );
        }
        else
        {
            /* The canvas has a shorter aspect than the subpicture:
             *  ie, crop the [scaled] subpicture vertically */
            fmt.video.i_visible_height = i_canvas_height
                               * i_canvas_aspect
                               / i_fmt_in_aspect;
            if( fmt.video.i_visible_height & 1 ) fmt.video.i_visible_height -= 1;
            i_padd = (fmt.video.i_visible_height - i_canvas_height) / 2;
            i_offset = (i_padd & 1);
            snprintf( psz_croppadd, 100, "croppadd{croptop=%d,cropbottom=%d}",
                      i_padd - i_offset, i_padd + i_offset );
        }
    }
    /* xxx, should the clean area include the letter-boxing?
     *  probably not, as some codecs can make use of that information
     *  and it should be a scaled version of the input clean area
     *   -- davidf */
    fmt.video.i_width = p_filter->fmt_in.video.i_width * fmt.video.i_visible_width / p_filter->fmt_in.video.i_visible_width;
    fmt.video.i_height = p_filter->fmt_in.video.i_height * fmt.video.i_visible_height / p_filter->fmt_in.video.i_visible_height;
    filter_chain_Reset( p_sys->p_chain, &p_filter->fmt_in, &fmt );
    /* Append scaling module */
    filter_chain_AppendFilter( p_sys->p_chain, NULL, NULL, NULL, NULL );
    /* Append padding module */
    filter_chain_AppendFromString( p_sys->p_chain, psz_croppadd );
    fmt = *filter_chain_GetFmtOut( p_sys->p_chain );
    es_format_Copy( &p_filter->fmt_out, &fmt );
    vlc_ureduce( &p_filter->fmt_out.video.i_sar_num,
        &p_filter->fmt_out.video.i_sar_den,
        i_canvas_aspect    * p_filter->fmt_out.video.i_visible_height,
        VOUT_ASPECT_FACTOR * p_filter->fmt_out.video.i_visible_width,
        0);
    if( p_filter->fmt_out.video.i_visible_width != i_canvas_width
     || p_filter->fmt_out.video.i_visible_height != i_canvas_height )
    {
        msg_Warn( p_filter, "Looks like something went wrong. "
                  "Output size is %dx%d while we asked for %dx%d",
                  p_filter->fmt_out.video.i_visible_width,
                  p_filter->fmt_out.video.i_visible_height,
                  i_canvas_width, i_canvas_height );
    }
    p_filter->pf_video_filter = Filter;
    return VLC_SUCCESS;
}
