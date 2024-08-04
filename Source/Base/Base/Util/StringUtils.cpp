#include "StringUtils.h"

#include <sstream>
#include <iomanip>
#include <codecvt>

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

    std::string GetFileNameFromPath(std::string const& path)
    {
        size_t lastSlashPos = path.find_last_of("/\\");
        if (lastSlashPos == std::string::npos)
            lastSlashPos = 0;  // If no '/' or '\\', start from the beginning of the string
        else
            lastSlashPos += 1;  // start from the character after the '/' or '\\'

        size_t lastDotPos = path.find_last_of(".");
        if (lastDotPos == std::string::npos || lastDotPos <= lastSlashPos)
            lastDotPos = path.length();  // If no '.', the end is the end of the string

        return path.substr(lastSlashPos, lastDotPos - lastSlashPos);
    }

    std::vector<std::string> SplitString(std::string const& string, char delim)
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

    bool SearchString(const std::string& ref, const std::string& key, bool insensitive)
    {
        std::string str = ref;
        std::string substr = key;

        if (insensitive)
        {
            std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
            std::transform(substr.begin(), substr.end(), substr.begin(), [](unsigned char c) { return std::tolower(c); });
        }

        return Contains(str, substr);
    }

    bool StringIsAlphaAndAtLeastLength(const std::string& input, u16 maxLength)
    {
        // Check if the string is at least maxLength characters long
        if (input.length() < maxLength)
            return false;

        // Check each character to ensure it is an English alphabet letter
        for (char c : input)
        {
            if (!std::isalpha(static_cast<unsigned char>(c)))
                return false;
        }

        return true;
    }

    bool StringIsNumeric(const std::string& input)
    {
        for (char c : input)
        {
            if (!std::isdigit(static_cast<unsigned char>(c)))
                return false;
        }

        return true;
    }

    std::wstring StringToWString(const std::string& s)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

        return converter.from_bytes(s);
    }

    std::string WStringToString(const std::wstring& wstr)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

        return converter.to_bytes(wstr);
    }
}
