}
static subpicture_t *DecodeSubtitleMessage(decoder_t *dec,
                                           const uint8_t *data, int size,
                                           mtime_t date)
{
    if (size < 12)
        goto error;
    /* Parse the header */
    bool pre_clear_display = data[3] & 0x80;
    int  display_standard = data[3] & 0x1f;
    int subtitle_type = data[8] >> 4;
    int display_duration = ((data[8] & 0x07) << 8) | data[9];
    int block_length = GetWBE(&data[10]);
    size -= 12;
    data += 12;
    if (block_length > size)
        goto error;
    if (subtitle_type == 1) {
        subpicture_region_t *region = DecodeSimpleBitmap(dec, data, block_length);
        if (!region)
            goto error;
        subpicture_t *sub = decoder_NewSubpicture(dec, NULL);
        if (!sub) {
            subpicture_region_Delete(region);
            return NULL;
        }
        int frame_duration;
        switch (display_standard) {
        case 0:
            sub->i_original_picture_width  = 720;
            sub->i_original_picture_height = 480;
            frame_duration = 33367;
            break;
        case 1:
            sub->i_original_picture_width  = 720;
            sub->i_original_picture_height = 576;
            frame_duration = 40000;
            break;
        case 2:
            sub->i_original_picture_width  = 1280;
            sub->i_original_picture_height =  720;
            frame_duration = 16683;
            break;
        case 3:
            sub->i_original_picture_width  = 1920;
            sub->i_original_picture_height = 1080;
            frame_duration = 16683;
            break;
        default:
            msg_Warn(dec, "Unknown display standard");
            sub->i_original_picture_width  = 0;
            sub->i_original_picture_height = 0;
            frame_duration = 40000;
            break;
        }
        sub->b_absolute = true;
        if (!pre_clear_display)
            msg_Warn(dec, "SCTE-27 subtitles without pre_clear_display flag are not well supported");
        sub->b_ephemer = true;
        sub->i_start = date;
        sub->i_stop = date + display_duration * frame_duration;
        sub->p_region = region;
        return sub;
    } else {
        /* Reserved */
        return NULL;
    }
error:
    msg_Err(dec, "corrupted subtitle_message");
    return NULL;
}
