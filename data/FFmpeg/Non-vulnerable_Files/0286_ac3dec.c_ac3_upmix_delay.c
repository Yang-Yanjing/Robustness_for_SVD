static void ac3_upmix_delay(AC3DecodeContext *s)
{
    int channel_data_size = sizeof(s->delay[0]);
    switch (s->channel_mode) {
    case AC3_CHMODE_DUALMONO:
    case AC3_CHMODE_STEREO:
        
        memcpy(s->delay[1], s->delay[0], channel_data_size);
        break;
    case AC3_CHMODE_2F2R:
        memset(s->delay[3], 0, channel_data_size);
    case AC3_CHMODE_2F1R:
        memset(s->delay[2], 0, channel_data_size);
        break;
    case AC3_CHMODE_3F2R:
        memset(s->delay[4], 0, channel_data_size);
    case AC3_CHMODE_3F1R:
        memset(s->delay[3], 0, channel_data_size);
    case AC3_CHMODE_3F:
        memcpy(s->delay[2], s->delay[1], channel_data_size);
        memset(s->delay[1], 0, channel_data_size);
        break;
    }
}
