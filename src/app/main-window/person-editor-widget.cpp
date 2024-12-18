#include "person-editor-widget.h"
#include "SqlDB.h"
#include <QDateTimeEdit>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>


PersonEditorWidget::PersonEditorWidget(QWidget* parent)
 : QWidget(parent), db(mftb::SqlDB::getInstance())
{
	firstNameEdit = new QLineEdit(this);
	middleNameEdit = new QLineEdit(this);
	surnameEdit = new QLineEdit(this);
	genderBox = new QComboBox(this);
	genderBox->addItems({"M", "F", "U"});
	applyBtn = new QPushButton("Apply", this);
	birthDateEdit = new QDateTimeEdit(this);
	deathDateEdit = new QDateTimeEdit(this);


	foreach(QWidget* widget, findChildren<QWidget*>()){
		widget->setDisabled(true);
	}

	QFormLayout* layout = new QFormLayout(this);
	layout->addRow("Name", firstNameEdit);
	layout->addRow("Middle Name", middleNameEdit);
	layout->addRow("Surname", surnameEdit);
	layout->addRow("Gender", genderBox);
	layout->addRow("Birth Date", birthDateEdit);
	layout->addRow("Death Date", deathDateEdit);
	layout->addRow(applyBtn);


	connect(applyBtn, &QPushButton::clicked,
		this, &PersonEditorWidget::ApplyChanges);

}


id_t PersonEditorWidget::ConnectedPerson() const{
	return connected_person;
}

void PersonEditorWidget::ApplyChanges(){

	if(connected_person == 0){
		return;
	}
	Person new_person_entry{
		.gender = genderBox->currentText().at(0),
		.first_name = firstNameEdit->text(),
		.middle_name = middleNameEdit->text(),
		.last_name = surnameEdit->text(),
		.birth_date = birthDateEdit->date(),
		.death_date = deathDateEdit->date()
	};

	db->updatePerson(new_person_entry, connected_person);
	emit personChanged(connected_person);
}

void PersonEditorWidget::ConnectToPerson(id_t new_id){

        connected_person = new_id;
	if(new_id != 0){
          auto new_person = db->getPersonById(connected_person);
          firstNameEdit->setText(new_person->first_name);
          middleNameEdit->setText(new_person->middle_name);
          surnameEdit->setText(new_person->last_name);
          genderBox->setCurrentText(QString(new_person->gender));
          birthDateEdit->setDate(new_person->birth_date);
          deathDateEdit->setDate(new_person->death_date);
          foreach (QWidget *widget, findChildren<QWidget *>()) {
            widget->setDisabled(false);
          }
        } else {
          foreach (QWidget *widget, findChildren<QWidget *>()) {
            widget->setDisabled(true);
          }
        }

}

