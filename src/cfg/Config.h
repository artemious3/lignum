

#include <qnamespace.h>
#include <QString>

struct NodePlacerConfig {
    double primary_person_border_increment = 1;
    double zero_partner_children_left_border_decrement = 1;
    double nonzero_partner_children_left_border_decrement = 1;
    double couple_right_pos_and_next_border_diff = 1;
};

struct AncestorNodePlacerConfig{
  double distance_between_families = 0;
};


struct RendererConfig {
  int FIRST_FAMILY_LINE_BIAS = 70;
  int DISTANCE_BETWEEN_TREE_LEAVES = 90;
  int DISTANCE_BETWEEN_GENERATIONS = 170;
  int DISTANCE_BETWEEN_FAMILY_LINES = 7;
};


struct ConnectorConfig{
  int pen_width = 1;
};

struct PersonItemConfig{
  float icon_size = 40;
	float icon_padding = 20;
  float text_width = 2 * icon_size;
  Qt::Alignment text_alignment = Qt::AlignCenter;

};

struct Keys {
	Qt::Key REMOVE = Qt::Key_R;
	Qt::Key ADD_PARENT = Qt::Key_G;
	Qt::Key ADD_CHILD = Qt::Key_C;
	Qt::Key ADD_PARTNER  = Qt::Key_P;
	QString OPEN = "Ctrl+O";
	QString SAVE = "Ctrl+Shift+S";
} ;


class Config {

private:
    static inline RendererConfig balancer_cfg;
    static inline NodePlacerConfig node_placer_cfg;
    static inline ConnectorConfig connector_cfg;
    static inline PersonItemConfig person_item_cfg;
    static inline AncestorNodePlacerConfig ancestor_node_placer_cfg;
    static inline Keys keys;


public:
    static inline int StackSizeLimit = 32768; 
    
    static ConnectorConfig ConnectorConfig();
    static RendererConfig  BalancerConfig();
    static NodePlacerConfig NodePlacerConfig();
    static PersonItemConfig PersonItemConfig();
    static AncestorNodePlacerConfig  AncestorNodePlacerConfig();
    static Keys KeysConfig();
};
