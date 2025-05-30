#include "BiographiesView.h"
#include "FamilyTreeSqlModel.h"
#include "qcontainerfwd.h"
#include "qdialog.h"
#include "qlistwidget.h"
#include "qlogging.h"
#include "qpushbutton.h"
#include "qwindow.h"
#include "ui_BiographiesView.h"
#include "FamilyTreeModel.h"



BiographiesView::BiographiesView(QWidget* parent) : QDialog(parent),
	ui(new Ui::BiographiesView),
 model("mongodb://127.0.0.1:27017"){
	ui->setupUi(this);



	const auto* db = mftb::FamilyTreeSqlModel::getInstance();

	auto persons = db->getPeopleIds(-1);
	for(auto id : persons){
		auto person = db->getPersonById(id);
		ui->w_list->addItem(new QListWidgetItem(
					QString("%1 %2").arg(QString::number(id), person->fullName())
					));
	}

	connect(ui->w_list, &QListWidget::currentTextChanged,
			    this, &BiographiesView::on_selection_changed);

	connect(ui->w_saveBtn, &QPushButton::clicked,
			this, &BiographiesView::save);


}


void BiographiesView::on_selection_changed(const QString& text){

	bool ok;
	id_t id = text.split(" ")[0].toLong(&ok);
	qDebug() << text.split(" ")[0];

	if(ok){
		auto bio_opt = model.get_biography(id);
		if(bio_opt){
			ui->w_bio->setPlainText(QString::fromStdString(bio_opt.value()));
		} else {
			ui->w_bio->setPlainText("");
		}


	const auto* db = mftb::FamilyTreeSqlModel::getInstance();
		ui->w_name->setText(db->getPersonById(id)->fullName());
	}
}


void BiographiesView::save(){
	QString cur_text = ui->w_list->currentItem()->text();

	bool ok;
	id_t id = cur_text.split(" ")[0].toLong(&ok);

	if(ok){
		model.set_biography(id, ui->w_bio->toPlainText().toStdString());
	}
}
