#include <set>

#include <boost/flyweight.hpp>
#include <catch2/catch_test_macros.hpp>
#include <libassert/assert-catch2.hpp>

import sdp.grammar;
import sdp.grammar.analysis;

static boost::flyweight<std::string>
operator""_fw(const char *str, std::size_t len)
{
	return boost::flyweight<std::string>{str, len};
}

TEST_CASE("FIRST and FOLLOW sets")
{
	sdp::Grammar const grammar{
		.rules =
			{
				{"A"_fw, {{"a"_fw, "B"_fw}, {"C"_fw, "D"_fw}}},
				{"B"_fw, {{"b"_fw, "C"_fw}}},
				{"C"_fw, {{"epsilon"_fw}}},
				{"D"_fw, {{"d"_fw}, {"epsilon"_fw}}},
			},
		.start = {"A"_fw},
	};

	auto const first_sets = sdp::compute_first_sets(grammar);

	ASSERT(first_sets.size() == 4);
	ASSERT((first_sets.at("A"_fw) == std::set{"a"_fw, "d"_fw, "epsilon"_fw}));
	ASSERT((first_sets.at("B"_fw) == std::set{"b"_fw}));
	ASSERT((first_sets.at("C"_fw) == std::set{"epsilon"_fw}));
	ASSERT((first_sets.at("D"_fw) == std::set{"d"_fw, "epsilon"_fw}));

	auto const follow_set = sdp::compute_follow_sets(grammar, first_sets);
}