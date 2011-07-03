#include <src/mai-node_funcs.h>

#define GET_NEW ((MaiNode *)g_object_new(mai_node_get_type(), NULL))

struct _MaiNode *
mai_node_new_from (struct aiNode *from, struct _MaiNode *parent)
{
  struct _MaiNode * self = GET_NEW;
  int cnt;

  self->parent = parent;
  g_xassert (from->mParent == NULL ? parent == NULL : 1);
  self->name = g_strdup (from->mName.data);
  if (parent == NULL)
    {
      self->children = NULL;
      return self;
    }

  self->children = g_ptr_array_new_with_free_func (g_object_unref);
  for (cnt=0; cnt<from->mNumChildren; ++cnt)
    {
      g_ptr_array_add (self->children, mai_node_new_from (from->mChildren[cnt], self));
    }

  return self;
}
