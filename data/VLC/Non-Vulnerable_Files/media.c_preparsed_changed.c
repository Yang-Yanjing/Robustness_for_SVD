#include "test.h"
static void preparsed_changed(const libvlc_event_t *event, void *user_data)
{
    (void)event;
    int *received = user_data;
    *received = true;
}
