}
block_t *screen_Capture(demux_t *p_demux)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    screen_data_t *p_data = (screen_data_t *)p_sys->p_data;
    block_t *p_block;
    CGRect capture_rect;
    CGImageRef image;
    /* forward cursor location */
    CGPoint cursor_pos;
    CGEventRef event = CGEventCreate(NULL);
    cursor_pos = CGEventGetLocation(event);
    CFRelease(event);
    cursor_pos.x -= p_data->screen_left;
    cursor_pos.y -= p_data->screen_top;
    if (p_sys->b_follow_mouse)
        FollowMouse(p_sys, cursor_pos.x, cursor_pos.y);
    capture_rect.origin.x = p_sys->i_left;
    capture_rect.origin.y = p_sys->i_top;
    capture_rect.size.width = p_data->width;
    capture_rect.size.height = p_data->height;
    /* fetch image data */
    image = CGDisplayCreateImageForRect(p_data->display_id, capture_rect);
    if (!image) {
        msg_Warn(p_demux, "no image!");
        return NULL;
    }
    /* create offscreen context */
    if (!p_data->offscreen_context) {
        CGColorSpaceRef colorspace;
        colorspace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
        p_data->offscreen_bitmap_size = p_sys->fmt.video.i_width * p_sys->fmt.video.i_height * 4;
        p_data->offscreen_bitmap = calloc(1, p_data->offscreen_bitmap_size);
        if (p_data->offscreen_bitmap == NULL) {
            msg_Warn(p_demux, "can't allocate offscreen bitmap");
            CFRelease(image);
            return NULL;
        }
        p_data->offscreen_context = CGBitmapContextCreate(p_data->offscreen_bitmap, p_sys->fmt.video.i_width, p_sys->fmt.video.i_height, 8, p_sys->fmt.video.i_width * 4, colorspace, kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little);
        if (!p_data->offscreen_context) {
            msg_Warn(p_demux, "can't create offscreen bitmap context");
            CFRelease(image);
            return NULL;
        }
        CGColorSpaceRelease(colorspace);
        p_data->offscreen_rect = CGRectMake(0, 0, p_sys->fmt.video.i_width, p_sys->fmt.video.i_height);
    }
    /* fetch cursor image */
    CGImageRef cursor_image;
    int cid = CGSMainConnectionID();
    CGPoint outHotSpot;
    cursor_image = CGSCreateRegisteredCursorImage(cid, (char *)"com.apple.coregraphics.GlobalCurrent", &outHotSpot);
    /* draw screen image and cursor image */
    CGRect cursor_rect;
    cursor_rect.size.width = CGImageGetWidth(cursor_image);
    cursor_rect.size.height = CGImageGetHeight(cursor_image);
    cursor_rect.origin.x = cursor_pos.x - p_sys->i_left - outHotSpot.x;
    cursor_rect.origin.y = p_data->offscreen_rect.size.height
        - (cursor_pos.y + cursor_rect.size.height - p_sys->i_top - outHotSpot.y);
    CGContextDrawImage(p_data->offscreen_context, p_data->offscreen_rect, image);
    CGContextDrawImage(p_data->offscreen_context, cursor_rect, cursor_image);
    /* build block */
    p_block = block_Alloc(p_data->offscreen_bitmap_size);
    if (!p_block) {
        msg_Warn(p_demux, "can't get block");
        CFRelease(image);
        return NULL;
    }
    memmove(p_block->p_buffer, p_data->offscreen_bitmap, p_data->offscreen_bitmap_size);
    CFRelease(image);
    return p_block;
}
