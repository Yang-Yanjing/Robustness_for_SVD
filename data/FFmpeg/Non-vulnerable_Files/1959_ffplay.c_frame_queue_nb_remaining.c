static int frame_queue_nb_remaining(FrameQueue *f)
{
    return f->size - f->rindex_shown;
}
