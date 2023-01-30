static int64_t find_startcode(AVIOContext *bc, uint64_t code, int64_t pos)
{
    for (;;) {
        uint64_t startcode = find_any_startcode(bc, pos);
        if (startcode == code)
            return avio_tell(bc) - 8;
        else if (startcode == 0)
            return -1;
        pos = -1;
    }
}
