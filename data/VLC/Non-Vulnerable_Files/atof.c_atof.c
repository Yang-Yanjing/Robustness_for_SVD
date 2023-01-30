#ifndef __ANDROID__
double atof (const char *str)
{
    return strtod (str, NULL);
}
