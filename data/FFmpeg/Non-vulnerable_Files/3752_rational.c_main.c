int main(void)
{
    AVRational a,b,r;
    for (a.num = -2; a.num <= 2; a.num++) {
        for (a.den = -2; a.den <= 2; a.den++) {
            for (b.num = -2; b.num <= 2; b.num++) {
                for (b.den = -2; b.den <= 2; b.den++) {
                    int c = av_cmp_q(a,b);
                    double d = av_q2d(a) == av_q2d(b) ?
                               0 : (av_q2d(a) - av_q2d(b));
                    if (d > 0)       d = 1;
                    else if (d < 0)  d = -1;
                    else if (d != d) d = INT_MIN;
                    if (c != d)
                        av_log(NULL, AV_LOG_ERROR, "%d/%d %d/%d, %d %f\n", a.num,
                               a.den, b.num, b.den, c,d);
                    r = av_sub_q(av_add_q(b,a), b);
                    if(b.den && (r.num*a.den != a.num*r.den || !r.num != !a.num || !r.den != !a.den))
                        av_log(NULL, AV_LOG_ERROR, "%d/%d ", r.num, r.den);
                }
            }
        }
    }
    for (a.num = 1; a.num <= 10; a.num++) {
        for (a.den = 1; a.den <= 10; a.den++) {
            if (av_gcd(a.num, a.den) > 1)
                continue;
            for (b.num = 1; b.num <= 10; b.num++) {
                for (b.den = 1; b.den <= 10; b.den++) {
                    int start;
                    if (av_gcd(b.num, b.den) > 1)
                        continue;
                    if (av_cmp_q(b, a) < 0)
                        continue;
                    for (start = 0; start < 10 ; start++) {
                        int acc= start;
                        int i;
                        for (i = 0; i<100; i++) {
                            int exact = start + av_rescale_q(i+1, b, a);
                            acc = av_add_stable(a, acc, b, 1);
                            if (FFABS(acc - exact) > 2) {
                                av_log(NULL, AV_LOG_ERROR, "%d/%d %d/%d, %d %d\n", a.num,
                                       a.den, b.num, b.den, acc, exact);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    for (a.den = 1; a.den < 0x100000000U/3; a.den*=3) {
        for (a.num = -1; a.num < (1<<27); a.num += 1 + a.num/100) {
            float f  = av_int2float(av_q2intfloat(a));
            float f2 = av_q2d(a);
            if (fabs(f - f2) > fabs(f)/5000000) {
                av_log(NULL, AV_LOG_ERROR, "%d/%d %f %f\n", a.num,
                       a.den, f, f2);
                return 1;
            }
        }
    }
    return 0;
}
