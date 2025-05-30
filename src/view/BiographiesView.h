
#include <QDialog>
#include "qtmetamacros.h"
#include "qwidget.h"
#include "BiographiesModel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class BiographiesView;
}
QT_END_NAMESPACE

class BiographiesView : public QDialog {
	Q_OBJECT

public:
	BiographiesView(QWidget * parent);

	Ui::BiographiesView *ui;

	BiographiesModel model;


	public slots:
		void on_selection_changed(const QString& text);
		void save();


};
