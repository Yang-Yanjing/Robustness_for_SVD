static int wtvfile_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    WtvFile *wf = opaque;
    AVIOContext *pb = wf->pb_filesystem;
    int nread = 0;
    if (wf->error || pb->error)
        return -1;
    if (wf->position >= wf->length || avio_feof(pb))
        return 0;
    buf_size = FFMIN(buf_size, wf->length - wf->position);
    while(nread < buf_size) {
        int n;
        int remaining_in_sector = (1 << wf->sector_bits) - (wf->position & ((1 << wf->sector_bits) - 1));
        int read_request        = FFMIN(buf_size - nread, remaining_in_sector);
        n = avio_read(pb, buf, read_request);
        if (n <= 0)
            break;
        nread += n;
        buf += n;
        wf->position += n;
        if (n == remaining_in_sector) {
            int i = wf->position >> wf->sector_bits;
            if (i >= wf->nb_sectors ||
                (wf->sectors[i] != wf->sectors[i - 1] + (1 << (wf->sector_bits - WTV_SECTOR_BITS)) &&
                seek_by_sector(pb, wf->sectors[i], 0) < 0)) {
                wf->error = 1;
                break;
            }
        }
    }
    return nread;
}
