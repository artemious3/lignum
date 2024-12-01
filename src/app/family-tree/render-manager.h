#include "family-tree-item.h"
#include "renderer.h"
#include "render-preprocessor.h"
#include <qobject.h>


class RenderManager : public QObject {

public:
    FamilyTreeItem const* tree_item;
    Renderer renderer;


public:
    RenderManager(QObject* parent, FamilyTreeItem*);

    void prepare();
    void prepare_add_child_to_family(id_t couple_id);
    void prepare_add_partner(id_t person);
    void prepare_add_parent(id_t person);
    void render();

};