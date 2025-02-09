module;

#include <exception>
#include <format>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include <boost/flyweight.hpp>

export module sdp.grammar.parser;

import sdp.grammar;
import sdp.grammar.tokenizer;

namespace sdp
{

export class ParseException : public std::exception
{
  public:
	[[nodiscard]] const char *
	what() const override
	{
		return "Unknown parse exception";
	}
};

export class UnexpectedTokenError : public ParseException
{

  public:
	UnexpectedTokenError(Token const &token) { message = std::format("Unexpected token {}", token); }

	[[nodiscard]] const char *
	what() const override
	{
		return message.c_str();
	}

	std::string message;
};

template <std::input_iterator I, std::sentinel_for<I> S>
class Parser
{
  public:
	Parser(I iter, S sentinel) : _iter{std::move(iter)}, _sentinel{std::move(sentinel)} {}

	Grammar
	parse()
	{
		Grammar grammar;

		auto [var, productions] = parse_rule();
		grammar.start = var;
		grammar.rules.emplace(var, productions);

		while (_iter != _sentinel)
		{
			grammar.rules.emplace(parse_rule());
		}

		return grammar;
	}

  private:
	std::pair<boost::flyweight<std::string>, std::vector<std::vector<boost::flyweight<std::string>>>>
	parse_rule()
	{
		auto value = *eat(TokenType::Symbol);

		eat(TokenType::Define);

		std::vector<std::vector<boost::flyweight<std::string>>> productions;
		productions.push_back(parse_sequence());
		while (check(TokenType::Alternate))
		{
			++_iter;
			productions.push_back(parse_sequence());
		}

		eat(TokenType::Semicolon);
		return {value, productions};
	}

	std::vector<boost::flyweight<std::string>>
	parse_sequence()
	{
		std::vector<boost::flyweight<std::string>> sequence;
		while (check(TokenType::Symbol))
		{
			auto &symbol = *(*_iter).symbol;
			sequence.push_back(symbol);
			++_iter;
		}
		return sequence;
	}

	std::optional<boost::flyweight<std::string>>
	eat(TokenType type)
	{
		if (not check(type))
		{
			throw UnexpectedTokenError{*_iter};
		}
		auto result = (*_iter).symbol;
		++_iter;
		return result;
	}

	bool
	check(TokenType type)
	{
		return _iter != _sentinel and (*_iter).type == type;
	}

	I _iter;
	S _sentinel;
};

export template <std::input_iterator I, std::sentinel_for<I> S>
Grammar
parse(I iter, S sentinel)
{
	return Parser<I, S>{std::move(iter), std::move(sentinel)}.parse();
}

} // namespace sdp