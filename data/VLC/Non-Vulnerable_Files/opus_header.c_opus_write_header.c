}
int opus_write_header(uint8_t **p_extra, int *i_extra, OpusHeader *header, const char *vendor)
{
    unsigned char header_data[100];
    const int packet_size = opus_header_to_packet(header, header_data,
                                                  sizeof(header_data));
    unsigned char *data[2];
    size_t size[2];
    data[0] = header_data;
    size[0] = packet_size;
    size_t comments_length;
    char *comments = comment_init(&comments_length, vendor);
    if (!comments)
        return 1;
    if (comment_add(&comments, &comments_length, "ENCODER=",
                    "VLC media player"))
    {
        free(comments);
        return 1;
    }
    if (comment_pad(&comments, &comments_length))
    {
        free(comments);
        return 1;
    }
    data[1] = (unsigned char *) comments;
    size[1] = comments_length;
    for (unsigned i = 0; i < ARRAY_SIZE(data); ++i)
        if (xiph_AppendHeaders(i_extra, (void **) p_extra, size[i], data[i]))
        {
            *i_extra = 0;
            *p_extra = NULL;
        }
    return 0;
}
