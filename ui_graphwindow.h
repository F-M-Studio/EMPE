/*
* Nazwa Projektu: EMPE
 * Plik: ui_graphwindow.h
 *
 * Krótki opis pliku: Wygenerowany automatycznie kod interfejsu użytkownika dla okna wykresu.
 *
 * Autorzy:
 * Mateusz Korniak <mkorniak04@gmail.com>
 * Mateusz Machowski <machowskimateusz51@gmail.com>
 * Filip Leśnik <filip.lesnik170@gmail.com>
 *
 * Data Utworzenia: 10 Marca 2025
 * Ostatnia Modyfikacja: 18 Czerwaca 2025
 *
 * Ten program jest wolnym oprogramowaniem; możesz go rozprowadzać i/lub
 * modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
 * opublikowanej przez Free Software Foundation, w wersji 3 tej Licencji
 * lub (według twojego wyboru) dowolnej późniejszej wersji.
 *
 * Ten program jest rozpowszechniany w nadziei, że będzie użyteczny, ale
 * BEZ ŻADNEJ GWARANCJI; nawet bez domyślnej gwarancji PRZYDATNOŚCI
 * HANDLOWEJ lub PRZYDATNOŚCI DO OKREŚLONEGO CELU. Zobacz Powszechną
 * Licencję Publiczną GNU, aby uzyskać więcej szczegółów.
 *
 * Powinieneś otrzymać kopię Powszechnej Licencji Publicznej GNU wraz z
 * tym programem. Jeśli nie, zobacz <http://www.gnu.org/licenses/>.
*/

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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphWindow
{
public:
    QWidget *centralwidget;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QMenuBar *menubar;

    void setupUi(QMainWindow *GraphWindow)
    {
        if (GraphWindow->objectName().isEmpty())
            GraphWindow->setObjectName("GraphWindow");
        GraphWindow->resize(830, 829);
        centralwidget = new QWidget(GraphWindow);
        centralwidget->setObjectName("centralwidget");
        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(10, 10, 780, 550));
        frame->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        frame->setAutoFillBackground(false);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setSpacing(1);
        horizontalLayout->setObjectName("horizontalLayout");
        GraphWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GraphWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 830, 24));
        GraphWindow->setMenuBar(menubar);

        retranslateUi(GraphWindow);

        QMetaObject::connectSlotsByName(GraphWindow);
    } // setupUi

    void retranslateUi(QMainWindow *GraphWindow)
    {
        GraphWindow->setWindowTitle(QCoreApplication::translate("GraphWindow", "MainWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GraphWindow: public Ui_GraphWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHWINDOW_H
