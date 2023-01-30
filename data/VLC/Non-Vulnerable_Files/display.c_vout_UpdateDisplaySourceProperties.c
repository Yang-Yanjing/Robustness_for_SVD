}
void vout_UpdateDisplaySourceProperties(vout_display_t *vd, const video_format_t *source)
{
    vout_display_owner_sys_t *osys = vd->owner.sys;
    if (source->i_sar_num * osys->source.i_sar_den !=
        source->i_sar_den * osys->source.i_sar_num) {
        osys->source.i_sar_num = source->i_sar_num;
        osys->source.i_sar_den = source->i_sar_den;
        vlc_ureduce(&osys->source.i_sar_num, &osys->source.i_sar_den,
                    osys->source.i_sar_num, osys->source.i_sar_den, 0);
        /* FIXME it will override any AR that the user would have forced */
        osys->ch_sar = true;
        osys->sar.num = osys->source.i_sar_num;
        osys->sar.den = osys->source.i_sar_den;
    }
    if (source->i_x_offset       != osys->source.i_x_offset ||
        source->i_y_offset       != osys->source.i_y_offset ||
        source->i_visible_width  != osys->source.i_visible_width ||
        source->i_visible_height != osys->source.i_visible_height) {
        video_format_CopyCrop(&osys->source, source);
        /* Force the vout to reapply the current user crop settings over the new decoder
         * crop settings. */
        osys->ch_crop = true;
    }
}
