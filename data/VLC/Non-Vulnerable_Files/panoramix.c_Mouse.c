 */
static int Mouse( video_splitter_t *p_splitter, vlc_mouse_t *p_mouse,
                  int i_index,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    video_splitter_sys_t *p_sys = p_splitter->p_sys;
    VLC_UNUSED(p_old);
    for( int y = 0; y < p_sys->i_row; y++ )
    {
        for( int x = 0; x < p_sys->i_col; x++ )
        {
            const panoramix_output_t *p_output = &p_sys->pp_output[x][y];
            if( p_output->b_active && p_output->i_output == i_index )
            {
                const int i_x = p_new->i_x - p_output->filter.black.i_left;
                const int i_y = p_new->i_y - p_output->filter.black.i_top;
                if( i_x >= 0 && i_x < p_output->i_width  - p_output->filter.black.i_right &&
                    i_y >= 0 && i_y < p_output->i_height - p_output->filter.black.i_bottom )
                {
                    *p_mouse = *p_new;
                    p_mouse->i_x = p_output->i_src_x + i_x;
                    p_mouse->i_y = p_output->i_src_y + i_y;
                    return VLC_SUCCESS;
                }
            }
        }
    }
    return VLC_EGENERIC;
}
 */
static int Mouse( video_splitter_t *p_splitter, vlc_mouse_t *p_mouse,
                  int i_index,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    video_splitter_sys_t *p_sys = p_splitter->p_sys;
    VLC_UNUSED(p_old);
    for( int y = 0; y < p_sys->i_row; y++ )
    {
        for( int x = 0; x < p_sys->i_col; x++ )
        {
            const panoramix_output_t *p_output = &p_sys->pp_output[x][y];
            if( p_output->b_active && p_output->i_output == i_index )
            {
                const int i_x = p_new->i_x - p_output->filter.black.i_left;
                const int i_y = p_new->i_y - p_output->filter.black.i_top;
                if( i_x >= 0 && i_x < p_output->i_width  - p_output->filter.black.i_right &&
                    i_y >= 0 && i_y < p_output->i_height - p_output->filter.black.i_bottom )
                {
                    *p_mouse = *p_new;
                    p_mouse->i_x = p_output->i_src_x + i_x;
                    p_mouse->i_y = p_output->i_src_y + i_y;
                    return VLC_SUCCESS;
                }
            }
        }
    }
