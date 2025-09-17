#include "outmanage.h"
#include <QScrollBar>
C输出管理::C输出管理(QPlainTextEdit &a文本框):
	w输出框(&a文本框) {
	w输出框->setReadOnly(true);
	w输出框->setWordWrapMode(QTextOption::WrapAnywhere);
}
void C输出管理::f打印程序信息() {
	f输出行("《命令行快捷方式》");
#ifdef __clang__
	f输出行(QString("编译器: Clang %1.%2.%3").arg(__clang_major__, __clang_minor__, __clang_patchlevel__));
#elif defined(__GNUC__)
	f输出行(QString("编译器: GCC %1.%2.%3").arg(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__));
#elif defined(_MSC_VER)
	f输出行(QString("编译器: MSVC %1").arg(_MSC_VER));
#else
	f输出行("编译器: 未知");
#endif
	f输出行(QString("编译时间: %1 %2").arg(__DATE__, __TIME__));
}
void C输出管理::f输出(const QString &a文本) {
	w输出框->moveCursor(QTextCursor::End);
	w输出框->insertPlainText(a文本);	//不会输出换行符
	w输出框->verticalScrollBar()->setValue(w输出框->verticalScrollBar()->maximum());
}
void C输出管理::f输出行(const QString &a文本) {
	w输出框->moveCursor(QTextCursor::End);
	w输出框->insertPlainText(a文本);
	w输出框->appendPlainText("");
	w输出框->verticalScrollBar()->setValue(w输出框->verticalScrollBar()->maximum());
}
void C输出管理::f换行() {
	w输出框->appendPlainText("");
}