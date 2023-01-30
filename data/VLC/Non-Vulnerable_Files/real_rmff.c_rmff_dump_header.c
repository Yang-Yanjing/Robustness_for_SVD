}
int rmff_dump_header(rmff_header_t *h, void *buf_gen, int max) {
    uint8_t *buffer = buf_gen;
    int written=0, size;
    rmff_mdpr_t **stream=h->streams;
    if ((size=rmff_dump_fileheader(h->fileheader, &buffer[written], max)) < 0)
        return -1;
    written += size;
    max -= size;
    if ((size=rmff_dump_prop(h->prop, &buffer[written], max)) < 0)
        return -1;
    written += size;
    max -= size;
    if ((size=rmff_dump_cont(h->cont, &buffer[written], max)) < 0)
        return -1;
    written += size;
    max -= size;
    if (stream) {
        while(*stream) {
            if ((size=rmff_dump_mdpr(*stream, &buffer[written], max)) < 0)
                return -1;
            written += size;
            max -= size;
            stream++;
        }
    }
    if ((size=rmff_dump_dataheader(h->data, &buffer[written], max)) < 0)
        return -1;
    written+=size;
    return written;
}