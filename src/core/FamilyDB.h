#pragma once
#include "Relationship.h"
#include "Family.h"
#include <QSet>
#include <cstdint>
#include <memory>
#include <qlist.h>
#include <vector>

class FamilyDB {
public:
  FamilyDB() = default;
  void addRelationship(std::shared_ptr<const Relationship> relationship);

  std::pair<uint32_t, uint32_t> getParents(uint32_t id) const;
  QList<std::uint32_t> getChildren(uint32_t id) const;

  int getAmountOfSecondaryFamilies(uint32_t id) const;
  const QSet<std::shared_ptr<Family>>& getFamilies() const;

private:
  std::pair<uint32_t, uint32_t> static extract_ids_from_relationship(
      std::shared_ptr<const Relationship>, bool *);

  void addParentChildRelationship(uint32_t, uint32_t);
  void addCoupleRelationship(uint32_t, uint32_t);

private:
  QSet<std::shared_ptr<Family>> families;
  QHash<uint32_t, std::shared_ptr<Family> > primary_family_map;
  QMultiHash<uint32_t, std::shared_ptr<Family>> secondary_family_map;
};