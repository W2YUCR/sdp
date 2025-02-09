module;

#include <print>

#include <mio/mmap.hpp>

export module sdp.ll_table_gen;

import sdp.grammar;
import sdp.grammar.tokenizer;
import sdp.grammar.parser;
import sdp.grammar.analysis;

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
			std::print("FIRST({}) =", var.get());
			for (auto const &first : firsts)
			{
				std::print(" {}", first.get());
			}
			std::println();
		}

		for (auto const &[var, follow] : follow_sets)
		{
			std::print("FOLLOW({}) =", var.get());
			for (auto const &first : follow)
			{
				std::print(" {}", first.get());
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