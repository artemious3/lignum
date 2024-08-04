/*
 * File: /src/app/mftb-window/tree-scene/family-connectors-db.cpp
 * Project: MFTB
 * File Created: Friday, 2nd August 2024 10:59:50 am
 * Author: Artsiom Padhaiski (artempodgaisky@gmail.com)
 * Copyright 2024 - 2024 Artsiom Padhaiski
 *
 * ______________________________________________________________
 *
 * This file is part of MFTB.
 *
 * MFTB is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 *  version.
 *
 * MFTB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with MFTB. If not, see <https: //www.gnu.org/licenses/>.
 */

/* ---Description ---
 * This is an abstraction layer between GEDCOM X Relationship type
 * and more convinient FamilyConnector type. */

#include "family-connectors-db.h"
#include "individual-item.h"
#include "family-tree-item.h"
#include <cstdint>
#include <qlogging.h>
#include <vector>

FamilyConnectorsDB::FamilyConnectorsDB(const FamilyTreeItem &ascene)
    : scene(ascene) {}

void FamilyConnectorsDB::addRelationship(
    std::shared_ptr<const Relationship> rel) {
  bool extraction_success;
  auto person_ids = extract_ids_from_relationship(rel, &extraction_success);

  if (!extraction_success) {
    qWarning("Failed to add Relationship\n");
    return;
  }

  if (rel->type == Relationship::ParentChildType) {
    addParentChildRelationship(person_ids.first, person_ids.second);
  } else if (rel->type == Relationship::CoupleType) {
    addCoupleRelationship(person_ids.first, person_ids.second);
  }
}

std::pair<uint32_t, uint32_t> FamilyConnectorsDB::extract_ids_from_relationship(
    std::shared_ptr<const Relationship> rel, bool *success) {

  *success = true;
  bool id_conversion_success[2];
  uint32_t p1_id = rel->person1.toUInt(&id_conversion_success[0]);
  uint32_t p2_id = rel->person2.toUInt(&id_conversion_success[1]);

  if (!id_conversion_success[0] || !id_conversion_success[1]) {
    qWarning("Failed to convert id into number (succeded with id1: %b, "
             "succeded with id2: %b)",
             id_conversion_success[0], id_conversion_success[1]);
    *success = false;
    return {};
  }

  return {p1_id, p2_id};
}

void FamilyConnectorsDB::addParentChildRelationship(uint32_t parent_id,
                                                    uint32_t child_id) {
  FamilyConnector *const existing_child_family = primary_family_map[child_id];
  auto *const parent = scene.getPersonById(parent_id);
  auto *const child = scene.getPersonById(child_id);

  if (existing_child_family == nullptr) {
    FamilyConnector *new_child_family = new FamilyConnector(parent);
    new_child_family->addChild(child);

    primary_family_map[child_id] = new_child_family;
    secondary_family_map.insert(parent_id, new_child_family);
    families.insert(new_child_family);

  } else if (existing_child_family->isSingleParent()) {
    existing_child_family->setEmptyParent(parent);

    secondary_family_map.insert(parent_id, existing_child_family);
  } else {
    qWarning("Primary family of given child already has two parents. Adding "
             "parent to this family is ambiguous, hence can not be done.");
    return;
  }
}

void FamilyConnectorsDB::addCoupleRelationship(uint32_t id1, uint32_t id2) {

  auto [person1_families, p1_range_end] = secondary_family_map.equal_range(id1);
  auto [person2_families, p2_range_end] = secondary_family_map.equal_range(id2);

  auto *p1 = scene.getPersonById(id1);
  auto *p2 = scene.getPersonById(id2);

  auto existing_couple_in_p1_families = std::find_if(
      person1_families, p1_range_end,
      [=](FamilyConnector *family) { return family->hasParent(id2); });

  auto existing_couple_in_p2_families = std::find_if(
      person2_families, p2_range_end,
      [=](FamilyConnector *family) { return family->hasParent(id1); });

  if (existing_couple_in_p1_families == secondary_family_map.end() &&
      existing_couple_in_p2_families == secondary_family_map.end()) {

    FamilyConnector *new_family = new FamilyConnector(p1, p2);
    secondary_family_map.insert(id1, new_family);
    secondary_family_map.insert(id2, new_family);
    families.insert(new_family);

  } else {
    qWarning("The couple already exists.");
    return;
  }
}

int FamilyConnectorsDB::getAmountOfFamilies(uint32_t id) const {
  return secondary_family_map.count(id);
}

const QSet<FamilyConnector*>& FamilyConnectorsDB::getFamilies() const {  
  return families;
}

std::pair<std::uint32_t, std::uint32_t> FamilyConnectorsDB::getParents(uint32_t id) const{
  auto *const primary_family = primary_family_map[id];
  if(primary_family == nullptr){
    return {0,0};
  }
  auto parents = primary_family->getParents();
  return {parents.first->getId(), parents.second->getId()};
}

std::vector<std::uint32_t> FamilyConnectorsDB::getChildren(uint32_t id) const{
  std::vector<uint32_t> children;

  auto [secondary_families, secondary_families_end] = secondary_family_map.equal_range(id);

  for(auto fam = secondary_families; fam != secondary_families_end; ++fam){
    auto current_family_children = fam.value()->getChildren();
    foreach(auto child, current_family_children){
      children.push_back(child->getId());
    }
  }

  return children;
}
