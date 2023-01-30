}
static void InvalidateAllPictures(decoder_t *p_dec)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    vlc_mutex_lock(get_android_opaque_mutex());
    for (int i = 0; i < p_sys->i_output_buffers; ++i) {
        picture_t *p_pic = p_sys->inflight_picture[i];
        if (p_pic) {
            p_pic->p_sys->b_valid = false;
            p_sys->inflight_picture[i] = NULL;
        }
    }
    vlc_mutex_unlock(get_android_opaque_mutex());
}
