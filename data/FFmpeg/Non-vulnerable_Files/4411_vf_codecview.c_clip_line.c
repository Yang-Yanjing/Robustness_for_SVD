static int clip_line(int *sx, int *sy, int *ex, int *ey, int maxx)
{
    if(*sx > *ex)
        return clip_line(ex, ey, sx, sy, maxx);
    if (*sx < 0) {
        if (*ex < 0)
            return 1;
        *sy = *ey + (*sy - *ey) * (int64_t)*ex / (*ex - *sx);
        *sx = 0;
    }
    if (*ex > maxx) {
        if (*sx > maxx)
            return 1;
        *ey = *sy + (*ey - *sy) * (int64_t)(maxx - *sx) / (*ex - *sx);
        *ex = maxx;
    }
    return 0;
}
