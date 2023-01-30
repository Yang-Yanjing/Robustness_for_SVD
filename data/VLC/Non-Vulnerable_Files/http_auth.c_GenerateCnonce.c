}
static char *GenerateCnonce()
{
    char ps_random[32];
    struct md5_s md5;
    vlc_rand_bytes( ps_random, sizeof( ps_random ) );
    InitMD5( &md5 );
    AddMD5( &md5, ps_random, sizeof( ps_random ) );
    EndMD5( &md5 );
    return psz_md5_hash( &md5 );
}
