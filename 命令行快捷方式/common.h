#ifndef COMMON_H
#define COMMON_H
#include <QString>
enum E参数类型 {	//禁止组合
	e任意,
	e整数,
	e路径,
	e开关,
};
QString f生成参数类型(E参数类型);
E参数类型 f解析参数类型(const QString &);
enum E平台 {	//可以组合
	e空 = 0,
	windows = 1,
	linux = 2,
	macos = 4,
	e全部 = windows | linux | macos,
};
#if defined(Q_OS_WIN)
constexpr E平台 c当前平台 = E平台::windows;
#elif defined(Q_OS_LINUX)
constexpr E平台 c当前平台 = E平台::linux;
#elif defined(Q_OS_MACOS)
constexpr E平台 c当前平台 = E平台::macos;
#else
constexpr E平台 c当前平台 = E平台::e空;
#endif
QString f生成平台(E平台);
E平台 f解析平台(const QString &);

#endif // COMMON_H
