module;

#include <map>
#include <string>
#include <vector>

#include <boost/flyweight.hpp>

export module sdp.grammar;

export namespace sdp
{

struct Grammar
{
	std::map<boost::flyweight<std::string>, std::vector<std::vector<boost::flyweight<std::string>>>> rules;
	boost::flyweight<std::string> start;
};

} // namespace sdp