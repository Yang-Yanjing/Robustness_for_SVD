 */
static void Place(vout_display_t *vd, vout_display_place_t *place)
{
    vout_display_sys_t *sys = vd->sys;
    vout_display_PlacePicture(place, &vd->source, vd->cfg, false);
    const int canvas_width   = cucul_get_canvas_width(sys->cv);
    const int canvas_height  = cucul_get_canvas_height(sys->cv);
    const int display_width  = caca_get_display_width(sys->dp);
    const int display_height = caca_get_display_height(sys->dp);
    if (display_width > 0 && display_height > 0) {
        place->x      =  place->x      * canvas_width  / display_width;
        place->y      =  place->y      * canvas_height / display_height;
        place->width  = (place->width  * canvas_width  + display_width/2)  / display_width;
        place->height = (place->height * canvas_height + display_height/2) / display_height;
    } else {
        place->x = 0;
        place->y = 0;
        place->width  = canvas_width;
        place->height = display_height;
    }
}
