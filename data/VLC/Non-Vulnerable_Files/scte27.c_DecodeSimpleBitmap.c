}
static subpicture_region_t *DecodeSimpleBitmap(decoder_t *dec,
                                               const uint8_t *data, int size)
{
    VLC_UNUSED(dec);
    /* Parse the bitmap and its properties */
    bs_t bs;
    bs_init(&bs, data, size);
    bs_skip(&bs, 5);
    int is_framed = bs_read(&bs, 1);
    int outline_style = bs_read(&bs, 2);
    scte27_color_t character_color = bs_read_color(&bs);
    int top_h = bs_read(&bs, 12);
    int top_v = bs_read(&bs, 12);
    int bottom_h = bs_read(&bs, 12);
    int bottom_v = bs_read(&bs, 12);
    if (top_h >= bottom_h || top_v >= bottom_v)
        return NULL;
    int frame_top_h = top_h;
    int frame_top_v = top_v;
    int frame_bottom_h = bottom_h;
    int frame_bottom_v = bottom_v;
    scte27_color_t frame_color = scte27_color_transparent;
    if (is_framed) {
        frame_top_h = bs_read(&bs, 12);
        frame_top_v = bs_read(&bs, 12);
        frame_bottom_h = bs_read(&bs, 12);
        frame_bottom_v = bs_read(&bs, 12);
        frame_color = bs_read_color(&bs);
        if (frame_top_h > top_h ||
            frame_top_v > top_v ||
            frame_bottom_h < bottom_h ||
            frame_bottom_v < bottom_v)
            return NULL;
    }
    int outline_thickness = 0;
    scte27_color_t outline_color = scte27_color_transparent;
    int shadow_right = 0;
    int shadow_bottom = 0;
    scte27_color_t shadow_color = scte27_color_transparent;
    if (outline_style == 1) {
        bs_skip(&bs, 4);
        outline_thickness = bs_read(&bs, 4);
        outline_color = bs_read_color(&bs);
    } else if (outline_style == 2) {
        shadow_right = bs_read(&bs, 4);
        shadow_bottom = bs_read(&bs, 4);
        shadow_color = bs_read_color(&bs);
    } else if (outline_style == 3) {
        bs_skip(&bs, 24);
    }
    bs_skip(&bs, 16); // bitmap_compressed_length
    int bitmap_h = bottom_h - top_h;
    int bitmap_v = bottom_v - top_v;
    int bitmap_size = bitmap_h * bitmap_v;
    bool *bitmap = malloc(bitmap_size * sizeof(*bitmap));
    if (!bitmap)
        return NULL;
    for (int position = 0; position < bitmap_size;) {
        if (bs_eof(&bs)) {
            for (; position < bitmap_size; position++)
                bitmap[position] = false;
            break;
        }
        int run_on_length = 0;
        int run_off_length = 0;
        if (!bs_read1(&bs)) {
            if (!bs_read1(&bs)) {
                if (!bs_read1(&bs)) {
                    if (bs_read(&bs, 2) == 1) {
                        int next = __MIN((position / bitmap_h + 1) * bitmap_h,
                                         bitmap_size);
                        for (; position < next; position++)
                            bitmap[position] = false;
                    }
                } else {
                    run_on_length = 4;
                }
            } else {
                run_off_length = 6;
            }
        } else {
            run_on_length = 3;
            run_off_length = 5;
        }
        if (run_on_length > 0) {
            int run = bs_read(&bs, run_on_length);
            if (!run)
                run = 1 << run_on_length;
            for (; position < bitmap_size && run > 0; position++, run--)
                bitmap[position] = true;
        }
        if (run_off_length > 0) {
            int run = bs_read(&bs, run_off_length);
            if (!run)
                run = 1 << run_off_length;
            for (; position < bitmap_size && run > 0; position++, run--)
                bitmap[position] = false;
        }
    }
    /* Render the bitmap into a subpicture_region_t */
    /* Reserve the place for the style
     * FIXME It's unclear if it is needed or if the bitmap should already include
     * the needed margin (I think the samples I have do both). */
    int margin_h = 0;
    int margin_v = 0;
    if (outline_style == 1) {
        margin_h =
        margin_v = outline_thickness;
    } else if (outline_style == 2) {
        margin_h = shadow_right;
        margin_v = shadow_bottom;
    }
    frame_top_h -= margin_h;
    frame_top_v -= margin_v;
    frame_bottom_h += margin_h;
    frame_bottom_v += margin_v;
    const int frame_h = frame_bottom_h - frame_top_h;
    const int frame_v = frame_bottom_v - frame_top_v;
    const int bitmap_oh = top_h - frame_top_h;
    const int bitmap_ov = top_v - frame_top_v;
    enum {
        COLOR_FRAME,
        COLOR_CHARACTER,
        COLOR_OUTLINE,
        COLOR_SHADOW,
    };
    video_palette_t palette = {
        .i_entries = 4,
        .palette = {
            [COLOR_FRAME] = {
                frame_color.y,
                frame_color.u,
                frame_color.v,
                frame_color.alpha
            },
            [COLOR_CHARACTER] = {
                character_color.y,
                character_color.u,
                character_color.v,
                character_color.alpha
            },
            [COLOR_OUTLINE] = {
                outline_color.y,
                outline_color.u,
                outline_color.v,
                outline_color.alpha
            },
            [COLOR_SHADOW] = {
                shadow_color.y,
                shadow_color.u,
                shadow_color.v,
                shadow_color.alpha
            },
        },
    };
    video_format_t fmt = {
        .i_chroma = VLC_CODEC_YUVP,
        .i_width = frame_h,
        .i_visible_width = frame_h,
        .i_height = frame_v,
        .i_visible_height = frame_v,
        .i_sar_num = 0, /* Use video AR */
        .i_sar_den = 1,
        .p_palette = &palette,
    };
    subpicture_region_t *r = subpicture_region_New(&fmt);
    if (!r) {
        free(bitmap);
        return NULL;
    }
    r->i_x = frame_top_h;
    r->i_y = frame_top_v;
    /* Fill up with frame (background) color */
    for (int y = 0; y < frame_v; y++)
        memset(&r->p_picture->p->p_pixels[y * r->p_picture->p->i_pitch],
               COLOR_FRAME,
               frame_h);
    /* Draw the outline/shadow if requested */
    if (outline_style == 1) {
        /* Draw an outline
         * XXX simple but slow and of low quality (no anti-aliasing) */
        bool circle[16][16];
        for (int dy = 0; dy <= 15; dy++) {
            for (int dx = 0; dx <= 15; dx++)
                circle[dy][dx] = (dx > 0 || dy > 0) &&
                                 dx * dx + dy * dy <= outline_thickness * outline_thickness;
        }
        for (int by = 0; by < bitmap_v; by++) {
            for (int bx = 0; bx < bitmap_h; bx++) {
                if (!bitmap[by * bitmap_h + bx])
                    continue;
                for (int dy = 0; dy <= outline_thickness; dy++) {
                    for (int dx = 0; dx <= outline_thickness; dx++) {
                        if (circle[dy][dx]) {
                            SetYUVPPixel(r->p_picture,
                                         bx + bitmap_oh + dx, by + bitmap_ov + dy, COLOR_OUTLINE);
                            SetYUVPPixel(r->p_picture,
                                         bx + bitmap_oh - dx, by + bitmap_ov + dy, COLOR_OUTLINE);
                            SetYUVPPixel(r->p_picture,
                                         bx + bitmap_oh + dx, by + bitmap_ov - dy, COLOR_OUTLINE);
                            SetYUVPPixel(r->p_picture,
                                         bx + bitmap_oh - dx, by + bitmap_ov - dy, COLOR_OUTLINE);
                        }
                    }
                }
            }
        }
    } else if (outline_style == 2) {
        /* Draw a shadow by drawing the character shifted by shaddow right/bottom */
        for (int by = 0; by < bitmap_v; by++) {
            for (int bx = 0; bx < bitmap_h; bx++) {
                if (bitmap[by * bitmap_h + bx])
                    SetYUVPPixel(r->p_picture,
                                 bx + bitmap_oh + shadow_right,
                                 by + bitmap_ov + shadow_bottom,
                                 COLOR_SHADOW);
            }
        }
    }
    /* Draw the character */
    for (int by = 0; by < bitmap_v; by++) {
        for (int bx = 0; bx < bitmap_h; bx++) {
            if (bitmap[by * bitmap_h + bx])
                SetYUVPPixel(r->p_picture,
                             bx + bitmap_oh, by + bitmap_ov, COLOR_CHARACTER);
        }
    }
    free(bitmap);
    return r;
}
