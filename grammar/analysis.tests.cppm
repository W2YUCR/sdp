#include <set>

#include <boost/flyweight.hpp>
#include <catch2/catch_test_macros.hpp>
#include <libassert/assert-catch2.hpp>

import sdp.grammar;
import sdp.grammar.analysis;

TEST_CASE("FIRST and FOLLOW sets")
{
	SECTION("Test 1")
	{
		sdp::Grammar const grammar{
			.rules =
				{
					{"A", {{"B", "C"}}},
					{"B", {{"b"}}},
					{"C", {{"c"}}},
				},
			.start = {"A"},
		};

		auto const first_sets = sdp::compute_first_sets(grammar);

		ASSERT(first_sets.size() == 3);
		ASSERT(first_sets.at("A") == std::set<sdp::Symbol>{"b"});
		ASSERT(first_sets.at("B") == std::set<sdp::Symbol>{"b"});
		ASSERT(first_sets.at("C") == std::set<sdp::Symbol>{"c"});

		auto const follow_set = sdp::compute_follow_sets(grammar, first_sets);

		ASSERT(follow_set.size() == 3);
		ASSERT(follow_set.at("A") == std::set<sdp::Symbol>{"$"});
		ASSERT(follow_set.at("B") == std::set<sdp::Symbol>{"c"});
		ASSERT(follow_set.at("C") == std::set<sdp::Symbol>{"$"});
	}

	SECTION("Test 2")
	{
		sdp::Grammar const grammar{
			.rules =
				{
					{"A", {{"a", "B"}, {"C", "D"}}},
					{"B", {{"b", "C"}}},
					{"C", {{"epsilon"}}},
					{"D", {{"d"}, {"epsilon"}}},
				},
			.start = {"A"},
		};

		auto const first_sets = sdp::compute_first_sets(grammar);

		ASSERT(first_sets.size() == 4);
		ASSERT(first_sets.at("A") == (std::set<sdp::Symbol>{"a", "d", "epsilon"}));
		ASSERT(first_sets.at("B") == std::set<sdp::Symbol>{"b"});
		ASSERT(first_sets.at("C") == std::set<sdp::Symbol>{"epsilon"});
		ASSERT(first_sets.at("D") == (std::set<sdp::Symbol>{"d", "epsilon"}));
	}
}