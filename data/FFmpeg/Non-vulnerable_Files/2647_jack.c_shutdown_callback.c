static void shutdown_callback(void *arg)
{
    JackData *self = arg;
    self->client = NULL;
}
