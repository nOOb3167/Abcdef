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
sr_draw_tri (NxMat *mst, NxVec4 pts[3])
{
  ALLEGRO_COLOR clr;
  clr = al_map_rgb (255, 0, 0);

  NxVec4 cur;
  NxVec4 nxt;
  cur = pts[0];
  sr_project_one (mst, &cur);
  nxt = pts[1];
  sr_project_one (mst, &nxt);
  al_draw_line (cur.vals[0], cur.vals[1], nxt.vals[0], nxt.vals[1],
                clr, 1.0f);

  cur = pts[1];
  sr_project_one (mst, &cur);
  nxt = pts[2];
  sr_project_one (mst, &nxt);
  al_draw_line (cur.vals[0], cur.vals[1], nxt.vals[0], nxt.vals[1],
                clr, 1.0f);

  cur = pts[2];
  sr_project_one (mst, &cur);
  nxt = pts[0];
  sr_project_one (mst, &nxt);
  al_draw_line (cur.vals[0], cur.vals[1], nxt.vals[0], nxt.vals[1],
                clr, 1.0f);
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
      al_flip_display ();
      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}
