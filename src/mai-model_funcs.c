#include <stdio.h>
#include <src/mai-model_funcs.h>

GHashTable *
_nx_mai_collect_node_map (MaiNode *from);


MaiModel *
mai_model_new_from (struct aiScene *scene)
{
  MaiModel *self = GET_NEW;

  g_xassert (scene->mRootNode);
  struct aiNode *root_node;
  root_node = scene->mRootNode;

  MaiNode *mn;
  mn = mai_node_new_from (scene, root_node, NULL);
  printf ("Nodename '%s'\n", mn->name);
  int tmp1;
  for (tmp1=0; tmp1<mn->children->len; ++tmp1)
    {
      printf ("Subname '%s'\n", ((MaiNode*)(g_ptr_array_index(mn->children, tmp1)))->name);
    }

  GHashTable *name_node_map;
  name_node_map = _nx_mai_collect_node_map (mn);

  {
    void pht (gpointer key, gpointer value, gpointer data)
    {
      printf ("KK %s\n", key);
    }
    g_hash_table_foreach (name_node_map, pht, NULL);
  }

  self->nodes = mn;
  self->name_node_map = name_node_map;

  self->anims = g_mai_anim_array_new ();

  g_xassert (scene->mNumAnimations >= 1);

  int cnt;
  for (cnt=0; cnt<scene->mNumAnimations; ++cnt)
    {
      MaiAnim *an;
      an = mai_anim_new_from (scene, scene->mAnimations[0]);
      g_mai_anim_array_append_val (self->anims, an);
    }

  return self;
}

GHashTable *
_nx_mai_collect_node_map (MaiNode *from)
{
  GHashTable *ret;
  ret = g_hash_table_new (g_str_hash, g_str_equal);

  void _collector (GHashTable *ht, MaiNode *node)
  {
    g_hash_table_insert (ht, g_strdup (node->name), node);
    if (node->children->len == 0)
      return;
    int cnt;
    for (cnt=0; cnt<node->children->len; ++cnt)
      _collector (ht, g_ptr_array_index(node->children, cnt));
  }

  _collector (ret, from);

  return ret;
}
