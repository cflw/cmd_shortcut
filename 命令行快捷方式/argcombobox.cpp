#include "argcombobox.h"
#include "cmdarg.h"
//==============================================================================
// 参数
//==============================================================================
QString S组合框选项::fg显示文本() const {
	return m名称.isEmpty() ? m值 : QString("%1(%2)").arg(m名称, m值);
}
//==============================================================================
// 代理
//==============================================================================
C参数组合框代理::C参数组合框代理(QComboBox &a组合框):
	w组合框(&a组合框) {
}
QString C参数组合框代理::fg值() const {
	const int v索引 = w组合框->currentIndex();
	return m参数->f选项_g值(v索引);
}
void C参数组合框代理::fs值(const QString &a值) const {
	const int v索引 = m参数->f选项_找索引(a值);
	w组合框->setCurrentIndex(v索引);
}
bool C参数组合框代理::fi值() const {
	return w组合框->currentIndex();
}
void C参数组合框代理::fs颜色(E颜色 a颜色) const {
	switch (a颜色) {
	case E颜色::e白:
		w组合框->setStyleSheet("");
		break;
	case E颜色::e红:
		w组合框->setStyleSheet("background-color: #ffe0e0;");
		break;
	case E颜色::e绿:
		w组合框->setStyleSheet("background-color: #e0ffe0;");
		break;
	case E颜色::e蓝:
		w组合框->setStyleSheet("background-color: #e0e0ff;");
		break;
	}
}
void C参数组合框代理::f清除输入() const {	//组合框应该回到默认选项
	w组合框->setCurrentIndex(m参数->f选项_默认索引());
}
void C参数组合框代理::fs索引(int a索引) const {
	w组合框->setCurrentIndex(a索引);
}
int C参数组合框代理::fg索引() const {
	return w组合框->currentIndex();
}