#include <stdlib.h>
static void test_dictionary_validity (vlc_dictionary_t * p_dict, const char ** our_keys, int size )
{
    /* Test values and keys now */
    char ** keys = vlc_dictionary_all_keys( p_dict );
    intptr_t i, j;
    assert( keys );
    for( j = 0; keys[j]; j++ )
    {
        bool found = false;
        for( i = 0; i < size; i++ )
        {
            if(!strcmp( keys[j], our_keys[i] ))
            {
                found = true;
                break;
            }
        }
        free( keys[j] );
        assert( found );
    }
    free( keys );
    for( i = 0; i < size; i++ )
        assert( vlc_dictionary_value_for_key( p_dict, our_keys[i] ) == (void *)i );
}
