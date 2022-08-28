static inline double get_psnr(double mse, uint64_t nb_frames, int max)
{
    return 10.0 * log(pow2(max) / (mse / nb_frames)) / log(10.0);
}
