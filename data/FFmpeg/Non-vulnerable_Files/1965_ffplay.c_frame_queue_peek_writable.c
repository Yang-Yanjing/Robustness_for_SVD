static Frame *frame_queue_peek_writable(FrameQueue *f)
{
    
    SDL_LockMutex(f->mutex);
    while (f->size >= f->max_size &&
           !f->pktq->abort_request) {
        SDL_CondWait(f->cond, f->mutex);
    }
    SDL_UnlockMutex(f->mutex);
    if (f->pktq->abort_request)
        return NULL;
    return &f->queue[f->windex];
}
