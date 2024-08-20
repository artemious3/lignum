#include "zoom-on-scroll-graphics-view.h"
#include <qobject.h>

namespace mftb {

class FamilyTreeView : public ZoomOnScrollGraphicsView {

public:
  FamilyTreeView(QWidget *parent = nullptr);
};

} // namespace mftb