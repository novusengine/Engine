#include "StringUtils.h"

#include <sstream>
#include <iomanip>

#ifdef _WINDOWS
#include <Windows.h>
#endif

namespace StringUtils
{
    void HexStrToBytes(const char* src, u8* target)
    {
        while (*src && src[1])
        {
            *(target++) = HexCharToInt(*src) * 16 + HexCharToInt(src[1]);
            src += 2;
        }
    }

    std::string BytesToHexStr(u8* data, size_t length)
    {
        std::stringstream ss;
        ss << std::hex;

        for (i32 i(0); i < length; ++i)
            ss << std::setw(2) << std::setfill('0') << (i32)data[i];

        return ss.str();
    }

    i32 HexCharToInt(char input)
    {
        if (input >= '0' && input <= '9')
            return input - '0';
        if (input >= 'A' && input <= 'F')
            return input - 'A' + 10;
        if (input >= 'a' && input <= 'f')
            return input - 'a' + 10;
        throw std::invalid_argument("Invalid input string");
    }

    

    std::string GetLineFromCin()
    {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    std::vector<std::string> SplitString(std::string string, char delim)
    {
        std::vector<std::string> results;
        std::stringstream ss(string);
        std::string token;

        while (std::getline(ss, token, delim))
        {
            results.push_back(token);
        }

        return results;
    }

    std::string EscapeString(std::string const& string)
    {
        std::size_t n = string.length();
        std::string escaped;
        escaped.reserve(n * 2); // pessimistic preallocation

        for (std::size_t i = 0; i < n; ++i)
        {
            if (string[i] == '\\' || string[i] == '\'')
                escaped += '\\';
            escaped += string[i];
        }
        return escaped;
    }

    std::string FormatThousandSeparator(i32 n)
    {
        std::string ans = "";

        // Convert the given integer
        // to equivalent string
        std::string num = std::to_string(n);

        // Initialise count
        i32 count = 0;

        // Traverse the string in reverse
        for (i32 i = static_cast<i32>(num.size()) - 1; i >= 0; i--)
        {
            count++;
            ans.push_back(num[i]);

            // If three characters
            // are traversed
            if (count == 3)
            {
                ans.push_back(',');
                count = 0;
            }
        }

        // Reverse the string to get
        // the desired output
        std::reverse(ans.begin(), ans.end());

        // If the given string is
        // less than 1000
        if (ans.size() % 4 == 0)
        {
            // Remove ','
            ans.erase(ans.begin());
        }

        return ans;
    }

    bool BeginsWith(std::string_view const& fullString, std::string_view const& beginning)
    {
        if (fullString.length() >= beginning.length())
        {
            return (0 == fullString.compare(0, beginning.length(), beginning));
        }
        else
        {
            return false;
        }
    }

    bool EndsWith(std::string const& fullString, std::string const& ending)
    {
        if (fullString.length() >= ending.length())
        {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else
        {
            return false;
        }
    }

    bool Contains(std::string const& fullString, std::string const& substring)
    {
        return fullString.find(substring) != std::string::npos;
    }

#ifdef _WINDOWS
    std::wstring StringToWString(const std::string& s)
    {
        // deal with trivial case of empty string
        if (s.empty())    return std::wstring();

        // determine required length of new string
        size_t reqLength = ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), 0, 0);

        // construct new string of required length
        std::wstring ret(reqLength, L'\0');

        // convert old string to new string
        ::MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), &ret[0], (int)ret.length());

        // return new string
        return ret;
    }

    std::string WStringToString(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
#endif
}