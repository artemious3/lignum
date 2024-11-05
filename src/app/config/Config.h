

#include <qnamespace.h>
struct NodePlacerConfig {
    double primary_person_border_increment = 1;
    double zero_partner_children_left_border_decrement = 1;
    double nonzero_partner_children_left_border_decrement = 1;
    double couple_right_pos_and_next_border_diff = 1;
};


struct BalancerConfig {
  int FIRST_FAMILY_LINE_BIAS = 35;
  int DISTANCE_BETWEEN_TREE_LEAVES = 90;
  int DISTANCE_BETWEEN_GENERATIONS = 140;
  int DISTANCE_BETWEEN_FAMILY_LINES = 7;
};


struct ConnectorConfig{
  int pen_width = 3;
};

struct PersonItemConfig{
  float icon_size = 40;
  float text_width = 2 * icon_size;
  Qt::Alignment text_alignment = Qt::AlignCenter;

};


class Config {

private:
    static inline BalancerConfig balancer_cfg;
    static inline NodePlacerConfig node_placer_cfg;
    static inline ConnectorConfig connector_cfg;
    static inline PersonItemConfig person_item_cfg;

    


public:
    static inline int StackSizeLimit = 32768; 
    
    static ConnectorConfig ConnectorConfig();
    static BalancerConfig  BalancerConfig();
    static NodePlacerConfig NodePlacerConfig();
    static PersonItemConfig PersonItemConfig();
};