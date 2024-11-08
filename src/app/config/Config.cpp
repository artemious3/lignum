#include "Config.h"

ConnectorConfig Config::ConnectorConfig() {
    return connector_cfg;
}

RendererConfig Config::BalancerConfig()  {
    return balancer_cfg;    
}

NodePlacerConfig Config::NodePlacerConfig()  {
    return node_placer_cfg;    
}

PersonItemConfig Config::PersonItemConfig() {
    return person_item_cfg;
}

AncestorNodePlacerConfig Config::AncestorNodePlacerConfig() {
    return ancestor_node_placer_cfg;    
}
