#include <stdio.h>
#include <src/ai_example.h>
#include <src/mai-model_funcs.h>

#define GET_NEW ((MaiModel *)g_object_new(mai_model_get_type(), NULL))

MaiModel *
_mai_model_new_from_scene (struct aiScene *scene);

GHashTable *
_nx_mai_collect_node_map (MaiNode *from);


MaiModel *
mai_model_new_from (struct aiScene *scene)
{
  MaiModel *ret;
  ret = _mai_model_new_from_scene (scene);

  printf ("Nodename '%s'\n", ret->nodes->name);
  int tmp1;
  for (tmp1=0; tmp1<ret->nodes->children->len; ++tmp1)
    {
      printf ("Subname '%s'\n", ((MaiNode*)(g_mai_node_ptr_array_index(ret->nodes->children, tmp1)))->name);
    }

  void pht (gpointer key, gpointer value, gpointer data)
  {
    printf ("KK %s\n", key);
  }
  g_hash_table_foreach (ret->name_node_map, pht, NULL);

  return ret;
}

MaiModel *
mai_model_new_from_file (const char *file_name)
{
  struct aiScene *scene;
  scene = ai_scene_from_file (file_name);

  return _mai_model_new_from_scene (scene);
}

MaiModel *
_mai_model_new_from_scene (struct aiScene *scene)
{
  MaiModel *self = GET_NEW;

  g_xassert (scene->mRootNode);
  struct aiNode *root_node;
  root_node = scene->mRootNode;

  MaiNode *mn;
  mn = mai_node_new_from (scene, root_node, NULL);

  GHashTable *name_node_map;
  name_node_map = _nx_mai_collect_node_map (mn);


  self->name_node_map = name_node_map;
  self->nodes = mn;

  self->anims = g_mai_anim_ptr_array_new ();

  g_xassert (scene->mNumAnimations >= 1);

  int cnt;
  for (cnt=0; cnt<scene->mNumAnimations; ++cnt)
    {
      MaiAnim *an;
      an = mai_anim_new_from (scene, scene->mAnimations[cnt]);
      g_mai_anim_ptr_array_add (self->anims, an);
    }

  return self;
}

MaiAnimInstance *
mai_model_get_anim_by_name (MaiModel *self, char * name)
{
  MaiAnimInstance *ret;
  MaiAnim *matching_anim;

  int cnt;
  matching_anim = NULL;
  for (cnt=0; cnt<=self->anims->len; ++cnt)
    {
      MaiAnim *candidate;
      candidate = g_mai_anim_ptr_array_index (self->anims, cnt);
      if (0 == g_strcmp0 (name, candidate->name))
        {
          matching_anim = candidate;
          break;
        }
    }
  g_xassert (NULL != matching_anim);

  ret = mai_anim_instance_new_from_anim (matching_anim, self->name_node_map, self->nodes);

  return ret;
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
      _collector (ht, g_mai_node_ptr_array_index(node->children, cnt));
  }

  _collector (ret, from);

  return ret;
}
