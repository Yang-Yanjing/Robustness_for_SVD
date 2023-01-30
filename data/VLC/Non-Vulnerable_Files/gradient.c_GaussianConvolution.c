 *****************************************************************************/
static void GaussianConvolution( picture_t *p_inpic, uint32_t *p_smooth )
{
    const uint8_t *p_inpix = p_inpic->p[Y_PLANE].p_pixels;
    const int i_src_pitch = p_inpic->p[Y_PLANE].i_pitch;
    const int i_src_visible = p_inpic->p[Y_PLANE].i_visible_pitch;
    const int i_num_lines = p_inpic->p[Y_PLANE].i_visible_lines;
    int x,y;
    for( y = 2; y < i_num_lines - 2; y++ )
    {
        for( x = 2; x < i_src_visible - 2; x++ )
        {
            p_smooth[y*i_src_visible+x] = (uint32_t)(
              /* 2 rows up */
                ( p_inpix[(y-2)*i_src_pitch+x-2] )
              + ((p_inpix[(y-2)*i_src_pitch+x-1]
              +   p_inpix[(y-2)*i_src_pitch+x]
              +   p_inpix[(y-2)*i_src_pitch+x+1])<<1 )
              + ( p_inpix[(y-2)*i_src_pitch+x+2] )
              /* 1 row up */
              + ((p_inpix[(y-1)*i_src_pitch+x-2]
              + ( p_inpix[(y-1)*i_src_pitch+x-1]<<1 )
              + ( p_inpix[(y-1)*i_src_pitch+x]*3 )
              + ( p_inpix[(y-1)*i_src_pitch+x+1]<<1 )
              +   p_inpix[(y-1)*i_src_pitch+x+2]
              /* */
              +   p_inpix[y*i_src_pitch+x-2]
              + ( p_inpix[y*i_src_pitch+x-1]*3 )
              + ( p_inpix[y*i_src_pitch+x]<<2 )
              + ( p_inpix[y*i_src_pitch+x+1]*3 )
              +   p_inpix[y*i_src_pitch+x+2]
              /* 1 row down */
              +   p_inpix[(y+1)*i_src_pitch+x-2]
              + ( p_inpix[(y+1)*i_src_pitch+x-1]<<1 )
              + ( p_inpix[(y+1)*i_src_pitch+x]*3 )
              + ( p_inpix[(y+1)*i_src_pitch+x+1]<<1 )
              +   p_inpix[(y+1)*i_src_pitch+x+2] )<<1 )
              /* 2 rows down */
              + ( p_inpix[(y+2)*i_src_pitch+x-2] )
              + ((p_inpix[(y+2)*i_src_pitch+x-1]
              +   p_inpix[(y+2)*i_src_pitch+x]
              +   p_inpix[(y+2)*i_src_pitch+x+1])<<1 )
              + ( p_inpix[(y+2)*i_src_pitch+x+2] )
              ) >> 6 /* 115 */;
        }
    }
}
