/*
 * File: /src/app/tree-wizard/treewizard.h
 * Project: MFTB
 * File Created: Monday, 15th July 2024 7:53:02 pm
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

#pragma once
#include "family-tree-item.h"
#include <QMainWindow>
#include "tree-manager.h"
#include <qgraphicsscene.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MFTBWindow;
}
QT_END_NAMESPACE

class MFTBWindow : public QMainWindow {
  Q_OBJECT

public:
  MFTBWindow();
  void initialize_actions();


public slots:
  void show_selected_person(id_t);

  void add_partner_action();
  void add_child_action();
  void add_parent_action();



private:
  Ui::MFTBWindow *ui;
  FamilyTreeItem *family_tree;
  std::unique_ptr<TreeManager> treeManager;
};
