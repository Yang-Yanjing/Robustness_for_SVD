static void pullup_pack_frame(PullupContext *s, PullupFrame *fr)
{
    int i;
    if (fr->buffer)
        return;
    if (fr->length < 2)
        return; 
    for (i = 0; i < 2; i++) {
        if (fr->ofields[i]->lock[i^1])
            continue;
        fr->buffer = fr->ofields[i];
        pullup_lock_buffer(fr->buffer, 2);
        copy_field(s, fr->buffer, fr->ofields[i^1], i^1);
        return;
    }
    fr->buffer = pullup_get_buffer(s, 2);
    copy_field(s, fr->buffer, fr->ofields[0], 0);
    copy_field(s, fr->buffer, fr->ofields[1], 1);
}
