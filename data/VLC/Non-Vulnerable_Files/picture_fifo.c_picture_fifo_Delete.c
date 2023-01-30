}
void picture_fifo_Delete(picture_fifo_t *fifo)
{
    picture_fifo_Flush(fifo, INT64_MAX, true);
    vlc_mutex_destroy(&fifo->lock);
    free(fifo);
}
