void ff_rm_free_rmstream (RMStream *rms)
{
    av_free_packet(&rms->pkt);
}
