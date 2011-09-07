#ifndef SR_H_
#define SR_H_

#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>
#include <src/cogl_include.h>
#include <glib.h>
#include <src/mai-model.h>
#include <src/mai-node-anim.h>
#include <nx_mat.h>

struct NxState
{
  float yaw, pitch, roll;
  NxMat p_mat, w_mat;
  float dr_n, dr_f;
  float vp_w, vp_h, vp_x, vp_y;
};

/*
 * Should be only the node info (As needed for animations / skinning
 * Should be easy to copy
 */
struct SrNode
{
  char *name;
  char *parent_name;
  int child_names_len;
  char **child_names;

  NxMat transformation;
  /**
   * NO Nodes should only hold transform data
   *   ie for acc_transform related
   * Make external data structures reference these
   *   anim -> channels -> srnode idx
   * Actually is there any reason to use int idx instad of string names?
   *   Except for micooptimisation?
   * NxMat transformation;
   * GMaiBonePtrArray bones;
   */
};

struct SrNodeGraph
{
  char *name;
  int nodes_len;
  struct SrNode *nodes;
  GHashTable *name_node_map;
};

void
sr_nx_mat_from_cogl_matrix (NxMat *mat, CoglMatrix *cogl_matrix);

void
sr_cogl_matrix_from_nx_mat (CoglMatrix *cogl_matrix, NxMat *mat);

void
sr_project_one (NxMat *mst, NxVec4 *vec_inout);

void
sr_viewport_one (NxVec4 *vec_inout);

void
sr_draw_unit_vec_at (NxMat *mst, NxVec4 *pos, NxVec4 *dir);

void
sr_draw_tri (NxMat *mst, NxVec4 pts[3]);

void
sr_allegro_draw_node (NxMat *mst, GArray *verts, GArray *indices, GArray *uvs);

void
sr_draw_node (NxMat *mv_mtx, NxMat *node_mtx, GArray *verts, GArray *indices, GArray *uvs);

void
sr_allegro_skeletal_draw_node_trans (NxMat *mst,
                             struct SrNodeGraph *sr_model,
                             MaiNode *mesh_node,
                             GArray *verts);

void
sr_skeletal_draw_node_trans (NxMat *mv_mat,
                             struct SrNodeGraph *sr_model,
                             MaiNode *mesh_node,
                             GArray *verts);

void
sr_update_global_ypr (ALLEGRO_KEYBOARD_STATE *aks);

struct SrNode *
_sr_node_walk (struct SrNodeGraph *mdl, MaiNode *nde_foreign);

void
sr_model_from_mai_model (struct SrNodeGraph **result, MaiModel *model);

void
sr_weight_dump (MaiModel *model);

void
sr_update_node_graph (MaiAnimInstance *mai, struct SrNodeGraph *graph);

void
sr_node_accumulate (struct SrNodeGraph *sr_model,
                    struct SrNode *src,
                    NxMat *result);

void
sr_bone_matrix (struct SrNodeGraph *sr_model,
                struct SrNode *mesh_node,
                MaiBone *bone,
                NxMat *result_out);

void
sr_bone_matrices (struct SrNodeGraph *sr_model,
                  struct SrNode *mesh_node_sr,
                  MaiNode *mesh_node,
                  GHashTable **nbm_out);

void
sr_vertex_bones (MaiNode *mesh_node, GPtrArray **vbmap_out);

void
sr_vertex_transform_calculate (MaiNode *mesh_node,
                               GPtrArray *vbmap,
                               GHashTable *name_bone_mtx_map,
                               GArray **trans_verts_out);

void
sr_node_graph_from_model (MaiModel *model,
                          struct SrNodeGraph **sr_model_out);

void
sr_skeletal_anim (MaiModel *model,
                  MaiAnimInstance *mai,
                  MaiNode *mesh_node,
                  struct SrNodeGraph *sr_model_io,
                  GArray **trans_verts_out);

void
sr_skeletal_anim_node_graph (MaiAnimInstance *mai,
                             struct SrNodeGraph *sr_model_io);

void
sr_skeletal_anim_verts (MaiModel *model,
                        MaiAnimInstance *mai,
                        struct SrNodeGraph *sr_model,
                        GHashTable **name_verts_map_out);

struct SrNode *
_sr_copy_node_walk (struct SrNodeGraph *res_mdl,
                    struct SrNodeGraph *orig_mdl,
                    struct SrNode *nde);

void
sr_node_graph_copy (struct SrNodeGraph **result, struct SrNodeGraph *what);

void
_sr_node_graph_draw_one (struct SrNodeGraph *sr_model,
                         NxMat *mst, NxMat *acc,
                         struct SrNode *node);

void
sr_node_graph_draw (NxMat *mst, struct SrNodeGraph *sr_model);

#endif /* SR_H_ */
