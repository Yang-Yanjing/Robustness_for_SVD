#ifndef NDEBUG
static void BlockNoRelease( block_t *b )
{
    fprintf( stderr, "block %p has no release callback! This is a bug!\n", b );
    abort();
}
