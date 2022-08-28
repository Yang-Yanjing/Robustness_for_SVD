static void framesync_sync_level_update(FFFrameSync *fs)
{
    unsigned i, level = 0;
    for (i = 0; i < fs->nb_in; i++)
        if (fs->in[i].state != STATE_EOF)
            level = FFMAX(level, fs->in[i].sync);
    av_assert0(level <= fs->sync_level);
    if (level < fs->sync_level)
        av_log(fs, AV_LOG_VERBOSE, "Sync level %u\n", level);
    if (level)
        fs->sync_level = level;
    else
        fs->eof = 1;
}
