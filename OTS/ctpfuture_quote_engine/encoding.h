#pragma once
#include <string>
#include <locale>
#include <vector>
#include <codecvt>
#pragma warning( disable : 4996)


//将GBK编码的字符串转换为UTF8
inline std::string gbk2utf8(const std::string& gb2312)
{
#ifdef _MSC_VER
    const static std::locale loc("zh-CN");
#else
    const static std::locale loc("zh_CN.GB18030");
#endif

    std::vector<wchar_t> wstr(gb2312.size());
    wchar_t* wstrEnd = nullptr;
    const char* gbEnd = nullptr;
    mbstate_t state = {};
    int res = use_facet<std::codecvt<wchar_t, char, mbstate_t> >
        (loc).in(state,
            gb2312.data(), gb2312.data() + gb2312.size(), gbEnd,
            wstr.data(), wstr.data() + wstr.size(), wstrEnd);

    if (std::codecvt_base::ok == res)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
        return cutf8.to_bytes(std::wstring(wstr.data(), wstrEnd));
    }

    return std::string();
}
