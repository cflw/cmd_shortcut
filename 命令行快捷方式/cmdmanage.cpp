#include "cmdmanage.h"
#include "outmanage.h"
C命令管理::C命令管理(QWidget *a父, C输出管理 &a输出管理):
	QObject(a父),
	m输出(&a输出管理),
	m执行进程(new QProcess(this)) {
	//连接信号槽
	connect(m执行进程.get(), &QProcess::readyReadStandardOutput, this, &C命令管理::onReadyReadOutput);
	connect(m执行进程.get(), &QProcess::readyReadStandardError, this, &C命令管理::onReadyReadError);
	connect(m执行进程.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &C命令管理::onFinished);

}
void C命令管理::f添加命令(const S待执行命令 &a命令) {
	m命令队列.push_back(a命令);
	f调度命令();
}
void C命令管理::f调度命令() {
	if (m执行进程->state() != QProcess::NotRunning) {
		return;
	}
	if (m命令队列.empty()) {
		return;
	}
	const S待执行命令 &v命令 = m命令队列.front();
	f开始命令(v命令);
	m命令队列.pop_front();
}
void C命令管理::f开始命令(const S待执行命令 &a命令) {
	m输出->f输出行("================================================================================");
	m输出->f输出行(a命令.m命令);
	const QStringList va字符串 = a命令.m命令.split(' ');
	m执行进程->setWorkingDirectory(a命令.m工作目录);
	m执行进程->start(va字符串.front(), va字符串.sliced(1));	//start()需要将程序和参数分离才能正常运行
}
//槽
void C命令管理::onReadyReadOutput() {
	const QString v输出 = QString::fromLocal8Bit(m执行进程->readAllStandardOutput());
	m输出->f输出(v输出);
}
void C命令管理::onReadyReadError() {
	const QString v错误 = QString::fromLocal8Bit(m执行进程->readAllStandardError());
	m输出->f输出("发生错误:");
	m输出->f输出行(v错误);
}
void C命令管理::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	static const QString c模板 = "命令执行完成, 退出代码: %1, Qt退出状态: %2";
	m输出->f换行();
	m输出->f输出行(c模板.arg(QString::number(exitCode), QString::number(exitStatus)));
	f调度命令();
}