static uint64_t find_any_startcode(AVIOContext *bc, int64_t pos)
{
    uint64_t state = 0;
    if (pos >= 0)
        

        avio_seek(bc, pos, SEEK_SET);
    while (!avio_feof(bc)) {
        state = (state << 8) | avio_r8(bc);
        if ((state >> 56) != 'N')
            continue;
        switch (state) {
        case MAIN_STARTCODE:
        case STREAM_STARTCODE:
        case SYNCPOINT_STARTCODE:
        case INFO_STARTCODE:
        case INDEX_STARTCODE:
            return state;
        }
    }
    return 0;
}
