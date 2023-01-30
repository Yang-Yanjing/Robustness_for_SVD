enum { ft2_num_errors = sizeof(ft2_errorindex)/sizeof(*ft2_errorindex) };
const char *ft2_strerror(unsigned err)
{
    unsigned i;
    for( i=0; i<ft2_num_errors; ++i )
        if( err==ft2_errorindex[i] )
            break;
    return i<ft2_num_errors ? ft2_errorstrings[i] :
           "An error freetype2 neglected to specify";
}
