}
static void PictureFifoPush(picture_fifo_t *fifo, picture_t *picture)
{
    assert(!picture->p_next);
    *fifo->last_ptr = picture;
    fifo->last_ptr  = &picture->p_next;
}
