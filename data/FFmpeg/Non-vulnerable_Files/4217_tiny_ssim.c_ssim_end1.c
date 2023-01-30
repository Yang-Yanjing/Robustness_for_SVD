static float ssim_end1( int s1, int s2, int ss, int s12 )
{



#if BIT_DEPTH > 9
#define type float
    static const float ssim_c1 = .01*.01*PIXEL_MAX*PIXEL_MAX*64;
    static const float ssim_c2 = .03*.03*PIXEL_MAX*PIXEL_MAX*64*63;
#else
#define type int
    static const int ssim_c1 = (int)(.01*.01*PIXEL_MAX*PIXEL_MAX*64 + .5);
    static const int ssim_c2 = (int)(.03*.03*PIXEL_MAX*PIXEL_MAX*64*63 + .5);
#endif
    type fs1 = s1;
    type fs2 = s2;
    type fss = ss;
    type fs12 = s12;
    type vars = fss*64 - fs1*fs1 - fs2*fs2;
    type covar = fs12*64 - fs1*fs2;
    return (float)(2*fs1*fs2 + ssim_c1) * (float)(2*covar + ssim_c2)
         / ((float)(fs1*fs1 + fs2*fs2 + ssim_c1) * (float)(vars + ssim_c2));
#undef type
}
