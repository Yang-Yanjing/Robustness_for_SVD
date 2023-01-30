AVFILTER_DEFINE_CLASS(deshake);
static int cmp(const double *a, const double *b)
{
    return *a < *b ? -1 : ( *a > *b ? 1 : 0 );
}
