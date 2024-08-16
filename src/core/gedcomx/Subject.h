#ifndef C6FBBC53_E229_4BD4_8F23_870538CD1BB8
#define C6FBBC53_E229_4BD4_8F23_870538CD1BB8

#include <QObject>
#include <qtmetamacros.h>

class Subject : public QObject {
  
  Q_OBJECT

  Q_PROPERTY(QString identifier MEMBER identifier)

public:
  Q_INVOKABLE Subject(QObject *par = nullptr);

  QString identifier;
};

#endif /* C6FBBC53_E229_4BD4_8F23_870538CD1BB8 */
