static double frame_sum_square_err(const int *hist, const double *median)
{
    int i;
    double err, sum_sq_err = 0;
    for (i = 0; i < HIST_SIZE; i++) {
        err = median[i] - (double)hist[i];
        sum_sq_err += err*err;
    }
    return sum_sq_err;
}
