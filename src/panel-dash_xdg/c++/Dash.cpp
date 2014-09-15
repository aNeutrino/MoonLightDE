/*
 * File:   Dash.cpp
 * Author: alexis
 *
 * Created on 10 de septiembre de 2014, 16:21
 */

#include "Dash.h"

#include "AppButton.h"

#include <qt5xdg/XdgDesktopFile>

#include <QDebug>
#include <QGridLayout>
#include <QPushButton>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QLabel>
#include <QTime>

#include <algorithm>
#include <qt4/QtCore/qnamespace.h>

/**
 * As XdgDesktopFile doesn't provides the opperator < we must implement it here.
 */
struct XdgDesktopFileComparisonFunctor {

    bool
    operator()(const XdgDesktopFile * __x, const XdgDesktopFile * __y) const {
        return __x->name() < __y->name();
    }
};

Dash::Dash() : m_settings("panel-dash_xdg") {
    m_ui.setupUi(this);
    setWindowFlags(Qt::Popup);
    setFrameStyle(QFrame::NoFrame);

    
    built = false;
}

Dash::~Dash() {
}

void Dash::build() {
    QTime time;
    time.start();

    QGridLayout *layoutApps = new QGridLayout(m_ui.tabApps);
    layoutApps->setSpacing(0);
    layoutApps->setMargin(16);
   
    QGridLayout *layoutSettings = new QGridLayout(m_ui.tabSettings);
    layoutSettings->setSpacing(0);
    layoutSettings->setMargin(16);
    
    const QRect screen = qApp->desktop()->screenGeometry();


    const QSize iconSize(64, 64);
    const QSize boxSize(100, 100);


    const int maxColumnApps = m_ui.tabs->size().width() / (boxSize.width() + 32);
    //    qDebug() << "maxColumnApps: " << maxColumnApps;
    int currentRowApps = 0;
    int currentColumnApp = 0;

    const int maxColumnSettings = maxColumnApps;
    //    qDebug() << "maxColumnSettings: " << maxColumnSettings;
    int currentRowSettings = 0;
    int currentColumnSettings = 0;


    qDebug() << "Layouts init: " << time.elapsed();
    // Idividual items are released insed the AppButton class
    QList<XdgDesktopFile*> appList = XdgDesktopFileCache::getAllFiles();
    std::sort(appList.begin(), appList.end(), XdgDesktopFileComparisonFunctor());
    //    qSort(appList.begin(), appList.end());
    qDebug() << "XdgDesktopFile lockup: " << time.elapsed();

    foreach(XdgDesktopFile * app, appList) {
        if (app->type() != XdgDesktopFile::ApplicationType) {
            delete app;
            continue;
        }

        AppButton * bttn = new AppButton(app, this);

        QLabel *label = new QLabel(app->name());
        label->setAlignment(Qt::AlignHCenter);

        label->setWordWrap(true);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        label->setMaximumSize(boxSize.width(), 35);
        

        bttn->setFlat(true);
        bttn->setIconSize(iconSize);
        bttn->setMinimumSize(boxSize);
        bttn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

        connect(bttn, &AppButton::released, this, &Dash::hide);


        QString appCategories = app->value("Categories", "None").toString();
        //        qDebug() << "appCategories: " << appCategories;
        if (appCategories.contains("Settings", Qt::CaseInsensitive)) {
            //            qDebug() << "Settings: " << app->name();
            layoutSettings->addWidget(bttn, currentRowSettings, currentColumnSettings, Qt::AlignHCenter);
            layoutSettings->addWidget(label, currentRowSettings + 1, currentColumnSettings, Qt::AlignHCenter);

            currentColumnSettings++;
            if (currentColumnSettings >= maxColumnSettings) {
                currentColumnSettings = 0;
                currentRowSettings += 2;
            }

        } else {
            //            qDebug() << "Apps: " << app->name();
            layoutApps->addWidget(bttn, currentRowApps, currentColumnApp, Qt::AlignHCenter);
            layoutApps->addWidget(label, currentRowApps + 1, currentColumnApp, Qt::AlignHCenter);

            currentColumnApp++;
            if (currentColumnApp >= maxColumnApps) {
                currentColumnApp = 0;
                currentRowApps += 2;
            }
        }

    }
    qDebug() << "Layouts built: " << time.elapsed();

    qDebug() << "appcontainerSize" << m_ui.tabs->size();

    m_ui.apps->setGeometry(0, 0, m_ui.scrollArea->size().width() - (layoutApps->spacing() * maxColumnApps) , currentRowApps * boxSize.height());
    m_ui.apps->setLayout(layoutApps);


    m_ui.settings->setGeometry(0, 0, m_ui.scrollAreaSettings->size().width(), currentRowSettings * boxSize.height());
    m_ui.settings->setLayout(layoutSettings);

}

void Dash::free() {

}

void Dash::onItemTrigerred() {
    //    XdgCachedMenuAction* action = static_cast<XdgCachedMenuAction*> (sender());
    //    XdgDesktopFile df;
    //    char* desktop_file = menu_cache_item_get_file_path(action->item());
    //    df.load(desktop_file);
    //    g_free(desktop_file);
    //    df.startDetached();
}

// taken from libqtxdg: XdgMenuWidget



// taken from libqtxdg: XdgMenuWidget

void Dash::handleMouseMoveEvent(QMouseEvent *event) {
    //    if (!(event->buttons() & Qt::LeftButton))
    //        return;
    //
    //    if ((event->pos() - mDragStartPosition).manhattanLength() < QApplication::startDragDistance())
    //        return;
    //
    //    XdgCachedMenuAction *a = qobject_cast<XdgCachedMenuAction*>(actionAt(event->pos()));
    //    if (!a)
    //        return;
    //
    //    QList<QUrl> urls;
    //    char* desktop_file = menu_cache_item_get_file_path(a->item());
    //    urls << QUrl(desktop_file);
    //    g_free(desktop_file);
    //
    //    QMimeData *mimeData = new QMimeData();
    //    mimeData->setUrls(urls);
    //
    //    QDrag *drag = new QDrag(this);
    //    drag->setMimeData(mimeData);
    //    drag->exec(Qt::CopyAction | Qt::LinkAction);
}

void Dash::showEvent(QShowEvent * event) {
    qDebug() << "Show event";

    if (!built) {
        build();
        built = true;
    }

    m_ui.tabs->setCurrentWidget(m_ui.tabStart);
    show();
}