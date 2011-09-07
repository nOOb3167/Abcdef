#include <src/mai-info-win.h>

NX_GET_NEW (mai_info_win);

GObject *
mai_info_win_new (void)
{
  MaiInfoWin * self;

  self = MAI_INFO_WIN (nx_get_new ());

  return G_OBJECT (self);
}
