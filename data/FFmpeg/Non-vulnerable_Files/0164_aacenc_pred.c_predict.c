static inline void predict(PredictorState *ps, float *coef, float *rcoef, int set)
{
    float k2;
    const float a     = 0.953125; 
    const float alpha = 0.90625;  
    const float   k1 = ps->k1;
    const float   r0 = ps->r0,     r1 = ps->r1;
    const float cor0 = ps->cor0, cor1 = ps->cor1;
    const float var0 = ps->var0, var1 = ps->var1;
    const float e0 = *coef - ps->x_est;
    const float e1 = e0 - k1 * r0;
    if (set)
        *coef = e0;
    ps->cor1 = flt16_trunc(alpha * cor1 + r1 * e1);
    ps->var1 = flt16_trunc(alpha * var1 + 0.5f * (r1 * r1 + e1 * e1));
    ps->cor0 = flt16_trunc(alpha * cor0 + r0 * e0);
    ps->var0 = flt16_trunc(alpha * var0 + 0.5f * (r0 * r0 + e0 * e0));
    ps->r1   = flt16_trunc(a * (r0 - k1 * e0));
    ps->r0   = flt16_trunc(a * e0);
    
    ps->k1   = ps->var0 > 1 ? ps->cor0 * flt16_even(a / ps->var0) : 0;
    k2       = ps->var1 > 1 ? ps->cor1 * flt16_even(a / ps->var1) : 0;
    *rcoef   = ps->x_est = flt16_round(ps->k1*ps->r0 + k2*ps->r1);
}
