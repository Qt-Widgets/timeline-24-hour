#include "mainwindow.h"

#include <QGridLayout>
#include <QSlider>
#include <QPushButton>
#include <QInputDialog>
#include <QLabel>

using namespace TestProject;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* centralWidget = new QWidget(this);
    QGridLayout* mainLayout = new QGridLayout(this);

    m_timelineControl = std::make_unique<TimelineControl>();

    m_timeline = new TimelineWidget(*m_timelineControl, this);
    m_timeline->setMinimumWidth(500);

    QPushButton* amountRequestButton = new QPushButton("Bookmark settings", this);
    m_currentAmountWidget = new QLineEdit(this);
    m_currentAmountWidget->setReadOnly(true);

    connect(amountRequestButton, &QPushButton::clicked, this, [&]()
    {
        bool ok;
        auto amount = QInputDialog::getInt(this, "Bookmarks amount", "Amount:", 0, 0, 1000000, 1, &ok);
        if (ok)
        {
            m_timelineControl->generateBooksmarks(amount);
            m_currentAmountWidget->setText(QString::number(amount));
            m_timeline->update();
        }
    });

    mainLayout->addWidget(amountRequestButton, 0, 0);
    mainLayout->addWidget(new QLabel("Bookmarks amount:"), 0, 1);
    mainLayout->addWidget(m_currentAmountWidget, 0, 2);

    mainLayout->addWidget(m_timeline, 1, 0, 1, 3);

    centralWidget->setLayout(mainLayout);
    this->setCentralWidget(centralWidget);
}