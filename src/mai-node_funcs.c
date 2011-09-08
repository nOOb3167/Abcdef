#include <src/ai_example.h>
#include <src/mai-node.h>

#define GET_NEW ((MaiNode *)g_object_new(mai_node_get_type(), NULL))

XPTR_ARRAY_MACRO_TYPE_DEFINE (mai_node, MaiNode);

MaiNode *
mai_node_new_from (struct aiScene *scene, struct aiNode *from, MaiNode *parent)
{
  MaiNode * self = GET_NEW;

  g_xassert (from->mParent == NULL ? parent == NULL : TRUE);
  g_xassert (from->mNumMeshes <= 1);

  self->parent = parent;
  if (parent != NULL)
      g_object_ref (parent);

  self->name = g_strdup (from->mName.data);

  self->transformation = g_malloc0 (sizeof (*self->transformation));
  ai_matrix_to_cogl_matrix (&from->mTransformation, self->transformation);

  self->mesh_verts = g_array_new (FALSE, TRUE, sizeof (struct xvtx));
  self->mesh_indices = g_array_new (FALSE, TRUE, sizeof (unsigned int));
  self->mesh_uvs = g_array_new (FALSE, TRUE, sizeof (struct xvtx));
  self->bones = g_mai_bone_ptr_array_new ();

  if (from->mNumMeshes == 1)
    {
      struct aiMesh *mesh;

      /**
       * Warning:
       *   Assumes nodes have only one mesh each (Or none at all).
       *   Gets the first one.
       */
      mesh = scene->mMeshes[from->mMeshes[0]];

      g_xassert (mesh->mNumVertices > 0);
      g_xassert (mesh->mNumFaces > 0);
      g_xassert (mesh->mNumUVComponents[0] == 2);

      for (gint i = 0; i < mesh->mNumVertices; ++i)
        {
          struct xvtx vert;
          vert.x = mesh->mVertices[i].x;
          vert.y = mesh->mVertices[i].y;
          vert.z = mesh->mVertices[i].z;
          g_array_append_vals (self->mesh_verts, &vert, 1);
        }

      for (gint i = 0; i < mesh->mNumFaces; ++i)
        {
          unsigned int index;
          index = mesh->mFaces[i].mIndices[0];
          g_array_append_vals (self->mesh_indices, &index, 1);
          index = mesh->mFaces[i].mIndices[1];
          g_array_append_vals (self->mesh_indices, &index, 1);
          index = mesh->mFaces[i].mIndices[2];
          g_array_append_vals (self->mesh_indices, &index, 1);
        }

      /* mTextureCoords[x] is mNumVertices entries in size */
      for (gint i = 0; i < mesh->mNumVertices; ++i)
        {
          struct xvtx uvcoord;
          uvcoord.x = mesh->mTextureCoords[0][i].x;
          uvcoord.y = mesh->mTextureCoords[0][i].y;
          uvcoord.z = mesh->mTextureCoords[0][i ].z;
          g_array_append_vals (self->mesh_uvs, &uvcoord, 1);
        }

      for (gint i = 0; i < mesh->mNumBones; ++i)
        {
          MaiBone *bone;
          bone = mai_bone_new_from (mesh->mBones[i]);
          g_mai_bone_ptr_array_add (self->bones, bone);
        }
    }

  self->children = g_mai_node_ptr_array_new ();
  for (gint i = 0; i < from->mNumChildren; ++i)
    {
      g_mai_node_ptr_array_add (self->children,
                                mai_node_new_from (scene, from->mChildren[i], self));
    }

  return self;
}

void
mai_node_draw_recursive (MaiNode *self)
{
  CoglMatrix initial_mtx;
  cogl_matrix_init_identity (&initial_mtx);
  cogl_set_modelview_matrix (&initial_mtx);

  cogl_set_source_color4ub ('\x1', '\x1', '\xFF', 255);
  cogl_set_source_texture (g_testtex);
  cogl_ortho (0, 64, 0, 64, -1, 1);

  cogl_matrix_translate (&initial_mtx, 20.0f, 20.0f, 0.0f);
  cogl_matrix_scale (&initial_mtx, 5.0f, 5.0f, 1.0f);
  cogl_matrix_rotate(&initial_mtx, -90.0f, 1.0f, 0.0f, 0.0f);

  _mai_node_draw_recursive (self, &initial_mtx);

  cogl_flush ();
}

void
_mai_node_draw_recursive (MaiNode *self, CoglMatrix *acc_mtx)
{
  CoglPrimitive *to_draw;
  to_draw = nx_cogl_primitive_new (self->mesh_verts, self->mesh_indices, self->mesh_uvs);

  CoglMatrix cur_mtx;
  cogl_matrix_init_identity (&cur_mtx);
  cogl_matrix_multiply (&cur_mtx, &cur_mtx, acc_mtx);
  cogl_matrix_multiply (&cur_mtx, &cur_mtx, self->transformation);

  cogl_set_modelview_matrix (&cur_mtx);

  nx_cogl_primitive_draw (to_draw);

  cogl_object_unref (to_draw);

  int tmp1;
  for (tmp1=0; tmp1<self->children->len; ++tmp1)
    {
      MaiNode *child;
      child = g_mai_node_ptr_array_index (self->children, tmp1);
      _mai_node_draw_recursive (child, &cur_mtx);
    }
}
