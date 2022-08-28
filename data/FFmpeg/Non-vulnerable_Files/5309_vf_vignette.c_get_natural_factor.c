static double get_natural_factor(const VignetteContext *s, int x, int y)
{
    const int xx = (x - s->x0) * s->xscale;
    const int yy = (y - s->y0) * s->yscale;
    const double dnorm = hypot(xx, yy) / s->dmax;
    if (dnorm > 1) {
        return 0;
    } else {
        const double c = cos(s->angle * dnorm);
        return (c*c)*(c*c); 
    }
}
