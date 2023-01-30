 */
static void Prepare(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->d2_render_target && sys->d2_bitmap) {
        HRESULT hr = ID2D1Bitmap_CopyFromMemory(sys->d2_bitmap,
                                                NULL /*&r_src*/,
                                                picture->p[0].p_pixels,
                                                picture->p[0].i_pitch);
        if (hr != S_OK)
            msg_Err(vd, "Failed to copy bitmap memory (hr = 0x%x)!",
                    (unsigned)hr);
#ifndef NDEBUG
        /*msg_Dbg(vd, "Bitmap dbg: target = %p, pitch = %d, bitmap = %p",
                sys->d2_render_target, pitch, sys->d2_bitmap);*/
#endif
    }
    VLC_UNUSED(subpicture);
}
