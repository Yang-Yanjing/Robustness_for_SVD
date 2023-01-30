 */
static void PostprocessTigerImage( plane_t *p_plane, unsigned int i_width )
{
    PROFILE_START( tiger_renderer_postprocess );
    int y;
    for( y=0; y<p_plane->i_lines; ++y )
    {
        uint8_t *p_line = (uint8_t*)(p_plane->p_pixels + y*p_plane->i_pitch);
        unsigned int x;
        for( x=0; x<i_width; ++x )
        {
            uint8_t *p_pixel = p_line+x*4;
#ifdef WORDS_BIGENDIAN
            uint8_t a = p_pixel[0];
#else
            uint8_t a = p_pixel[3];
#endif
            if( a )
            {
#ifdef WORDS_BIGENDIAN
                uint8_t tmp = p_pixel[2];
                p_pixel[0] = clip_uint8_vlc((p_pixel[3] * 255 + a / 2) / a);
                p_pixel[3] = a;
                p_pixel[2] = clip_uint8_vlc((p_pixel[1] * 255 + a / 2) / a);
                p_pixel[1] = clip_uint8_vlc((tmp * 255 + a / 2) / a);
#else
                p_pixel[0] = clip_uint8_vlc((p_pixel[0] * 255 + a / 2) / a);
                p_pixel[1] = clip_uint8_vlc((p_pixel[1] * 255 + a / 2) / a);
                p_pixel[2] = clip_uint8_vlc((p_pixel[2] * 255 + a / 2) / a);
#endif
            }
            else
            {
                p_pixel[0] = 0;
                p_pixel[1] = 0;
                p_pixel[2] = 0;
                p_pixel[3] = 0;
            }
        }
    }
    PROFILE_STOP( tiger_renderer_postprocess );
}
