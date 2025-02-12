module;

#include <map>
#include <string>
#include <vector>

#include <boost/flyweight.hpp>

export module sdp.grammar;

export namespace sdp
{

struct Symbol final
{
	Symbol(char const *name) : _name{name} {}
	Symbol(std::string_view name) : _name{name} {}

	[[nodiscard]] std::string const &
	name() const
	{
		return _name;
	}

	std::strong_ordering
	operator<=>(Symbol const &) const = default;

  private:
	boost::flyweight<std::string> _name;
};

struct Grammar
{
	std::map<Symbol, std::vector<std::vector<Symbol>>> rules;
	Symbol start;
};

} // namespace sdp