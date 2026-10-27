#include "arglistmanage.h"
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QComboBox>
#include <QScrollBar>
#include <QEvent>
#include <QIcon>
#include <algorithm>
#include <vector>
#include <utility>
#include <QStringList>
#include "mainwindow.h"
#include "outmanage.h"
#include "ui_mainwindow.h"
#include "file_command.h"
#include "file_setting.h"
#include "arglistbuild.h"
#include "argtextbox.h"
#include "argfilebox.h"
//==============================================================================
// 参数列表
//==============================================================================
static const QString c其它名("其它");	//未分组参数的分组名
C参数列表::C参数列表(MainWindow &a主窗口):
	w主窗口(&a主窗口),
	w参数列表(a主窗口.ui->w_arglist),
	mp当前命令(&a主窗口.m当前命令),
	m模型(new QStandardItemModel(0, c列数, this)) {
	w参数列表->setModel(m模型.get());
	m模型->setHeaderData(c标签列, Qt::Horizontal, "参数名");
	m模型->setHeaderData(c输入列, Qt::Horizontal, "参数值");
	m模型->setHeaderData(c删除列, Qt::Horizontal, "删除");
	//垂直滚动条显示/隐藏时,重新设置列宽(缩短或恢复输入列)
	w参数列表->verticalScrollBar()->installEventFilter(this);
}
bool C参数列表::eventFilter(QObject *a对象, QEvent *a事件) {
	if (a对象 == w参数列表->verticalScrollBar() && (a事件->type() == QEvent::Show || a事件->type() == QEvent::Hide)) {
		f重置尺寸(w参数列表->width());	//可见性变化后重新设置列宽
	}
	return QObject::eventFilter(a对象, a事件);
}
void C参数列表::f重置尺寸(int a宽) {
	if (w参数列表->verticalScrollBar()->isVisible()) {	//垂直滚动条出现时才缩短输入列
		//预留垂直滚动条宽度,缩短输入列以容纳它,避免出现水平滚动条
		const int v竖条宽 = w参数列表->style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, w参数列表) + 2;	//加2像素,避免出现水平滚动条
		const int v可用宽 = a宽 - v竖条宽;
		const int v标签宽 = v可用宽 * 20 / 100 - 16;
		const int v删除宽 = 30;
		const int v输入宽 = v可用宽 - v标签宽 - v删除宽;	//输入列自动缩短,为垂直滚动条腾出空间
		w参数列表->setColumnWidth(c标签列, v标签宽);
		w参数列表->setColumnWidth(c输入列, v输入宽);
		w参数列表->setColumnWidth(c删除列, v删除宽);
	} else {	//无垂直滚动条,使用原始全宽布局,不缩短输入列
		w参数列表->setColumnWidth(c标签列, a宽 * 20 / 100 - 16);
		w参数列表->setColumnWidth(c输入列, a宽 * 80 / 100 - 16);
		w参数列表->setColumnWidth(c删除列, 30);
	}
	w参数列表->horizontalHeader()->setSectionResizeMode(c标签列, QHeaderView::Interactive);
	w参数列表->horizontalHeader()->setSectionResizeMode(c输入列, QHeaderView::Interactive);
	w参数列表->horizontalHeader()->setSectionResizeMode(c删除列, QHeaderView::Interactive);
}
void C参数列表::f刷新列表(const C命令文件 &a命令, const C实参组 *ap记忆实参) {	//在点击命令后调用
	f清空所有行();
	*mp当前命令 = &a命令;
	//先收集所有参数行(参数指针, 初始值), 多参数展开为多行
	std::vector<std::pair<const S命令参数 *, QString>> va参数值;
	if (ap记忆实参) {
		for (const S参数记忆 &a记忆 : ap记忆实参->ma参数值) {
			va参数值.push_back({a记忆.m参数, a记忆.m值});
		}
	} else {
		const C命令设置 &v设置 = a命令.fg设置();	//用来获取绑定设置值
		for (const S命令参数 &a参数 : a命令.fe命令参数()) {
			if (a参数.m绑定设置) {
				if (a参数.m多参数) {	//多参数可能绑定多个值,添加多行
					const std::vector<QString> v多参数值 = v设置.fg多参数值(a参数.m名称);
					if (v多参数值.empty()) {	//无绑定值,添加一行
						va参数值.push_back({&a参数, a参数.fg默认值()});
					} else {	//有绑定值,添加多行
						for (const QString &v绑定参数值 : v多参数值) {
							va参数值.push_back({&a参数, v绑定参数值});
						}
					}
				} else {	//单参数,添加一行: 有绑定值用绑定值,否则用命令文件的默认值
					const QString v绑定值 = v设置.fg绑定参数值(a参数.m名称);
					va参数值.push_back({&a参数, v绑定值.isEmpty() ? a参数.fg默认值() : v绑定值});
				}
			} else {	//无绑定设置,添加一行: 用命令文件的默认值
				va参数值.push_back({&a参数, a参数.fg默认值()});
			}
		}
	}
	//判断是否启用参数分组(仅当命令文件中存在分组属性)
	bool v有分组 = false;
	for (const auto &[v参数, v值] : va参数值) {
		if (!v参数->m分组.isEmpty()) {
			v有分组 = true;
			break;
		}
	}
	if (!v有分组) {	//没有任何参数分组, 不添加任何分组行(包括"其它")
		for (const auto &[v参数, v值] : va参数值) {
			f添加行(*v参数, v值);
		}
		return;
	}
	//按分组聚合(分组名按名称排序, "其它"放最后)
	std::map<QString, std::vector<std::pair<const S命令参数 *, QString>>> v分组;
	for (const auto &[v参数, v值] : va参数值) {
		const QString v组 = v参数->m分组.isEmpty() ? c其它名 : v参数->m分组;
		v分组[v组].push_back({v参数, v值});
	}
	QStringList v组名列表;
	bool v有其它 = false;
	for (const auto &[v名称, v列表] : v分组) {
		if (v名称 == c其它名) {
			v有其它 = true;
			continue;
		}
		v组名列表.append(v名称);
	}
	std::sort(v组名列表.begin(), v组名列表.end(), [](const QString &a, const QString &b)->bool {
		return a.compare(b, Qt::CaseInsensitive) < 0;
	});
	if (v有其它) {
		v组名列表.append(c其它名);
	}
	for (const QString &v组名 : v组名列表) {
		f添加分组行(v组名);
		for (const auto &[v参数, v值] : v分组[v组名]) {
			f添加行(*v参数, v值);
		}
	}
}
void C参数列表::f添加分组行(const QString &a组名) {	//添加参数分组行: 只显示分组名, 非参数行
	const int v新行号 = m模型->rowCount();
	m模型->insertRow(v新行号);
	QLabel *const w标签 = new QLabel(a组名, w参数列表);
	w标签->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	w标签->setStyleSheet("font-weight:bold; background-color:#e0e0e0;");	//分组行的标签列填充浅灰背景,与普通参数行区分
	w参数列表->setIndexWidget(m模型->index(v新行号, c标签列), w标签);
	//输入列和删除列也填充浅灰背景
	const QBrush v灰刷(QColor(224, 224, 224));
	for (int v列 = c输入列; v列 != c列数; ++v列) {
		m模型->setData(m模型->index(v新行号, v列), v灰刷, Qt::BackgroundRole);
	}
	//输入列和删除列留空
	ma代理.emplace_back();	//占位, 标记为分组行(空代理)
}
int C参数列表::fg行数() const {
	return m模型->rowCount();
}
void C参数列表::f清空所有行() {
	const int v行数 = m模型->rowCount();
	ma多参数计数.clear();	//清空行数统计,避免切换命令时用到旧计数
	if (v行数 == 0) {
		return;
	}
	for (int i = 0; i != v行数; ++i) {
		f删除行0(i);
	}
	//清空模型
	m模型->setRowCount(0);	//setRowCount保留表结构
	ma代理.clear();
}
void C参数列表::f添加行(const S命令参数 &a参数, QString a初始值) {
	const int v新行号 = m模型->rowCount();
	m模型->insertRow(v新行号);
	f构造行(v新行号, a参数, ma代理.emplace_back(), a初始值);
}
void C参数列表::f插入行(int a行号, const S命令参数 &a参数, QString a初始值) {
	const int v行数 = m模型->rowCount();
	m模型->insertRow(a行号);
	if (a行号 < v行数) {	//插入中间
		f构造行(a行号, a参数, *ma代理.emplace(ma代理.begin() + a行号), a初始值);
	} else {	//插入末尾
		f构造行(a行号, a参数, ma代理.emplace_back(), a初始值);
	}
}
void C参数列表::f构造行(int a行号, const S命令参数 &a参数, tp参数控件代理 &a代理, QString a初始值) {
	std::unique_ptr<I参数控件代理> v代理;
	//第1列标签
	QLabel *w标签 = new QLabel(a参数.m名称, w参数列表);
	w标签->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	w标签->setToolTip(a参数.m描述);
	w参数列表->setIndexWidget(m模型->index(a行号, c标签列), w标签);
	//第2列输入框
	const QModelIndex v输入控件索引 = m模型->index(a行号, c输入列);
	//判断参数类型,添加相应控件
	auto f建造输入控件 = [&](I参数列表建造 &a建造) {
		QWidget *const v新控件 = a建造.f创建控件();
		a建造.fs占位文本(a参数.m提示);
		if (!a初始值.isEmpty()) {
			a建造.fs初始值(a初始值);
		}
		w参数列表->setIndexWidget(v输入控件索引, v新控件);
		a建造.f连接(this);
		v代理 = a建造.f创建代理();
	};
	switch (a参数.m参数类型) {
	case E参数类型::e开关:
	{
		C建造复选框 v建造(w参数列表, a参数);
		f建造输入控件(v建造);
	}	break;
	case E参数类型::e路径:
	{
		C建造文件框 v建造(w参数列表, a参数);
		f建造输入控件(v建造);
	}	break;
	case E参数类型::e选项:
	{
		C建造组合框 v建造(w参数列表, a参数);
		f建造输入控件(v建造);
	}	break;
	default:
	{
		C建造文本框 v建造(w参数列表, a参数);
		f建造输入控件(v建造);
	}	break;
	}
	//第3列删除按钮(图标随后按是否多参数及行数确定)
	QPushButton *w删除按钮 = new QPushButton(w参数列表);
	w删除按钮->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh));
	w参数列表->setIndexWidget(m模型->index(a行号, c删除列), w删除按钮);
	connect(w删除按钮, &QPushButton::clicked, this, &C参数列表::on_argdelete_clicked);
	//添加附加数据
	v代理->m参数 = &a参数;
	a代理 = std::move(v代理);
	if (a参数.m多参数) {
		++ma多参数计数[a参数.m名称];
	}
	f更新删除按钮图标(a参数);	//行数变化后刷新该参数所有行的按钮图标
	//函数结束检查
	assert(w参数列表->indexWidget(v输入控件索引) != nullptr);	//新增控件忘记添加到模型会触发断言
}
void C参数列表::f删除行(int a行号) {
	//只有多参数才能调用
	const S命令参数 *const vp参数 = ma代理.at(a行号)->m参数;	//删除行后代理失效,先保存参数指针
	int &v多参数计数 = ma多参数计数.at(vp参数->m名称);
	assert(v多参数计数 > 1);
	--v多参数计数;
	f删除行0(a行号);
	f删除行1(a行号);
	f更新删除按钮图标(*vp参数);	//行数减少后刷新按钮图标(只剩一行时变回刷新图标)
}
void C参数列表::f更新删除按钮图标(const S命令参数 &a参数) {
	const bool v多行 = a参数.m多参数 && ma多参数计数.contains(a参数.m名称) && ma多参数计数.at(a参数.m名称) > 1;
	const QIcon v图标 = QIcon::fromTheme(v多行 ? QIcon::ThemeIcon::EditDelete : QIcon::ThemeIcon::ViewRefresh);
	const int v行数 = m模型->rowCount();
	for (int i = 0; i != v行数; ++i) {
		const tp参数控件代理 &v代理 = ma代理.at(i);
		if (!v代理 || v代理->m参数 != &a参数) {	//分组行或其它参数的行
			continue;
		}
		QPushButton *const w按钮 = qobject_cast<QPushButton *>(w参数列表->indexWidget(m模型->index(i, c删除列)));
		if (w按钮) {
			w按钮->setIcon(v图标);
			w按钮->setToolTip(v多行 ? "删除该行" : "恢复默认值");
		}
	}
}
void C参数列表::f删除行0(int a行号) {
	for (int v列号 = 0; v列号 != c列数; ++v列号) {
		const QModelIndex v索引 = m模型->index(a行号, v列号);
		QWidget *const w控件 = w参数列表->indexWidget(v索引);
		if (w控件) {
			w控件->deleteLater();
		}
		w参数列表->setIndexWidget(v索引, nullptr);
	}
}
void C参数列表::f删除行1(int a行号) {
	m模型->removeRow(a行号);
	//删除附加数据
	auto v当前代理迭代器 = ma代理.begin() + a行号;
	ma代理.erase(v当前代理迭代器);
}
void C参数列表::f清除输入(int a行号) {
	ma代理.at(a行号)->f清除输入();
}
bool C参数列表::fi可执行() const {
	bool v返回 = true;
	for (const tp参数控件代理 &v代理 : ma代理) {
		if (!v代理) {	//分组行, 跳过
			continue;
		}
		if (v代理->m参数->m必需) {
			if (!v代理->fi值()) {
				v返回 = false;
				v代理->fs颜色(E颜色::e红);
				C输出管理::g这->f输出行(QString("参数\"%1\"是必需, 不能为空").arg(v代理->m参数->m名称));
			} else {
				v代理->fs颜色(E颜色::e白);
			}
		}
	}
	return v返回;
}
C实参组 C参数列表::f生成实参组() {
	C实参组 v返回;
	const int v行数 = m模型->rowCount();
	for (int i = 0; i != v行数; ++i) {
		const tp参数控件代理 &v代理 = ma代理.at(i);
		if (!v代理) {	//分组行, 跳过
			continue;
		}
		const S命令参数 &v参数 = *v代理->m参数;
		const QString v值 = v代理->fg值();
		v返回.f添加(v参数, v值);
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
	const tp参数控件代理 &v当前代理 = ma代理.at(v当前行);
	if (v当前代理->m参数->m多参数 && ma多参数计数.at(v当前代理->m参数->m名称) > 1) {
		f删除行(v当前行);
	} else {
		f清除输入(v当前行);
	}
	w主窗口->f刷新命令框();
}
void C参数列表::on_argedit_returnPress() {
	W参数文本框 *const w文本框 = qobject_cast<W参数文本框 *>(sender());
	const int v当前行 = fg输入框所在行(w文本框);
	const I参数控件代理 *v当前代理 = ma代理.at(v当前行).get();	//不能引用智能指针,因为后续可能会修改代理列表,导致智能指针失效
	if (!v当前代理->m参数->m多参数) {
		return;	//不是多参数
	}
	const int v光标位置 = w文本框->cursorPosition();
	const QString v文本 = w文本框->text();
	const QString v分割文本0 = v文本.left(v光标位置);
	const QString v分割文本1 = v文本.right(v文本.size() - v光标位置);
	f插入行(v当前行, *v当前代理->m参数, v分割文本0);
	v当前代理->fs值(v分割文本1);
	w文本框->setCursorPosition(0);	//换行后光标放前面
	w主窗口->f刷新命令框();
}
void C参数列表::on_argedit_setMoreText(const QStringList &a) {
	const int v输入数量 = a.size();
	assert(v输入数量 > 0);
	W参数文本框 *const w文本框 = qobject_cast<W参数文本框 *>(sender());
	const int v当前行 = fg输入框所在行(w文本框);
	const S命令参数 *const v当前参数 = ma代理.at(v当前行)->m参数;
	if (v当前参数->m多参数) {	//是多参数,插入多行
		w文本框->setText(a.front());
		for (int i = 1; i != v输入数量; ++i) {	//插入行
			const int v新行 = v当前行 + i;
			f插入行(v新行, *v当前参数);
			tp参数控件代理 &v代理 = ma代理.at(v新行);
			v代理->fs值(a.at(i));
		}
	} else {	//不是多参数,转成单行
		w文本框->setText(a.join(" "));
	}
	w主窗口->f刷新命令框();
}
void C参数列表::on_argcombobox_currentIndexChanged(int a) {
	w主窗口->f刷新命令框();
}