} vlc_av_packet_t;
static void vlc_av_packet_Release(block_t *block)
{
    vlc_av_packet_t *b = (void *) block;
    av_free_packet(&b->packet);
    free(b);
}
