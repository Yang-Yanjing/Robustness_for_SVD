}
int main( void )
{
    log( "Testing config chain escaping\n" );
    test_config_StringEscape();
    log( "Testing config chain un-escaping\n" );
    test_config_StringUnEscape();
    log( "Testing config_ChainCreate()\n" );
    test_config_ChainCreate();
    log( "Testing config_ChainDuplicate()\n" );
    test_config_ChainDuplicate();
    return 0;
}
