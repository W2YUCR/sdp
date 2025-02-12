module;

#include <map>
#include <set>
#include <string>

#include <boost/flyweight.hpp>

export module sdp.grammar.analysis;

import sdp.grammar;

export namespace sdp
{

using symbol_sets = std::map<Symbol, std::set<Symbol>, std::less<>>;

symbol_sets
compute_first_sets(sdp::Grammar const &grammar)
{
	symbol_sets first_sets;

	bool changed = true;

	while (changed)
	{
		changed = false;

		for (auto const &[var, productions] : grammar.rules)
		{
			for (auto const &production : productions)
			{
				bool nullable = true;
				for (auto const &symbol : production)
				{
					// Pretty wasteful, probably should compute terminals separately
					if (not grammar.rules.contains(symbol))
					{
						auto [iter, success] = first_sets[var].insert(symbol);
						changed |= success;
						nullable = false;
						break;
					}
					bool has_epsilon = false;
					for (auto const &first : first_sets[symbol])
					{
						if (first.name() == "epsilon")
						{
							has_epsilon = true;
						}
						else
						{
							auto [iter, success] = first_sets[var].insert(first);
							changed |= success;
						}
					}
					nullable &= has_epsilon;
					if (not has_epsilon)
					{
						break;
					}
				}
				if (nullable)
				{
					first_sets[var].emplace("epsilon");
				}
			}
		}
	}

	return first_sets;
}

symbol_sets
compute_follow_sets(sdp::Grammar const &grammar, symbol_sets const &first_set)
{
	symbol_sets follow_sets;

	follow_sets[grammar.start].emplace("$");

	bool changed = true;
	while (changed)
	{
		changed = false;
		for (auto const &[var, productions] : grammar.rules)
		{
			for (auto const &production : productions)
			{
				for (std::size_t idx = 0; idx < production.size(); ++idx)
				{
					auto const &symbol = production[idx];

					if (not grammar.rules.contains(symbol))
					{
						continue;
					}

					if (idx == production.size() - 1)
					{
						for (auto const &follow : follow_sets[var])
						{
							auto [_, success] = follow_sets[symbol].insert(follow);
							changed |= success;
						}
						continue;
					}

					for (std::size_t next_idx = idx + 1; next_idx < production.size(); ++next_idx)
					{
						auto const &next = production[next_idx];
						if (not grammar.rules.contains(next))
						{
							auto [_, success] = follow_sets[symbol].insert(next);
							changed |= success;
							break;
						}

						bool nullable = false;

						for (auto const &first : first_set.at(next))
						{
							if (first.name() == "epsilon")
							{
								nullable = true;
							}
							else
							{
								auto [_, success] = follow_sets[symbol].insert(first);
								changed |= success;
							}
						}
						if (not nullable)
						{
							break;
						}
					}
				}
			}
		}
	}

	return follow_sets;
}

} // namespace sdp