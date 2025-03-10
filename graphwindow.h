// graphwindow.h
#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class GraphWindow; }
QT_END_NAMESPACE

class GraphWindow : public QMainWindow {
    Q_OBJECT

public:
    GraphWindow(QWidget *parent = nullptr);
    ~GraphWindow();

private:
    Ui::GraphWindow *ui;
};

#endif // GRAPHWINDOW_H