static int64_t int_sin(int64_t a)
{
    if (a < 0)
        a = MY_PI - a;  
    a %= 2 * MY_PI;     
    if (a >= MY_PI * 3 / 2)
        a -= 2 * MY_PI; 
    if (a >= MY_PI / 2)
        a = MY_PI - a;  
    return a - int_pow(a, 3) / 6 + int_pow(a, 5) / 120 - int_pow(a, 7) / 5040;
}
