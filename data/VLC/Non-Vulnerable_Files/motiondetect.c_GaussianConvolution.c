 *****************************************************************************/
static void GaussianConvolution( uint32_t *p_inpix, uint32_t *p_smooth,
                                 int i_src_pitch, int i_num_lines,
                                 int i_src_visible )
{
    int x,y;
    /* A bit overkill but ... simpler */
    memset( p_smooth, 0, sizeof(*p_smooth) * i_src_pitch * i_num_lines );
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
