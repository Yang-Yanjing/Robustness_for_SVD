}
static bool IsSimilar( int u, int v,
                       int refu, int refv, int reflength,
                       int i_satthres, int i_simthres )
{
    int length = sqrt(u * u + v * v);
    int diffu = refu * length - u * reflength;
    int diffv = refv * length - v * reflength;
    int64_t difflen2 = diffu * diffu + diffv * diffv;
    int64_t thres = length * reflength;
    thres *= thres;
    return length > i_satthres && (difflen2 * i_simthres < thres);
}
