}
static picture_t *PictureFifoPop(picture_fifo_t *fifo)
{
    picture_t *picture = fifo->first;
    if (picture) {
        fifo->first = picture->p_next;
        if (!fifo->first)
            fifo->last_ptr = &fifo->first;
        picture->p_next = NULL;
    }
    return picture;
}
