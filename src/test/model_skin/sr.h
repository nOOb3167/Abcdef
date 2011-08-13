#ifndef SR_H_
#define SR_H_

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
  struct SrModel *model;

  int idx;
  int parent_idx;
  int child_idxs[];
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

struct SrModel
{
  char *name;
  struct SrNode *nodes[];
};

#endif /* SR_H_ */
