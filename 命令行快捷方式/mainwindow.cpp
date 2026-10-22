#include "mainwindow.h"
#include <ranges>
#include <algorithm>
#include <set>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStringListModel>
#include <QStyledItemDelegate>
#include <QClipboard>
#include <QCheckBox>
#include <QTimer>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QStandardItem>
#include <QApplication>
#include <QStyledItemDelegate>
#include <QStyle>
#include <QPainter>
#include "ui_mainwindow.h"
#include "file_setting.h"
#include "file_command.h"
#include "arglistmanage.h"
#include "outmanage.h"
#include "cmdmanage.h"
#include "argtemplate.h"
#include <utility>
//==============================================================================
// 常量
//==============================================================================
static const QString c未分组名 = "未分组";
//==============================================================================
// 命令列表代理: 取消命令行的缩进(分组行保留缩进以显示折叠/展开按钮)
//==============================================================================
class C命令列表代理 : public QStyledItemDelegate {
public:
	C命令列表代理(QTreeView *a视图, QObject *a父 = nullptr): QStyledItemDelegate(a父), m视图(a视图) {}
	void paint(QPainter *a画笔, const QStyleOptionViewItem &a选项, const QModelIndex &a索引) const override {
		if (a索引.parent().isValid()) {	//命令行(子项): 取消缩进, 文字对齐到行首
			QStyleOptionViewItem v背景 = a选项;
			v背景.rect.adjust(-m视图->indentation(), 0, 0, 0);	//背景覆盖到行首, 避免左侧出现间隙
			v背景.text.clear();
			v背景.icon = QIcon();
			QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &v背景, a画笔, m视图);
			QStyleOptionViewItem v文字 = a选项;
			v文字.rect.adjust(-m视图->indentation(), 0, 0, 0);
			QPalette::ColorGroup v组 = (a选项.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
			if (v组 == QPalette::Normal && !(a选项.state & QStyle::State_Active)) {
				v组 = QPalette::Inactive;
			}
			const QColor v色 = a选项.palette.color(v组, QPalette::Text);
			a画笔->setPen(v色);
			a画笔->setFont(a选项.font);
			const QString v文本 = a选项.fontMetrics.elidedText(a索引.data(Qt::DisplayRole).toString(), a选项.textElideMode, v文字.rect.width());
			a画笔->drawText(v文字.rect, a选项.displayAlignment, v文本);
			return;
		}
		QStyledItemDelegate::paint(a画笔, a选项, a索引);
	}
private:
	QTreeView *m视图;
};
//==============================================================================
// 主窗口
//==============================================================================
MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent), ui(new Ui::MainWindow),
	m命令列表模型(new QStandardItemModel(this))
{
    ui->setupUi(this);
	ui->w_cmdlist->setModel(m命令列表模型.get());
	ui->w_cmdlist->setItemDelegate(new C命令列表代理(ui->w_cmdlist, this));
	m参数列表 = std::make_unique<C参数列表>(*this);
	m输出管理 = std::make_unique<C输出管理>(*ui->w_out);
	m命令管理 = std::make_unique<C命令管理>(this, *m输出管理);
	//connect(ui->w_copy, &QToolButton::clicked, this, &MainWindow::on_w_copy_clicked);
	//connect(ui->w_exec, &QToolButton::clicked, this, &MainWindow::on_w_exec_clicked);
	connect(ui->w_cmdlist->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &MainWindow::on_w_cmdlist_currentChanged);
	//右键菜单
	ui->w_cmdlist->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->w_cmdlist, &QTreeView::customContextMenuRequested,
			this, &MainWindow::f命令列表_右键菜单);
	//折叠状态跟踪
	connect(ui->w_cmdlist, &QTreeView::collapsed, this, [this](const QModelIndex &a索引) {
		if (QStandardItem *const v项 = m命令列表模型->itemFromIndex(a索引)) {
			ma折叠分组.insert(v项->text());
		}
	});
	connect(ui->w_cmdlist, &QTreeView::expanded, this, [this](const QModelIndex &a索引) {
		if (QStandardItem *const v项 = m命令列表模型->itemFromIndex(a索引)) {
			ma折叠分组.remove(v项->text());
		}
	});
	QTimer::singleShot(0, [this]() {	//主窗口显示后的初始化
		m参数列表->f重置尺寸(ui->w_arglist->width());
	});
}

MainWindow::~MainWindow() {
	m参数列表->f清空所有行();
}
void MainWindow::f载入文件() {
	m设置文件 = std::make_unique<C设置文件>();
	if (!C命令文件::c命令目录.exists()) {
		m输出管理->f输出行("错误: \"command\"目录不存在!");
		return;
	}
	//打开设置文件
	m设置文件->f打开(C设置文件::fg保存路径());
	//刷新命令文件列表
	m命令文件缓存 = std::make_unique<C命令文件缓存>();
	for (const QString &v新文件名 : C命令文件::fe所有文件() 
		| std::views::filter([&](const QString &a文件名0)->bool {	//{所有文件} - {存在文件} = {新文件}
			return !std::ranges::any_of(
				m设置文件->fe所有文件(),
				[&](const QString &a)->bool {return a == a文件名0; }); 
			})
		) {	//查找新文件
		m输出管理->f输出(QString("发现新文件: %1...").arg(v新文件名));
		C命令文件 *vp命令文件 = m命令文件缓存->f打开(v新文件名);
		C命令设置 &v命令设置 = m设置文件->f构造命令();
		v命令设置.m文件名 = v新文件名;
		if (!vp命令文件) {
			m输出管理->f输出行("打开失败");
			v命令设置.m启用 = false;
			continue;
		}
		v命令设置.m启用 = vp命令文件->fg平台() & m设置文件->fg平台();
		m输出管理->f输出行(v命令设置.m启用 ? "自动启用" : "不启用");
	}
	//结束
	f保存设置();
}
void MainWindow::f刷新命令列表(bool a重新打开) {	//在载入文件后窗口显示后调用,或者命令列表框刷新时调用
	if (m设置文件->fi空()) {
		m输出管理->f输出行("警告: 无命令");
	}
	//打开命令文件,并关联设置与命令
	std::vector<const C命令文件 *> va命令文件;
	for (C命令设置 &v命令设置 : m设置文件->fe启用命令()) {
		if (C命令文件 *const vp命令文件 = m命令文件缓存->f打开(v命令设置.m文件名, a重新打开)) {
			va命令文件.push_back(vp命令文件);
			vp命令文件->fs设置(v命令设置);
			for (S命令参数 &v命令参数 : vp命令文件->fe命令参数()) {
				if (v命令参数.m绑定设置) {
					if (!v命令设置.ma绑定参数.contains(v命令参数.m名称)) {	//try_emplace()
						v命令设置.ma绑定参数.emplace(v命令参数.m名称, QString());
					}
				}
			}
		} else {
			m输出管理->f输出行(QString("错误: 已启用的命令文件\"%1\"不存在!").arg(v命令设置.m文件名));
		}
	}
	//按名称排序
	std::sort(va命令文件.begin(), va命令文件.end(), [](const C命令文件 *a, const C命令文件 *b)->bool {
		return a->fg名称().compare(b->fg名称(), Qt::CaseInsensitive) < 0;
	});
	//设置列表
	ma启用命令 = std::move(va命令文件);
	f重建命令树();
}
void MainWindow::f重新载入命令() {	//重新扫描命令目录已启用的命令文件,重新加载并刷新列表框
	//保存当前命令设置,并采集所有命令的记忆实参(按文件名)用于刷新后恢复
	if (m设置文件) {
		f更新设置();
		f保存设置();
	}
	const QString v当前文件名 = m当前命令 ? m当前命令->fg文件名() : QString();
	const std::map<QString, std::vector<std::pair<QString, QString>>> v全部记忆 = f备份所有记忆();
	//重新加载会销毁旧的命令文件对象,旧的选择指针和实参记忆均已失效,先清空
	f清空命令列表();
	f刷新命令列表(true);
	//恢复所有命令的记忆实参,再恢复当前选择的命令
	f恢复所有记忆(v全部记忆);
	f恢复选择命令(v当前文件名);
}
std::map<QString, std::vector<std::pair<QString, QString>>> MainWindow::f备份所有记忆() const {	//采集所有命令的记忆实参,按文件名分类
	std::map<QString, std::vector<std::pair<QString, QString>>> v返回;
	for (const auto &[vp命令, v实参组] : ma记忆实参) {
		std::vector<std::pair<QString, QString>> v列表;
		v列表.reserve(v实参组.ma参数值.size());
		for (const S参数记忆 &v记忆 : v实参组.ma参数值) {
			if (v记忆.m参数) {
				v列表.emplace_back(v记忆.m参数->m名称, v记忆.m值);
			}
		}
		v返回[vp命令->fg文件名()] = std::move(v列表);
	}
	return v返回;
}
void MainWindow::f清空命令列表() {	//清空当前选择与记忆,避免悬空引用
	m当前命令 = nullptr;
	ma记忆实参.clear();
	m参数列表->f清空所有行();	//清空参数面板,避免其控件仍持有旧命令的引用
	ui->w_cmd->setText(QString());
}
C实参组 MainWindow::f恢复记忆实参(const C命令文件 *a命令, const std::vector<std::pair<QString, QString>> &a列表) const {	//用新命令参数指针重建记忆实参
	C实参组 v返回;
	std::map<QString, int> v已用;	//每个参数名已匹配的次数,用于区分多参数
	for (const auto &[v名, v值] : a列表) {
		int v序号 = 0;
		for (const S命令参数 &v参数 : a命令->fe命令参数()) {
			if (v参数.m名称 == v名) {
				if (v序号 == v已用[v名]) {
					v返回.f添加(v参数, v值);
					++v已用[v名];
					break;
				}
				++v序号;
			}
		}
	}
	return v返回;
}
QModelIndex MainWindow::f查找命令索引(const C命令文件 *a命令) const {	//在命令树中查找命令对应的索引
	for (int i = 0; i != m命令列表模型->rowCount(); ++i) {
		QStandardItem *const v组项 = m命令列表模型->item(i);
		if (!v组项) {
			continue;
		}
		for (int j = 0; j != v组项->rowCount(); ++j) {
			QStandardItem *const v命令项 = v组项->child(j);
			if (v命令项 && v命令项->data(Qt::UserRole).value<quint64>() == reinterpret_cast<quintptr>(a命令)) {
				return v命令项->index();
			}
		}
	}
	return QModelIndex();
}
void MainWindow::f恢复所有记忆(const std::map<QString, std::vector<std::pair<QString, QString>>> &a全部) {	//刷新后恢复所有命令的记忆实参
	for (const auto &[v文件名, v列表] : a全部) {
		const C命令文件 *vp新命令 = nullptr;
		for (const C命令文件 *vp命令 : ma启用命令) {
			if (vp命令->fg文件名() == v文件名) {
				vp新命令 = vp命令;
				break;
			}
		}
		if (vp新命令) {
			ma记忆实参[vp新命令] = f恢复记忆实参(vp新命令, v列表);
		}
	}
}
void MainWindow::f恢复选择命令(const QString &a文件名) {	//刷新后恢复当前选择的命令(记忆实参已由 f恢复所有记忆 恢复)
	if (a文件名.isEmpty()) {
		return;
	}
	const C命令文件 *vp新命令 = nullptr;
	for (const C命令文件 *vp命令 : ma启用命令) {
		if (vp命令->fg文件名() == a文件名) {
			vp新命令 = vp命令;
			break;
		}
	}
	if (!vp新命令) {
		return;
	}
	const QModelIndex v索引 = f查找命令索引(vp新命令);
	if (v索引.isValid()) {
		ui->w_cmdlist->setCurrentIndex(v索引);	//触发界面刷新恢复参数
	}
}
void MainWindow::f重建命令树() {	//根据启用命令和分组设置重建左侧树
	const C命令文件 *const v原当前 = m当前命令;
	//按分组收集命令: 分组优先级为 设置文件 > 命令文件默认分组 > 未分组
	std::map<QString, QList<const C命令文件 *>> v分组命令;
	for (const C命令文件 *vp命令 : ma启用命令) {
		QString v组 = m设置文件->fg命令分组(vp命令->fg文件名());
		if (v组.isEmpty()) {
			v组 = vp命令->fg分组();
		}
		if (v组.isEmpty()) {
			v组 = c未分组名;
		}
		v分组命令[v组].append(vp命令);
	}
	//分组名排序(未分组放在最后)
	QStringList v组名列表;
	bool v有未分组 = false;
	for (const auto &[v名称, v列表] : v分组命令) {
		if (v名称 == c未分组名) {
			v有未分组 = true;
			continue;
		}
		v组名列表.append(v名称);
	}
	std::sort(v组名列表.begin(), v组名列表.end(), [](const QString &a, const QString &b)->bool {
		return a.compare(b, Qt::CaseInsensitive) < 0;
	});
	if (v有未分组) {
		v组名列表.append(c未分组名);
	}
	//重建模型
	m命令列表模型->clear();
	QModelIndex v新当前索引;
	for (const QString &v组名 : v组名列表) {
		QStandardItem *const v组项 = new QStandardItem(v组名);
		v组项->setFlags(Qt::ItemIsEnabled);	//分组行不可作为命令选中
		QList<const C命令文件 *> v命令列表 = v分组命令[v组名];
		std::sort(v命令列表.begin(), v命令列表.end(), [](const C命令文件 *a, const C命令文件 *b)->bool {
			return a->fg名称().compare(b->fg名称(), Qt::CaseInsensitive) < 0;
		});
		for (const C命令文件 *vp命令 : v命令列表) {
			QStandardItem *const v命令项 = new QStandardItem(vp命令->fg名称());
			v命令项->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			v命令项->setData(QVariant::fromValue(static_cast<quint64>(reinterpret_cast<quintptr>(vp命令))), Qt::UserRole);
			v组项->appendRow(v命令项);
			if (vp命令 == v原当前) {
				v新当前索引 = v命令项->index();
			}
		}
		m命令列表模型->appendRow(v组项);
		ui->w_cmdlist->setExpanded(v组项->index(), !ma折叠分组.contains(v组名));	//默认展开
	}
	//恢复当前选择
	if (v新当前索引.isValid()) {
		ui->w_cmdlist->setCurrentIndex(v新当前索引);
	}
}
C实参组 &MainWindow::f生成实参组() {
	if (!m当前命令) {	//无选择命令时返回空实参组
		static C实参组 v空实参组;
		return v空实参组;
	}
	return ma记忆实参[m当前命令] = m参数列表->f生成实参组();
}
void MainWindow::f刷新命令框() {	//生成命令
	if (!m当前命令) {	//无选择命令时不生成
		return;
	}
	//生成命令
	const QString v命令 = m当前命令->f生成(f生成实参组());
	ui->w_cmd->setText(v命令);
}
QString MainWindow::fg命令() const {
	return ui->w_cmd->text();
}
void MainWindow::fs工作目录(const QString &a工作目录) {
	ui->w_workdir->setText(a工作目录);
}
QString MainWindow::fg工作目录() const {
	return ui->w_workdir->text();
}
void MainWindow::f更新设置() {
	if (m当前命令) {	//有选择命令才能更新
		m设置文件->f更新绑定参数(*m当前命令, ma记忆实参[m当前命令]);
		m设置文件->f更新工作目录(*m当前命令, fg工作目录());
	}
}
void MainWindow::f保存设置() {
	m设置文件->f保存(C设置文件::fg保存路径());
}
//事件
void MainWindow::showEvent(QShowEvent *event) {
	QMainWindow::showEvent(event);
	m输出管理->f打印程序信息();
	f载入文件();
	f刷新命令列表();
}
void MainWindow::closeEvent(QCloseEvent *event) {
	f更新设置();
	f保存设置();
	event->accept();
}
void MainWindow::resizeEvent(QResizeEvent *event) {
	QMainWindow::resizeEvent(event);
	const QSize v新尺寸 = event->size();
	const int v参数列表宽 = (v新尺寸.width() - 24) * 3 / 4;
	m参数列表->f重置尺寸(v参数列表宽);
}
//槽
void MainWindow::on_w_copy_clicked() {
	QClipboard *v剪贴板 = QApplication::clipboard();
	v剪贴板->setText(fg命令());
}
void MainWindow::on_w_exec_clicked() {
	if (m参数列表->fi可执行()) {
		m命令管理->f添加命令({fg命令(), fg工作目录()});
	}
}
void MainWindow::on_w_selectworkdir_clicked() {
	QString v目录 = QFileDialog::getExistingDirectory(
		nullptr,  // 父窗口部件，可以为nullptr
		"选择一个目录", // 对话框标题
		"",       // 默认打开的目录路径
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);
	if (v目录.isEmpty()) {
		return;
	}
	ui->w_workdir->setText(v目录);
}
void MainWindow::on_w_cmdlist_currentChanged(const QModelIndex &current, const QModelIndex &previous) {
	Q_UNUSED(previous);
	if (m当前命令) {
		f更新设置();
	}
	if (current.isValid()) {
		const quint64 v指针 = current.data(Qt::UserRole).value<quint64>();
		const C命令文件 *const vp命令 = v指针 ? reinterpret_cast<const C命令文件 *>(v指针) : nullptr;
		if (!vp命令) {	//分组行,不是命令
			return;
		}
		m当前命令 = vp命令;
		if (auto vp记忆实参 = ma记忆实参.find(vp命令); vp记忆实参 != ma记忆实参.end()) {
			m参数列表->f刷新列表(*vp命令, &vp记忆实参->second);
		} else {
			m参数列表->f刷新列表(*vp命令);
		}
		fs工作目录(vp命令->fg设置().m工作目录);
		f刷新命令框();
	} else {

	}
}
//==============================================================================
// 命令分组
//==============================================================================
std::vector<QString> MainWindow::f命令分组_g所有分组名() const {
	std::set<QString> v组;	//按名称排序且去重
	auto f获取分组名 = [this](const C命令文件 *a命令)->QString {
		QString v组名 = m设置文件->fg命令分组(a命令->fg文件名());
		if (!v组名.isEmpty()) {
			return v组名;
		}
		v组名 = a命令->fg分组();
		if (!v组名.isEmpty()) {
			return v组名;
		}
		return c未分组名;
	};
	for (const C命令文件 *vp命令 : ma启用命令) {
		if (const QString v组名 = f获取分组名(vp命令); v组名 != c未分组名) {
			v组.insert(v组名);
		}
	}
	std::vector<QString> v结果;
	v结果.reserve(v组.size());
	for (const QString &v名称 : v组) {
		v结果.push_back(v名称);
	}
	return v结果;
}
void MainWindow::f命令列表_右键菜单(const QPoint &a位置) {
	QModelIndex v索引 = ui->w_cmdlist->indexAt(a位置);
	//解析右键的命令(空白处或分组行无命令)
	const quint64 v指针 = v索引.isValid() ? v索引.data(Qt::UserRole).value<quint64>() : 0;
	const C命令文件 *const vp命令 = v指针 ? reinterpret_cast<const C命令文件 *>(v指针) : nullptr;
	QMenu w菜单(this);
	//刷新按钮: 无论右击命令还是空白处都显示
	QAction *const v刷新 = w菜单.addAction("刷新");
	connect(v刷新, &QAction::triggered, this, &MainWindow::f重新载入命令);
	w菜单.addSeparator();
	//右击命令才会显示的按钮
	if (vp命令) {
		//加入分组子菜单,前面是分组名,中间分隔线,后面是新建分组和移出分组
		QMenu *const w加入分组 = w菜单.addMenu("加入分组");
		for (const QString &v组名 : f命令分组_g所有分组名()) {
			QAction *const v动作 = w加入分组->addAction(v组名);
			connect(v动作, &QAction::triggered, this, [this, vp命令, v组名]() { f命令分组_把命令加入分组(vp命令, v组名); });
		}
		w加入分组->addSeparator();
		QAction *const v新建 = w加入分组->addAction("新建分组...");
		connect(v新建, &QAction::triggered, this, [this, vp命令]() { f命令分组_新建分组并加入(vp命令); });
		QAction *const v移出 = w加入分组->addAction("移出分组");
		connect(v移出, &QAction::triggered, this, [this, vp命令]() { f命令分组_把命令移出分组(vp命令); });
	}
	w菜单.exec(ui->w_cmdlist->viewport()->mapToGlobal(a位置));
}
void MainWindow::f命令分组_把命令加入分组(const C命令文件 *a命令, const QString &a组名) {
	m设置文件->f加入分组(a命令->fg文件名(), a组名);
	f重建命令树();
	f保存设置();
}
void MainWindow::f命令分组_新建分组并加入(const C命令文件 *a命令) {
	bool v确定 = false;
	const QString v名称 = QInputDialog::getText(this, "新建分组", "请输入分组名称:", QLineEdit::Normal, QString(), &v确定);
	if (!v确定 || v名称.trimmed().isEmpty()) {
		return;
	}
	const QString v组名 = v名称.trimmed();
	m设置文件->f构造分组(v组名);
	m设置文件->f加入分组(a命令->fg文件名(), v组名);
	f重建命令树();
	f保存设置();
}
void MainWindow::f命令分组_把命令移出分组(const C命令文件 *a命令) {
	m设置文件->f移出分组(a命令->fg文件名());
	f重建命令树();
	f保存设置();
}
