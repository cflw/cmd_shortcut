#include "arglistmanage.h"
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "file_command.h"
#include "argtextbox.h"
#include "argfilebox.h"
#include "argcheckbox.h"
//==============================================================================
// 参数列表
//==============================================================================
C参数列表::C参数列表(MainWindow &a主窗口):
	w主窗口(&a主窗口),
	w参数列表(a主窗口.ui->w_arglist),
	mp当前命令(&a主窗口.m当前命令),
	m模型(new QStandardItemModel(0, c列数, this)) {
	w参数列表->setModel(m模型.get());
	m模型->setHeaderData(c标签列, Qt::Horizontal, "参数名");
	m模型->setHeaderData(c输入列, Qt::Horizontal, "参数值");
	m模型->setHeaderData(c删除列, Qt::Horizontal, "删除");
}
void C参数列表::f重置尺寸(int a宽) {
	w参数列表->setColumnWidth(c标签列, a宽 * 20 / 100 - 16);
	w参数列表->setColumnWidth(c输入列, a宽 * 80 / 100 - 16);
	w参数列表->setColumnWidth(c删除列, 30);
	w参数列表->horizontalHeader()->setSectionResizeMode(c标签列, QHeaderView::Interactive);
	w参数列表->horizontalHeader()->setSectionResizeMode(c输入列, QHeaderView::Interactive);
	w参数列表->horizontalHeader()->setSectionResizeMode(c删除列, QHeaderView::Interactive);
}
void C参数列表::f刷新列表(const C命令文件 &a命令) {	//在点击命令后调用
	f清空所有行();
	*mp当前命令 = &a命令;
	for (const S命令参数 &a参数 : a命令.fe命令参数()) {
		f添加行(a参数);
	}
}
int C参数列表::fg行数() const {
	return m模型->rowCount();
}
void C参数列表::f清空所有行() {
	const int v行数 = m模型->rowCount();
	if (v行数 == 0) {
		return;
	}
	for (int i = 0; i != v行数; ++i) {
		f删除行0(i);
	}
	//清空模型
	m模型->setRowCount(0);	//setRowCount保留表结构
	ma附加数据.clear();
}
void C参数列表::f添加行(const S命令参数 &a参数) {
	const int v新行号 = m模型->rowCount();
	m模型->insertRow(v新行号);
	f构造行(v新行号, a参数, ma附加数据.emplace_back());
}
void C参数列表::f插入行(int a行号, const S命令参数 &a参数) {
	const int v行数 = m模型->rowCount();
	m模型->insertRow(a行号);
	if (a行号 < v行数) {	//插入中间
		f构造行(a行号, a参数, *ma附加数据.emplace(ma附加数据.begin() + a行号));
	} else {	//插入末尾
		f构造行(a行号, a参数, ma附加数据.emplace_back());
	}
}
void C参数列表::f构造行(int a行号, const S命令参数 &a参数, S附加数据 &a附加数据) {
	std::unique_ptr<const I参数控件代理> v代理;
	//第1列标签
	QLabel *w标签 = new QLabel(a参数.m名称, w参数列表);
	w标签->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	w标签->setToolTip(a参数.m描述);
	w参数列表->setIndexWidget(m模型->index(a行号, c标签列), w标签);
	//第2列输入框
	const QModelIndex v输入控件索引 = m模型->index(a行号, c输入列);
	const QString v绑定参数值 = (*mp当前命令)->fg绑定参数值(a参数.m名称);
	//判断参数类型,添加相应控件
	switch (a参数.m参数类型) {
	case E参数类型::e开关:
	{
		QCheckBox *w复选框 = new QCheckBox(w参数列表);
		w复选框->setText(a参数.m提示);
		if (a参数.m绑定设置) {	//绑定参数
			if (!v绑定参数值.isEmpty()) {
				w复选框->setChecked(true);
			}
		}
		w参数列表->setIndexWidget(v输入控件索引, w复选框);
		connect(w复选框, &QCheckBox::checkStateChanged, this, &C参数列表::on_argcheck_stateChanged);
		v代理 = std::make_unique<C参数复选框代理>(*w复选框);
	}	break;
	case E参数类型::e路径:
	{
		W参数文件框 *const w文件框 = new W参数文件框(w参数列表, a参数);
		w文件框->setPlaceholderText(a参数.m提示);
		if (a参数.m绑定设置) {	//绑定参数
			w文件框->setText(v绑定参数值);
		}
		w参数列表->setIndexWidget(v输入控件索引, w文件框);
		W参数文本框 *const w文本框 = &w文件框->fg文本框();
		connect(w文本框, &QLineEdit::textEdited, this, &C参数列表::on_argedit_textEdited);
		connect(w文本框, &QLineEdit::returnPressed, this, &C参数列表::on_argedit_returnPress);
		connect(w文本框, &W参数文本框::setMoreText, this, &C参数列表::on_argedit_setMoreText);
		v代理 = std::make_unique<C参数文本框代理>(w文件框->fg文本框());
	}	break;
	default:
	{	//默认添加文本框
		W参数文本框 *w文本框 = new W参数文本框(w参数列表);
		w文本框->setPlaceholderText(a参数.m提示);
		if (a参数.m绑定设置) {	//绑定参数
			w文本框->setText(v绑定参数值);
		}
		w参数列表->setIndexWidget(v输入控件索引, w文本框);
		connect(w文本框, &QLineEdit::textEdited, this, &C参数列表::on_argedit_textEdited);
		connect(w文本框, &QLineEdit::returnPressed, this, &C参数列表::on_argedit_returnPress);
		connect(w文本框, &W参数文本框::setMoreText, this, &C参数列表::on_argedit_setMoreText);
		v代理 = std::make_unique<C参数文本框代理>(*w文本框);
	}	break;
	}
	//第3列删除按钮
	QPushButton *w删除按钮 = new QPushButton(w参数列表);
	w删除按钮->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete));
	w参数列表->setIndexWidget(m模型->index(a行号, c删除列), w删除按钮);
	connect(w删除按钮, &QPushButton::clicked, this, &C参数列表::on_argdelete_clicked);
	//添加附加数据
	a附加数据.m参数 = &a参数;
	a附加数据.m代理 = std::move(v代理);
	if (a参数.m多参数) {
		++ma多参数计数[a参数.m名称];
	}
}
void C参数列表::f删除行(int a行号) {
	//只有多参数才能调用
	const S附加数据 &v附加数据 = ma附加数据.at(a行号);
	int &v多参数计数 = ma多参数计数.at(v附加数据.m参数->m名称);
	assert(v多参数计数 > 1);
	--v多参数计数;
	f删除行0(a行号);
	f删除行1(a行号);
}
void C参数列表::f删除行0(int a行号) {
	for (int v列号 = 0; v列号 != c列数; ++v列号) {
		const QModelIndex v索引 = m模型->index(a行号, v列号);
		QWidget *const w控件 = w参数列表->indexWidget(v索引);
		w控件->deleteLater();
		w参数列表->setIndexWidget(v索引, nullptr);
	}
}
void C参数列表::f删除行1(int a行号) {
	m模型->removeRow(a行号);
	//删除附加数据
	auto v当前附加数据迭代器 = ma附加数据.begin() + a行号;
	ma附加数据.erase(v当前附加数据迭代器);
}
void C参数列表::f清除输入(int a行号) {
	ma附加数据.at(a行号).m代理->f清除输入();
}
bool C参数列表::fi可执行() const {
	bool v返回 = true;
	for (const S附加数据 &v附加数据 : ma附加数据) {
		if (v附加数据.m参数->m必需) {
			if (!v附加数据.m代理->fi值()) {
				v返回 = false;
				v附加数据.m代理->fs颜色(E颜色::e红);
			} else {
				v附加数据.m代理->fs颜色(E颜色::e白);
			}
		}
	}
	return v返回;
}
C实参组 C参数列表::f生成实参组() {
	C实参组 v返回;
	const int v行数 = m模型->rowCount();
	for (int i = 0; i != v行数; ++i) {
		const S附加数据 &v附加数据 = ma附加数据.at(i);
		const S命令参数 &v参数 = *v附加数据.m参数;
		const I参数控件代理 &v代理 = *v附加数据.m代理;
		const QString v值 = v代理.fg值();
		if (v值.isEmpty()) {
			continue;
		} else {
			v返回.f添加(v参数.m名称, v值);
		}
	}
	return v返回;
}
int C参数列表::fg输入框所在行(QWidget *a输入框) const {
	const int v行数 = m模型->rowCount();
	for (int i = 0; i != v行数; ++i) {
		const QModelIndex v索引 = m模型->index(i, c输入列);
		QWidget *const w输入框 = w参数列表->indexWidget(v索引);
		if (a输入框 == w输入框) {
			return i;
		}
		if (W参数文件框 *const w文件框 = dynamic_cast<W参数文件框 *>(w输入框)) {
			if (&w文件框->fg文本框() == a输入框) {
				return i;
			}
		}
	}
	return -1;
}
int C参数列表::fg删除按钮所在行(QPushButton *a删除按钮) const {
	const int v行数 = m模型->rowCount();
	for (int i = 0; i != v行数; ++i) {
		const QModelIndex v索引 = m模型->index(i, c删除列);
		QWidget *const w删除按钮 = w参数列表->indexWidget(v索引);
		if (w删除按钮 == a删除按钮) {
			return i;
		}
	}
	return -1;
}
void C参数列表::on_argedit_textEdited(const QString &) {
	w主窗口->f刷新命令框();
}
void C参数列表::on_argcheck_stateChanged(int) {
	w主窗口->f刷新命令框();
}
void C参数列表::on_argdelete_clicked() {
	//判断按钮所在行号
	QPushButton *const w删除按钮 = qobject_cast<QPushButton *>(sender());
	const int v当前行 = fg删除按钮所在行(w删除按钮);	//必需非-1
	const S附加数据 &v附加数据 = ma附加数据.at(v当前行);
	if (v附加数据.m参数->m多参数 && ma多参数计数.at(v附加数据.m参数->m名称) > 1) {
		f删除行(v当前行);
	} else {
		f清除输入(v当前行);
	}
	w主窗口->f刷新命令框();
}
void C参数列表::on_argedit_returnPress() {
	W参数文本框 *const w文本框 = qobject_cast<W参数文本框 *>(sender());
	const int v当前行 = fg输入框所在行(w文本框);
	const S附加数据 &v附加数据 = ma附加数据.at(v当前行);
	if (!v附加数据.m参数->m多参数) {
		return;	//不是多参数
	}
	const int v光标位置 = w文本框->cursorPosition();
	const QString v文本 = w文本框->text();
	const QString v分割文本0 = v文本.left(v光标位置);
	const QString v分割文本1 = v文本.right(v文本.size() - v光标位置);
	f插入行(v当前行, *ma附加数据.at(v当前行).m参数);
	ma附加数据.at(v当前行).m代理->fs值(v分割文本0);
	w文本框->setText(v分割文本1);
	w文本框->setCursorPosition(0);	//换行后光标放前面
	w主窗口->f刷新命令框();
}
void C参数列表::on_argedit_setMoreText(const QStringList &a) {
	const int v输入数量 = a.size();
	assert(v输入数量 > 0);
	W参数文本框 *const w文本框 = qobject_cast<W参数文本框 *>(sender());
	const int v当前行 = fg输入框所在行(w文本框);
	const S命令参数 *const v参数 = ma附加数据.at(v当前行).m参数;
	if (v参数->m多参数) {	//是多参数,插入多行
		w文本框->setText(a.front());
		for (int i = 1; i != v输入数量; ++i) {	//插入行
			const int v新行 = v当前行 + i;
			f插入行(v新行, *v参数);
			const S附加数据 &v新附加数据 = ma附加数据.at(v新行);
			v新附加数据.m代理->fs值(a.at(i));
		}
	} else {	//不是多参数,转成单行
		w文本框->setText(a.join(" "));
	}
	w主窗口->f刷新命令框();
}