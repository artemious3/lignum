#pragma once
#include <QWidget>
#include <QFormLayout>
#include "FamilyTreeModel.h"
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <qlineedit.h>
#include "entities.h"


class PersonEditorWidget : public QWidget{
	Q_OBJECT

public:
	PersonEditorWidget(QWidget* parent = nullptr);
	void ConnectToPerson(id_t id);
	id_t ConnectedPerson() const;

public slots:
	void ApplyChanges();
signals:
	void personChanged(id_t id);
private:
	mftb::FamilyTreeModel* db;
	id_t connected_person = 0;


	QLineEdit *firstNameEdit,
		  *middleNameEdit, 
		  *surnameEdit;
	QComboBox *genderBox;
	QDateTimeEdit *birthDateEdit,
		      *deathDateEdit;

	QPushButton* applyBtn;
};
