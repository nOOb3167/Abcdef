#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>

#include <glib.h>
#include <src/error.h>
#include <src/ai_example.h>
#include <src/mai-model.h>
#include <src/mai-model_funcs.h>
#include <src/mai-node-anim.h>
#include <src/mai-node-anim_funcs.h>
#include <nx_mat.h>
#include <sr.h>

struct NxState *g_state;

void
sr_project_one (NxMat *mst, NxVec4 *vec_inout)
{
  NxVec4 temp;
  temp = *vec_inout;
  nx_mat_transform (mst, &temp);
  temp.vals[0] /= temp.vals[3];
  temp.vals[1] /= temp.vals[3];
  temp.vals[2] /= temp.vals[3];
  temp.vals[3] = 1.0f;
  *vec_inout = temp;
}

void
sr_draw_unit_vec_at (NxMat *mst, NxVec4 *pos, NxVec4 *dir)
{
  ALLEGRO_COLOR clr;
  clr = al_map_rgb (0, 255, 0);
  ALLEGRO_COLOR clr2;
  clr2 = al_map_rgb (0, 255, 0);


  float sf;
  sf = 20.0f;
  float ofx, ofy;
  ofx = 50.0f; ofy = 50.0f;

  NxVec4 start, end;
  start = *pos;
  nx_vec_add (&end, &start, dir);

  NxVec4 projs[2];
  projs[0] = start;
  projs[1] = end;
  sr_project_one (mst, &projs[0]);
  sr_project_one (mst, &projs[1]);

  NxVec4 s_pos;
  NxVec4 s_dir;
  s_pos = projs[0];
  nx_vec_sub (&s_dir, &projs[1], &projs[0]);
  NxVec4 z_dir = {0.0f, 0.0f, 1.0f, 1.0f};
  NxVec4 orth_f;
  nx_vec_cross_product (&orth_f, &s_dir, &z_dir);
  nx_vec_normalize3 (&orth_f, &orth_f);
  nx_vec_scale (&orth_f, &orth_f, 0.15f);

  NxVec4 ivec;
  nx_vec_scale (&ivec, &s_dir, 0.85f);
  nx_vec_add (&ivec, &s_pos, &ivec);

  al_draw_line (projs[0].vals[0]*sf+ofx, projs[0].vals[1]*sf+ofx,
                projs[1].vals[0]*sf+ofx, projs[1].vals[1]*sf+ofx,
                clr, 1.0f);

  NxVec4 projs2[2];

  NxVec4 upper, lower;
  nx_vec_add (&upper, &ivec, &orth_f);
  projs2[0] = projs[1];
  projs2[1] = upper;
  al_draw_line (projs2[0].vals[0]*sf+ofx, projs2[0].vals[1]*sf+ofx,
                projs2[1].vals[0]*sf+ofx, projs2[1].vals[1]*sf+ofx,
                clr2, 1.0f);
  nx_vec_negate (&orth_f, &orth_f);
  nx_vec_add (&lower, &ivec, &orth_f);
  projs2[0] = projs[1];
  projs2[1] = lower;
  al_draw_line (projs2[0].vals[0]*sf+ofx, projs2[0].vals[1]*sf+ofx,
                projs2[1].vals[0]*sf+ofx, projs2[1].vals[1]*sf+ofx,
                clr2, 1.0f);
}

void
sr_draw_tri (NxMat *mst, NxVec4 pts[3])
{
  ALLEGRO_COLOR clr;
  clr = al_map_rgb (255, 0, 0);

  float sf;
  sf = 20.0f;
  float ofx, ofy;
  ofx = 50.0f; ofy = 50.0f;

  NxVec4 projs[3];
  projs[0] = pts[0]; projs[1] = pts[1]; projs[2] = pts[2];
  sr_project_one (mst, &projs[0]);
  sr_project_one (mst, &projs[1]);
  sr_project_one (mst, &projs[2]);

  int cnt;
  for (cnt=0; cnt<3; ++cnt)
    {
      int cur, nxt;
      cur = cnt; nxt = (cnt + 1) % 3;
      al_draw_line (projs[cur].vals[0]*sf+ofx, projs[cur].vals[1]*sf+ofx,
                    projs[nxt].vals[0]*sf+ofx, projs[nxt].vals[1]*sf+ofx,
                    clr, 1.0f);
    }
}

void
sr_draw_node (NxMat *mst, GArray *verts, GArray *indices, GArray *uvs)
{
  g_xassert (indices->len > 0);
  g_xassert ((indices->len % 3) == 0);
  int cnt;
  for (cnt=0; cnt<indices->len; cnt+=3)
    {
      unsigned int idx;
      struct xvtx vert;
      NxVec4 xv[3];
      int cnt2;
      for (cnt2=0; cnt2<3; ++cnt2)
        {
          idx = g_array_index (indices, unsigned int, cnt+cnt2);
          g_xassert (idx < verts->len);
          vert = g_array_index (verts, struct xvtx, idx);
          xv[cnt2].vals[0] = vert.x; xv[cnt2].vals[1] = vert.y; xv[cnt2].vals[2] = vert.z; xv[cnt2].vals[3] = 1.0f;
        }
      sr_draw_tri (mst, xv);
    }
}

void
sr_update_global_ypr (ALLEGRO_KEYBOARD_STATE *aks)
{
  int left, right, up, down;
  left = al_key_down (aks, ALLEGRO_KEY_A);
  left = !!left;
  right = al_key_down (aks, ALLEGRO_KEY_D);
  right = !!right;
  up = al_key_down (aks, ALLEGRO_KEY_W);
  up = !!up;
  down= al_key_down (aks, ALLEGRO_KEY_S);
  down= !!down;

  if (al_key_down (aks, ALLEGRO_KEY_ESCAPE))
    {
      printf ("Escape\n");
      exit (0);
    }

  NxMat w_mat;
  w_mat = g_state->p_mat;
  if (left)
    g_state->yaw += 2.0f;
  if (right)
    g_state->yaw -= 2.0f;
  if (up)
    g_state->pitch += 2.0f;
  if (down)
    g_state->pitch -= 2.0f;

  nx_mat_rotate (&w_mat, g_state->yaw,
      0.0f, 1.0f, 0.0f);
  nx_mat_rotate (&w_mat, g_state->pitch,
      1.0f, 0.0f, 0.0f);

  g_state->w_mat = w_mat;
}

struct SrNode *
_sr_node_walk (struct SrNodeGraph *mdl, MaiNode *nde_foreign)
{
  struct SrNode *ret;

  ret = g_malloc0 (sizeof (*ret));

  g_hash_table_insert (mdl->name_node_map, nde_foreign->name, ret);

  ret->name = nde_foreign->name;
  ret->parent_name = nde_foreign->parent ? nde_foreign->parent->name : NULL;
  nx_mat_from_cogl_matrix (&ret->transformation, nde_foreign->transformation);

  ret->child_names_len = nde_foreign->children->len;
  ret->child_names = g_malloc0_n (nde_foreign->children->len, sizeof (*ret->child_names));

  int cnt;
  for (cnt=0; cnt<nde_foreign->children->len; ++cnt)
    {
      MaiNode *nnde;
      struct SrNode *xnnde;

      nnde = g_mai_node_ptr_array_index (nde_foreign->children, cnt);
      g_xassert (nnde);

      xnnde = _sr_node_walk (mdl, nnde);
      ret->child_names[cnt] = xnnde->name;
    }

  return ret;
}

void
sr_model_from_mai_model (struct SrNodeGraph **result, MaiModel *model)
{
  struct SrNodeGraph *ret;

  g_xassert (model->nodes);

  ret = g_malloc0 (sizeof (*ret));

  ret->name = "Err No Such Thing";
  ret->name_node_map = g_hash_table_new (g_str_hash, g_str_equal);
  ret->nodes_len = 1;
  ret->nodes = _sr_node_walk (ret, model->nodes);

  *result = ret;
}

void
sr_weight_dump (MaiModel *model)
{
  MaiNode *mesh_node;
  mesh_node = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mesh_node);
  g_xassert (mesh_node->bones->len == 2);

  int cnt;
  for (cnt=0; cnt<mesh_node->bones->len; ++cnt)
    {
      MaiBone *bone;
      bone = g_mai_bone_ptr_array_index (mesh_node->bones, cnt);
      printf ("Bone %s\n", bone->name);
      int idx;
      for (idx=0; idx<bone->weights->len; ++idx)
        {
          NxVertexWeight wt;
          wt = g_nx_vertex_weight_array_index (bone->weights, idx);
          printf ("%d: %f\n", wt.vertex_id, wt.weight);
        }
    }
}

void
sr_update_node_graph (MaiAnimInstance *mai, struct SrNodeGraph *graph)
{
  void
  sr_update_channel (MaiNodeAnim *man)
  {
    void
    sr_compose_mtx (NxMat *result)
    {
      /**
       * Stick to AnimEvaluator.cpp AnimEvaluator::Evaluate order.
       * Namely:
       *   Start with Rotation, apply 3x3 scale, set translation
       */
      struct NxAnimKey pos, rot, sca;
      pos = g_array_index (man->position_keys, struct NxAnimKey, mai->current_frame);
      rot = g_array_index (man->rotation_keys, struct NxAnimKey, mai->current_frame);
      sca = g_array_index (man->scaling_keys, struct NxAnimKey, mai->current_frame);

      NxMat mat;
      nx_mat_init_identity (&mat);

      float angle;
      struct xvtx axis;
      CoglQuaternion quat;
      quat.w = rot.val.rot.w;
      quat.x = rot.val.rot.x;
      quat.y = rot.val.rot.y;
      quat.z = rot.val.rot.z;
      nx_cogl_quaternion_to_rotation_axis_and_angle (&quat, &angle, &axis);
      nx_mat_rotate (&mat, angle, axis.x, axis.y, axis.z);

      NX_MAT_ELT (&mat, 0, 0) *= sca.val.vec.x;
      NX_MAT_ELT (&mat, 1, 0) *= sca.val.vec.x;
      NX_MAT_ELT (&mat, 2, 0) *= sca.val.vec.x;
      NX_MAT_ELT (&mat, 0, 1) *= sca.val.vec.y;
      NX_MAT_ELT (&mat, 1, 1) *= sca.val.vec.y;
      NX_MAT_ELT (&mat, 2, 1) *= sca.val.vec.y;
      NX_MAT_ELT (&mat, 0, 2) *= sca.val.vec.z;
      NX_MAT_ELT (&mat, 1, 2) *= sca.val.vec.z;
      NX_MAT_ELT (&mat, 2, 2) *= sca.val.vec.z;

      NX_MAT_ELT (&mat, 0, 3) = pos.val.vec.x;
      NX_MAT_ELT (&mat, 1, 3) = pos.val.vec.y;
      NX_MAT_ELT (&mat, 2, 3) = pos.val.vec.z;

      *result = mat;
    }

    MaiNode *mn;
    struct SrNode *sn;

    sn = g_hash_table_lookup (graph->name_node_map, man->node_name);
    g_xassert (sn);

    NxMat current_mat;
    sr_compose_mtx (&current_mat);

    sn->transformation = current_mat;
  }

  MaiAnim *ma;
  ma = mai->anim;

  for (int cnt = 0; cnt < ma->channels->len; ++cnt)
    {
      sr_update_channel (g_mai_node_anim_ptr_array_index (
          ma->channels, cnt));
    }
}

void
sr_skeletal (MaiModel *model, GArray *trans_verts_f)
{
  MaiAnimInstance *mai;

  g_xassert (model->anims->len == 1);
  mai = mai_anim_instance_new_from_anim (
                                         g_mai_anim_ptr_array_index (model->anims, 0),
                                         model->name_node_map,
                                         model->nodes);

  struct SrNodeGraph *sr_model;
  sr_model_from_mai_model (&sr_model, model);
  struct SrNode *cube_node;
  cube_node = g_hash_table_lookup (sr_model->name_node_map, "Cube");
  g_xassert (cube_node);

  /**
   * The copy routine is bad. Really bad.
   * sr_model does not survive update_node_graph.
   * What happens is sr_node_graph_copy does just
   * g_hash_table_ref, it should make a new one and populate
   * similar to _sr_node_walk.
   */
  struct SrNodeGraph *sr_model_copy;
  sr_node_graph_copy (&sr_model_copy, sr_model);
  g_xassert (sr_model_copy);

  sr_update_node_graph (mai, sr_model_copy);

  void
  sr_bone_mtx (NxMat *result, MaiBone *bone, struct SrNode *inv_mesh_node)
  {
    void
    sr_node_accumulate (NxMat *result, struct SrNode *src)
    {
      NxMat
      _sr_node_accumulate (struct SrNode *cur_src)
      {
        NxMat par_mtx;
        struct SrNode *par_node;
        gboolean found;

        nx_mat_init_identity (&par_mtx);

        /**
         * Name can be NULL (For topmost/root node).
         * lookup_extended can handle search for NULL keys.
         * Whereas lookup just segfaults.
         * Quoting documentation: "You can actually pass NULL for lookup_key"
         * But that seems a lie, I get segfaults anyway so guard against NULL.
         */
        if (0 == cur_src->parent_name)
          found = 0;
        else
          found = g_hash_table_lookup_extended (sr_model->name_node_map, cur_src->parent_name,
                                                 NULL, (void **)&par_node);
        if (0 == found)
          return par_mtx;

        g_xassert (par_node);

        NxMat result;
        par_mtx = _sr_node_accumulate (par_node);
        nx_mat_multiply(&result, &par_mtx, &cur_src->transformation);
        return result;
      }

      *result = _sr_node_accumulate (src);
    }

    NxMat offset_mtx;
    nx_mat_from_cogl_matrix (&offset_mtx, bone->offset_matrix);

    struct SrNode *tmp;
    tmp = g_hash_table_lookup (sr_model->name_node_map, bone->name);
    g_xassert (tmp);

    NxMat bone_ws;
    sr_node_accumulate (&bone_ws, tmp);

    NxMat inv_mesh_ws;
    sr_node_accumulate (&inv_mesh_ws, inv_mesh_node);
    nx_mat_get_inverse (&inv_mesh_ws, &inv_mesh_ws);

    /**
     * Probably also want inverse mesh node but whatever
     *
     * Edit:
     *   Shouldn't it be offset_mtx -> bone_ws -> inv_mesh_ws, missing bone_ws?
     */
    NxMat bone_mtx;
    nx_mat_init_identity (&bone_mtx);
    nx_mat_multiply (&bone_mtx, &bone_mtx, &offset_mtx);
    nx_mat_multiply (&bone_mtx, &bone_mtx, &bone_ws);
    nx_mat_multiply (&bone_mtx, &bone_mtx, &inv_mesh_ws);

    *result = bone_mtx;
  }

  /**
   * Warning implicit mesh selection
   */
  MaiNode *mn;
  mn = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mn);
  struct SrNode *mn_sr;
  mn_sr = g_hash_table_lookup (sr_model->name_node_map, "Cube");
  g_xassert (mn_sr);

  /* (string, NxMat *) */
  GHashTable *name_bone_mtx_map;
  name_bone_mtx_map = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  for (int cnt = 0; cnt < mn->bones->len; ++cnt)
    {
      MaiBone *bone;
      bone = g_mai_bone_ptr_array_index (mn->bones, cnt);
      NxMat *bone_mtx;
      bone_mtx = g_malloc0 (sizeof (*bone_mtx));
      sr_bone_mtx (bone_mtx, bone, mn_sr);

      g_hash_table_insert (name_bone_mtx_map, g_strdup (bone->name), bone_mtx);
    }

  /* (vert_id, (dummy, MaiBone *)) */
  /* In for(vtx) loop, link bone->name with name_bone_mtx map
   * then bone->weights[vtx] for weight */
  GPtrArray *vbmap;
  vbmap = g_ptr_array_new ();

  for (int cnt = 0; cnt < mn->mesh_verts->len; ++cnt)
    {
      g_ptr_array_add (vbmap, g_ptr_array_new ());
    }

  for (int cnt = 0; cnt < mn->bones->len; ++cnt)
    {
      MaiBone *bone;
      bone = g_mai_bone_ptr_array_index (mn->bones, cnt);
      for (int vid = 0; vid < bone->weights->len; ++vid)
        {
          NxVertexWeight vw;
          vw = g_nx_vertex_weight_array_index (bone->weights, vid);
          g_xassert (vw.vertex_id < vbmap->len);

          GPtrArray *varr;
          varr = g_ptr_array_index (vbmap, vw.vertex_id);
          g_ptr_array_add (varr, bone);
        }
    }

  /**
   * Now go transform some verts
   */
  g_xassert (mn->mesh_verts->len);
  for (int cnt = 0; cnt < mn->mesh_verts->len; ++cnt)
    {
      struct xvtx v1;
      NxVec4 v2;

      v1 = g_array_index (mn->mesh_verts, struct xvtx, cnt);
      v2 = (typeof (v2)) {v1.x, v1.y, v1.z, 1.0f};

      NxVec4 cumulative;
      cumulative = (typeof (cumulative)) {0.0f, 0.0f, 0.0f, 0.0f};

      GPtrArray *varr;
      varr = g_ptr_array_index (vbmap, cnt);
      g_xassert (("Verts referenced by no bones dont get transform",varr->len));
      for (int bid = 0; bid < varr->len; ++bid)
        {
          MaiBone *bone;
          bone = g_ptr_array_index (varr, bid);

          NxMat *bone_mtx;
          bone_mtx = g_hash_table_lookup (name_bone_mtx_map, bone->name);
          g_xassert (bone_mtx);

          NxVertexWeight vertex_weight;
          vertex_weight = g_nx_vertex_weight_array_index (bone->weights, cnt);

          NxVec4 partial;
          partial = v2;
          nx_mat_transform (bone_mtx, &partial);
          nx_vec_scale (&partial, &partial, vertex_weight.weight);

          nx_vec_add (&cumulative, &cumulative, &partial);

          int a = floor(0);
        }

      struct xvtx vout = {cumulative.vals[0], cumulative.vals[1], cumulative.vals[2]};
      g_array_append_vals (trans_verts_f, &vout, 1);
    }
}

int
main (int argc, char **argv)
{
  ALLEGRO_DISPLAY *display;

  g_type_init ();

  /**
   * Initialize g_state
   */
  struct NxState state = {0};
  g_state = g_new0 (struct NxState, 1);
  *g_state = state;

  NxMat z_mat;
  nx_mat_projection (&z_mat, -1.0f);
  //nx_mat_ortho (&z_mat);
  nx_mat_translation (&z_mat, 0.0f, 0.0f, -3.0f);
  g_state->p_mat = z_mat;
  g_state->w_mat = z_mat;

  al_init ();
  al_init_primitives_addon ();

  al_install_keyboard ();
  al_install_mouse ();

  display = al_create_display (100, 100);
  g_xassert (display);

  al_set_target_backbuffer (display);

  MaiModel *model;
  model = mai_model_new_from_file ("c_sr_weight.dae");
  MaiNode *mesh_node;
  mesh_node = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mesh_node);

  sr_weight_dump (model);

  GArray *trans_verts;
  trans_verts = g_array_new (FALSE, TRUE, sizeof (struct xvtx));
  sr_skeletal (model, trans_verts);

  /**
   * Plan
   *   Go make a node drawing function.
   *   Draws meshes and node direction vectors.
   */

  ALLEGRO_KEYBOARD_STATE aks;

  int frame;
  for (frame=0; frame<60; ++frame)
    {
      al_get_keyboard_state (&aks);
      sr_update_global_ypr (&aks);
      al_clear_to_color (al_map_rgb (0, 0, 0));
      sr_draw_node (&g_state->w_mat, trans_verts, mesh_node->mesh_indices, mesh_node->mesh_uvs);
      NxVec4 uvecs[2] = {{0.0f, 0.0f, 0.0f, 1.0f}, {3.0f, 3.0f, 0.0f, 1.0f}};
      sr_draw_unit_vec_at (&g_state->w_mat, &uvecs[0], &uvecs[1]);

      al_flip_display ();
      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}

struct SrNode *
_sr_copy_node_walk (struct SrNodeGraph *res_mdl,
                    struct SrNodeGraph *orig_mdl,
                    struct SrNode *nde)
{
  struct SrNode *ret;

  ret = g_malloc0 (sizeof (*ret));

  g_hash_table_insert (res_mdl->name_node_map, g_strdup (nde->name), ret);

  /**
   * Lol not ref counted or actually copied
   */

  ret->child_names = g_malloc0 (sizeof (*ret->child_names) * nde->child_names_len);
  memcpy (ret->child_names, nde->child_names, sizeof (*ret->child_names) * nde->child_names_len);

  ret->child_names_len = nde->child_names_len;
  ret->name = g_strdup (nde->name);
  ret->parent_name = g_strdup (nde->parent_name);
  ret->transformation = nde->transformation;

  for (int cnt = 0; cnt < ret->child_names_len; ++cnt)
    {
      struct SrNode *tc;
      tc = g_hash_table_lookup (orig_mdl->name_node_map, ret->child_names[cnt]);
      g_xassert (tc);
      _sr_copy_node_walk (res_mdl, orig_mdl, tc);
    }

  return ret;
}

void
sr_node_graph_copy (struct SrNodeGraph **result, struct SrNodeGraph *what)
{
  struct SrNodeGraph *ret;

  ret = g_malloc0 (sizeof (*ret));
  ret->name = g_strdup (what->name);
  ret->name_node_map = g_hash_table_new (g_str_hash, g_str_equal);
  ret->nodes_len = what->nodes_len;
  ret->nodes = _sr_copy_node_walk (ret, what, what->nodes);

  *result = ret;
}

void
nx_mat_from_cogl_matrix (NxMat *mat, CoglMatrix *cogl_matrix)
{
  g_xassert (mat);
  g_xassert (cogl_matrix);

  float tmp_vals[16] = {
      cogl_matrix->xx, cogl_matrix->yx, cogl_matrix->zx, cogl_matrix->wx,
      cogl_matrix->xy, cogl_matrix->yy, cogl_matrix->zy, cogl_matrix->wy,
      cogl_matrix->xz, cogl_matrix->yz, cogl_matrix->zz, cogl_matrix->wz,
      cogl_matrix->xw, cogl_matrix->yw, cogl_matrix->zw, cogl_matrix->ww,
  };

  nx_mat_init_from_array (mat, tmp_vals);
}
