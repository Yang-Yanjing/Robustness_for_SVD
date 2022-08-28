static inline float hermite_interpolation(float x, float x0, float x1,
                                          float p0, float p1,
                                          float m0, float m1)
{
    float width = x1 - x0;
    float t = (x - x0) / width;
    float t2, t3;
    float ct0, ct1, ct2, ct3;
    m0 *= width;
    m1 *= width;
    t2 = t*t;
    t3 = t2*t;
    ct0 = p0;
    ct1 = m0;
    ct2 = -3 * p0 - 2 * m0 + 3 * p1 - m1;
    ct3 = 2 * p0 + m0  - 2 * p1 + m1;
    return ct3 * t3 + ct2 * t2 + ct1 * t + ct0;
}
