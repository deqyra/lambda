#ifndef LAMBDA_PARSE_ERROR_HPP
#define LAMBDA_PARSE_ERROR_HPP

#include <format>
#include <exception>
#include <string>

namespace ld {

struct parse_error : std::exception {
    parse_error(std::string_view text, std::string_view expr) :
        _text(std::format("{} -- {}", text, expr))
    {
    }

    parse_error(std::string_view text, char c) :
        _text(std::format("{} -- {}", text, c))
    {
    }

    parse_error(std::string_view text) :
        _text(text)
    {
    }

    const char* what() const noexcept override {
        return _text.c_str();
    }

private:
    std::string _text;
};

#define OR_THROW_NESTED_PARSE_ERROR(...) \
    catch (const std::exception& e) {           \
        std::throw_with_nested(                 \
            parse_error(__VA_ARGS__)            \
        );                                      \
    }                                           \

inline std::string nested_exception_to_string(const std::exception& e, int level =  0)
{
    auto result = std::format("{}{}\n", std::string(level, ' '), e.what());

    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& nested) {
        result += nested_exception_to_string(nested, level + 1);
    } catch (...) {}

    return result;
}

} // namespace ld

#endif//LAMBDA_PARSE_ERROR_HPP