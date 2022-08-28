static int check_n_master(AVCodecContext *avctx, int n_master, int bs_xover_band)
{
    
    if (n_master <= 0) {
        av_log(avctx, AV_LOG_ERROR, "Invalid n_master: %d\n", n_master);
        return -1;
    }
    if (bs_xover_band >= n_master) {
        av_log(avctx, AV_LOG_ERROR,
               "Invalid bitstream, crossover band index beyond array bounds: %d\n",
               bs_xover_band);
        return -1;
    }
    return 0;
}
