#include "family-tree-view.h"


namespace mftb {
    FamilyTreeView::FamilyTreeView(QWidget *parent)
    : ZoomOnScrollGraphicsView(parent) {
        setRenderHint(QPainter::Antialiasing);
    }
}


void mftb::FamilyTreeView::toggle_hand_mode() {
    setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
}


void mftb::FamilyTreeView::toggle_mouse_mode() {
    setDragMode(QGraphicsView::DragMode::NoDrag);
}
