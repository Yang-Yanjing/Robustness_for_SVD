static int date_get_num(const char **pp,
                        int n_min, int n_max, int len_max)
{
    int i, val, c;
    const char *p;
    p = *pp;
    val = 0;
    for(i = 0; i < len_max; i++) {
        c = *p;
        if (!av_isdigit(c))
            break;
        val = (val * 10) + c - '0';
        p++;
    }
    
    if (p == *pp)
        return -1;
    if (val < n_min || val > n_max)
        return -1;
    *pp = p;
    return val;
}
