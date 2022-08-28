static int64_t seek_by_sector(AVIOContext *pb, int64_t sector, int64_t offset)
{
    return avio_seek(pb, (sector << WTV_SECTOR_BITS) + offset, SEEK_SET);
}
