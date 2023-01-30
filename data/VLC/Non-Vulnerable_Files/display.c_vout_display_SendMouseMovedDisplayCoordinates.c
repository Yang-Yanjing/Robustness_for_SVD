}
void vout_display_SendMouseMovedDisplayCoordinates(vout_display_t *vd, video_orientation_t orient_display, int m_x, int m_y, vout_display_place_t *place)
{
    video_format_t source_rot = vd->source;
    video_format_TransformTo(&source_rot, orient_display);
    if (place->width > 0 && place->height > 0) {
        int x = (int)(source_rot.i_x_offset +
                            (int64_t)(m_x - place->x) * source_rot.i_visible_width / place->width);
        int y = (int)(source_rot.i_y_offset +
                            (int64_t)(m_y - place->y) * source_rot.i_visible_height/ place->height);
        video_transform_t transform = video_format_GetTransform(vd->source.orientation, orient_display);
        int store;
        switch (transform) {
            case TRANSFORM_R90:
                store = x;
                x = y;
                y = vd->source.i_visible_height - store;
                break;
            case TRANSFORM_R180:
                x = vd->source.i_visible_width - x;
                y = vd->source.i_visible_height - y;
                break;
            case TRANSFORM_R270:
                store = x;
                x = vd->source.i_visible_width - y;
                y = store;
                break;
            case TRANSFORM_HFLIP:
                x = vd->source.i_visible_width - x;
                break;
            case TRANSFORM_VFLIP:
                y = vd->source.i_visible_height - y;
                break;
            case TRANSFORM_TRANSPOSE:
                store = x;
                x = y;
                y = store;
                break;
            case TRANSFORM_ANTI_TRANSPOSE:
                store = x;
                x = vd->source.i_visible_width - y;
                y = vd->source.i_visible_height - store;
                break;
            default:
                break;
        }
        vout_display_SendEventMouseMoved (vd, x, y);
    }
}
