#ifndef D1C30A0D_AA65_4A19_8394_D56CF78AC33D
#define D1C30A0D_AA65_4A19_8394_D56CF78AC33D

#include "NamePart.h"
#include <qobject.h>
#include <qtmetamacros.h>

class NameForm : public QObject{
    Q_OBJECT

Q_PROPERTY(QList<NamePart*> parts MEMBER parts)


public:
    Q_INVOKABLE NameForm(QObject* parent = nullptr);

    QList<NamePart*> parts;
    
};

#endif /* D1C30A0D_AA65_4A19_8394_D56CF78AC33D */
