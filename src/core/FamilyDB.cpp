/*
 * File: /src/core/FamilyDB.cpp
 * Project: MFTB
 * File Created: Sunday, 4th August 2024 4:52:37 pm
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
 * and more convinient Family type. */

#include "FamilyDB.h"
#include "Family.h"
#include "individual-item.h"
#include <cstdint>
#include <memory>
#include <qlogging.h>
#include <vector>

void FamilyDB::addRelationship(std::shared_ptr<const Relationship> rel) {
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

std::pair<uint32_t, uint32_t>
FamilyDB::extract_ids_from_relationship(std::shared_ptr<const Relationship> rel,
                                        bool *success) {

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

void FamilyDB::addParentChildRelationship(uint32_t parent_id,
                                          uint32_t child_id) {
  auto existing_child_family = primary_family_map[child_id];

  if (existing_child_family == nullptr) {
    auto new_child_family = std::make_shared<Family>(parent_id);
    new_child_family->addChild(child_id);

    primary_family_map[child_id] = new_child_family;
    secondary_family_map.insert(parent_id, new_child_family);
    families.insert(new_child_family);

  } else if (existing_child_family->isSingleParent()) {
    existing_child_family->setEmptyParent(parent_id);

    secondary_family_map.insert(parent_id, existing_child_family);
  } else {
    qWarning("Primary family of given child already has two parents. Adding "
             "parent to this family is ambiguous, hence can not be done.");
    return;
  }
}

void FamilyDB::addCoupleRelationship(uint32_t id1, uint32_t id2) {

  auto [person1_families, p1_range_end] = secondary_family_map.equal_range(id1);
  auto [person2_families, p2_range_end] = secondary_family_map.equal_range(id2);

  auto existing_couple_in_p1_families =
      std::find_if(person1_families, p1_range_end,
                   [=](auto family) { return family->hasParent(id2); });

  auto existing_couple_in_p2_families =
      std::find_if(person2_families, p2_range_end,
                   [=](auto family) { return family->hasParent(id1); });

  if (existing_couple_in_p1_families == secondary_family_map.end() &&
      existing_couple_in_p2_families == secondary_family_map.end()) {

    auto new_family = std::make_shared<Family>(id1, id2);
    secondary_family_map.insert(id1, new_family);
    secondary_family_map.insert(id2, new_family);
    families.insert(new_family);

  } else {
    qWarning("The couple already exists.");
    return;
  }
}

int FamilyDB::getAmountOfSecondaryFamilies(uint32_t id) const {
  return secondary_family_map.count(id);
}

const QSet<std::shared_ptr<Family>> &FamilyDB::getFamilies() const {
  return families;
}

std::pair<std::uint32_t, std::uint32_t>
FamilyDB::getParents(uint32_t id) const {
  auto primary_family = primary_family_map[id];
  if (primary_family == nullptr) {
    return {0, 0};
  }
  return primary_family->getParents();
}

QList<std::uint32_t> FamilyDB::getChildren(uint32_t id) const {

  auto [secondary_families, secondary_families_end] =
      secondary_family_map.equal_range(id);
  QList<uint32_t> children_ids;

  for (auto fam = secondary_families; fam != secondary_families_end; ++fam) {
    children_ids.emplace_back(fam.value()->getChildren());
  }

  return children_ids;
}git
