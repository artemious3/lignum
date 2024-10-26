#include "ColorManager.h"
#include <QApplication>
#include <QPalette>


QColor ColorManager::BackgroundColor()
{
    if(qApp){
        return qApp->palette().base().color();
    }
}

QColor ColorManager::TextColor()
{
    if(qApp){
        return qApp->palette().text().color();
    }
}

QColor ColorManager::AccentColor()
{
    if(qApp){
        return qApp->palette().accent().color();
    }
}

