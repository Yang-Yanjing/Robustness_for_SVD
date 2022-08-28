static void frac_add(FFFrac *f, int64_t incr)
{
    int64_t num, den;
    num = f->num + incr;
    den = f->den;
    if (num < 0) {
        f->val += num / den;
        num     = num % den;
        if (num < 0) {
            num += den;
            f->val--;
        }
    } else if (num >= den) {
        f->val += num / den;
        num     = num % den;
    }
    f->num = num;
}
