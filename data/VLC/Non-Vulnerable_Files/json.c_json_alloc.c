} json_state;
static void * json_alloc (json_state * state, unsigned long size, int zero)
{
   void * mem;
   if ((state->ulong_max - state->used_memory) < size)
      return 0;
   if (state->settings.max_memory
         && (state->used_memory += size) > state->settings.max_memory)
   {
      return 0;
   }
   if (! (mem = zero ? calloc (size, 1) : malloc (size)))
      return 0;
   return mem;
}
