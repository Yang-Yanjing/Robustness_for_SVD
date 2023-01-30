 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    unsigned int w, h;
    int x,y;
    uint8_t u,v;
    picture_t *p_converted;
    video_format_t fmt_out;
    memset( &fmt_out, 0, sizeof(video_format_t) );
    fmt_out.p_palette = NULL;
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    if( !p_filter->p_sys->p_image )
        p_filter->p_sys->p_image = image_HandlerCreate( p_filter );
    /* chrominance */
    u = p_filter->p_sys->u;
    v = p_filter->p_sys->v;
    for( y = 0; y<p_outpic->p[U_PLANE].i_lines; y++)
    {
        memset(
                p_outpic->p[U_PLANE].p_pixels+y*p_outpic->p[U_PLANE].i_pitch,
                u, p_outpic->p[U_PLANE].i_pitch );
        memset(
                p_outpic->p[V_PLANE].p_pixels+y*p_outpic->p[V_PLANE].i_pitch,
                v, p_outpic->p[V_PLANE].i_pitch );
        if( v == 0 && u != 0 )
            u --;
        else if( u == 0xff )
            v --;
        else if( v == 0xff )
            u ++;
        else if( u == 0 )
            v ++;
    }
    /* luminance */
    plane_CopyPixels( &p_outpic->p[Y_PLANE], &p_pic->p[Y_PLANE] );
    /* image visualization */
    fmt_out = p_filter->fmt_out.video;
    fmt_out.i_width = p_filter->fmt_out.video.i_width*p_filter->p_sys->scale/150;
    fmt_out.i_height = p_filter->fmt_out.video.i_height*p_filter->p_sys->scale/150;
    fmt_out.i_visible_width = fmt_out.i_width;
    fmt_out.i_visible_height = fmt_out.i_height;
    p_converted = image_Convert( p_filter->p_sys->p_image, p_pic,
                                 &(p_pic->format), &fmt_out );
    if( p_converted )
    {
#define copyimage( plane, b ) \
        for( y=0; y<p_converted->p[plane].i_visible_lines; y++) { \
        for( x=0; x<p_converted->p[plane].i_visible_pitch; x++) { \
            int nx, ny; \
            if( p_filter->p_sys->yinc == 1 ) \
                ny= y; \
            else \
                ny = p_converted->p[plane].i_visible_lines-y; \
            if( p_filter->p_sys->xinc == 1 ) \
                nx = x; \
            else \
                nx = p_converted->p[plane].i_visible_pitch-x; \
            p_outpic->p[plane].p_pixels[(p_filter->p_sys->x*b+nx)+(ny+p_filter->p_sys->y*b)*p_outpic->p[plane].i_pitch ] = p_converted->p[plane].p_pixels[y*p_converted->p[plane].i_pitch+x]; \
        } }
        copyimage( Y_PLANE, 2 );
        copyimage( U_PLANE, 1 );
        copyimage( V_PLANE, 1 );
#undef copyimage
        picture_Release( p_converted );
    }
    else
    {
        msg_Err( p_filter, "Image scaling failed miserably." );
    }
    p_filter->p_sys->x += p_filter->p_sys->xinc;
    p_filter->p_sys->y += p_filter->p_sys->yinc;
    p_filter->p_sys->scale += p_filter->p_sys->scaleinc;
    if( p_filter->p_sys->scale >= 50 ) p_filter->p_sys->scaleinc = -1;
    if( p_filter->p_sys->scale <= 1 ) p_filter->p_sys->scaleinc = 1;
    w = p_filter->fmt_out.video.i_width*p_filter->p_sys->scale/150;
    h = p_filter->fmt_out.video.i_height*p_filter->p_sys->scale/150;
    if( p_filter->p_sys->x*2 + w >= p_filter->fmt_out.video.i_width )
        p_filter->p_sys->xinc = -1;
    if( p_filter->p_sys->x <= 0 )
        p_filter->p_sys->xinc = 1;
    if( p_filter->p_sys->x*2 + w >= p_filter->fmt_out.video.i_width )
        p_filter->p_sys->x = (p_filter->fmt_out.video.i_width-w)/2;
    if( p_filter->p_sys->y*2 + h >= p_filter->fmt_out.video.i_height )
        p_filter->p_sys->y = (p_filter->fmt_out.video.i_height-h)/2;
    if( p_filter->p_sys->y*2 + h >= p_filter->fmt_out.video.i_height )
        p_filter->p_sys->yinc = -1;
    if( p_filter->p_sys->y <= 0 )
        p_filter->p_sys->yinc = 1;
    for( y = 0; y< 16; y++ )
    {
        if( p_filter->p_sys->v == 0 && p_filter->p_sys->u != 0 )
            p_filter->p_sys->u -= 1;
        else if( p_filter->p_sys->u == 0xff )
            p_filter->p_sys->v -= 1;
        else if( p_filter->p_sys->v == 0xff )
            p_filter->p_sys->u += 1;
        else if( p_filter->p_sys->u == 0 )
            p_filter->p_sys->v += 1;
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
