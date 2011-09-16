#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

void
r1 (GList *ll)
{
  GQueue *qu;
  qu = g_queue_new ();

  struct dd
  {
    GList *ll;
    gboolean dead;
  };

  struct dd elt = {ll, FALSE};
  g_queue_push_tail (qu, &elt);
  while (TRUE)
    {
      struct dd *cur;
      cur = g_queue_peek_tail (qu);
      if (NULL == cur)
          break;
      printf ("Hello\n");
      if (cur->dead != TRUE && cur->ll->next)
        {
          cur->dead = TRUE;
          struct dd *tmp;
          tmp = g_malloc0 (sizeof (*tmp));
          tmp->dead = FALSE;
          tmp->ll = cur->ll->next;
          g_queue_push_tail (qu, tmp);
        }
      else
          g_queue_pop_tail (qu);
    }
}

void
r2 (GList *ll)
{
  struct dd
  {
    GList *ll;
    gboolean dead;
  };

  GArray *ar;
  ar = g_array_new (FALSE, TRUE, sizeof (struct dd));

  struct dd elt = {ll, FALSE};
  g_array_append_vals (ar, &elt, 1);
  while (TRUE)
    {
      struct dd cur;
      cur = g_array_index (ar, struct dd, ar->len);
      if (0 == ar->len)
          break;
      printf ("Hello\n");
      if (cur.dead != TRUE && cur.ll->next)
        {
          cur.dead = TRUE;
          struct dd tmp = {cur.ll->next, FALSE};
          g_array_append_vals (ar, &tmp, 1);
        }
      else
          g_array_remove_index (ar, ar->len - 1);
    }
}

int
main (int argc, char **argv)
{
  GList *ll;
  ll = NULL;

  for (gint i = 0; i < 5; ++i)
    {
      ll = g_list_prepend (ll, GINT_TO_POINTER(i));
    }

  r1 (ll);
  printf ("DERP\n");
  r2 (ll);

  return EXIT_SUCCESS;
}
