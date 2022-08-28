
static void put_packet(NUTContext *nut, AVIOContext *bc, AVIOContext *dyn_bc,
                       int calculate_checksum, uint64_t startcode)
{
    uint8_t *dyn_buf = NULL;
    int dyn_size     = avio_close_dyn_buf(dyn_bc, &dyn_buf);
    int forw_ptr     = dyn_size + 4 * calculate_checksum;
    if (forw_ptr > 4096)
        ffio_init_checksum(bc, ff_crc04C11DB7_update, 0);
    avio_wb64(bc, startcode);
    ff_put_v(bc, forw_ptr);
    if (forw_ptr > 4096)
        avio_wl32(bc, ffio_get_checksum(bc));
    if (calculate_checksum)
        ffio_init_checksum(bc, ff_crc04C11DB7_update, 0);
    avio_write(bc, dyn_buf, dyn_size);
    if (calculate_checksum)
        avio_wl32(bc, ffio_get_checksum(bc));
    av_free(dyn_buf);
}
