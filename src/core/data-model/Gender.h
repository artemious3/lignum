#ifndef CF936778_F213_4399_A3AF_234DD63CDF2F
#define CF936778_F213_4399_A3AF_234DD63CDF2F

#include <qobject.h>
#include <qtmetamacros.h>

class Gender : public QObject {
  Q_OBJECT

  static const inline QStringList GenderList = {"Male", "Female", "Unknown",
                                                "Intersex"};

  Q_PROPERTY(QString type MEMBER type)

public:
  Q_INVOKABLE Gender(QObject *parent = nullptr);
  Gender(QString atype, QObject* parent = nullptr);


public:
  QString type;

public:
  static const Gender Male;
  static const Gender Female;
  static const Gender Intersex;
  static const Gender Unknown;
};

#endif /* CF936778_F213_4399_A3AF_234DD63CDF2F */
