static int decode_i_mb(FourXContext *f)
{
    int ret;
    int i;
    f->bdsp.clear_blocks(f->block[0]);
    for (i = 0; i < 6; i++)
        if ((ret = decode_i_block(f, f->block[i])) < 0)
            return ret;
    return 0;
}
