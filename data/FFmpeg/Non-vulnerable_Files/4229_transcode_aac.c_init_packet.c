static void init_packet(AVPacket *packet)
{
    av_init_packet(packet);
    
    packet->data = NULL;
    packet->size = 0;
}
