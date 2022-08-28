static int read_mfra(struct Tracks *tracks, int start_index,
                     const char *file, int split, int ismf,
                     const char *basename, const char* output_prefix)
{
    int err = 0;
    const char* err_str = "";
    AVIOContext *f = NULL;
    int32_t mfra_size;
    if ((err = avio_open2(&f, file, AVIO_FLAG_READ, NULL, NULL)) < 0)
        goto fail;
    avio_seek(f, avio_size(f) - 4, SEEK_SET);
    mfra_size = avio_rb32(f);
    avio_seek(f, -mfra_size, SEEK_CUR);
    if (avio_rb32(f) != mfra_size) {
        err = AVERROR_INVALIDDATA;
        err_str = "mfra size mismatch";
        goto fail;
    }
    if (avio_rb32(f) != MKBETAG('m', 'f', 'r', 'a')) {
        err = AVERROR_INVALIDDATA;
        err_str = "mfra tag mismatch";
        goto fail;
    }
    while (!read_tfra(tracks, start_index, f)) {
        
    }
    if (split || ismf)
        err = write_fragments(tracks, start_index, f, basename, split, ismf,
                              output_prefix);
    err_str = "error in write_fragments";
fail:
    if (f)
        avio_close(f);
    if (err)
        fprintf(stderr, "Unable to read the MFRA atom in %s (%s)\n", file, err_str);
    return err;
}
