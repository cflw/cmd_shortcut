#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <memory>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class QStringListModel;
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
	std::unique_ptr<QStringListModel> m命令列表模型;
	std::unique_ptr<C参数列表> m参数列表;
	std::unique_ptr<C输出管理> m输出管理;
	std::unique_ptr<C命令管理> m命令管理;
	std::vector<const C命令文件 *> ma启用命令;
	const C命令文件 *m当前命令 = nullptr;
	std::unique_ptr<C实参组> m实参组;	//缓存,上次生成结果
};
#endif // MAINWINDOW_H
