static inline double bound(const double threshold, const double val)
{
    const double CONST = 0.8862269254527580136490837416705725913987747280611935; 
    return erf(CONST * (val / threshold)) * threshold;
}
