static void writer_print_data_hash(WriterContext *wctx, const char *name,
                                   uint8_t *data, int size)
{
    char *p, buf[AV_HASH_MAX_SIZE * 2 + 64] = { 0 };
    if (!hash)
        return;
    av_hash_init(hash);
    av_hash_update(hash, data, size);
    snprintf(buf, sizeof(buf), "%s:", av_hash_get_name(hash));
    p = buf + strlen(buf);
    av_hash_final_hex(hash, p, buf + sizeof(buf) - p);
    writer_print_string(wctx, name, buf, 0);
}
