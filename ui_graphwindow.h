/********************************************************************************
** Form generated from reading UI file 'graphwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHWINDOW_H
#define UI_GRAPHWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphWindow {
public:
    QWidget *centralwidget;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GraphWindow) {
        if (GraphWindow->objectName().isEmpty())
            GraphWindow->setObjectName("GraphWindow");
        GraphWindow->resize(800, 600);
        centralwidget = new QWidget(GraphWindow);
        centralwidget->setObjectName("centralwidget");
        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(10, 10, 780, 550));
        frame->setMaximumSize(QSize(780, 550));
        frame->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        frame->setAutoFillBackground(false);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(1);
        horizontalLayout->setObjectName("horizontalLayout");
        GraphWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GraphWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 23));
        GraphWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(GraphWindow);
        statusbar->setObjectName("statusbar");
        GraphWindow->setStatusBar(statusbar);

        retranslateUi(GraphWindow);

        QMetaObject::connectSlotsByName(GraphWindow);
    } // setupUi

    static void retranslateUi(QMainWindow *GraphWindow) {
        GraphWindow->setWindowTitle(QCoreApplication::translate("GraphWindow", "MainWindow", nullptr));
    }
};

namespace Ui {
    class GraphWindow : public Ui_GraphWindow {
    };
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHWINDOW_H