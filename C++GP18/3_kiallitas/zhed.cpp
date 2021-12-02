#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <stack>
#include <fstream>

using Coord = std::pair<std::size_t, std::size_t>;

template<class Type>
struct State {
	std::vector<Type> vec;
	const std::size_t lineSize;
	const std::size_t lineCount;

	State(std::vector<Type>&& a, std::size_t ls) : vec(std::move(a)), lineSize(ls), lineCount(vec.size() / lineSize) {}
	
	static State<char> get(std::string&& str) {
		std::vector<char> ch;
		ch.reserve(str.size());

		std::stringstream ss(std::move(str));
		std::getline(ss, str);
		std::size_t lineSize{}, lineCount{};
		std::stringstream(std::move(str)) >> lineCount >> lineSize;

		while (std::getline(ss, str)) {
			ch.insert(ch.end(), str.begin(), str.end());
		}

		return State<char>{ std::move(ch), lineSize };
	}

	inline bool valid(const Coord& c) const {
		return c.first < lineCount && c.second < lineSize;
	}

	inline Type& operator()(std::size_t x, std::size_t y) {
		return vec[x * lineSize + y];
	}

	inline Type& operator()(const Coord& c) {
		return operator()(c.first, c.second);
	}

	template<class Fun>
	auto map(Fun&& f) const {
		std::vector<std::result_of_t<Fun(Type)>> cpstate;
		cpstate.reserve(vec.size());
		std::transform(vec.begin(), vec.end(), std::back_inserter(cpstate), f);
		return State<std::result_of_t<Fun(Type)>>{ std::move(cpstate), lineSize };
	}
};

enum class Direction : char {
	LEFT = '<', RIGHT = '>', UP = '^',DOWN = 'v'
};

constexpr static std::array<Direction, 4> directions = { Direction::LEFT, Direction::RIGHT, Direction::UP, Direction::DOWN };

constexpr inline int directionIndex(Direction dir) {
	return dir == Direction::LEFT ? 0 :
		dir == Direction::RIGHT ? 1 :
		dir == Direction::UP ? 2 :
		dir == Direction::DOWN ? 3 : 
		-1;
}

constexpr inline Direction opposite(Direction dir) {
	return dir == Direction::LEFT ? Direction::RIGHT :
		dir == Direction::RIGHT ? Direction::LEFT :
		dir == Direction::UP ? Direction::DOWN :
		dir == Direction::DOWN ? Direction::UP : dir;
}

constexpr inline Direction rotateLeft(Direction dir) {
	return dir == Direction::LEFT ? Direction::DOWN :
		dir == Direction::DOWN ? Direction::RIGHT :
		dir == Direction::RIGHT ? Direction::UP :
		dir == Direction::UP ? Direction::LEFT : dir;
}

template<Direction dir, class Type = std::size_t, bool forward = directionIndex(dir) % 2 == 1>
using SortedSet = std::set<Type, std::conditional_t<forward, std::less<Type>, std::greater<Type>>>;

template<Direction dir, class It, class Predicate, bool forward = directionIndex(dir) % 2 == 1>
inline It find_if_dir(It begin, It end, Predicate&& c) {
	if (forward) {
		return std::find_if(begin, end, c);
	} else {
		auto revEnd = std::make_reverse_iterator(begin);
		auto it = std::find_if(std::make_reverse_iterator(end), revEnd, c);
		return it == revEnd ? end : std::prev(it.base());
	}
}

template<class It, class Consumer>
inline It find_if_dir(Direction dir, It begin, It end, Consumer&& c) {
	switch (dir) {
	case Direction::UP:
		return find_if_dir<Direction::UP>(begin, end, std::forward<Consumer&&>(c));
	case Direction::DOWN:
		return find_if_dir<Direction::DOWN>(begin, end, std::forward<Consumer&&>(c));
	case Direction::LEFT:
		return find_if_dir<Direction::LEFT>(begin, end, std::forward<Consumer&&>(c));
	case Direction::RIGHT:
		return find_if_dir<Direction::RIGHT>(begin, end, std::forward<Consumer&&>(c));
	}
	return end;
}

inline Direction whereToGo(Coord from, Coord to) {
	if (to.first < from.first) return Direction::UP;
	else if (to.first > from.first) return Direction::DOWN;
	else if (to.second < from.second) return Direction::LEFT;
	else if (to.second > from.second) return Direction::RIGHT;
	return Direction::UP;
}

inline Coord moveTo(Coord from, Direction dir) {
	return{ from.first + (dir == Direction::UP ? -1 : dir == Direction::DOWN), from.second + (dir == Direction::LEFT ? -1 : dir == Direction::RIGHT) };
}

using ResultLine = std::pair<Coord, Direction>;
using Result = std::vector<ResultLine>;

inline std::ostream& operator<<(std::ostream& o, const ResultLine& res) {
	return o << res.first.first+1 << " " << res.first.second+1 << " " << (char)res.second;
}

inline std::ostream& operator<<(std::ostream& o, const Result& results) {
	o << results.size() << "\n";
	for (auto& res : results)
		o << res << "\n";
	return o;
}

static auto mapElementIsFix = [](auto&& pair) { return pair.second.fix; };

struct DirectionalMap {
	std::map<std::size_t, std::set<std::size_t>> before;
	std::map<std::size_t, std::set<std::size_t>> after;

	template<class It>
	void add(std::set<std::size_t>& toSet, std::size_t elem, It&& begin, It&& end) {
		toSet.insert(elem);
		toSet.insert(begin, end);
	}

	bool add(std::size_t from, std::size_t to) {
		if (from == to)
			return false;

		auto& beforeFrom = before[from];
		if (beforeFrom.count(to))
			return false;

		auto& afterTo = after[to];
		
		add(before[to], from, beforeFrom.begin(), beforeFrom.end());
		add(after[from], to, afterTo.begin(), afterTo.end());

		for (std::size_t beforeFromElem : beforeFrom)
			add(after[beforeFromElem], to, afterTo.begin(), afterTo.end());
		for (std::size_t afterToElem : afterTo)
			add(before[afterToElem], from, beforeFrom.begin(), beforeFrom.end());
		return true;
	}

	bool isBefore(std::size_t from, std::size_t to) const {
		auto it = after.find(from);
		return it != after.end() && it->second.count(to);
	}

	bool isAfter(std::size_t from, std::size_t to) const {
		return isBefore(to, from);
	}

	bool merge(const DirectionalMap& dm) {
		if (after.empty()) {
			after = dm.after;
			before = dm.before;
		} else {
			for (auto& p : dm.after)
				for (std::size_t to : p.second) {
					if (isBefore(p.first, to))
						continue;
						
					if (!add(p.first, to))
						return false;
				}
		}
		return true;
	}

	std::set<std::size_t>& getBefore(std::size_t w) {
		return before[w];
	}

	std::set<std::size_t>& getAfter(std::size_t t) {
		return after[t];
	}
};

Result result;

struct Solver {
	using FromIndex = std::size_t;
	using EmptyIndex = std::size_t;
	using FromDependency = struct { Direction dir; std::set<EmptyIndex> empties; bool nomore; };
	using FromToEmptyDependencies = std::map<FromIndex, FromDependency>;
	using FromToEmptyInformation = struct { bool fix; FromToEmptyDependencies dependencies; std::map<FromIndex, std::set<EmptyIndex>> prohibited; DirectionalMap fromDependencies; std::map<EmptyIndex, std::set<FromIndex>> emptyToFromNeeded; };
	using StateIndex = struct { bool empty; std::size_t index; };

	struct From {
		const FromIndex index;
		const std::size_t length;
		const Coord coord;
		std::map<Direction, std::map<EmptyIndex, FromToEmptyInformation>> possibles;
		bool required, fix;

		From(FromIndex index, std::size_t length, Coord coord) : index(index), length(length), coord(coord), required{}, fix{} {}

		bool setIsFix() {
			if (fix)
				return false;
			return fix = possibles.size() == 1 && possibles.begin()->second.size() <= length;
		}

		std::size_t currentFixCount() const {
			return possibles.size() != 1 ? 0 :
				std::count_if(possibles.begin()->second.begin(), possibles.begin()->second.end(), mapElementIsFix);
		}
	};

	struct Empty {
		const EmptyIndex index;
		const Coord coord;
		std::map<Direction, std::unordered_set<FromIndex>> possibilities;
		std::set<FromIndex> theseFromsNeedThis;
		bool required, fix;

		Empty(EmptyIndex index, Coord coord, bool required = {}) : index(index), coord(coord), required(required), fix{} {}

		bool setIsFix() {
			if (!required || fix)
				return false;
			return fix = possibilities.size() == 1 && possibilities.begin()->second.size() == 1;
		}
	};

	std::vector<From> froms;
	std::vector<Empty> empties;
	DirectionalMap fromDependencies;

	State<StateIndex> states;
	
	Solver(State<char>&& startState) : states(startState.map([&startState, this, i = 0ul, j = 0ul](char c) mutable {
			StateIndex res;
			if (c == '.' || c == 'E') {
				res = { true, empties.size() };
				empties.emplace_back(empties.size(), Coord{ i, j }, c == 'E');
			} else {
				res = { false, froms.size() };
				froms.emplace_back(froms.size(), c - '0', Coord{ i, j });
			}
			if (!(++j %= startState.lineSize))
				++i;
			return res;
		}))
	{
		for (From& from : froms)
			for (Direction dir : directions) {
				auto& possiblesAtDir = from.possibles[dir];
				forEachCoord < true > (moveTo(from.coord, dir), dir, [&, this](StateIndex& si) {
					possiblesAtDir[si.index].fix = false;
					empties[si.index].possibilities[dir].insert(from.index);
				}, [&](Coord& c) { return states.valid(c); });
			}
	}

	bool isCalculated() const {
		return std::all_of(froms.begin(), froms.end(), [](const From& f) { return !f.required || f.fix; }) &&
			std::all_of(empties.begin(), empties.end(), [](const Empty& e) { return !e.required || e.fix; });
	}

	bool filterFromToDir(From& f, Direction dir) {
		bool wasChange = false;
		for (auto it = f.possibles.begin(); it != f.possibles.end();) {
			if (it->first == dir) {
				++it;
				continue;
			}
			removeFromDir(f, it++->first);
			wasChange = true;
		}
		return wasChange;
	}

	bool filterEmptyToDir(Empty& e, Direction dir) {
		bool wasChange = false;
		for (auto it = e.possibilities.begin(); it != e.possibilities.end();) {
			if (it->first == dir) {
				++it;
				continue;
			}

			for (FromIndex fromIndex : it->second)
				removeFromPossible(froms[fromIndex], it->first, e.index);
			it = e.possibilities.erase(it);
			wasChange = true;
		}
		return wasChange;
	}

	void requiredEmptyFound(Empty& e) {
		Direction dir = e.possibilities.begin()->first;
		FromIndex fromIndex = *e.possibilities.begin()->second.begin();

		From& from = froms[fromIndex];
		filterFromToDir(from, dir);

		from.possibles.at(dir).at(e.index).fix = true;
		from.required = true;

		forEachCoord < true > (moveTo(from.coord, dir), dir, [&](StateIndex i) {
			Empty& othE = empties[i.index];
			othE.required = true;
			if (othE.fix) {
				if (othE.possibilities.empty())
					throw std::runtime_error("empty between!");
				FromIndex othF = *othE.possibilities.begin()->second.begin();
				if (othF != fromIndex)
					if (!fromDependencies.add(othF, fromIndex))
						throw new std::runtime_error("circular dependency!");
			}
			else
				othE.theseFromsNeedThis.emplace(fromIndex);
		}, [&e](Coord c) { return c != e.coord; });

		for (FromIndex fi : e.theseFromsNeedThis)
			if (fi != fromIndex)
				if (!fromDependencies.add(fromIndex, fi))
					throw new std::runtime_error("circular dependency!");
		e.theseFromsNeedThis.clear();
	}

	bool removeFromDir(From& from, Direction dir) {
		auto dirIt = from.possibles.find(dir);
		if (dirIt == from.possibles.end())
			return false;

		if (from.fix) throw std::runtime_error("From is fix!");

		for (auto& possiblePair : dirIt->second) {
			if (mapElementIsFix(possiblePair)) 
				throw std::runtime_error("Remove fix field!");
			removeEmptyPossible(empties[possiblePair.first], dir, from.index);
		}

		from.possibles.erase(dirIt);

		if (from.required && from.possibles.empty())
			throw std::runtime_error("Required from is empty");

		return true;
	}

	bool removeFromPossible(From& from, Direction dir, EmptyIndex emptyIndex) {
		auto dirIt = from.possibles.find(dir);
		if (dirIt == from.possibles.end())
			return false;

		auto& possibleMap = dirIt->second;
		auto possibleIt = possibleMap.find(emptyIndex);
		if (possibleIt == possibleMap.end())
			return false;

		if (mapElementIsFix(*possibleIt))
			throw std::runtime_error("Remove fix field!");
		
		possibleMap.erase(possibleIt);

		if (possibleMap.empty())
			removeFromDir(from, dir);
		return true;
	}

	bool removeEmptyDir(Empty& empty, Direction dir) {
		auto dirIt = empty.possibilities.find(dir);
		if (dirIt == empty.possibilities.end())
			return false;

		if (empty.fix)
			throw std::runtime_error("Fix empty removed dir");

		for (auto& fromIndex : dirIt->second)
			removeFromPossible(froms[fromIndex], dir, empty.index);

		empty.possibilities.erase(dirIt);

		if (empty.required && empty.possibilities.empty())
			throw std::runtime_error("Required empty is empty");

		return true;

	}

	bool removeEmptyPossible(Empty& empty, Direction dir, FromIndex fromIndex) {
		auto& emptyDirPossibles = empty.possibilities;
		auto dirIt = emptyDirPossibles.find(dir);
		if (dirIt == emptyDirPossibles.end())
			return false;

		auto& emptyPossibles = dirIt->second;
		auto possibleIt = emptyPossibles.find(fromIndex);
		if (possibleIt == emptyPossibles.end())
			return false;

		if (empty.fix)
			throw std::runtime_error("Called remove on fix empty");

		emptyPossibles.erase(possibleIt);
		if (emptyPossibles.empty()) {
			emptyDirPossibles.erase(dirIt);
			if (empty.required && emptyDirPossibles.empty()) 
				throw std::runtime_error("Required empty has no more possible: " + std::to_string(empty.index));
		}
		return true;
	}

	void setEmptyPossible(Empty& e, Direction dir, FromIndex from) {
		filterEmptyToDir(e, dir);

		auto& dirElements = e.possibilities.begin()->second;
		for (auto possibleIt = dirElements.begin(); possibleIt != dirElements.end(); ) {
			if (*possibleIt == from) {
				++possibleIt;
				continue;
			}
			removeFromPossible(froms[*possibleIt], dir, e.index);
			possibleIt = dirElements.erase(possibleIt);
		}
	}

	template<bool onlyEmpties = false, class DoWithStateIndex, class While >
	Coord forEachCoord(Coord from, Direction dir, DoWithStateIndex&& doWith, While&& whileOK) {
		Coord coord = from;
		for (; whileOK(coord); coord = moveTo(coord, dir)) {
			StateIndex i = states(coord);
			if (!onlyEmpties || i.empty)
				doWith(i);
		}
		return coord;
	}

	void fromFound(From& f) {
		Direction dir = f.possibles.begin()->first;
		auto& possibleMap = f.possibles.begin()->second;

		for (auto& possiblePair : possibleMap) {
			Empty& e = empties[possiblePair.first];
			f.required |= e.required;

			setEmptyPossible(e, dir, f.index);
		}

		if (!f.required)
			return;

		std::size_t found = 0;
		forEachCoord < true > (f.coord, dir, [&](StateIndex pair) {
			Empty& e = empties[pair.index];

			if (possibleMap.count(e.index)) // our empty
				++found;
			else if (e.fix && !e.required)
				froms[*e.possibilities.begin()->second.begin()].required = true;

			e.required = true;
		}, [&](auto&&) { return found != possibleMap.size(); });
	}

	template<Direction dir>
	void resolveSameDir(const SortedSet<dir>& indices, std::set<EmptyIndex>&& fillableEmpties) {
		auto fromIt = indices.begin();
		From& firstFrom = froms[*fromIt];

		// watch out the sequence sorting!!!! :(
		forEachCoord < true > (moveTo(firstFrom.coord, dir), dir, [&, fromLength = firstFrom.length, fromLengthUsed = 0ul] (StateIndex p) mutable {
			auto emptyIt = fillableEmpties.find(p.index);
			if (emptyIt == fillableEmpties.end())
				return;

			Empty& e = empties[p.index];
			setEmptyPossible(e, dir, *fromIt);
			fillableEmpties.erase(emptyIt);

			if (++fromLengthUsed == fromLength && !fillableEmpties.empty()) {
				fromLengthUsed = 0;
				fromLength = froms[*++fromIt].length;
			}
		},
		[&](auto&&) { return !fillableEmpties.empty(); });
	}

	inline void resolveSameDir(Direction dir, const std::set<FromIndex>& indices, std::set<EmptyIndex>&& fillableEmpties) {
		switch (dir)
		{
		case Direction::DOWN:
			resolveSameDir<Direction::DOWN>(indices, std::move(fillableEmpties));
			break;
		case Direction::RIGHT:
			resolveSameDir<Direction::RIGHT>(indices, std::move(fillableEmpties));
			break;
		case Direction::UP:
			resolveSameDir<Direction::UP>({ indices.begin(), indices.end() }, std::move(fillableEmpties));
			break;
		case Direction::LEFT:
			resolveSameDir<Direction::LEFT>({ indices.begin(), indices.end() }, std::move(fillableEmpties));
			break;
		}
	}

	bool fromCanBeAssignThese(const From& from, Direction dir, const std::set<EmptyIndex>& empties, const std::set<EmptyIndex>& prohibited) {
		auto dirPairIt = from.possibles.find(dir);
		if (dirPairIt == from.possibles.end())
			return false;

		std::size_t assigned = 0;
		std::size_t fix = 0;
		std::size_t notProhibited = 0;
		for (auto& mapPair : dirPairIt->second) {
			if (empties.count(mapPair.first)) {
				if (prohibited.count(mapPair.first)) // can be ...
					return false;

				++assigned;
			}
			else if (mapPair.second.fix) {
				if (prohibited.count(mapPair.first)) // can be ...
					return false;

				++fix;
			}
			else if (!prohibited.count(mapPair.first))
				++notProhibited;
		}
		return assigned == empties.size() && assigned + fix <= from.length && from.length <= assigned + fix + notProhibited;
	}

	bool setEmptiesRequiredToOneDirection(Empty& e) {
		bool changedResult = false;
		Direction dir = e.possibilities.begin()->first;
		Direction oppositeDir = opposite(dir);

		std::size_t needFromDir = 1;
		std::size_t gotLengthsFromDir = 0;

		auto fromCompareWithLength = [](auto& f1, auto& f2) { return f1->length > f2->length; };
		std::set<From*, decltype(fromCompareWithLength)> possibleFroms(fromCompareWithLength);
		std::set<Empty*> notOnlyOneDirection;

		forEachCoord(moveTo(e.coord, oppositeDir), oppositeDir, [&, needRequired = true](StateIndex index) mutable {
			if (gotLengthsFromDir >= needFromDir)
				needRequired = false;

			if (index.empty) {
				Empty& othE = empties[index.index];
				if (needRequired && !othE.required)
					changedResult = othE.required = true;
				if ((othE.possibilities.size() == 1 && othE.possibilities.begin()->first == dir) ||
					(othE.possibilities.size() == 2 && othE.possibilities.count(dir) && othE.possibilities.count(oppositeDir))) {
					if (needRequired)
						++needFromDir;
				}
				else
					notOnlyOneDirection.insert(&othE);
			}
			else {
				From& f = froms[index.index];
				if (f.possibles.count(dir)) {
					if (needRequired && !f.required) {
						changedResult = true;
						f.required = true;
					}
					gotLengthsFromDir += f.length;
					possibleFroms.insert(&f);
				}
			}
		}, [&](Coord& c) { return states.valid(c); });

		while (!possibleFroms.empty() && gotLengthsFromDir < needFromDir + (*possibleFroms.begin())->length) {
			From& f = **possibleFroms.begin();

			if (f.possibles.size() > 1) {
				changedResult = true;
				filterFromToDir(f, dir);
			}

			if (gotLengthsFromDir == needFromDir) {
				for (Empty* e : notOnlyOneDirection) {
					changedResult |= removeFromPossible(f, dir, e->index);
					changedResult |= removeEmptyPossible(*e, dir, f.index);
				}
			}

			gotLengthsFromDir -= f.length;
			possibleFroms.erase(possibleFroms.begin());
			if (needFromDir < f.length) {
				break;
			}
			needFromDir -= f.length;
		}
		return changedResult;
	}

	bool setFromDirIfHasAnyFix(From& f) {
		for (auto& dirMap : f.possibles) {
			auto& possibleMap = dirMap.second;
			if (std::any_of(possibleMap.begin(), possibleMap.end(), mapElementIsFix)) {
				filterFromToDir(f, dirMap.first);
				return true;
			}
		}
		return false;
	}

	bool filterUnusableDirections(From& f) {
		bool successFiltered = false;
		for (Direction dir : directions) {
			auto it = f.possibles.find(dir);
			if (it == f.possibles.end())
				continue;

			bool wasUnused = false;
			bool usable = false;
			bool fromLengthIsOne = f.length == 1;
			Empty* latestUsable = nullptr;

			Direction needToUsable[] = { rotateLeft(dir), rotateLeft(opposite(dir)) };
			Coord latest = forEachCoord < true > (f.coord, dir, [&](const StateIndex& si) {
				Empty& e = empties[si.index];
				if (e.possibilities.empty()) {
					wasUnused = true;
					return;
				}
				if (e.required) {
					latestUsable = &e;
					usable = true;
					return;
				}

				for (Direction checkDirection : needToUsable) {
					bool wasInnerUnused = false;
					forEachCoord < true > (moveTo(e.coord, checkDirection), checkDirection, [&](const StateIndex& si) {
						Empty& othE = empties[si.index];
						if (othE.possibilities.empty()) {
							wasInnerUnused = true;
							return;
						}

						auto dirIt = othE.possibilities.find(checkDirection);
						if (dirIt == othE.possibilities.end())
							return;
						
						for (const FromIndex& i : dirIt->second) {
							if (whereToGo(froms[i].coord, e.coord) == checkDirection) {
								latestUsable = &e;
								usable = true;
								break;
							}
						}
					}, [&](const Coord& c) { return states.valid(c) && !wasInnerUnused && (!usable || fromLengthIsOne); });
				}
			}, [&](const Coord& c) { return states.valid(c) && !wasUnused && (!usable || fromLengthIsOne); });

			if (!usable) {
				successFiltered |= removeFromDir(f, dir);
			}
			else if (fromLengthIsOne) {
				forEachCoord < true > (moveTo(latestUsable->coord, dir), dir, [&](const StateIndex& i) {
					successFiltered |= removeFromPossible(f, dir, i.index);
					successFiltered |= removeEmptyPossible(empties[i.index], dir, f.index);
				}, [&](const Coord& c) { return c != latest; });
			}
		}
		return successFiltered;
	}

	template<class WhoKnowsThisType>
	bool checkFromSameDirectionWithSameUnknowns(From& f, WhoKnowsThisType&& sameDirectionWithSameUnknownMap) {
		Direction dir = f.possibles.begin()->first;
		auto& possibleMap = f.possibles.begin()->second;

		auto& sameLineColumnVector = sameDirectionWithSameUnknownMap[directionIndex(dir)][directionIndex(dir) / 2 ? f.coord.second : f.coord.first];

		bool resolved = false;
		std::size_t to = sameLineColumnVector.size();

		std::set<EmptyIndex> possibleEmpties;
		for (auto& pair : possibleMap) {
			possibleEmpties.insert(pair.first);
		}

		std::set<FromIndex> usedIndices;

		for (std::size_t i = 0; i < to; ++i) {
			auto& otherTup = sameLineColumnVector.at(i);

			std::size_t needCount = std::get<1>(otherTup) + f.length;
			std::set<EmptyIndex> otherSet = std::get<2>(otherTup);
			otherSet.insert(possibleEmpties.begin(), possibleEmpties.end());

			if (needCount == otherSet.size()) {
				usedIndices.insert(std::get<0>(otherTup).begin(), std::get<0>(otherTup).end());
				usedIndices.insert(f.index);

				resolveSameDir(dir, usedIndices, std::move(otherSet));
				resolved = true;
				break;
			}
			else {
				auto cp = std::get<0>(otherTup);
				cp.emplace_back(f.index);
				sameLineColumnVector.emplace_back(cp, needCount, std::move(otherSet));
			}
		}

		if (!resolved) {
			sameLineColumnVector.emplace_back(std::vector<FromIndex>{ f.index }, f.length, std::move(possibleEmpties));
			return false;
		}

		sameLineColumnVector.resize(to);
		sameLineColumnVector.erase(
			std::remove_if(sameLineColumnVector.begin(), sameLineColumnVector.end(), [&](auto& vec) { return std::any_of(std::get<0>(vec).begin(), std::get<0>(vec).end(),
				[&](auto&& i) { return usedIndices.count(i); }); }), sameLineColumnVector.end());

		return true;
	}

	bool checkFromFixDirectionFixCount(From& f, std::size_t fixCount) {
		if (fixCount > f.length) throw std::runtime_error("moar fix than length!! :(");

		auto& possiblePair = *f.possibles.begin();
		auto& possibleMap = possiblePair.second;
		if (fixCount != f.length || possibleMap.size() == f.length)
			return false;

		for (auto it = possibleMap.begin(); it != possibleMap.end();) {
			if (mapElementIsFix(*it)) {
				++it;
				continue;
			}
			removeEmptyPossible(empties[it->first], possiblePair.first, f.index);
			it = possibleMap.erase(it);
		}
		return true;
	}

	bool fromDistanceFilter(From& f) {
		bool changed = false;
		for (auto it = f.possibles.begin(); it != f.possibles.end();) {
			Direction dir = it->first;
			auto& possibleMap = it->second;

			std::size_t possiblesFound = 0;
			std::size_t ourOrNoone = 0;

			bool noFurther = false;
			Coord latest = forEachCoord < true > (moveTo(f.coord, dir), dir, [&](StateIndex pair) {
				Empty& e = empties[pair.index];
				if (e.possibilities.empty()) {
					noFurther = true;
					return;
				}
				if (e.possibilities.size() == 1) {
					auto& possibleSet = e.possibilities.begin()->second;
					if (possibleSet.size() == 1 && *possibleSet.begin() == f.index) {
						++ourOrNoone;
					}
				}

				if (possibleMap.count(pair.index))
					++possiblesFound;
			}, [&](const Coord& c) { return !noFurther && ourOrNoone < f.length && states.valid(c); });

			if (noFurther && possiblesFound < f.length) { // can not make it!! 
				changed = true;
				removeFromDir(f, it++->first);
				continue;
			}
			if (possiblesFound < possibleMap.size()) {
				forEachCoord < true > (latest, dir, [&](StateIndex pair) {
					auto it = possibleMap.find(pair.index);
					if (it == possibleMap.end())
						return;

					changed |= removeEmptyPossible(empties[pair.index], dir, f.index);
					possibleMap.erase(it);
				}, [&](auto&&) { return possiblesFound < possibleMap.size(); });
			}
			++it;
		}
		return changed;
	}

	bool dependencyFilter(From& f, std::size_t fixCount) {
		auto& pair = *f.possibles.begin();

		bool changed = false;
		forEachCoord < true > (f.coord, pair.first, [&] (StateIndex& si) {
			Empty& e = empties[si.index];
			auto it = pair.second.find(si.index);

			if (e.fix) {
				if (it != pair.second.end())
					--fixCount;
				return;
			}

			std::vector<std::pair<Direction, FromIndex>> remove;
			for (auto& p : e.possibilities)
				for (FromIndex fi : p.second)
					if (fromDependencies.isBefore(f.index, fi))
						remove.emplace_back(p.first, fi);

			changed |= !remove.empty();

			for (auto p : remove) {
				removeEmptyPossible(e, p.first, p.second);
				removeFromPossible(froms[p.second], p.first, e.index);
			}
		}, [&fixCount](auto&&) { return fixCount; });
		return changed;
	}

	template<class Getter>
	bool checkLine(std::size_t max, Getter&& getter, Direction dir) {
		bool wasChange = false;
		
		std::size_t toDirection = 0;

		for (std::size_t n = 0; n < max; ++n) {
			StateIndex si = getter(n);

			if (si.empty) {
				Empty& e = empties[si.index];
				if (e.possibilities.empty()) {
					toDirection = 0;
					continue;
				}

				auto dirIt = e.possibilities.find(dir);
				if (dirIt == e.possibilities.end())
					continue;
				if (e.possibilities.size() == 1 && toDirection)
					--toDirection;
				else if (!toDirection)
					wasChange |= removeEmptyDir(e, dir);
			}
			else {
				From& f = froms[si.index];
				
				auto dirIt = f.possibles.find(dir);
				if (dirIt == f.possibles.end())
					continue;

				toDirection += f.length;
			}
		}
		return wasChange;
	}

	bool filterPossibles() {
		bool changed = true;

		while (!isCalculated() && changed) {
			changed = false;

			for (Empty& e : empties) {
				if (e.fix)
					continue;

				if (e.possibilities.size() == 0) {
					e.fix = true;
					continue;
				}

				if (e.setIsFix()) {
					changed = true;
					requiredEmptyFound(e);
					continue;
				}

				if (e.required && e.possibilities.size() == 1)
					changed |= setEmptiesRequiredToOneDirection(e);
			}

			for (std::size_t i = 0; i < states.lineCount; ++i) {
				changed |= checkLine(states.lineSize, [&](std::size_t j) { return states(i, j); }, Direction::RIGHT);
				changed |= checkLine(states.lineSize, [&](std::size_t j) { return states(i, states.lineSize - 1 - j); }, Direction::LEFT);
			}

			for (std::size_t j = 0; j < states.lineSize; ++j) {
				changed |= checkLine(states.lineCount, [&](std::size_t i) { return states(i, j); }, Direction::DOWN);
				changed |= checkLine(states.lineCount, [&](std::size_t i) { return states(states.lineCount - 1 - i, j); }, Direction::UP);
			}

			{
				std::array<std::map<std::size_t, std::vector<std::tuple<std::vector<FromIndex>, std::size_t, std::set<EmptyIndex>>>>, 4> sameDirectionWithSameUnknownMap;

				for (From& f : froms) {
					if (f.fix)
						continue;

					if (f.possibles.size() > 1)
						changed |= setFromDirIfHasAnyFix(f);

					changed |= filterUnusableDirections(f);


					if (f.possibles.size() == 1) {
						std::size_t fixCount = f.currentFixCount();
						changed |= checkFromFixDirectionFixCount(f, fixCount);

						if (fixCount)
							changed |= dependencyFilter(f, fixCount);
					}

					changed |= fromDistanceFilter(f);

					if (f.setIsFix()) {
						changed = true;
						fromFound(f);
						continue;
					}

					if (f.possibles.size() == 1)
						changed |= checkFromSameDirectionWithSameUnknowns(f, sameDirectionWithSameUnknownMap);
				}
			}

			if (!changed && !isCalculated()) {
				DependencyCalculator dc{ this };	
				changed |= dc.calculate();
			}
			if (!changed && !isCalculated()) {
				if (guessing())
					return false;
				else
					changed = true;
			}
		}
		return true;
	}

	std::tuple<FromIndex, Direction, EmptyIndex, FromToEmptyInformation*> findMostDependent() {
		std::tuple<FromIndex, Direction, EmptyIndex, FromToEmptyInformation*> res;
		std::tuple<long long, long long, bool> ff = std::make_tuple(-1, 0, false);
		for (From& f : froms) {
			for (auto& dirPair : f.possibles) {
				for (auto& emptyPair : dirPair.second) {
					FromToEmptyInformation& ftei = emptyPair.second;
					if (ftei.fix)
						continue;

					long long emptiesSizeTofixing = 0;
					for (auto& fromDepPair : ftei.dependencies)
						for (EmptyIndex ei : fromDepPair.second.empties)
							if (!empties[ei].fix)
								++emptiesSizeTofixing;

					auto tup = std::make_tuple(emptiesSizeTofixing, (long long)ftei.prohibited.size(), empties[emptyPair.first].required);
					if (tup > ff) {
						res = std::make_tuple(f.index, dirPair.first, emptyPair.first, &ftei);
						ff = tup;
					}
				}
			}
		}

		return res;
	}

	bool guessing() {
		auto tup = findMostDependent();
		if (!std::get<3>(tup))
			throw std::runtime_error("No more guessing :(");

		try {
			Solver cp = *this;
			DependencyCalculator dc{ &cp };
			dc.writeSolution(std::get<3>(tup)->dependencies);
			if (cp.filterPossibles())
				cp.collectResult();
			return true;
		} catch (...) {
			removeFromPossible(froms[std::get<0>(tup)], std::get<1>(tup), std::get<2>(tup));
			return false;
		}
	}

	struct DependencyCalculator {
		enum class TraversarInfo { NOT_YET, TEMPORARY, FINAL, DISCARDED, SOLUTION };
		using DependencyInfo = struct { TraversarInfo mark; bool changed; FromToEmptyInformation& infos; };
		using DependencyKey = struct { From* from; Direction dir; EmptyIndex emptyIndex; };

		using DependencyKeySorter = struct {
			bool operator()(const DependencyKey& d1, const DependencyKey& d2) const {
				return std::tie(d1.from, d1.emptyIndex) < std::tie(d2.from, d2.emptyIndex);
			}
		};

		Solver* solver;
		bool changed;
		std::map<DependencyKey, DependencyInfo, DependencyKeySorter> toDependencies;
		std::vector<Coord> requiredEmptyCoords;

		void initDependencies() {
			for (From& f : solver->froms) {
				for (auto& dirPair : f.possibles) {
					for (auto& infoPair : dirPair.second) {
						if (!infoPair.second.fix) {
							EmptyIndex emptyIndex { infoPair.first };
							DependencyInfo& di =  toDependencies.emplace(DependencyKey{ &f, dirPair.first, emptyIndex },
								DependencyInfo{ TraversarInfo::NOT_YET, false, infoPair.second }).first->second;
							di.changed = false;

							if (infoPair.second.dependencies.empty()) {
								infoPair.second.dependencies.emplace(f.index, FromDependency{ dirPair.first,{ infoPair.first },{} }); // self only once
							}

							if (infoPair.second.prohibited.empty()) {
								Empty& e = solver->empties[emptyIndex];
								for (auto& dirPair : e.possibilities) {
									for (const FromIndex& fi : dirPair.second) {
										if (fi != f.index) {
											infoPair.second.prohibited[fi].insert(emptyIndex);
										}
									}
								}
							}

							bool foundEmpty = false;
							infoPair.second.fromDependencies.merge(solver->fromDependencies);

							solver->forEachCoord < true > (f.coord, dirPair.first, [&](StateIndex& si) {
								if (si.index == emptyIndex) {
									foundEmpty = true;
									return;
								}

								Empty& othE = solver->empties[si.index];
								if (othE.fix) {
									if (othE.possibilities.empty()) { // empty empty but not found yet our empty
										di.mark = TraversarInfo::DISCARDED;
										return;
									}

									FromIndex fi = *othE.possibilities.begin()->second.begin();
									if (solver->fromDependencies.isAfter(f.index, fi)) // we know this after
										return;

									if (solver->fromDependencies.isBefore(f.index, fi)) { // can not be possible...
										di.mark = TraversarInfo::DISCARDED;
										return;
									}

									infoPair.second.fromDependencies.add(fi, f.index);
								} else {
									infoPair.second.emptyToFromNeeded[si.index].emplace(f.index);
								}
							}, [&foundEmpty](auto&&) { return !foundEmpty; });
						}
					}
				}
			}
		}

		void initRequiredEmpties() {
			for (Empty& e : solver->empties) {
				if (e.required && !e.fix) {
					requiredEmptyCoords.push_back(e.coord);
				}
			}
		}

		bool setNomore(From& f, FromDependency& myselfDeps, std::set<EmptyIndex>& setProhibiteds) {
			std::size_t currAssigned = f.currentFixCount();
			if (currAssigned > 0) {
				for (EmptyIndex ei : myselfDeps.empties) {
					currAssigned += !solver->empties[ei].fix;
				}
			}
			else {
				currAssigned = myselfDeps.empties.size();
			}
			myselfDeps.nomore = currAssigned == f.length;

			if (myselfDeps.nomore) {
				for (auto& depDirPair : f.possibles) {
					for (auto& depPair : depDirPair.second) {
						if (!myselfDeps.empties.count(depPair.first) && !depPair.second.fix) {
							setProhibiteds.insert(depPair.first);
						}
					}
				}
				return true;
			}
			return false;
		}

		template<class QueueType, class Predicate>
		Coord addToCoords(QueueType& to, Coord from, Direction dir, Predicate pred) {
			while (pred(from)) {
				to.emplace(from);
				from = moveTo(from, dir);
			}
			return from;
		}

		template<class QueueType>
		void addToCoordsFromTo(QueueType& to, Coord fromCoord, Coord toCoord, bool addTo = false) {
			if (fromCoord != toCoord)
				addToCoords(to, fromCoord, whereToGo(fromCoord, toCoord), [&toCoord](auto&& coord) { return coord != toCoord; });
			
			if (addTo)
				to.emplace(fromCoord);
		}

		template<class QueueType>
		TraversarInfo mergeTwoDependency(FromToEmptyInformation& infos, bool& wasChange,
			QueueType& coords, const FromToEmptyInformation& otherInfo) {
			
			FromToEmptyDependencies& currentDependencies = infos.dependencies;
			auto& prohibiteds = infos.prohibited;

			if (!infos.fromDependencies.merge(otherInfo.fromDependencies))
				return TraversarInfo::DISCARDED;

			for (auto& p : otherInfo.emptyToFromNeeded) {
				bool found = false;
				for (auto& depPair : infos.dependencies) {
					for (EmptyIndex empty : depPair.second.empties) {
						if (empty == p.first) {
							for (FromIndex fi : p.second)
								if (depPair.first != fi && !infos.fromDependencies.add(depPair.first, fi))
									return TraversarInfo::DISCARDED;
							found = true;
							break;
						}
					}
					if (found)
						break;
				}
				if (!found)
					infos.emptyToFromNeeded[p.first].insert(p.second.begin(), p.second.end());
			}

			for (auto& otherProhibitedPair : otherInfo.prohibited) {
				auto& newDep = prohibiteds.emplace(otherProhibitedPair).first->second;

				auto depIt = currentDependencies.find(otherProhibitedPair.first);
				if (depIt != currentDependencies.end())
					for (EmptyIndex prohibitedElement : otherProhibitedPair.second)
						if (depIt->second.empties.count(prohibitedElement))
							return TraversarInfo::DISCARDED;

				std::size_t preSize = newDep.size();
				newDep.insert(otherProhibitedPair.second.begin(), otherProhibitedPair.second.end());
				changed |= wasChange |= newDep.size() != preSize;
			}

			for (auto& otherDepPair : otherInfo.dependencies) {
				FromIndex fromOtherDep = otherDepPair.first;
				const FromDependency& fromDep = otherDepPair.second;

				auto& newDep = currentDependencies.emplace(otherDepPair).first->second;

				if (newDep.dir != fromDep.dir)
					return TraversarInfo::DISCARDED;

				std::size_t preSize = newDep.empties.size();
				for (EmptyIndex ei : fromDep.empties) {
					auto it = infos.emptyToFromNeeded.find(ei);
					if (it != infos.emptyToFromNeeded.end())
						for (FromIndex after : it->second)
							if (fromOtherDep != after) if (
								!infos.fromDependencies.add(fromOtherDep, after))
								return TraversarInfo::DISCARDED;

					for (FromIndex after : solver->empties[ei].theseFromsNeedThis)
						if (fromOtherDep != after) if (
							!infos.fromDependencies.add(fromOtherDep, after))
							return TraversarInfo::DISCARDED;
				}
				newDep.empties.insert(fromDep.empties.begin(), fromDep.empties.end());
				changed |= wasChange |= newDep.empties.size() != preSize;

				From& othF = solver->froms[fromOtherDep];
				auto& prohibitedEmpties = prohibiteds[fromOtherDep];
				if (!fromDep.nomore && newDep.empties.size() != preSize) {
					changed |= wasChange |= setNomore(othF, newDep, prohibitedEmpties);
				}
				if (!solver->fromCanBeAssignThese(othF, newDep.dir, newDep.empties, prohibitedEmpties))
					return TraversarInfo::DISCARDED;

				addToCoords(coords, moveTo(othF.coord, newDep.dir), newDep.dir,
					[&, counter = 0u](auto& e) mutable { if (counter == newDep.empties.size()) return false;
				auto& st = solver->states(e);
				return !st.empty || !newDep.empties.count(st.index) || ++counter > 0; });
			}
			return TraversarInfo::FINAL;
		}

		template<class QueueType, class Prohibiteds>
		TraversarInfo oppositeDirectionProhibit(FromToEmptyDependencies& currentDependencies, Prohibiteds& prohibiteds, bool& wasChange,
			QueueType& coords, Empty& e, Coord fromCoord, Direction dir) {
			Direction oppositeDir = opposite(dir);
			for (Coord c = moveTo(fromCoord, dir); true; c = moveTo(c, dir)) {
				StateIndex i = solver->states(c);
				if (!i.empty) continue;
				if (i.index == e.index) break;

				Empty& checkE = solver->empties[i.index];
				auto pairPossibleOppositeDirIt = checkE.possibilities.find(oppositeDir);
				if (pairPossibleOppositeDirIt == checkE.possibilities.end())
					continue;

				bool pushed = false;
				for (auto& fromIndex : pairPossibleOppositeDirIt->second) {
					if (whereToGo(solver->froms[fromIndex].coord, e.coord) == oppositeDir// this means collapse can happen -> prohibited!!!
						&& !prohibiteds[fromIndex].count(i.index)) {
						prohibiteds[fromIndex].insert(i.index);
						auto d = currentDependencies.find(fromIndex);
						if (d != currentDependencies.end() && d->second.empties.count(i.index)) {
							return TraversarInfo::DISCARDED;
						}
						if (!pushed) {
							coords.emplace(checkE.coord);
							changed = wasChange = true;
							pushed = true;
						}
					}
				}
			}
			return TraversarInfo::FINAL;
		}

		template<class QueueType>
		TraversarInfo processCoords(FromToEmptyInformation& infos, bool& wasChange, 
			QueueType& coords, std::size_t max, bool recursive) {

			FromToEmptyDependencies& currentDependencies = infos.dependencies;
			auto& prohibiteds = infos.prohibited;
			while (!coords.empty() && max--) {
				Coord currentCoord = coords.top();
				coords.pop();

				StateIndex& i = solver->states(currentCoord);

				if (!i.empty)
					continue;

				Empty& e = solver->empties[i.index];

				if (e.fix)
					continue;

				DependencyKey additionalDepKey;
				int count = 0;
				for (auto& dirPair : e.possibilities) {
					for (auto& fromIndex : dirPair.second) {
						auto it = currentDependencies.find(fromIndex);
						if (it != currentDependencies.end() && it->second.empties.count(e.index)) {
							if (oppositeDirectionProhibit(currentDependencies, prohibiteds, wasChange, coords, e, solver->froms[fromIndex].coord, dirPair.first) == TraversarInfo::DISCARDED)
								return TraversarInfo::DISCARDED;

							count = 2;
							break;
						}
					}
					if (count)
						break;
				}

				if (!count)
					for (auto& dirPair : e.possibilities) {
						for (auto& fromIndex : dirPair.second) {
							auto it = currentDependencies.find(fromIndex);
							if (it != currentDependencies.end()) {
								if (it->second.nomore)
									continue;
								if (it->second.dir != dirPair.first)
									continue;
							}
							auto prohibitedIt = prohibiteds.find(fromIndex);
							if (prohibitedIt != prohibiteds.end() && prohibitedIt->second.count(e.index)) {
								continue;
							}

							bool dependencyCheckFailed = false;
							auto emptyNeededToIt = infos.emptyToFromNeeded.find(e.index);
							if (emptyNeededToIt != infos.emptyToFromNeeded.end())
								for (FromIndex after : emptyNeededToIt->second)
									if (infos.fromDependencies.isBefore(after, fromIndex)) {
										dependencyCheckFailed = true;
										break;
									}

							if (!dependencyCheckFailed)
								for (FromIndex after : e.theseFromsNeedThis)
									if (infos.fromDependencies.isBefore(after, fromIndex)) {
										dependencyCheckFailed = true;
										break;
									}

							if (dependencyCheckFailed)
								continue;

							if (++count == 2)
								break;

							additionalDepKey = DependencyKey{ &solver->froms[fromIndex], dirPair.first, e.index };
						}
						if (count == 2)
							break;
					}

				if (count == 0)
					return TraversarInfo::DISCARDED;

				if (count == 1) {
					auto it = toDependencies.find(additionalDepKey);
					if (it == toDependencies.end())
						return TraversarInfo::DISCARDED;


					switch (recursive ? visit(*it) : TraversarInfo::FINAL)
					{
					case TraversarInfo::NOT_YET:
					case TraversarInfo::TEMPORARY:
						break;
					case TraversarInfo::DISCARDED:
						return TraversarInfo::DISCARDED;
					case TraversarInfo::SOLUTION:
						return TraversarInfo::SOLUTION;
					case TraversarInfo::FINAL:
						if (mergeTwoDependency(infos, wasChange, coords, it->second.infos) == TraversarInfo::DISCARDED)
							return TraversarInfo::DISCARDED;
					}
				}
			}


			return TraversarInfo::FINAL;
		}

		template<class QueueType>
		void addCoordsBetweenFromAndEmpties(QueueType& to, FromIndex f, Direction dir, std::set<EmptyIndex>& es) {
			addToCoordsFromTo(to, moveTo(solver->froms[f].coord, dir), solver->empties[
				directionIndex(dir) % 2 == 0 ?
				*es.begin() :
				*es.rbegin()
			].coord, true);
		}

		template<class QueueType>
		void addDependentCoordsToQueue(QueueType& to, FromToEmptyDependencies& currentDependencies) {
			for (auto& dep : currentDependencies) {
				if (dep.second.empties.empty())
					continue;

				addCoordsBetweenFromAndEmpties(to, dep.first, dep.second.dir, dep.second.empties);
			}
		}

		const FromIndex* findEmptyBelongingDependency(FromToEmptyDependencies& currentDependencies, EmptyIndex ei) {
			Empty& e = solver->empties[ei];
			if (e.fix) {
				if (e.possibilities.empty())
					return nullptr;
				return &*e.possibilities.begin()->second.begin();
			}
			
			for (auto& dep : currentDependencies) {
				if (dep.second.empties.count(ei))
					return &dep.first;
			}

			return nullptr;
		}

		bool hasEnughFreeNumber(FromToEmptyDependencies& currentInfos, std::size_t needFreeNumber) {
			std::size_t free = 0;
			for (From& f : solver->froms) {
				auto fromDepsIt = currentInfos.find(f.index);
				free += f.length - (fromDepsIt != currentInfos.end() ? fromDepsIt->second.empties.size() : 0);
			}
			return free >= needFreeNumber;
		}

		// FINAL if OK, DISCARDED if not.
		TraversarInfo getDependenciesResult(FromToEmptyInformation& currentInfos, bool& changed) {
			bool wasChange = true;
			while (wasChange) {
				wasChange = false;
				
				for (auto& dep : currentInfos.dependencies) {
					From& f = solver->froms[dep.first];
					if (!dep.second.nomore)
						changed |= setNomore(f, dep.second, currentInfos.prohibited[dep.first]);
				}

				std::stack<Coord, std::vector<Coord>> coordsX;
				addDependentCoordsToQueue(coordsX, currentInfos.dependencies);

				if (processCoords(currentInfos, wasChange, coordsX, -1, true) == TraversarInfo::DISCARDED)
					return TraversarInfo::DISCARDED;

				std::stack<Coord, std::vector<Coord>> ({ requiredEmptyCoords }).swap(coordsX);

				if (processCoords(currentInfos, wasChange, coordsX, coordsX.size(), false) == TraversarInfo::DISCARDED)
					return TraversarInfo::DISCARDED;

				changed |= wasChange;
			}

			std::size_t notSolves = notSolveSize(currentInfos.dependencies);
			if (!notSolves)
				return TraversarInfo::SOLUTION;

			if (!hasEnughFreeNumber(currentInfos.dependencies, notSolves)) {
				return TraversarInfo::DISCARDED;
			}

			return TraversarInfo::FINAL;
		}

		std::size_t notSolveSize(FromToEmptyDependencies& deps) {
			std::set<EmptyIndex> ei;
			for (auto& p : deps)
				ei.insert(p.second.empties.begin(), p.second.empties.end());

			std::set<Coord> coordsX(requiredEmptyCoords.begin(), requiredEmptyCoords.end());
			addDependentCoordsToQueue(coordsX, deps);

			std::size_t result{};
			for (const Coord& c : coordsX) {
				StateIndex si = solver->states(c);
				if (si.empty && (!ei.count(si.index) && !solver->empties[si.index].fix)) {
					++result;
				}
			}

			return result;
		}

		TraversarInfo visit(std::pair<const DependencyKey, DependencyInfo>& depPair) {

			DependencyInfo& depInfo = depPair.second;
			TraversarInfo& mark = depInfo.mark;
			switch (mark)
			{
			case TraversarInfo::TEMPORARY:
				return TraversarInfo::FINAL; // say that it is ok
			case TraversarInfo::NOT_YET:
				break;
			default:
				return mark;
			}

			mark = TraversarInfo::TEMPORARY;

			return mark = getDependenciesResult(depInfo.infos, depInfo.changed);
		}

		void writeSolution(const FromToEmptyDependencies& dep) {
			for (auto& depPair : dep) {
				FromIndex from = depPair.first;
				From& f = solver->froms[from];
				f.required = true;

				solver->filterFromToDir(f, depPair.second.dir);
				
				for (const EmptyIndex& empty : depPair.second.empties) {
					Empty& e = solver->empties[empty];
					e.required = true;
					solver->setEmptyPossible(e, depPair.second.dir, from);

					if (e.setIsFix()) {
						solver->requiredEmptyFound(e);
					}
				}
			}
		}

		bool calculate() {
			changed = false;

			initDependencies();
			initRequiredEmpties();

			for (auto& pair : toDependencies) {
				TraversarInfo result = visit(pair);
				if (result == TraversarInfo::DISCARDED) {
					changed |= solver->removeEmptyPossible(solver->empties[pair.first.emptyIndex], pair.first.dir, pair.first.from->index);
					changed |= solver->removeFromPossible(*pair.first.from, pair.first.dir, pair.first.emptyIndex);
				}
				else if (result == TraversarInfo::SOLUTION) {
					writeSolution(pair.second.infos.dependencies);

					return true;
				}
			}

			return changed;
		}
	};

	void collectResult() {
		if (!filterPossibles())
			return;

		std::unordered_map<FromIndex, std::unordered_set<FromIndex>> afters;
		for (From& from : froms)
			if (from.required) {
				auto& dirPair = *from.possibles.begin();
				std::size_t possiblesFound = 0;
				forEachCoord < true >(moveTo(from.coord, dirPair.first), dirPair.first, [&](StateIndex si) {
					if (dirPair.second.count(si.index))
						++possiblesFound;
					else
						afters[*empties[si.index].possibilities.begin()->second.begin()].insert(from.index);
				}, [&](auto&&) { return possiblesFound < dirPair.second.size(); });
			}

		using PermanentMark = bool;
		std::map<FromIndex, PermanentMark> unmarkedNodes;

		for (From& f : froms)
			if (f.required)
				unmarkedNodes.emplace(f.index, PermanentMark{ false });
		
		auto visit = [&](FromIndex index, auto& visit) {
			auto it = unmarkedNodes.find(index);
			if (it == unmarkedNodes.end())
				return;

			it->second = PermanentMark{ true };

			From& from = froms[index];
			for (auto& after : afters[index]) {
				visit(after, visit);
			}
			unmarkedNodes.erase(index);
			result.emplace_back(ResultLine{ from.coord, from.possibles.begin()->first });
		};

		while (!unmarkedNodes.empty())
			visit(unmarkedNodes.begin()->first, visit);
		std::reverse(result.begin(), result.end());
	}

	Result solve() {
		collectResult();
		return result;
	}
};

int main(int argc, char** argv)
{
	std::stringstream ss;
	ss << (argc >= 2 ? std::ifstream(argv[1]).rdbuf() : std::cin.rdbuf());
	std::cout << Solver(State<char>::get(ss.str())).solve() << std::endl;
}
