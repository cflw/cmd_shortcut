#include "cmdmanage.h"
#include <vector>
#include <utility>
#include "outmanage.h"
//==============================================================================
// 连续执行符号拆分
//==============================================================================
static std::vector<std::pair<QString, E执行条件>> f拆分连续命令(const QString &a命令) {	//返回(命令, 该命令前面的连续执行符号对应的执行条件)
	std::vector<std::pair<QString, E执行条件>> v返回;
	QString v当前;
	QChar v引号;	//当前所在引号,空表示不在引号内
	const int c长度 = a命令.size();
	for (int i = 0; i != c长度; ++i) {
		const QChar v字符 = a命令.at(i);
		if (!v引号.isNull()) {	//引号内的内容原样保留,不拆分
			v当前.append(v字符);
			if (v字符 == v引号) {
				v引号 = QChar();
			}
			continue;
		}
		if (v字符 == '"' || v字符 == '\'') {
			v引号 = v字符;
			v当前.append(v字符);
			continue;
		}
		if (v字符 == '&' && i > 0 && a命令.at(i - 1) == '>') {	//重定向(如2>&1)不是连续执行符号
			v当前.append(v字符);
			continue;
		}
		if (v字符 != '&' && v字符 != '|') {
			v当前.append(v字符);
			continue;
		}
		const bool v双字符 = (i + 1 != c长度) && (a命令.at(i + 1) == v字符);
		if (v字符 == '|' && !v双字符) {	//单个|是管道,不拆分,交给shell处理
			v当前.append(v字符);
			continue;
		}
		E执行条件 v条件 = E执行条件::e无条件;
		if (v双字符) {
			v条件 = (v字符 == '&') ? E执行条件::e成功后 : E执行条件::e失败后;
		}
		v返回.emplace_back(v当前.trimmed(), v条件);
		v当前.clear();
		if (v双字符) {
			++i;	//跳过第二个符号
		}
	}
	v返回.emplace_back(v当前.trimmed(), E执行条件::e无条件);	//最后一段
	return v返回;
}
C命令管理::C命令管理(QWidget *a父, C输出管理 &a输出管理):
	QObject(a父),
	m输出(&a输出管理),
	m执行进程(new QProcess(this)) {
	//连接信号槽
	connect(m执行进程.get(), &QProcess::readyReadStandardOutput, this, &C命令管理::onReadyReadOutput);
	connect(m执行进程.get(), &QProcess::readyReadStandardError, this, &C命令管理::onReadyReadError);
	connect(m执行进程.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &C命令管理::onFinished);
	//启动失败时不会发出 finished 信号,需在此恢复状态并继续执行队列
	connect(m执行进程.get(), &QProcess::errorOccurred, this, [this](QProcess::ProcessError a错误) {
		if (a错误 == QProcess::FailedToStart) {
			m输出->f换行();
			m输出->f输出行("命令启动失败: " + m执行进程->errorString());
			m手动停止 = false;
			m上条成功 = false;	//启动失败视为执行失败
			f调度命令();
		}
	});
}
void C命令管理::f添加命令(const S待执行命令 &a命令) {
	//大多数命令不含连续执行符号,先快速判断,避免无谓拆分
	if (!a命令.m命令.contains('&') && !a命令.m命令.contains('|')) {
		m命令队列.push_back(a命令);
		f调度命令();
		return;
	}
	//命令中含连续执行符号(& && ||)时拆分成多条命令依次入队,执行完一条后按结果决定是否执行下一条
	const std::vector<std::pair<QString, E执行条件>> va拆分 = f拆分连续命令(a命令.m命令);
	if (va拆分.size() == 1) {	//符号都在引号内,或是管道/重定向,没有实际拆分
		m命令队列.push_back(a命令);
		f调度命令();
		return;
	}
	bool v第一条 = true;
	for (const auto &[v命令文本, v分隔条件] : va拆分) {
		if (v命令文本.isEmpty()) {	//连续符号之间没有命令,忽略
			continue;
		}
		S待执行命令 v新命令 = a命令;
		v新命令.m命令 = v命令文本;
		v新命令.m执行条件 = v第一条 ? a命令.m执行条件 : v分隔条件;	//第一条沿用原条件,其余由分隔符决定
		m命令队列.push_back(v新命令);
		v第一条 = false;
	}
	f调度命令();
}
void C命令管理::f调度命令() {
	if (m执行进程->state() != QProcess::NotRunning) {
		return;
	}
	while (!m命令队列.empty()) {
		const S待执行命令 v命令 = m命令队列.front();
		m命令队列.pop_front();
		if (!f条件满足(v命令.m执行条件)) {	//上一条命令的结果不满足执行条件,跳过本条命令
			m输出->f输出行(QString("跳过命令(上一条命令结果不满足执行条件): %1").arg(v命令.m命令));
			continue;
		}
		f开始命令(v命令);
		return;
	}
}
bool C命令管理::f条件满足(E执行条件 a条件) const {
	switch (a条件) {
	case E执行条件::e成功后:
		return m上条成功;
	case E执行条件::e失败后:
		return !m上条成功;
	default:
		return true;
	}
}
void C命令管理::f停止命令() {	//停止当前正在执行的命令,连同其子进程一起结束
	if (m执行进程->state() == QProcess::NotRunning) {
		return;
	}
	m手动停止 = true;
	const qint64 v进程号 = m执行进程->processId();
#ifdef Q_OS_WIN
	//taskkill 的 /T 结束整棵进程树,避免 cmd.exe 派生的子进程残留
	if (v进程号 > 0) {
		QProcess::execute("taskkill", QStringList() << "/PID" << QString::number(v进程号) << "/T" << "/F");
	}
#else
	//先结束子进程(如 sh -c 管道中的各进程),再结束自身
	if (v进程号 > 0) {
		QProcess::execute("pkill", QStringList() << "-KILL" << "-P" << QString::number(v进程号));
	}
#endif
	if (m执行进程->state() != QProcess::NotRunning) {	//进程树未能结束时的兜底
		m执行进程->kill();
	}
}
bool C命令管理::fi正在执行() const {	//进程正在运行或队列中仍有待执行命令
	return m执行进程->state() != QProcess::NotRunning || !m命令队列.empty();
}
void C命令管理::f开始命令(const S待执行命令 &a命令) {
	m输出->f输出行("================================================================================");
	m输出->f输出行(a命令.m命令);
	m执行进程->setWorkingDirectory(a命令.m工作目录);
	if (a命令.m命令.contains('|') || a命令.m命令.contains('&')) {	//含管道符号或重定向(如2>&1)时,交给系统 shell 执行,使管道数据在进程间正确传递
#ifdef Q_OS_WIN
		m执行进程->start("cmd.exe", QStringList() << "/c" << a命令.m命令);
#else
		m执行进程->start("sh", QStringList() << "-c" << a命令.m命令);
#endif
	} else {	//无管道,按原方式将程序和参数分离执行
		const QStringList va字符串 = a命令.m命令.split(' ');
		m执行进程->start(va字符串.front(), va字符串.sliced(1));	//start()需要将程序和参数分离才能正常运行
	}
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
void C命令管理::onFinished(int a退出代码, QProcess::ExitStatus a退出状态) {
	m输出->f换行();
	if (m手动停止) {	//手动停止时只提示已停止,不显示退出代码
		m手动停止 = false;
		m输出->f输出行("命令已手动停止");
	} else {
		static const QString c模板 = "命令执行完成, 退出代码: %1, Qt退出状态: %2";
		m输出->f输出行(c模板.arg(QString::number(a退出代码), QString::number(a退出状态)));
	}
	m上条成功 = (a退出状态 == QProcess::NormalExit && a退出代码 == 0);	//记录结果,决定下一条命令是否执行
	f调度命令();
}