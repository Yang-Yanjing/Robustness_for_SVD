}
static rectangle_t r_img( const ASS_Image *p_img )
{
    return r_create( p_img->dst_x, p_img->dst_y, p_img->dst_x+p_img->w, p_img->dst_y+p_img->h );
}
