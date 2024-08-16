


#pragma once
#include "NameForm.h"
#include <qcontainerfwd.h>
#include <qobject.h>
#include <qtmetamacros.h>
class Name : public QObject {

  Q_OBJECT

  static const inline QStringList TypesList = {
    "BirthName",
    "MarriedName",
    "AlsoKnownAs",
    "Nickname",
    "AdoptiveName",
    "FormalName",
    "ReligiousName"
  };

  Q_PROPERTY(QString type MEMBER type)
  Q_PROPERTY(QList<NameForm*> nameForms MEMBER nameForms)

public:
  Q_INVOKABLE Name(QObject* parent = nullptr);

  QString type;
  QList<NameForm*> nameForms;


  QString getFullName() const;
};

