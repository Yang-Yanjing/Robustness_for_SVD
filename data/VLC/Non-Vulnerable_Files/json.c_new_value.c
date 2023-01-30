}
static int new_value
   (json_state * state, json_value ** top, json_value ** root, json_value ** alloc, json_type type)
{
   json_value * value;
   int values_size;
   if (!state->first_pass)
   {
      value = *top = *alloc;
      *alloc = (*alloc)->_reserved.next_alloc;
      if (!*root)
         *root = value;
      switch (value->type)
      {
         case json_array:
            if (! (value->u.array.values = (json_value **) json_alloc
               (state, value->u.array.length * sizeof (json_value *), 0)) )
            {
               return 0;
            }
            value->u.array.length = 0;
            break;
         case json_object:
            values_size = sizeof (*value->u.object.values) * value->u.object.length;
            if (! ((*(void **) &value->u.object.values) = json_alloc
                  (state, values_size + ((unsigned long) value->u.object.values), 0)) )
            {
               return 0;
            }
            value->_reserved.object_mem = (*(char **) &value->u.object.values) + values_size;
            value->u.object.length = 0;
            break;
         case json_string:
            if (! (value->u.string.ptr = (json_char *) json_alloc
               (state, (value->u.string.length + 1) * sizeof (json_char), 0)) )
            {
               return 0;
            }
            value->u.string.length = 0;
            break;
         default:
            break;
      };
      return 1;
   }
   value = (json_value *) json_alloc (state, sizeof (json_value), 1);
   if (!value)
      return 0;
   if (!*root)
      *root = value;
   value->type = type;
   value->parent = *top;
   if (*alloc)
      (*alloc)->_reserved.next_alloc = value;
   *alloc = *top = value;
   return 1;
}
