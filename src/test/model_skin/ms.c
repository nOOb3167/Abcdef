#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>
#include <src/cogl_include.h>

#include <src/error.h>
#include <src/gfx_lib_setup.h>
#include <src/mai-model.h>
#include <src/mai-model_funcs.h>
#include <src/misc.h>
#include <ma.h>

void
_display_loop (void);

void
_ms_stuff (MaiModel *mm);

void
_display_loop (void)
{
  context_switch_allegro ();

  ALLEGRO_BITMAP *bmp;
  bmp = al_create_bitmap (640, 480);

  int blah;
  for (blah=0; blah < 10; ++blah)
    {
      struct fbstate_data fbd;

      context_switch_cogl ();
      fbd = fbstate_get_data ();

      context_switch_allegro ();
      ALLEGRO_LOCKED_REGION *rgn;
      rgn = al_lock_bitmap (bmp, ALLEGRO_PIXEL_FORMAT_BGR_888, ALLEGRO_LOCK_READWRITE);
      int cnt;
      char *data;
      for (cnt=0,data=rgn->data; cnt < 480; ++cnt,data+=rgn->pitch)
        {
          memcpy (data, &fbd.data[cnt*640*3], 640*3);
        }
      al_unlock_bitmap (bmp);

      context_switch_cogl ();

      context_switch_allegro ();
      al_set_target_backbuffer (fbd.display);
      al_draw_bitmap (bmp, 0, 0, 0);
      al_flip_display ();

      al_rest (0.1f);
    }

  al_rest (2.0f);
}

int
main (int argc, char **argv)
{
    g_type_init ();

    gfx_lib_setup ();

    printf ("COGL SETUP COMPLETE\n");

    g_testtex = cogl_texture_new_from_file ("testtex.bmp", COGL_TEXTURE_NONE, COGL_PIXEL_FORMAT_ANY, NULL);
    g_xassert (g_testtex != COGL_INVALID_HANDLE);

    MaiModel *mm;
    mm = mai_model_new_from_file ("c_multipart_collada_skin_ms.dae");

    _ms_stuff (mm);

    _display_loop ();

    return EXIT_SUCCESS;
}

void
_ms_stuff (MaiModel *mm)
{
  MaiNode *scene_node;
  MaiNode *cube_node;
  MaiNode *bone_node;

  void nnmp (gpointer key, gpointer value, gpointer data)
  {
    printf ("nnmp %s\n", key);
  }
  g_hash_table_foreach (mm->name_node_map, nnmp, NULL);

  scene_node = g_hash_table_lookup (mm->name_node_map, "Scene");
  cube_node = g_hash_table_lookup (mm->name_node_map, "Cube");
  bone_node = g_hash_table_lookup (mm->name_node_map, "Bone");
  g_xassert (scene_node);
  g_xassert (cube_node);
  g_xassert (bone_node);

  MaiBone *bone;
  bone = g_mai_bone_ptr_array_index (cube_node->bones, 0);
  g_xassert (bone);

  /**
   * Confirmed bone->transform_matrix from mesh to bone space.
   *   When translating Cube by -2y, offset_matrix also -2y.
   * However bone space seems to be rot=identity; trans=What Blender 'n' properties key shows.
   *
   * XX:
   * Maybe the Scene node is superfluous.
   * Assimp http://assimp.sourceforge.net/lib_html/data.html section Bones suggested algo would suggest
   *   something that would not leave the Scene node in?
   * As far as I can tell the scene node transform is
   *   invert z
   *   Err isn't that newz=-oldy and newy=oldz?
   */

  /**
   * Results of transform mesh -> bone seem ok to me.
   */
  float tmp_vtx1[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  cogl_matrix_transform_point (bone->offset_matrix, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);

  void acc_transform (MaiNode *node, CoglMatrix *acc_mtx)
  {
    if (node == NULL)
      {
        cogl_matrix_init_identity (acc_mtx);
        return;
      }
    acc_transform (node->parent, acc_mtx);
    cogl_matrix_multiply (acc_mtx, acc_mtx, node->transformation);
  }

  CoglMatrix bone_ws;
  acc_transform (bone_node, &bone_ws);

  /**
   * Seems ok (Z inverted due to Scene, that's the right thing?).
   * Err see above on swap.
   */
  cogl_matrix_transform_point (&bone_ws, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);

  CoglMatrix cube_ws;
  acc_transform (cube_node, &cube_ws);
  CoglMatrix cube_ws_inv;
  cogl_matrix_get_inverse (&cube_ws, &cube_ws_inv);

  cogl_matrix_transform_point (&cube_ws_inv, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);
  cogl_matrix_transform_point (&cube_ws, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);

  return;
}
