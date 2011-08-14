#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>

#include <glib.h>
#include <src/error.h>
#include <src/mai-model.h>
#include <src/mai-model_funcs.h>
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

void sr_weight_dump (MaiModel *model)
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
      sr_draw_node (&g_state->w_mat, mesh_node->mesh_verts, mesh_node->mesh_indices, mesh_node->mesh_uvs);
      NxVec4 uvecs[2] = {{0.0f, 0.0f, 0.0f, 1.0f}, {3.0f, 3.0f, 0.0f, 1.0f}};
      sr_draw_unit_vec_at (&g_state->w_mat, &uvecs[0], &uvecs[1]);

      al_flip_display ();
      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}
