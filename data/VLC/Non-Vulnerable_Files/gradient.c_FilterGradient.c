 *****************************************************************************/
static void FilterGradient( filter_t *p_filter, picture_t *p_inpic,
                                                picture_t *p_outpic )
{
    int x, y;
    const int i_src_pitch = p_inpic->p[Y_PLANE].i_pitch;
    const int i_src_visible = p_inpic->p[Y_PLANE].i_visible_pitch;
    const int i_dst_pitch = p_outpic->p[Y_PLANE].i_pitch;
    const int i_num_lines = p_inpic->p[Y_PLANE].i_visible_lines;
    const uint8_t *p_inpix = p_inpic->p[Y_PLANE].p_pixels;
    uint8_t *p_outpix = p_outpic->p[Y_PLANE].p_pixels;
    uint32_t *p_smooth;
    if( !p_filter->p_sys->p_buf32 )
        p_filter->p_sys->p_buf32 =
        (uint32_t *)malloc( i_num_lines * i_src_visible * sizeof(uint32_t));
    p_smooth = p_filter->p_sys->p_buf32;
    if( !p_smooth ) return;
    if( p_filter->p_sys->b_cartoon )
    {
        plane_CopyPixels( &p_outpic->p[U_PLANE], &p_inpic->p[U_PLANE] );
        plane_CopyPixels( &p_outpic->p[V_PLANE], &p_inpic->p[V_PLANE] );
    }
    else
    {
        memset( p_outpic->p[U_PLANE].p_pixels, 0x80,
            p_outpic->p[U_PLANE].i_lines * p_outpic->p[U_PLANE].i_pitch );
        memset( p_outpic->p[V_PLANE].p_pixels, 0x80,
            p_outpic->p[V_PLANE].i_lines * p_outpic->p[V_PLANE].i_pitch );
    }
    GaussianConvolution( p_inpic, p_smooth );
    /* Sobel gradient
     | -1 0 1 |     |  1  2  1 |
     | -2 0 2 | and |  0  0  0 |
     | -1 0 1 |     | -1 -2 -1 | */
#define FOR                                                     \
    for( y = 1; y < i_num_lines - 1; y++ )                      \
    {                                                           \
        for( x = 1; x < i_src_visible - 1; x++ )                \
        {                                                       \
            const uint32_t a =                                  \
            (                                                   \
              abs(                                              \
                 ( p_smooth[(y-1)*i_src_visible+x-1]            \
                   - p_smooth[(y+1)*i_src_visible+x-1] )        \
               + ( ( p_smooth[(y-1)*i_src_visible+x]            \
                    - p_smooth[(y+1)*i_src_visible+x] ) <<1 )   \
               + ( p_smooth[(y-1)*i_src_visible+x+1]            \
                   - p_smooth[(y+1)*i_src_visible+x+1] )        \
              )                                                 \
            +                                                   \
              abs(                                              \
                 ( p_smooth[(y-1)*i_src_visible+x-1]            \
                   - p_smooth[(y-1)*i_src_visible+x+1] )        \
               + ( ( p_smooth[y*i_src_visible+x-1]              \
                    - p_smooth[y*i_src_visible+x+1] ) <<1 )     \
               + ( p_smooth[(y+1)*i_src_visible+x-1]            \
                   - p_smooth[(y+1)*i_src_visible+x+1] )        \
              )                                                 \
            );
    if( p_filter->p_sys->i_gradient_type )
    {
        if( p_filter->p_sys->b_cartoon )
        {
            FOR
            if( a > 60 )
            {
                p_outpix[y*i_dst_pitch+x] = 0x00;
            }
            else
            {
                if( p_smooth[y*i_src_visible+x] > 0xa0 )
                    p_outpix[y*i_dst_pitch+x] =
                        0xff - ((0xff - p_inpix[y*i_src_pitch+x] )>>2);
                else if( p_smooth[y*i_src_visible+x] > 0x70 )
                    p_outpix[y*i_dst_pitch+x] =
                        0xa0 - ((0xa0 - p_inpix[y*i_src_pitch+x] )>>2);
                else if( p_smooth[y*i_src_visible+x] > 0x28 )
                    p_outpix[y*i_dst_pitch+x] =
                        0x70 - ((0x70 - p_inpix[y*i_src_pitch+x] )>>2);
                else
                    p_outpix[y*i_dst_pitch+x] =
                        0x28 - ((0x28 - p_inpix[y*i_src_pitch+x] )>>2);
            }
            }}
        }
        else
        {
            FOR
            p_outpix[y*i_dst_pitch+x] = clip_uint8_vlc( a );
            }}
        }
    }
    else
    {
        FOR
        if( a>>8 )
            p_outpix[y*i_dst_pitch+x] = 0;
        else
            p_outpix[y*i_dst_pitch+x] = 0xff-(uint8_t)a;
        }}
    }
