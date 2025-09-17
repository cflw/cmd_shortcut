#include "argcheckbox.h"
C参数复选框代理::C参数复选框代理(QCheckBox &a控件):
	w复选框(&a控件) {
}
QString C参数复选框代理::fg值() const {
	if (w复选框->isChecked()) {
		return "true";
	}
	return "";
}
void C参数复选框代理::fs值(const QString &a文本) const {
    w复选框->setChecked(a文本 == "true");
}
bool C参数复选框代理::fi值() const {
	return w复选框->isChecked();
}
void C参数复选框代理::fs颜色(E颜色 a颜色) const {
    switch (a颜色) {
    case E颜色::e白:
        w复选框->setStyleSheet("");
        break;
    case E颜色::e红:
        w复选框->setStyleSheet("background-color: #ffc0c0;");
        break;
    case E颜色::e绿:
        w复选框->setStyleSheet("background-color: #c0ffc0;");
        break;
    case E颜色::e蓝:
        w复选框->setStyleSheet("background-color: #c0c0ff;");
        break;
    }
}
void C参数复选框代理::f清除输入() const {
    w复选框->setChecked(false);
}