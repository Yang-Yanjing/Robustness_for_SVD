#endif
lldiv_t lldiv (long long num, long long denom)
{
    lldiv_t d = { num / denom, num % demon, };
    return d;
}