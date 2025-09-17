#include "mainwindow.h"
#include <ranges>
#include <QMessageBox>
#include <QCloseEvent>
#include <QStringListModel>
#include <QStyledItemDelegate>
#include <QClipboard>
#include <QCheckBox>
#include <QTimer>
#include <QFileDialog>
#include "ui_mainwindow.h"
#include "file_setting.h"
#include "file_command.h"
#include "arglistmanage.h"
#include "outmanage.h"
#include "cmdmanage.h"
//==============================================================================
// 主窗口
//==============================================================================
MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent), ui(new Ui::MainWindow),
	m命令列表模型(new QStringListModel(this)),
	m实参组(new C实参组())
{
    ui->setupUi(this);
	ui->w_cmdlist->setModel(m命令列表模型.get());
	m参数列表 = std::make_unique<C参数列表>(*this);
	m输出管理 = std::make_unique<C输出管理>(*ui->w_out);
	m命令管理 = std::make_unique<C命令管理>(this, *m输出管理);
	//connect(ui->w_copy, &QToolButton::clicked, this, &MainWindow::on_w_copy_clicked);
	//connect(ui->w_exec, &QToolButton::clicked, this, &MainWindow::on_w_exec_clicked);
	connect(ui->w_cmdlist->selectionModel(), &QItemSelectionModel::currentChanged,
			this, &MainWindow::on_w_cmdlist_currentChanged);
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
		S命令设置 &v命令设置 = m设置文件->f构造命令();
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
void MainWindow::f刷新命令列表() {	//在载入文件后调用,只调用一次
	if (m设置文件->fi空()) {
		m输出管理->f输出行("警告: 无命令");
	}
	//打开命令文件,并关联设置与命令
	std::vector<const C命令文件 *> va命令文件;
	for (S命令设置 &v命令设置 : m设置文件->fe启用命令()) {
		if (C命令文件 *const vp命令文件 = m命令文件缓存->f打开(v命令设置.m文件名)) {
			va命令文件.push_back(vp命令文件);
			vp命令文件->fs设置(v命令设置);
			for (S命令参数 &v命令参数 : vp命令文件->fe命令参数()) {
				if (v命令参数.m绑定设置) {
					v命令设置.ma绑定参数.try_emplace(v命令参数.m名称, QString());
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
	QStringList v命令列表;
	for (const C命令文件 *vp命令文件 : va命令文件) {
		v命令列表 << vp命令文件->fg名称();
	}
	m命令列表模型->setStringList(v命令列表);
	//结束
	ma启用命令 = std::move(va命令文件);
}
C实参组 &MainWindow::f生成实参组() {
	*m实参组 = m参数列表->f生成实参组();
	return *m实参组;
}
void MainWindow::f刷新命令框() {	//生成命令
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
		m设置文件->f更新绑定参数(*m当前命令, *m实参组);
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
		const C命令文件 *const vp命令 = ma启用命令.at(current.row());
		m参数列表->f刷新列表(*vp命令);
		fs工作目录(vp命令->fg设置().m工作目录);
		f刷新命令框();
	} else {

	}
}
