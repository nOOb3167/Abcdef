#ifndef SR_H_
#define SR_H_

#include <glib.h>
#include <nx_mat.h>

struct NxState
{
  float yaw, pitch, roll;
  NxMat p_mat, w_mat;
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
sr_node_graph_copy (struct SrNodeGraph **result, struct SrNodeGraph *what);

void
nx_mat_from_cogl_matrix (NxMat *mat, CoglMatrix *cogl_matrix);

void
sr_node_accumulate (struct SrNodeGraph *sr_model,
                    struct SrNode *src,
                    NxMat *result);

#endif /* SR_H_ */
