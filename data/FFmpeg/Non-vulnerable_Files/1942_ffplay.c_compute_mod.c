static inline int compute_mod(int a, int b)
{
    return a < 0 ? a%b + b : a%b;
}
