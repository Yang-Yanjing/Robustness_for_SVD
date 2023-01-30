 * Prepare a picture for display */
static void Prepare(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    assert(vd->fmt.i_width  == aa_imgwidth(sys->aa_context) &&
           vd->fmt.i_height == aa_imgheight(sys->aa_context));
#if 0
    if (picture->format.p_palette) {
        for (int i = 0; i < 256; i++) {
            aa_setpalette(vd->sys->palette, 256 - i,
                           red[ i ], green[ i ], blue[ i ]);
        }
    }
#else
    VLC_UNUSED(picture);
#endif
    VLC_UNUSED(subpicture);
    aa_fastrender(sys->aa_context, 0, 0,
                  vd->fmt.i_width, vd->fmt.i_height);
}
