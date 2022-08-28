static int get_packet_size(const uint8_t *buf, int size)
{
    int score, fec_score, dvhs_score;
    if (size < (TS_FEC_PACKET_SIZE * 5 + 1))
        return AVERROR_INVALIDDATA;
    score      = analyze(buf, size, TS_PACKET_SIZE,      NULL, 0);
    dvhs_score = analyze(buf, size, TS_DVHS_PACKET_SIZE, NULL, 0);
    fec_score  = analyze(buf, size, TS_FEC_PACKET_SIZE,  NULL, 0);
    av_log(NULL, AV_LOG_TRACE, "score: %d, dvhs_score: %d, fec_score: %d \n",
            score, dvhs_score, fec_score);
    if (score > fec_score && score > dvhs_score)
        return TS_PACKET_SIZE;
    else if (dvhs_score > score && dvhs_score > fec_score)
        return TS_DVHS_PACKET_SIZE;
    else if (score < fec_score && dvhs_score < fec_score)
        return TS_FEC_PACKET_SIZE;
    else
        return AVERROR_INVALIDDATA;
}
