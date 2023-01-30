static double output_gain(double lin_slope, double ratio, double thres,
                          double knee, double knee_start, double knee_stop,
                          double compressed_knee_stop, int detection)
{
    double slope = log(lin_slope);
    double gain = 0.0;
    double delta = 0.0;
    if (detection)
        slope *= 0.5;
    if (IS_FAKE_INFINITY(ratio)) {
        gain = thres;
        delta = 0.0;
    } else {
        gain = (slope - thres) / ratio + thres;
        delta = 1.0 / ratio;
    }
    if (knee > 1.0 && slope < knee_stop)
        gain = hermite_interpolation(slope, knee_start, knee_stop,
                                     knee_start, compressed_knee_stop,
                                     1.0, delta);
    return exp(gain - slope);
}
