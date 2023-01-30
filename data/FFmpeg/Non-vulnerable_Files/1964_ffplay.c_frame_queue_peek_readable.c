static Frame *frame_queue_peek_readable(FrameQueue *f)
{
    
    SDL_LockMutex(f->mutex);
    while (f->size - f->rindex_shown <= 0 &&
           !f->pktq->abort_request) {
        SDL_CondWait(f->cond, f->mutex);
    }
    SDL_UnlockMutex(f->mutex);
    if (f->pktq->abort_request)
        return NULL;
    return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}
