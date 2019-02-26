#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "timelinewidget.h"
#include <memory>

#include <QMainWindow>
#include <QLineEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QLineEdit * m_currentAmountWidget;
    TestProject::TimelineWidget* m_timeline;

    std::unique_ptr<TestProject::TimelineControl> m_timelineControl;
};

#endif