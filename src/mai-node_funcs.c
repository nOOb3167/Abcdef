#include <src/ai_example.h>
#include <src/mai-node_funcs.h>

#define GET_NEW ((MaiNode *)g_object_new(mai_node_get_type(), NULL))

struct _MaiNode *
mai_node_new_from (struct aiScene *scene, struct aiNode *from, struct _MaiNode *parent)
{
  struct _MaiNode * self = GET_NEW;
  int cnt;

  g_xassert (from->mParent == NULL ? parent == NULL : 1);
  self->parent = parent;
  self->name = g_strdup (from->mName.data);

  self->transformation = g_new0 (CoglMatrix, 1);
  ai_matrix_to_cogl_matrix (&from->mTransformation, self->transformation);

  g_xassert (from->mNumMeshes <= 1);
  self->mesh_verts = g_array_new (FALSE, TRUE, sizeof (struct xvtx));
  if (from->mNumMeshes == 1)
    {
      struct aiMesh *mesh;
      mesh = scene->mMeshes[from->mMeshes[0]];
      for (cnt=0; cnt<mesh->mNumVertices; ++cnt)
        {
          struct xvtx vert;
          vert.x = mesh->mVertices[cnt].x;
          vert.y = mesh->mVertices[cnt].y;
          vert.z = mesh->mVertices[cnt].z;
          g_array_append_vals (self->mesh_verts, &vert, 1);
        }

      g_xassert (mesh->mNumFaces > 0);
      self->mesh_indices = g_array_new (FALSE, TRUE, sizeof (unsigned int));
      for (cnt=0; cnt<mesh->mNumFaces; ++cnt)
        {
          unsigned int index;
          index = mesh->mFaces[cnt].mIndices[0];
          g_array_append_vals (self->mesh_indices, &index, 1);
          index = mesh->mFaces[cnt].mIndices[1];
          g_array_append_vals (self->mesh_indices, &index, 1);
          index = mesh->mFaces[cnt].mIndices[2];
          g_array_append_vals (self->mesh_indices, &index, 1);
        }
    }

  self->children = g_ptr_array_new_with_free_func (g_object_unref);
  for (cnt=0; cnt<from->mNumChildren; ++cnt)
    {
      g_ptr_array_add (self->children, mai_node_new_from (scene, from->mChildren[cnt], self));
    }

  return self;
}

void
mai_node_draw_recursive (struct _MaiNode * self)
{
  CoglPrimitive *to_draw;
  to_draw = nx_cogl_primitive_new (self->mesh_verts, self->mesh_indices);

  cogl_set_source_color4ub ('\x1', '\x1', '\xFF', 255);
  cogl_ortho (0, 64, 0, 64, -1, 1);

  CoglMatrix idmtx;
  cogl_matrix_init_identity (&idmtx);
  cogl_set_modelview_matrix (&idmtx);

  cogl_translate (20.0f, 20.0f, 0.0f);
  cogl_scale (5.0f, 5.0f, 1.0f);

  nx_cogl_primitive_draw (to_draw);

  cogl_flush ();

  cogl_object_unref (to_draw);
}
