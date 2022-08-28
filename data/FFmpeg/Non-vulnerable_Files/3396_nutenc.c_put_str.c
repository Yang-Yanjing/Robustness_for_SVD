static void put_str(AVIOContext *bc, const char *string)
{
    size_t len = strlen(string);
    ff_put_v(bc, len);
    avio_write(bc, string, len);
}
