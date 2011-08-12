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

  NxVec4 z_dir = {0.0f, 0.0f, 1.0f, 1.0f};
  NxVec4 orth_dir_f;
  nx_vec_cross_product (&orth_dir_f, dir, &z_dir);
  nx_vec_normalize4 (&orth_dir_f, &orth_dir_f);
  nx_vec_scale (&orth_dir_f, &orth_dir_f, 0.2f);

  NxVec4 idir;
  nx_vec_scale (&idir, dir, 0.8f);
  NxVec4 ivec;
  nx_vec_add (&ivec, &start, &idir);

  al_draw_line (projs[0].vals[0]*sf+ofx, projs[0].vals[1]*sf+ofx,
                projs[1].vals[0]*sf+ofx, projs[1].vals[1]*sf+ofx,
                clr, 1.0f);

  NxVec4 upper, lower;
  nx_vec_add (&upper, &ivec, &orth_dir_f);
  projs[0] = ivec;
  projs[1] = upper;
  sr_project_one (mst, &projs[0]);
  sr_project_one (mst, &projs[1]);
//  al_draw_line (projs[0].vals[0]*sf+ofx, projs[0].vals[1]*sf+ofx,
//                projs[1].vals[0]*sf+ofx, projs[1].vals[1]*sf+ofx,
//                clr, 1.0f);
  nx_vec_negate (&orth_dir_f, &orth_dir_f);
  nx_vec_add (&lower, &ivec, &orth_dir_f);
  projs[0] = ivec;
  projs[1] = lower;
  sr_project_one (mst, &projs[0]);
  sr_project_one (mst, &projs[1]);
//  al_draw_line (ivec.vals[0]*sf+ofx, ivec.vals[1]*sf+ofx,
//                lower.vals[0]*sf+ofx, lower.vals[1]*sf+ofx,
//                clr, 1.0f);
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

int
main (int argc, char **argv)
{
  ALLEGRO_DISPLAY *display;

  g_type_init ();

  al_init ();
  al_init_primitives_addon ();

  display = al_create_display (100, 100);
  g_xassert (display);

  ALLEGRO_BITMAP *bmp;
  ALLEGRO_COLOR clr;
  bmp = al_create_bitmap (10, 10);
  g_xassert (bmp);
  clr = al_map_rgb (255, 0, 0);

  al_set_target_bitmap (bmp);
  al_clear_to_color (clr);

  al_set_target_backbuffer (display);

  MaiModel *model;
  model = mai_model_new_from_file ("c_multipart_collada_skin_ms.dae");
  MaiNode *mesh_node;
  mesh_node = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mesh_node);

  /**
   * Plan
   *   Go make a node drawing function.
   *   Draws meshes and node direction vectors.
   */

  NxMat p_mat;
  nx_mat_projection (&p_mat, 1.0f);
  nx_mat_scale (&p_mat, 10.0f, 10.0f, 1.0f);
  nx_mat_translation (&p_mat, 1.0f, 1.0f, 0.0f);
  NxVec4 vec = {1.0f, 1.0f, -1.0f, 1.0f};

  sr_project_one (&p_mat, &vec);

  NxMat z_mat;
  nx_mat_projection (&z_mat, -1.0f);
  //nx_mat_ortho (&z_mat);
  nx_mat_translation (&z_mat, 0.0f, 0.0f, -3.0f);

  NxVec4 tri[] = {
      {0.0f, 0.0f, -1.0f, 1.0f},
      {1.0f, 0.0f, -1.0f, 1.0f},
      {1.0f, 1.0f, -1.0f, 1.0f}
  };

  int frame;
  for (frame=0; frame<60; ++frame)
    {
      al_clear_to_color (al_map_rgb (0, 0, 0));
      NxMat r_mat;
      r_mat = p_mat;
      nx_mat_rotate (&r_mat, 1.0f * frame, 0.0f, 0.3f, 1.0f);
      sr_draw_tri (&r_mat, tri);

      NxMat w_mat;
      w_mat = z_mat;
      NxVec4 rotvec = {0.0f, 1.0f, 1.0f, 0.0f};
      nx_vec_normalize4 (&rotvec, &rotvec);
      nx_mat_rotate (&w_mat, 2.0f * frame,
          rotvec.vals[0], rotvec.vals[1], rotvec.vals[2]);
      sr_draw_node (&w_mat, mesh_node->mesh_verts, mesh_node->mesh_indices, mesh_node->mesh_uvs);
      NxVec4 uvecs[2] = {{0.0f, 0.0f, 0.0f, 1.0f}, {3.0f, 3.0f, 0.0f, 1.0f}};
      sr_draw_unit_vec_at (&w_mat, &uvecs[0], &uvecs[1]);

      al_flip_display ();
      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}
