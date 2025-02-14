module;

#include <map>
#include <memory>
#include <print>
#include <ranges>
#include <set>
#include <span>
#include <vector>

#include <libassert/assert.hpp>

export module sdp.ll_table;

import sdp.grammar;
import sdp.grammar.analysis;

export namespace sdp
{

class LLTable
{
  public:
	[[nodiscard]] std::size_t
	rows() const
	{
		return _rows;
	}

	[[nodiscard]] std::size_t
	columns() const
	{
		return _columns;
	}

	std::size_t &
	entry(std::size_t row, std::size_t column)
	{
		DEBUG_ASSERT(0 <= row);
		DEBUG_ASSERT(row < _rows);
		DEBUG_ASSERT(0 <= column);
		DEBUG_ASSERT(column < _columns);
		return _table_buf[(row * _columns) + column];
	}

	std::vector<std::vector<Symbol>> const &
	productions() const
	{
		return _productions;
	}

	std::map<Symbol, std::size_t> const &
	var_indices()
	{
		return _var_indices;
	}

	std::map<Symbol, std::size_t> const &
	terminal_indices()
	{
		return _terminal_indices;
	}

	LLTable static parse(Grammar const &grammar, symbol_sets const &first_sets, symbol_sets const &follow_sets)
	{
		LLTable table;

		// I really should get a better way to get the terminals
		std::set<Symbol> terminals;
		for (auto const &[var, productions] : grammar.rules)
		{
			for (auto const &production : productions)
			{
				terminals.insert_range(
					production | std::views::filter([&](auto &symbol) { return not grammar.rules.contains(symbol); }));
			}
		}

		for (Symbol const &symbol : terminals)
		{
			table._terminal_indices.emplace(symbol, table._terminal_indices.size());
		}

		table._table_buf = std::make_unique<std::size_t[]>(terminals.size() * grammar.rules.size());
		std::uninitialized_fill_n(table._table_buf.get(), terminals.size() * grammar.rules.size(), -1);
		table._rows = grammar.rules.size();
		table._columns = terminals.size();

		for (auto const &[var, productions] : grammar.rules)
		{
			table._var_indices.emplace(var, table._var_indices.size());
			for (auto const &production : productions)
			{
				std::size_t index = table._productions.size();
				table._productions.push_back(production);
				std::set<Symbol> first_set = compute_first_set(grammar, first_sets, production);

				for (Symbol const &symbol : first_set)
				{
					if (symbol.name() != "epsilon")
					{
						table.entry(table._var_indices[var], table._terminal_indices[symbol]) = index;
					}
					else
					{
						for (Symbol const &follow : follow_sets.at(var))
						{
							table.entry(table._var_indices[var], table._terminal_indices[follow]) = index;
						}
					}
				}
			}
		}

		return table;
	}

  private:
	std::size_t _rows;
	std::size_t _columns;
	std::map<Symbol, std::size_t> _var_indices;
	std::map<Symbol, std::size_t> _terminal_indices;
	std::unique_ptr<std::size_t[]> _table_buf;
	std::vector<std::vector<Symbol>> _productions;
};

} // namespace sdp