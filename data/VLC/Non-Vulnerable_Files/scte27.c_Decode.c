}
static subpicture_t *Decode(decoder_t *dec, block_t **block)
{
    decoder_sys_t *sys = dec->p_sys;
    if (block == NULL || *block == NULL)
        return NULL;
    block_t *b = *block; *block = NULL;
    subpicture_t *sub_first = NULL;
    subpicture_t **sub_last = &sub_first;
    if (b->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED))
        goto exit;
    while (b->i_buffer > 3) {
        const int table_id =  b->p_buffer[0];
        if (table_id != 0xc6) {
            //if (table_id != 0xff)
            //    msg_Err(dec, "Invalid SCTE-27 table id (0x%x)", table_id);
            break;
        }
        const int section_length = ((b->p_buffer[1] & 0xf) << 8) | b->p_buffer[2];
        if (section_length <= 1 + 4 || b->i_buffer < 3 + (unsigned)section_length) {
            msg_Err(dec, "Invalid SCTE-27 section length");
            break;
        }
        const int protocol_version = b->p_buffer[3] & 0x3f;
        if (protocol_version != 0) {
            msg_Err(dec, "Unsupported SCTE-27 protocol version (%d)", protocol_version);
            break;
        }
        const bool segmentation_overlay = b->p_buffer[3] & 0x40;
        subpicture_t *sub = NULL;
        if (segmentation_overlay) {
            if (section_length < 1 + 5 + 4)
                break;
            int id = GetWBE(&b->p_buffer[4]);
            int last = (b->p_buffer[6] << 4) | (b->p_buffer[7] >> 4);
            int index = ((b->p_buffer[7] & 0x0f) << 8) | b->p_buffer[8];
            if (index > last)
                break;
            if (index == 0) {
                sys->segment_id = id;
                sys->segment_size = 0;
                sys->segment_date = b->i_pts > VLC_TS_INVALID ? b->i_pts : b->i_dts;
            } else {
                if (sys->segment_id != id || sys->segment_size <= 0) {
                    sys->segment_id = -1;
                    break;
                }
            }
            int segment_size = section_length - 1 - 5 - 4;
            sys->segment_buffer = xrealloc(sys->segment_buffer,
                                           sys->segment_size + segment_size);
            memcpy(&sys->segment_buffer[sys->segment_size],
                   &b->p_buffer[9], segment_size);
            sys->segment_size += segment_size;
            if (index == last) {
                sub = DecodeSubtitleMessage(dec,
                                            sys->segment_buffer,
                                            sys->segment_size,
                                            sys->segment_date);
                sys->segment_size = 0;
            }
        } else {
            sub = DecodeSubtitleMessage(dec,
                                        &b->p_buffer[4],
                                        section_length - 1 - 4,
                                        b->i_pts > VLC_TS_INVALID ? b->i_pts : b->i_dts);
        }
        *sub_last = sub;
        if (*sub_last)
            sub_last = &(*sub_last)->p_next;
        b->i_buffer -= 3 + section_length;
        b->p_buffer += 3 + section_length;
        break;
    }
exit:
    block_Release(b);
    return sub_first;
}
