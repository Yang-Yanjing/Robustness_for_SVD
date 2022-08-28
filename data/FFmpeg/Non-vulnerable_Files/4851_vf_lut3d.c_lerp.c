static inline struct rgbvec lerp(const struct rgbvec *v0, const struct rgbvec *v1, float f)
{
    struct rgbvec v = {
        lerpf(v0->r, v1->r, f), lerpf(v0->g, v1->g, f), lerpf(v0->b, v1->b, f)
    };
    return v;
}
