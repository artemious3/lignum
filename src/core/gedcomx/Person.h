#pragma once
#include "Gender.h"
#include "Name.h"
#include "Subject.h"
#include <QVector>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>

class Family;

class Person : public Subject {

  Q_OBJECT

  Q_PROPERTY(const Gender* gender MEMBER gender)
  Q_PROPERTY(QList< Name*> names MEMBER names)

public:
  Q_INVOKABLE Person(QObject *parent = nullptr);

  const Gender *gender;
  QList<Name *> names;

private:
  Family* primaryFamily;
  Family* secondaryFamilies;
};
