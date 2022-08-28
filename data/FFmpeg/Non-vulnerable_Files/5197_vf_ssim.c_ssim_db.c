static double ssim_db(double ssim, double weight)
{
    return 10 * (log(weight) / log(10) - log(weight - ssim) / log(10));
}
