#include "arglist.h"
#include <QStandardItemModel>
C�����б����::C�����б����(MainWindow &a������):
	m������(&a������),
	m�����б�ģ��(new QStandardItemModel(0, 2, &a������)) {
}
void C�����б����::f�����() {

}
void C�����б����::f�����(const S���� &a����) {
	const int v���к� = m�����б�ģ��->rowCount();
	m�����б�ģ��->insertRow(v���к�);

}
