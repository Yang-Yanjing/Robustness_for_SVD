static void put_ebml_string(AVIOContext *pb, unsigned int elementid,
                            const char *str)
{
    put_ebml_binary(pb, elementid, str, strlen(str));
}
