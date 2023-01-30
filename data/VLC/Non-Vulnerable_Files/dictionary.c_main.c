}
int main (void)
{
    static const char * our_keys[] = {
        "Hello", "Hella", "flowmeter", "Frostnipped", "frostnipped", "remiform", "quadrifoliolate", "singularity", "unafflicted"
    };
    const int size = sizeof(our_keys)/sizeof(our_keys[0]);
    char ** keys;
    intptr_t i = 0;
    vlc_dictionary_t dict;
    vlc_dictionary_init( &dict, 0 );
    assert( vlc_dictionary_keys_count( &dict ) == 0 );
    keys = vlc_dictionary_all_keys( &dict );
    assert( keys && !keys[0] );
    free(keys);
    /* Insert some values */
    for( i = 0; i < size; i++ )
        vlc_dictionary_insert( &dict, our_keys[i], (void *)i );
    test_dictionary_validity( &dict, our_keys, size );
    vlc_dictionary_remove_value_for_key( &dict, our_keys[size-1], NULL, NULL );
    test_dictionary_validity( &dict, our_keys, size-1 );
    vlc_dictionary_clear( &dict, NULL, NULL );
    assert( vlc_dictionary_keys_count( &dict ) == 0 );
    return 0;
}
