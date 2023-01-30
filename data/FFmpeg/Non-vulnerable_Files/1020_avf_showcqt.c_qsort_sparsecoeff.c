static inline int qsort_sparsecoeff(const SparseCoeff *a, const SparseCoeff *b)
{
    if (fabsf(a->value) >= fabsf(b->value))
        return 1;
    else
        return -1;
}
