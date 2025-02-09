module;

#include <cstdint>
#include <exception>
#include <optional>
#include <string>

#include <boost/flyweight.hpp>
#include <magic_enum/magic_enum_format.hpp>

export module sdp.grammar.tokenizer;

export namespace sdp
{

enum class TokenType : std::uint8_t
{
	Symbol,
	Define,
	Alternate,
	Semicolon,
};

struct Token
{
	TokenType type;
	std::optional<boost::flyweight<std::string>> symbol;
};

class TokenizationError : public std::exception
{
};

class Tokenizer
{
  public:
	using difference_type = std::ptrdiff_t;
	using value_type = Token;

	Tokenizer(char const *begin, char const *end) : _iter{begin}, _end{end} { ++*this; }

	Token const &
	operator*() const
	{
		return _current;
	}

	bool
	operator==(std::default_sentinel_t /*unused*/) const
	{
		return _ended;
	}

	void
	operator++(int)
	{
		++*this;
	}

	Tokenizer &
	operator++()
	{
		while (_iter != _end)
		{
			if (std::isspace(*_iter))
			{
				++_iter;
				continue;
			}

			if (std::isalnum(*_iter))
			{
				char const *start = _iter;
				while (_iter != _end and std::isalnum(*_iter))
				{
					++_iter;
				}
				_current = {.type = TokenType::Symbol, .symbol = boost::flyweight<std::string>{start, _iter}};
				return *this;
			}
			if (*_iter == '|')
			{
				++_iter;
				_current = {.type = TokenType::Alternate};
				return *this;
			}
			if (*_iter == '=')
			{
				++_iter;
				_current = {.type = TokenType::Define};
				return *this;
			}
			if (*_iter == ';')
			{
				++_iter;
				_current = {.type = TokenType::Semicolon};
				return *this;
			}
			throw std::runtime_error{"Unknown character"};
		}
		_ended = true;
		return *this;
	}

  private:
	Token _current;
	char const *_iter;
	char const *_end;
	bool _ended = false;
};

static_assert(std::input_iterator<Tokenizer>);

} // namespace sdp

export template <>
struct std::formatter<sdp::Token> : std::formatter<std::string_view>
{
	template <class FmtContext>
	FmtContext::iterator
	format(sdp::Token const &token, FmtContext &ctx) const
	{
		return token.symbol
			? std::format_to(ctx.out(), "{}({})", token.type, token.symbol.value().get())
			: std::format_to(ctx.out(), "{}", token.type);
	}
};