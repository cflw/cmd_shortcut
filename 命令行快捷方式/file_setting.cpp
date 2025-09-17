#include "file_setting.h"
#include <QFile>
#include <QSysInfo>
#include "xml_utility.h"
#include "argtemplate.h"
#include "file_command.h"
//==============================================================================
// 参数
//==============================================================================
t绑定参数 f绑定参数_构造(const QDomElement &a参数节点) {
	return {
		a参数节点.attribute("名称"), 
		a参数节点.firstChildElement("值").text()
	};
}
QDomElement f绑定参数_转元素(QDomDocument &a文档, const t绑定参数 &a绑定参数) {
	QDomElement v参数节点 = a文档.createElement("参数");
	v参数节点.setAttribute("名称", a绑定参数.first);
	v参数节点.appendChild(createSimpleElement(a文档, "值", a绑定参数.second));
	return v参数节点;
}
S命令设置::S命令设置(const QDomElement &a命令节点):
	m文件名(a命令节点.attribute("文件名")),
	m工作目录(a命令节点.firstChildElement("工作目录").text()),
	m启用(a命令节点.attribute("启用") == "true") {
	if (!m启用) {
		return;
	}
	for (const QDomNode &v参数节点 : a命令节点.elementsByTagName("参数")) {
		ma绑定参数.emplace(f绑定参数_构造(v参数节点.toElement()));
	}
}
QDomElement S命令设置::ft元素(QDomDocument &a文档) const {
	QDomElement v命令节点 = a文档.createElement("命令");
	v命令节点.setAttribute("文件名", m文件名);
	v命令节点.setAttribute("启用", m启用 ? "true" : "false");
	if (!m工作目录.isEmpty()) {
		v命令节点.appendChild(createSimpleElement(a文档, "工作目录", m工作目录));
	}
	for (const t绑定参数 &v绑定参数 : ma绑定参数) {
		QDomElement v参数节点 = f绑定参数_转元素(a文档, v绑定参数);
		v命令节点.appendChild(v参数节点);
	}
	return v命令节点;
}
QString S命令设置::fg文件名() const {
	return m文件名;
}
QString S命令设置::fg绑定参数值(const QString &a名称) const {
	if (auto v找 = ma绑定参数.find(a名称); v找 != ma绑定参数.end()) {
		return v找->second;
	} else {
		return QString();
	}
}
bool S命令设置::fs绑定参数值(const QString &a名称, const QString &a值) {
	if (auto v找 = ma绑定参数.find(a名称); v找 != ma绑定参数.end()) {
		if (v找->second == a值) {
			return false;
		} else {
			v找->second = a值;
			return true;
		}
	} else {
		return false;
	}
}
//==============================================================================
// 设置文件
//==============================================================================
const QString C设置文件::c设置目录名("setting");
const QDir C设置文件::c设置目录("./" + c设置目录名);
const QString C设置文件::c主机名 = QSysInfo::machineHostName();
C设置文件::C设置文件():
	m主机名(c主机名) {
}
QString C设置文件::fg保存文件名() {
	return c主机名 + ".xml";
}
QString C设置文件::fg保存路径() {
	return c设置目录.filePath(fg保存文件名());
}
bool C设置文件::f打开(const QString &a路径) {
	//打开文档
	QDomDocument v文档;
	if (!openXmlWithDom(a路径, v文档)) {
		//如果目录不存在,创建目录,为后面保存设置作准备
		if (!c设置目录.exists()) {
			QDir::current().mkdir(c设置目录名);
		}
		return false;	//打开失败
	}
	const QDomElement v根节点 = v文档.documentElement();
	//设置
	const QDomElement v设置节点 = v根节点.firstChildElement("设置");
	m主机名 = v设置节点.firstChildElement("主机名").text();
	m平台 = f解析平台(v设置节点.firstChildElement("平台").text());
	//命令组
	const QDomElement v命令组节点 = v根节点.firstChildElement("命令组");
	for (const QDomNode v命令节点 : v命令组节点.elementsByTagName("命令")) {
		ma命令设置.emplace_back(std::make_unique<S命令设置>(v命令节点.toElement()));
	}
	return true;
}
bool C设置文件::f保存(const QString &a路径) {
	if (!m编辑) {
		return true;	//没有修改,无需保存,直接成功
	}
	QDomDocument v文档 = ft文档();
	const bool v结果 = saveXmlToFile(v文档, a路径);
	m编辑 &= !v结果;
	return v结果;
}
QDomDocument C设置文件::ft文档() const {
	QDomDocument v文档;
	QDomElement v根节点 = v文档.createElement("根");
	v文档.appendChild(v根节点);
	//设置
	QDomElement v设置节点 = v文档.createElement("设置");
	v设置节点.appendChild(createSimpleElement(v文档, "主机名", m主机名));
	v设置节点.appendChild(createSimpleElement(v文档, "平台", f生成平台(m平台)));
	v根节点.appendChild(v设置节点);
	//命令组
	QDomElement v命令组节点 = v文档.createElement("命令组");
	for (auto &vp命令设置 : ma命令设置) {
		QDomElement v命令节点 = vp命令设置->ft元素(v文档);
		v命令组节点.appendChild(v命令节点);
	}
	v根节点.appendChild(v命令组节点);
	//返回
	return v文档;
}
std::generator<S命令设置 &> C设置文件::fe命令设置() {
	for (auto &vp命令设置 : ma命令设置) {
		co_yield *vp命令设置;
	}
}
std::generator<S命令设置 &> C设置文件::fe启用命令() {
	for (auto &vp命令设置 : ma命令设置) {
		if (vp命令设置->m启用) {
			co_yield *vp命令设置;
		}
	}
}
std::generator<QString> C设置文件::fe所有文件() const {
	for (auto &vp命令设置 : ma命令设置) {
		co_yield vp命令设置->m文件名;
	}
}
std::generator<QString> C设置文件::fe启用文件() const {
	for (auto &vp命令设置 : ma命令设置) {
		if (vp命令设置->m启用) {
			co_yield vp命令设置->m文件名;
		}
	}
}
bool C设置文件::fi空() const {
	return ma命令设置.empty();
}
S命令设置 &C设置文件::f构造命令() {
	m编辑 = true;
	return *ma命令设置.emplace_back(new S命令设置());
}
const QString &C设置文件::fg主机名() const {
	return m主机名;
}
E平台 C设置文件::fg平台() const {
	return m平台;
}
S命令设置 *C设置文件::fg命令设置(const QString &a文件名) {
	auto v找 = std::find_if(ma命令设置.begin(), ma命令设置.end(), [&a文件名](const std::unique_ptr<S命令设置> &a)->bool {return a->m文件名 == a文件名; });
	if (v找 == ma命令设置.end()) {
		return nullptr;
	}
	return v找->get();
}
void C设置文件::f更新绑定参数(const C命令文件 &a命令文件, const C实参组 &a参数组) {
	if (S命令设置 *vp命令设置 = fg命令设置(a命令文件.fg文件名())) {
		for (const t绑定参数 &v绑定参数 : a参数组.ma参数值) {
			m编辑 |= vp命令设置->fs绑定参数值(v绑定参数.first, v绑定参数.second);
		}
	}
}
void C设置文件::f更新工作目录(const C命令文件 &a命令文件, const QString &a工作目录) {
	if (S命令设置 *vp命令设置 = fg命令设置(a命令文件.fg文件名())) {
		if (vp命令设置->m工作目录 != a工作目录) {
			vp命令设置->m工作目录 = a工作目录;
			m编辑 |= true;
		}
	}
}