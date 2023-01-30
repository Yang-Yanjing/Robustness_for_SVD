}
static void usage (const char *path)
{
    printf (
"Usage: %s [-f] <path>\n"
"Generate the LibVLC plugins cache for the specified plugins directory.\n"
" -f, --force  forcefully reset the plugin cache (if it exists)\n",
            path);
}
