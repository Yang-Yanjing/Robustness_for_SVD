static int amf_get_string(AVIOContext *ioc, char *buffer, int buffsize)
{
    int length = avio_rb16(ioc);
    if (length >= buffsize) {
        avio_skip(ioc, length);
        return -1;
    }
    avio_read(ioc, buffer, length);
    buffer[length] = '\0';
    return length;
}
