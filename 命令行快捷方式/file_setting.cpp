#include "file_setting.h"
#include <QFile>
#include <QSysInfo>
#include <algorithm>
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
C命令设置::C命令设置(const QDomElement &a命令节点):
	m文件名(a命令节点.attribute("文件名")),
	m工作目录(a命令节点.firstChildElement("工作目录").text()),
	m启用(a命令节点.attribute("启用") == "true") {
	if (!m启用) {
		return;
	}
	for (const QDomNode &v参数节点 : a命令节点.elementsByTagName("参数")) {
		QDomElement v元素 = v参数节点.toElement();
		const QString v名称 = v元素.attribute("名称");
		for (const QDomNode &v值节点: v元素.elementsByTagName("值")) {
			const QString v值 = v值节点.toElement().text();
			ma绑定参数.emplace(v名称, v值);
		}
	}
}
QDomElement C命令设置::ft元素(QDomDocument &a文档) const {
	QDomElement v命令节点 = a文档.createElement("命令");
	v命令节点.setAttribute("文件名", m文件名);
	v命令节点.setAttribute("启用", m启用 ? "true" : "false");
	if (!m工作目录.isEmpty()) {
		v命令节点.appendChild(createSimpleElement(a文档, "工作目录", m工作目录));
	}
	for (auto v键迭代 = ma绑定参数.begin(); v键迭代 != ma绑定参数.end(); v键迭代 = ma绑定参数.upper_bound(v键迭代->first)) {
		QDomElement v参数节点 = a文档.createElement("参数");
		v参数节点.setAttribute("名称", v键迭代->first);
		auto v值范围 = ma绑定参数.equal_range(v键迭代->first);
		for (auto v值迭代 = v值范围.first; v值迭代 != v值范围.second; ++v值迭代) {
			v参数节点.appendChild(createSimpleElement(a文档, "值", v值迭代->second));
		}
		v命令节点.appendChild(v参数节点);
	}
	return v命令节点;
}
QString C命令设置::fg文件名() const {
	return m文件名;
}
QString C命令设置::fg绑定参数值(const QString &a名称) const {
	if (auto v找 = ma绑定参数.find(a名称); v找 != ma绑定参数.end()) {
		return v找->second;
	} else {
		return QString();
	}
}
bool C命令设置::fs绑定参数值(const QString &a名称, const QString &a值) {
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
void C命令设置::f清空多参数值(const QString &a键) {
	ma绑定参数.erase(a键);
}
void C命令设置::f添加多参数值(const QString &a键, const QString &a值) {
	ma绑定参数.emplace(a键, a值);
}
std::generator<QString> C命令设置::fe多参数值(const QString &a键) const {
	auto [v开始, v结束] = ma绑定参数.equal_range(a键);
	for (auto v找 = v开始; v找 != v结束; ++v找) {
		co_yield v找->second;
	}
}
std::vector<QString> C命令设置::fg多参数值(const QString &a键) const {
	std::vector<QString> v结果;
	auto [v开始, v结束] = ma绑定参数.equal_range(a键);
	for (auto v找 = v开始; v找 != v结束; ++v找) {
		v结果.push_back(v找->second);
	}
	return v结果;
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
		ma命令设置.emplace_back(std::make_unique<C命令设置>(v命令节点.toElement()));
	}
	//分组
	const QDomElement v分组组节点 = v根节点.firstChildElement("命令分组");
	if (!v分组组节点.isNull()) {
		for (const QDomNode v分组节点 : v分组组节点.elementsByTagName("分组")) {
			const QDomElement v分组元素 = v分组节点.toElement();
			const QString v组名 = v分组元素.attribute("名称");
			std::vector<QString> &v列表 = ma分组[v组名];
			for (const QDomNode v命令节点 : v分组元素.elementsByTagName("命令")) {
				v列表.push_back(v命令节点.toElement().attribute("文件名"));
			}
		}
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
	//分组
	QDomElement v分组组节点 = v文档.createElement("命令分组");
	for (const auto &[v组名, v列表] : ma分组) {
		if (v列表.empty()) {
			continue;	//不保存空分组
		}
		QDomElement v分组元素 = v文档.createElement("分组");
		v分组元素.setAttribute("名称", v组名);
		for (const QString &v文件名 : v列表) {
			QDomElement v命令元素 = v文档.createElement("命令");
			v命令元素.setAttribute("文件名", v文件名);
			v分组元素.appendChild(v命令元素);
		}
		v分组组节点.appendChild(v分组元素);
	}
	v根节点.appendChild(v分组组节点);
	//返回
	return v文档;
}
std::generator<C命令设置 &> C设置文件::fe命令设置() {
	for (auto &vp命令设置 : ma命令设置) {
		co_yield *vp命令设置;
	}
}
std::generator<C命令设置 &> C设置文件::fe启用命令() {
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
C命令设置 &C设置文件::f构造命令() {
	m编辑 = true;
	return *ma命令设置.emplace_back(new C命令设置());
}
const QString &C设置文件::fg主机名() const {
	return m主机名;
}
E平台 C设置文件::fg平台() const {
	return m平台;
}
C命令设置 *C设置文件::fg命令设置(const QString &a文件名) {
	auto v找 = std::find_if(ma命令设置.begin(), ma命令设置.end(), [&a文件名](const std::unique_ptr<C命令设置> &a)->bool {return a->m文件名 == a文件名; });
	if (v找 == ma命令设置.end()) {
		return nullptr;
	}
	return v找->get();
}
void C设置文件::f更新绑定参数(const C命令文件 &a命令文件, const C实参组 &a参数组) {
	C命令设置 *const vp命令设置 = fg命令设置(a命令文件.fg文件名());
	if (!vp命令设置) {
		return;
	}
	std::vector<QString> v多参数缓冲;	//写入
	QString v多参数名;
	auto f写入多参数 = [&]() {
		//判断是否编辑
		std::vector<QString> v多参数值 = vp命令设置->fg多参数值(v多参数名);
		bool v需要写入 = false;
		if (v多参数缓冲.size() != v多参数值.size()) {
			v需要写入 = true;
		} else {	//数量相同,比较每个值
			for (size_t i = 0; i != v多参数缓冲.size(); ++i) {
				if (v多参数缓冲[i] != v多参数值[i]) {
					v需要写入 = true;
					break;
				}
			}
		}
		if (v需要写入) {
			vp命令设置->f清空多参数值(v多参数名);
			for (const QString &v值 : v多参数缓冲) {
				vp命令设置->f添加多参数值(v多参数名, v值);
			}
			v多参数缓冲.clear();
			m编辑 |= true;
		}
	};
	for (const S参数记忆 &v记忆 : a参数组.ma参数值) {
		if (v记忆.m参数->m多参数) {	//多参数
			if (v多参数名 == v记忆.m参数->m名称) {	//连续多参数,先存到缓冲
				v多参数缓冲.push_back(v记忆.m值);
			} else if (!v多参数名.isEmpty()) {	//新的多参数
				f写入多参数();
				//结束
				v多参数缓冲.push_back(v记忆.m值);
				v多参数名 = v记忆.m参数->m名称;
			} else {	//上次是开头或单参数
				v多参数缓冲.push_back(v记忆.m值);
				v多参数名 = v记忆.m参数->m名称;
			}
		} else {	//单参数
			if (!v多参数名.isEmpty()) {	//上次是多参数,写入
				f写入多参数();
				v多参数名.clear();
			}
			m编辑 |= vp命令设置->fs绑定参数值(v记忆.m参数->m名称, v记忆.m值);
		}
	}
	if (!v多参数名.isEmpty()) {	//多参数结尾,写入
		f写入多参数();
	}
}
void C设置文件::f更新工作目录(const C命令文件 &a命令文件, const QString &a工作目录) {
	C命令设置 *const vp命令设置 = fg命令设置(a命令文件.fg文件名());
	if (!vp命令设置) {
		return;
	}
	if (vp命令设置->m工作目录 != a工作目录) {
		vp命令设置->m工作目录 = a工作目录;
		m编辑 |= true;
	}
}
//==============================================================================
// 分组
//==============================================================================
std::generator<QString> C设置文件::fe分组名() const {
	for (const auto &[v组名, v列表] : ma分组) {
		co_yield v组名;
	}
}
QString C设置文件::fg命令分组(const QString &a命令文件名) const {
	for (const auto &[v组名, v列表] : ma分组) {
		if (std::find(v列表.begin(), v列表.end(), a命令文件名) != v列表.end()) {
			return v组名;
		}
	}
	return QString();
}
void C设置文件::f构造分组(const QString &a分组名) {
	if (ma分组.find(a分组名) != ma分组.end()) {
		return;
	}
	ma分组[a分组名];	//创建空分组
	m编辑 = true;
}
void C设置文件::f加入分组(const QString &a命令文件名, const QString &a分组名) {
	//先从所有分组移除该命令
	for (auto &[v组名, v列表] : ma分组) {
		const auto v找 = std::find(v列表.begin(), v列表.end(), a命令文件名);
		if (v找 != v列表.end()) {
			v列表.erase(v找);
		}
	}
	ma分组[a分组名].push_back(a命令文件名);
	m编辑 = true;
}
void C设置文件::f移出分组(const QString &a命令文件名) {
	for (auto &[v组名, v列表] : ma分组) {
		const auto v找 = std::find(v列表.begin(), v列表.end(), a命令文件名);
		if (v找 != v列表.end()) {
			v列表.erase(v找);
			m编辑 = true;
		}
	}
}