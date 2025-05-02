#include "family-tree-item.h"
#include "renderer.h"
#include "render-preprocessor.h"
#include <qobject.h>
#include <qtmetamacros.h>


class RenderManager : public QObject {
Q_OBJECT

public:
    FamilyTreeView *const tree_item;
    Renderer renderer;
    


public:
    RenderManager(QObject* parent, FamilyTreeView*);

    void prepare();
    void prepare_add_child_to_family(id_t couple_id, const Person& child);
    void prepare_add_partner(id_t person,  const Person& partner);
    void prepare_add_parent(id_t person,  const Person& partner);
    void render();

};
