#pragma once
#include "Subject.h"
#include "core.h"
#include <qtmetamacros.h>

class Relationship : public Subject {

public:
  enum class Type { ParentChild, Couple };

  static const inline QString ParentChildType =
      g_GEDCOMX_PREFIX + "ParentChild";
  static const inline QString CoupleType = g_GEDCOMX_PREFIX + "Couple";

  Q_PROPERTY(QString type MEMBER type)
  Q_PROPERTY(QString person1 MEMBER person1)
  Q_PROPERTY(QString person2 MEMBER person2)

  Q_INVOKABLE Relationship(QObject *par = nullptr);

  QString type;
  QString person1;
  QString person2;

  void setType(Relationship::Type type);
};