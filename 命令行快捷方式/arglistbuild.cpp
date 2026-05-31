#include "arglistbuild.h"
#include "arglistmanage.h"
#include "arginterface.h"
#include "cmdarg.h"
#include "argtextbox.h"
#include "argfilebox.h"
#include "argcheckbox.h"
#include "argcombobox.h"
//==============================================================================
// 接口
//==============================================================================
I参数列表建造::I参数列表建造(QTableView *a父, const S命令参数 &a参数):
	m父(a父), m参数(&a参数) {
}
//==============================================================================
// 文本框
//==============================================================================
void f文本框连接(W参数文本框 *a文本框, C参数列表 *a参数列表) {
	QObject::connect(a文本框, &QLineEdit::textEdited, a参数列表, &C参数列表::on_argedit_textEdited);
	QObject::connect(a文本框, &QLineEdit::returnPressed, a参数列表, &C参数列表::on_argedit_returnPress);
	QObject::connect(a文本框, &W参数文本框::setMoreText, a参数列表, &C参数列表::on_argedit_setMoreText);
}
QWidget *C建造文本框::f创建控件() {
	assert(m控件 == nullptr);	//只能创建一次
	m控件 = new W参数文本框(m父);
	return m控件;
}
std::unique_ptr<I参数控件代理> C建造文本框::f创建代理() const {
	W参数文本框 *const v文本框 = static_cast<W参数文本框 *>(m控件);
	return std::make_unique<C参数文本框代理>(*v文本框);
}
void C建造文本框::fs占位文本(QString a文本) const {
	W参数文本框 *const v文本框 = static_cast<W参数文本框 *>(m控件);
	v文本框->setPlaceholderText(a文本);
}
void C建造文本框::fs初始值(QString a值) const {
	W参数文本框 *const v文本框 = static_cast<W参数文本框 *>(m控件);
	v文本框->setText(a值);
}
void C建造文本框::f连接(C参数列表 *a参数列表) const {
	W参数文本框 *const v文本框 = static_cast<W参数文本框 *>(m控件);
	f文本框连接(v文本框, a参数列表);
}
//==============================================================================
// 文件框
//==============================================================================
QWidget *C建造文件框::f创建控件() {
	assert(m控件 == nullptr);	//只能创建一次
	m控件 = new W参数文件框(m父, *m参数);
	return m控件;
}
std::unique_ptr<I参数控件代理> C建造文件框::f创建代理() const {
	W参数文件框 *const v文件框 = static_cast<W参数文件框*>(m控件);
	return std::make_unique<C参数文本框代理>(v文件框->fg文本框());
}
void C建造文件框::fs占位文本(QString a文本) const {
	W参数文件框 *const v文件框 = static_cast<W参数文件框 *>(m控件);
	v文件框->setPlaceholderText(a文本);
}
void C建造文件框::fs初始值(QString a值) const {
	W参数文件框 *const v文件框 = static_cast<W参数文件框 *>(m控件);
	v文件框->setText(a值);
}
void C建造文件框::f连接(C参数列表 *a参数列表) const {
	W参数文件框 *const v文件框 = static_cast<W参数文件框 *>(m控件);
	W参数文本框 *const v文本框 = &v文件框->fg文本框();
	f文本框连接(v文本框, a参数列表);
}
//==============================================================================
// 复选框
//==============================================================================
QWidget *C建造复选框::f创建控件() {
	assert(m控件 == nullptr);	//只能创建一次
	m控件 = new QCheckBox(m父);
	return m控件;
}
std::unique_ptr<I参数控件代理> C建造复选框::f创建代理() const {
	QCheckBox *const v复选框 = static_cast<QCheckBox *>(m控件);
	return std::make_unique<C参数复选框代理>(*v复选框);
}
void C建造复选框::fs占位文本(QString a文本) const {
	QCheckBox *const v复选框 = static_cast<QCheckBox *>(m控件);
	v复选框->setText(a文本);
}
void C建造复选框::fs初始值(QString a值) const {
	QCheckBox *const v复选框 = static_cast<QCheckBox *>(m控件);
	v复选框->setChecked(!a值.isEmpty());
}
void C建造复选框::f连接(C参数列表 *a参数列表) const {
	QCheckBox *const v复选框 = static_cast<QCheckBox *>(m控件);
	QObject::connect(v复选框, &QCheckBox::checkStateChanged, a参数列表, &C参数列表::on_argcheck_stateChanged);
}
//==============================================================================
// 组合框
//==============================================================================
QWidget *C建造组合框::f创建控件() {
	assert(m控件 == nullptr);	//只能创建一次
	QComboBox *const v组合框 = new QComboBox(m父);
	for (const S组合框选项 &v选项 : m参数->ma选项) {
		v组合框->addItem(v选项.fg显示文本());
	}
	m控件 = v组合框;
	return m控件;
}
std::unique_ptr<I参数控件代理> C建造组合框::f创建代理() const {
	QComboBox *const v组合框 = static_cast<QComboBox *>(m控件);
	return std::make_unique<C参数组合框代理>(*v组合框);
}
void C建造组合框::fs占位文本(QString a文本) const {
	QComboBox *const v组合框 = static_cast<QComboBox *>(m控件);
	v组合框->setPlaceholderText(a文本);
}
void C建造组合框::fs初始值(QString a值) const {
	QComboBox *const v组合框 = static_cast<QComboBox *>(m控件);
	const int v索引 = m参数->f选项_找索引(a值);
	v组合框->setCurrentIndex(v索引);
}
void C建造组合框::f连接(C参数列表 *a参数列表) const {
	QComboBox *const v组合框 = static_cast<QComboBox *>(m控件);
	QObject::connect(v组合框, &QComboBox::currentIndexChanged, a参数列表, &C参数列表::on_argcombobox_currentIndexChanged);
}