#pragma once
#include "Relationship.h"
#include "family-connector.h"
#include <QSet>
#include <cstdint>
#include <vector>

class FamilyTreeItem;

class FamilyConnectorsDB {
public:
  FamilyConnectorsDB(const FamilyTreeItem& ascene);
  void addRelationship(std::shared_ptr<const Relationship> relationship);

  std::pair<uint32_t, uint32_t> getParents(uint32_t id) const;
  std::vector<std::uint32_t> getChildren(uint32_t id) const;

  int getAmountOfFamilies(uint32_t id) const;
  const QSet<FamilyConnector*>& getFamilies() const;

private:
  std::pair<uint32_t, uint32_t> static extract_ids_from_relationship(
      std::shared_ptr<const Relationship>, bool *);

  void addParentChildRelationship(uint32_t, uint32_t);
  void addCoupleRelationship(uint32_t, uint32_t);

private:
  const FamilyTreeItem& scene;
  QSet<FamilyConnector*> families;
  QHash<uint32_t, FamilyConnector *> primary_family_map;
  QMultiHash<uint32_t, FamilyConnector *> secondary_family_map;
};