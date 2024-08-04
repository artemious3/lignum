#ifndef F484BBFA_2DA4_4799_A3B8_A726D5F46C67
#define F484BBFA_2DA4_4799_A3B8_A726D5F46C67

#include <qobject.h>
#include <qtmetamacros.h>

class NamePart : public QObject {
  Q_OBJECT

  static const inline QStringList TypeList{"Prefix", "Suffix", "Given",
                                           "Surname"};

  Q_PROPERTY(QString type MEMBER type)
  Q_PROPERTY(QString value MEMBER value)

public:
  Q_INVOKABLE NamePart(QObject* parent = nullptr);

  QString type{};
  QString value{};
};

#endif /* F484BBFA_2DA4_4799_A3B8_A726D5F46C67 */
