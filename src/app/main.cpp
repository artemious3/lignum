/*
 * File: /src/app/main.cpp
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


#include "mftbwindow.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <QApplication>
#include <qapplication.h>

int main(int argc, char *argv[]) {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("%20s(%-3#): %^[%l]%$ %v");

  QApplication application(argc, argv);
  MFTBWindow main_window;
  main_window.show();
  return application.exec();
}
