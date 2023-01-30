}
static int cidcmp (const void *a, const void *b)
{
    const uint32_t *id = a;
    const vlc_v4l2_ctrl_name_t *name = b;
    return (int32_t)(*id - name->cid);
}
