#include "zoom-on-scroll-graphics-view.h"
#include <qobject.h>

namespace mftb {

class FamilyTreeView : public ZoomOnScrollGraphicsView {
  Q_OBJECT 

public:
  FamilyTreeView(QWidget *parent = nullptr);

public slots:
  void toggle_hand_mode();
  void toggle_mouse_mode();
};

} // namespace mftb