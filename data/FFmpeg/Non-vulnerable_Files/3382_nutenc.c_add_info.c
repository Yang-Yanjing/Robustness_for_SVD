static int add_info(AVIOContext *bc, const char *type, const char *value)
{
    put_str(bc, type);
    put_s(bc, -1);
    put_str(bc, value);
    return 1;
}
