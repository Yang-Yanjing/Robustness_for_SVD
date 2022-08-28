static int64_t wtvfile_seek(void *opaque, int64_t offset, int whence)
{
    WtvFile *wf = opaque;
    AVIOContext *pb = wf->pb_filesystem;
    if (whence == AVSEEK_SIZE)
        return wf->length;
    else if (whence == SEEK_CUR)
        offset = wf->position + offset;
    else if (whence == SEEK_END)
        offset = wf->length;
    wf->error = offset < 0 || offset >= wf->length ||
                seek_by_sector(pb, wf->sectors[offset >> wf->sector_bits],
                               offset & ((1 << wf->sector_bits) - 1)) < 0;
    wf->position = offset;
    return offset;
}
