module;

#include <print>
#include <ranges>

#include <mio/mmap.hpp>

export module sdp.ll_table_gen;

import sdp.grammar;
import sdp.grammar.tokenizer;
import sdp.grammar.parser;
import sdp.grammar.analysis;
import sdp.ll_table;

int
main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		std::println(stderr, "Usage: ll-table-gen <input>");
		return EXIT_FAILURE;
	}

	try
	{
		mio::mmap_source mmap{argv[1]};
		sdp::Tokenizer tokenizer{mmap.data(), mmap.data() + mmap.size()};
		sdp::Grammar grammar = sdp::parse(tokenizer, std::default_sentinel);
		auto first_sets = sdp::compute_first_sets(grammar);
		auto follow_sets = sdp::compute_follow_sets(grammar, first_sets);

		for (auto const &[var, firsts] : first_sets)
		{
			std::print("FIRST({}) =", var.name());
			for (auto const &first : firsts)
			{
				std::print(" {}", first.name());
			}
			std::println();
		}

		for (auto const &[var, follow] : follow_sets)
		{
			std::print("FOLLOW({}) =", var.name());
			for (auto const &first : follow)
			{
				std::print(" {}", first.name());
			}
			std::println();
		}

		sdp::LLTable table = sdp::LLTable::parse(grammar, first_sets, follow_sets);

		for (auto const &[idx, production] : table.productions() | std::views::enumerate)
		{
			std::print("{}:", idx);
			for (auto const &symbol : production)
			{
				std::print(" {}", symbol.name());
			}
			std::println();
		}

		std::println("Variables");
		for (auto const &[symbol, idx] : table.var_indices())
		{
			std::println("{} = {}", symbol.name(), idx);
		}

		std::println("Terminals");
		for (auto const &[symbol, idx] : table.terminal_indices())
		{
			std::println("{} = {}", symbol.name(), idx);
		}

		for (std::size_t row = 0; row < table.rows(); ++row)
		{
			for (std::size_t column = 0; column < table.columns(); ++column)
			{
				std::print("[{}]", table.entry(row, column));
			}
			std::println();
		}
	}
	catch (std::exception &ex)
	{
		std::println(stderr, "Error: {}", ex.what());
		return EXIT_FAILURE;
	}
}