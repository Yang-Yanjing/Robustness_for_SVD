 */
static int RenderCommon( filter_t *p_filter, subpicture_region_t *p_region_out,
                         subpicture_region_t *p_region_in, bool b_html,
                         const vlc_fourcc_t *p_chroma_list )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if( !p_region_in )
        return VLC_EGENERIC;
    if( b_html && !p_region_in->psz_html )
        return VLC_EGENERIC;
    if( !b_html && !p_region_in->psz_text )
        return VLC_EGENERIC;
    const size_t i_text_max = strlen( b_html ? p_region_in->psz_html
                                             : p_region_in->psz_text );
    uni_char_t *psz_text = calloc( i_text_max, sizeof( *psz_text ) );
    text_style_t **pp_styles = calloc( i_text_max, sizeof( *pp_styles ) );
    if( !psz_text || !pp_styles )
    {
        free( psz_text );
        free( pp_styles );
        return VLC_EGENERIC;
    }
    /* Reset the default fontsize in case screen metrics have changed */
    p_filter->p_sys->style.i_font_size = GetFontSize( p_filter );
    /* */
    int rv = VLC_SUCCESS;
    int i_text_length = 0;
    FT_BBox bbox;
    int i_max_face_height;
    line_desc_t *p_lines = NULL;
    uint32_t *pi_k_durations   = NULL;
    if( b_html )
    {
        stream_t *p_sub = stream_MemoryNew( VLC_OBJECT(p_filter),
                                            (uint8_t *) p_region_in->psz_html,
                                            strlen( p_region_in->psz_html ),
                                            true );
        if( unlikely(p_sub == NULL) )
        {
            free( psz_text );
            free( pp_styles );
            return VLC_SUCCESS;
        }
        xml_reader_t *p_xml_reader = GetXMLReader( p_filter, p_sub );
        if( !p_xml_reader )
            rv = VLC_EGENERIC;
        if( !rv )
        {
            /* Look for Root Node */
            const char *node;
            if( xml_ReaderNextNode( p_xml_reader, &node ) == XML_READER_STARTELEM )
            {
                if( strcasecmp( "karaoke", node ) == 0 )
                {
                    pi_k_durations = calloc( i_text_max, sizeof( *pi_k_durations ) );
                }
                else if( strcasecmp( "text", node ) != 0 )
                {
                    /* Only text and karaoke tags are supported */
                    msg_Dbg( p_filter, "Unsupported top-level tag <%s> ignored.",
                             node );
                    rv = VLC_EGENERIC;
                }
            }
            else
            {
                msg_Err( p_filter, "Malformed HTML subtitle" );
                rv = VLC_EGENERIC;
            }
        }
        if( !rv )
        {
            rv = ProcessNodes( p_filter,
                               psz_text, pp_styles, pi_k_durations, &i_text_length,
                               p_xml_reader, p_region_in->p_style, &p_filter->p_sys->style );
        }
        if( p_xml_reader )
            p_filter->p_sys->p_xml = xml_ReaderReset( p_xml_reader, NULL );
        stream_Delete( p_sub );
    }
    else
    {
        text_style_t *p_style;
        if( p_region_in->p_style )
            p_style = CreateStyle( p_region_in->p_style->psz_fontname ? p_region_in->p_style->psz_fontname
                                                                      : p_sys->style.psz_fontname,
                                   p_region_in->p_style->i_font_size > 0 ? p_region_in->p_style->i_font_size
                                                                         : p_sys->style.i_font_size,
                                   (p_region_in->p_style->i_font_color & 0xffffff) |
                                   ((p_region_in->p_style->i_font_alpha & 0xff) << 24),
                                   0x00ffffff,
                                   p_region_in->p_style->i_style_flags & (STYLE_BOLD |
                                                                          STYLE_ITALIC |
                                                                          STYLE_UNDERLINE |
                                                                          STYLE_STRIKEOUT) );
        else
        {
            uint32_t i_font_color = var_InheritInteger( p_filter, "freetype-color" );
            i_font_color = VLC_CLIP( i_font_color, 0, 0xFFFFFF );
            p_style = CreateStyle( p_sys->style.psz_fontname,
                                   p_sys->style.i_font_size,
                                   (i_font_color & 0xffffff) |
                                   ((p_sys->style.i_font_alpha & 0xff) << 24),
                                   0x00ffffff, 0);
        }
        if( p_sys->style.i_style_flags & STYLE_BOLD )
            p_style->i_style_flags |= STYLE_BOLD;
        i_text_length = SetupText( p_filter,
                                   psz_text,
                                   pp_styles,
                                   NULL,
                                   p_region_in->psz_text, p_style, 0 );
    }
    if( !rv && i_text_length > 0 )
    {
        rv = ProcessLines( p_filter,
                           &p_lines, &bbox, &i_max_face_height,
                           psz_text, pp_styles, pi_k_durations, i_text_length );
    }
    p_region_out->i_x = p_region_in->i_x;
    p_region_out->i_y = p_region_in->i_y;
    /* Don't attempt to render text that couldn't be layed out
     * properly. */
    if( !rv && i_text_length > 0 && bbox.xMin < bbox.xMax && bbox.yMin < bbox.yMax )
    {
        const vlc_fourcc_t p_chroma_list_yuvp[] = { VLC_CODEC_YUVP, 0 };
        const vlc_fourcc_t p_chroma_list_rgba[] = { VLC_CODEC_RGBA, 0 };
        if( var_InheritBool( p_filter, "freetype-yuvp" ) )
            p_chroma_list = p_chroma_list_yuvp;
        else if( !p_chroma_list || *p_chroma_list == 0 )
            p_chroma_list = p_chroma_list_rgba;
        uint8_t i_background_opacity = var_InheritInteger( p_filter, "freetype-background-opacity" );
        i_background_opacity = VLC_CLIP( i_background_opacity, 0, 255 );
        const int i_margin = i_background_opacity > 0 ? i_max_face_height / 4 : 0;
        for( const vlc_fourcc_t *p_chroma = p_chroma_list; *p_chroma != 0; p_chroma++ )
        {
            rv = VLC_EGENERIC;
            if( *p_chroma == VLC_CODEC_YUVP )
                rv = RenderYUVP( p_filter, p_region_out, p_lines, &bbox );
            else if( *p_chroma == VLC_CODEC_YUVA )
                rv = RenderAXYZ( p_filter, p_region_out, p_lines, &bbox, i_margin,
                                 VLC_CODEC_YUVA,
                                 YUVFromRGB,
                                 FillYUVAPicture,
                                 BlendYUVAPixel );
            else if( *p_chroma == VLC_CODEC_RGBA )
                rv = RenderAXYZ( p_filter, p_region_out, p_lines, &bbox, i_margin,
                                 VLC_CODEC_RGBA,
                                 RGBFromRGB,
                                 FillRGBAPicture,
                                 BlendRGBAPixel );
            else if( *p_chroma == VLC_CODEC_ARGB )
                rv = RenderAXYZ( p_filter, p_region_out, p_lines, &bbox,
                                 i_margin, *p_chroma, RGBFromRGB,
                                 FillARGBPicture, BlendARGBPixel );
            if( !rv )
                break;
        }
        /* With karaoke, we're going to have to render the text a number
         * of times to show the progress marker on the text.
         */
        if( pi_k_durations )
            var_SetBool( p_filter, "text-rerender", true );
    }
    FreeLines( p_lines );
    free( psz_text );
    for( int i = 0; i < i_text_length; i++ )
    {
        if( pp_styles[i] && ( i + 1 == i_text_length || pp_styles[i] != pp_styles[i + 1] ) )
            text_style_Delete( pp_styles[i] );
    }
    free( pp_styles );
    free( pi_k_durations );
    return rv;
}
