#include "argtemplate.h"
#include <QDomElement>
#include "common.h"
#include "file_command.h"
//==============================================================================
// 参数组
//==============================================================================
void C实参组::f清空() {
	ma参数值.clear();
}
bool C实参组::fi空() const {
	return ma参数值.empty();
}
void C实参组::f添加(const QString &a键, const QString &a值) {
	ma参数值.emplace_back(a键, a值);
}
QString C实参组::f查找(const QString &a键) const {
	for (const auto &[v键, v值] : ma参数值) {
		if (v键 == a键) {
			return v值;
		}
	}
	return "";
}
std::generator<QString> C实参组::fe查找(const QString &a键) const {
	std::vector<const QString *> v返回;
	for (const auto &[v键, v值] : ma参数值) {
		if (v键 == a键) {
			co_yield v值;
		}
	}
}
bool C实参组::fi存在(const QString &a键) const {
	for (const auto &[v键, v值] : ma参数值) {
		if (v键 == a键) {
			return !v值.isEmpty();
		}
	}
	return false;
}
//==============================================================================
// 参数模板
//==============================================================================
QString f参数模板_原始(const QString &a值) {
	return a值;
}
tf参数模板 F参数模板_固定(const QString &a模板) {
	return [=](const QString &a值) {
		return a模板;
	};
}
tf参数模板 F参数模板_模板(const QString &a前面, const QString &a后面) {
	return [=](const QString &a值) {
		return a前面 + a值 + a后面;
	};
}
tf参数模板 F自动参数模板(const QDomElement &a参数节点) {
	const QString v模板 = a参数节点.firstChildElement("模板").text();
	if (v模板.isEmpty()) {	//无模板
		const QString v名称 = a参数节点.attribute("名称");
		const E参数类型 v参数类型 = f解析参数类型(a参数节点.attribute("类型"));
		if (v参数类型 == E参数类型::e开关) {	//开关
			return F参数模板_固定(v名称);
		} else if (const QChar v首字符 = v名称.at(0); v首字符 == '-' || v首字符 == '/') {	//-或/开头的参数
			return F参数模板_模板(v名称 + " ", "");	//模板="名称 {}"
		} else {	//目标
			return &f参数模板_原始;
		}
	} else if (const qsizetype v括号开始 = v模板.indexOf('{'); v括号开始 != -1) {	//识别带{}的模板
		if (const qsizetype v括号结束 = v模板.indexOf('}', v括号开始); v括号结束 != -1) {
			return F参数模板_模板(v模板.left(v括号开始), v模板.mid(v括号结束+1));
		} else {	//括号不完整
			return F参数模板_固定(v模板);
		}
	} else {	//无括号,或括号不完整
		return F参数模板_固定(v模板);
	}
}//==============================================================================
// 生成
//==============================================================================
std::vector<S生成> f构造生成序列(const QString &a模板, const std::vector<S命令参数> &aa参数) {
	std::vector<S生成> v返回;
	qsizetype v文本开始 = 0;
	qsizetype v括号开始 = a模板.indexOf('{');
	while (v括号开始 != -1) {
		if (v文本开始 != v括号开始) {	//文本到括号之间存在固定文本
			v返回.emplace_back(nullptr, F参数模板_固定(a模板.mid(v文本开始, v括号开始 - v文本开始)));
		}
		const qsizetype v括号结束 = a模板.indexOf('}', v括号开始);
		if (v括号结束 == -1) {	//括号未闭合,把括号当成固定文本处理
			v返回.emplace_back(nullptr, F参数模板_固定(a模板.mid(v括号开始)));
			break;
		}
		const QString v参数名 = a模板.mid(v括号开始 + 1, v括号结束 - v括号开始 - 1);
		if (auto v找 = std::find_if(aa参数.begin(), aa参数.end(), [&v参数名](const S命令参数 &a参数)->bool {return a参数.m名称 == v参数名; }); v找 != aa参数.end()) {
			v返回.emplace_back(&*v找, v找->mf模板);
		} else {	//没找到参数,生成原文
			v返回.emplace_back(nullptr, F参数模板_固定(a模板.mid(v括号开始, v括号结束 - v括号开始 + 1)));
		}
		//结束
		v文本开始 = v括号结束 + 1;
		v括号开始 = a模板.indexOf('{', v文本开始);
	}
	return v返回;
}
//==============================================================================
// 命令模板
//==============================================================================
QString f去除连续空格(const QString &a字符串) {
	const qsizetype v文本长度 = a字符串.size();
	QString v返回;
	v返回.reserve(v文本长度);
	qsizetype v文本开始 = 0;
	qsizetype v空格开始 = a字符串.indexOf(' ');
	if (v文本开始 != v空格开始) {
		v返回 += a字符串.mid(v文本开始, v空格开始 - v文本开始);
	}
	while (v空格开始 != -1) {
		qsizetype v空格结束 = v空格开始 + 1;
		if (v空格结束 >= v文本长度) {	//到结尾
			return v返回;
		}
		while (a字符串.at(v空格结束) == ' ') {
			++v空格结束;
			if (v空格结束 >= v文本长度) {	//到结尾
				return v返回;
			}
		}
		//非空格位置,下一循环
		v文本开始 = v空格结束 - 1;
		v空格开始 = a字符串.indexOf(' ', v空格结束);
		v返回 += a字符串.mid(v文本开始, v空格开始 - v文本开始);
	}
	return v返回;
}
C命令模板::C命令模板(const QString &a模板, const std::vector<S命令参数> &aa参数):
	m模板(a模板),
	ma生成(f构造生成序列(a模板, aa参数)) {
}
C命令模板::operator const QString &() const {
	return m模板;
}
QString C命令模板::f生成(const C实参组 &a参数组) const {
	QString v返回;
	v返回.reserve(m模板.size());
	for (const S生成 &v生成 : ma生成) {
		if (!v生成.m参数) {	//非参数,固定文本
			v返回 += v生成.mf生成("");
		} else if (v生成.m参数->m多参数) {	//多参数
			for (QString v值 : a参数组.fe查找(v生成.m参数->m名称)) {
				v返回 += v生成.mf生成(v值) + " ";
			}
			v返回.erase(v返回.end());	//删除末尾的分隔符
		} else {	//单参数
			if (QString v值 = a参数组.f查找(v生成.m参数->m名称); !v值.isEmpty()) {
				v返回 += v生成.mf生成(v值);
			}
		}
	}
	return f去除连续空格(v返回);
}
