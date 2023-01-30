}
void picture_fifo_Flush(picture_fifo_t *fifo, mtime_t date, bool flush_before)
{
    picture_t *picture;
    vlc_mutex_lock(&fifo->lock);
    picture = fifo->first;
    PictureFifoReset(fifo);
    picture_fifo_t tmp;
    PictureFifoReset(&tmp);
    while (picture) {
        picture_t *next = picture->p_next;
        picture->p_next = NULL;
        if (( flush_before && picture->date <= date) ||
            (!flush_before && picture->date >= date))
            PictureFifoPush(&tmp, picture);
        else
            PictureFifoPush(fifo, picture);
        picture = next;
    }
    vlc_mutex_unlock(&fifo->lock);
    for (;;) {
        picture_t *picture = PictureFifoPop(&tmp);
        if (!picture)
            break;
        picture_Release(picture);
    }
}
