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
#include "family-tree-view.h"
#include <QMainWindow>
#include "qdialog.h"
#include "tree-manager.h"
#include <qgraphicsscene.h>
#include <qwidget.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class LignumWindow;
}
QT_END_NAMESPACE

class LignumWindow : public QMainWindow {
  Q_OBJECT

public:
	  LignumWindow();

private:
  void initialize_actions();
  void add_child_action(const Person& person);
  void add_parent(const Person& person);
  void update_person(id_t id, const Person& person);

  void update_actions_availability(id_t target_person);


public slots:
  void show_selected_person(id_t);
  void person_changed(id_t id);

  void on_actionAddPartner_triggered();
  void on_actionAddSon_triggered();
  void on_actionAddDaughter_triggered();
  void on_actionAddMother_triggered();
  void on_actionAddFather_triggered();

  void on_actionRemove_triggered();

  bool on_actionSave_triggered();
  bool on_actionOpen_triggered();

  void on_actionLoadGedcom_triggered();
  void on_actionWriteGedcom_triggered();


  void on_actionSwitchGender_triggered();

	void on_editBio_triggered();

private:
  Ui::LignumWindow *ui;
  FamilyTreeView *family_tree;
  std::unique_ptr<TreeManager> treeManager;


  bool db_changed = false;
};
