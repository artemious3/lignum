#include "Config.h"

struct ConnectorConfig Config::ConnectorConfig() {
    return connector_cfg;
}

struct RendererConfig Config::BalancerConfig()  {
    return balancer_cfg;    
}

struct NodePlacerConfig Config::NodePlacerConfig()  {
    return node_placer_cfg;    
}

struct PersonItemConfig Config::PersonItemConfig() {
    return person_item_cfg;
}

struct AncestorNodePlacerConfig Config::AncestorNodePlacerConfig() {
    return ancestor_node_placer_cfg;    
}


Keys Config::KeysConfig(){
	return keys;
}
