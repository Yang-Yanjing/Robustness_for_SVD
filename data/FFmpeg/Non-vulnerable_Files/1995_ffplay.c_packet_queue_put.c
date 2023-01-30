static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    int ret;
    SDL_LockMutex(q->mutex);
    ret = packet_queue_put_private(q, pkt);
    SDL_UnlockMutex(q->mutex);
    if (pkt != &flush_pkt && ret < 0)
        av_packet_unref(pkt);
    return ret;
}
