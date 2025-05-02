#include "ColorManager.h"
#include <QApplication>
#include <QPalette>


QColor ColorManager::BackgroundColor()
{
    if(qApp){
        return qApp->palette().base().color();
    }
    return {};
}

QColor ColorManager::TextColor()
{
    if(qApp){
        return qApp->palette().text().color();
    }
    return {};
}

QColor ColorManager::AccentColor()
{
    if(qApp){
        return qApp->palette().accent().color();
    }
    return {};
}

