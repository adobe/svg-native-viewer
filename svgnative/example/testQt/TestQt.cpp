/*
Copyright 2025 Adobe. All rights reserved.
Copyright 2025 KATO Kanryu All rights reserved.
Developed and contributed by KATO Kanryu.

This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/
#include <memory>
#include <QtWidgets/QApplication>
#include "mainwindow.h"

static const std::string gSVGString = "<svg viewBox=\"0 0 200 200\"><circle cx=\"100\" cy=\"100\" r=\"100\" fill=\"yellow\"/></svg>";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    w.renderSVG(QString(gSVGString.c_str()));
    return app.exec();
}
