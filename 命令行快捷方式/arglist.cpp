#include "arglist.h"
#include <QStandardItemModel>
C参数列表控制::C参数列表控制(MainWindow &a主窗口):
	m主窗口(&a主窗口),
	m参数列表模型(new QStandardItemModel(0, 2, &a主窗口)) {
}
void C参数列表控制::f清空行() {

}
void C参数列表控制::f添加行(const S参数 &a参数) {
	const int v新行号 = m参数列表模型->rowCount();
	m参数列表模型->insertRow(v新行号);

}
