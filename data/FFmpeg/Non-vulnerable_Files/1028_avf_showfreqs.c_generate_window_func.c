static void generate_window_func(float *lut, int N, int win_func, float *overlap)
{
    int n;
    switch (win_func) {
    case WFUNC_RECT:
        for (n = 0; n < N; n++)
            lut[n] = 1.;
        *overlap = 0.;
        break;
    case WFUNC_BARTLETT:
        for (n = 0; n < N; n++)
            lut[n] = 1.-FFABS((n-(N-1)/2.)/((N-1)/2.));
        *overlap = 0.5;
        break;
    case WFUNC_HANNING:
        for (n = 0; n < N; n++)
            lut[n] = .5*(1-cos(2*M_PI*n/(N-1)));
        *overlap = 0.5;
        break;
    case WFUNC_HAMMING:
        for (n = 0; n < N; n++)
            lut[n] = .54-.46*cos(2*M_PI*n/(N-1));
        *overlap = 0.5;
        break;
    case WFUNC_BLACKMAN:
        for (n = 0; n < N; n++)
            lut[n] = .42659-.49656*cos(2*M_PI*n/(N-1))+.076849*cos(4*M_PI*n/(N-1));
        *overlap = 0.661;
        break;
    case WFUNC_WELCH:
        for (n = 0; n < N; n++)
            lut[n] = 1.-(n-(N-1)/2.)/((N-1)/2.)*(n-(N-1)/2.)/((N-1)/2.);
        *overlap = 0.293;
        break;
    case WFUNC_FLATTOP:
        for (n = 0; n < N; n++)
            lut[n] = 1.-1.985844164102*cos( 2*M_PI*n/(N-1))+1.791176438506*cos( 4*M_PI*n/(N-1))-
                        1.282075284005*cos( 6*M_PI*n/(N-1))+0.667777530266*cos( 8*M_PI*n/(N-1))-
                        0.240160796576*cos(10*M_PI*n/(N-1))+0.056656381764*cos(12*M_PI*n/(N-1))-
                        0.008134974479*cos(14*M_PI*n/(N-1))+0.000624544650*cos(16*M_PI*n/(N-1))-
                        0.000019808998*cos(18*M_PI*n/(N-1))+0.000000132974*cos(20*M_PI*n/(N-1));
        *overlap = 0.841;
        break;
    case WFUNC_BHARRIS:
        for (n = 0; n < N; n++)
            lut[n] = 0.35875-0.48829*cos(2*M_PI*n/(N-1))+0.14128*cos(4*M_PI*n/(N-1))-0.01168*cos(6*M_PI*n/(N-1));
        *overlap = 0.661;
        break;
    case WFUNC_BNUTTALL:
        for (n = 0; n < N; n++)
            lut[n] = 0.3635819-0.4891775*cos(2*M_PI*n/(N-1))+0.1365995*cos(4*M_PI*n/(N-1))-0.0106411*cos(6*M_PI*n/(N-1));
        *overlap = 0.661;
        break;
    case WFUNC_BHANN:
        for (n = 0; n < N; n++)
            lut[n] = 0.62-0.48*FFABS(n/(double)(N-1)-.5)-0.38*cos(2*M_PI*n/(N-1));
        *overlap = 0.5;
        break;
    case WFUNC_SINE:
        for (n = 0; n < N; n++)
            lut[n] = sin(M_PI*n/(N-1));
        *overlap = 0.75;
        break;
    case WFUNC_NUTTALL:
        for (n = 0; n < N; n++)
            lut[n] = 0.355768-0.487396*cos(2*M_PI*n/(N-1))+0.144232*cos(4*M_PI*n/(N-1))-0.012604*cos(6*M_PI*n/(N-1));
        *overlap = 0.663;
        break;
    default:
        av_assert0(0);
    }
}
