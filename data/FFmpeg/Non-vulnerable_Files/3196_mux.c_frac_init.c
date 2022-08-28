static void frac_init(FFFrac *f, int64_t val, int64_t num, int64_t den)
{
    num += (den >> 1);
    if (num >= den) {
        val += num / den;
        num  = num % den;
    }
    f->val = val;
    f->num = num;
    f->den = den;
}
