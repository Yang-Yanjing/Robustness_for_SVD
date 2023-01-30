 *****************************************************************************/
static int dummy_Run( visual_effect_t * p_effect, vlc_object_t *p_aout,
                      const block_t * p_buffer , picture_t * p_picture)
{
    VLC_UNUSED(p_effect); VLC_UNUSED(p_aout); VLC_UNUSED(p_buffer);
    VLC_UNUSED(p_picture);
    return 0;
}
