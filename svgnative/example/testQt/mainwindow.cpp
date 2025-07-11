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
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "mainwindow.h"
#include "svgnative/SVGDocument.h"
#include "svgnative/ports/qt/QSVGRenderer.h"

using namespace SVGNative;

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        label = new QLabel(MainWindow);
        label->setObjectName("label");
        label->setGeometry(QRect(370, 200, 100, 100));
        label->setMinimumSize(QSize(100, 100));
        // MainWindow->setCentralWidget(centralwidget);
        MainWindow->setCentralWidget(label);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle("TestQt");
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
class MainWindow: public Ui_MainWindow {};
} // namespace Ui

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mLabel = ui->label;
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list"))
    {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    if(e->mimeData()->hasUrls()) {
        QList<QUrl> urlList = e->mimeData()->urls();
        for (int i = 0; i < 1; i++) {
            QUrl url = urlList[i];
            QFile file(url.toLocalFile());
            if (file.open(QIODevice::ReadOnly))
            {
                QTextStream in( &file );
                renderSVG(in.readAll());
            }
        }
    }
}

void MainWindow::renderSVG(QString svgdata)
{
    // offline rendering into QImage
    auto renderer = std::shared_ptr<QSVGRenderer>(new QSVGRenderer);
    auto svgDocument = SVGDocument::CreateSVGDocument(svgdata.toUtf8().constData(), renderer);

    // render twice size
    QSize svgSize(2*svgDocument.get()->Width(), 2*svgDocument.get()->Height());
    QImage image(svgSize, QImage::Format_ARGB32);
    {
        QPainter painter(&image);
        painter.setWindow(0, 0, svgDocument.get()->Width(), svgDocument.get()->Height());
        renderer->SetPainter(&painter);
        svgDocument->Render();
    }

    // view image as a pixmap
    mLabel->setPixmap(QPixmap::fromImage(image));
    mLabel->resize(svgSize);
}

