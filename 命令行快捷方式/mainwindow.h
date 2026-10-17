#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <map>
#include <memory>
#include <QMainWindow>
#include <QSet>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class C设置文件;
class C命令文件;
class C命令文件缓存;
class C实参组;
class C输出管理;
class C命令管理;
class MainWindow : public QMainWindow
{
	Q_OBJECT;
	friend class C参数列表;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
	void f载入文件();
	void f刷新命令列表();
	C实参组 &f生成实参组();	//生成实参组,刷新缓存
	void f刷新命令框();
	QString fg命令() const;	//返回命令框的文本
	void fs工作目录(const QString &);
	QString fg工作目录() const;	//返回工作目录框的文本
	void f更新设置();
	void f保存设置();
	//命令列表和命令分组
	void f重建命令树();	//根据启用命令和分组设置重建左侧树
	std::vector<QString> f命令分组_g所有分组名() const;	//返回当前所有已存在的分组名(含命令文件默认分组, 不含"未分组")
	void f命令列表_右键菜单(const QPoint &a位置);	//对着命令点右键时调用,弹出命令菜单
	void f命令分组_把命令加入分组(const C命令文件 *a命令, const QString &a组名);
	void f命令分组_新建分组并加入(const C命令文件 *a命令);
	void f命令分组_把命令移出分组(const C命令文件 *a命令);
protected:
	void showEvent(QShowEvent *) override;
	void closeEvent(QCloseEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
private slots:	//qt槽函数不能用中文
    void on_w_copy_clicked();
	void on_w_exec_clicked();
	void on_w_selectworkdir_clicked();
	void on_w_cmdlist_currentChanged(const QModelIndex &at, const QModelIndex &previous);
private:
    std::unique_ptr<Ui::MainWindow> ui;
	std::unique_ptr<C设置文件> m设置文件;
	std::unique_ptr<C命令文件缓存> m命令文件缓存;
	std::unique_ptr<QStandardItemModel> m命令列表模型;
	std::unique_ptr<C参数列表> m参数列表;
	std::unique_ptr<C输出管理> m输出管理;
	std::unique_ptr<C命令管理> m命令管理;
	std::vector<const C命令文件 *> ma启用命令;
	const C命令文件 *m当前命令 = nullptr;
	std::map<const C命令文件 *, C实参组> ma记忆实参;	//保存每个命令上次填写的实参,切换命令时可以恢复
	QSet<QString> ma折叠分组;	//记录被折叠的分组名
};
#endif // MAINWINDOW_H
