#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <bitset>

#ifdef _MSC_VER
#	include <ctime>
#else
#	include <sys/time.h>
#endif


#define RUNNING_TIME 30.0	// sec
#define RANDOM_SEED 123456789


std::string mainString =
"`+syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyo:\n"
"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy/\n"
"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy+\n"
"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy+\n"
"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy+                                       .-:::::-.`\n"
"syyyyyyyy+      `/yyyyyyyyyyyyy`      :yyyyyys:             yyyyyyy/      ./oyyyyyyyyyyys+:`\n"
"syyyyyyyy+        `+yyyyyyyyyyy`      :yyyyyyyys-           yyyyyyy/   `/syyyyyyyyyyyyyyyyyyo-\n"
"syyyyyyyy+          -syyyyyyyyy`      :yyyyyyyyyy+`         yyyyyyy/  :syyyyyyyyyyyyyyyyyyyyyyo`\n"
"syyyyyyyy+            :syyyyyyy`      :yyyyyyyyyyyy/`       yyyyyyy/ +yyyyyyys/-`   `.:oyyyyyyys.\n"
"syyyyyyyy+             `+yyyyyy`      :yyyyyyyyyyyyys-      yyyyyyy//yyyyyyy:           `////////\n"
"syyyyyyyy+               .oyyyy`      :yyyyyyyyyyyyyyyo.    yyyyyyyoyyyyyyy.     `````````````````\n"
"syyyyyyyy+       :         -syy`      :yyyyyyy/yyyyyyyyy+`  yyyyyyyyyyyyyy/      syyyyyyyyyyyyyyyy\n"
"syyyyyyyy+       ys-         /y`      :yyyyyys .oyyyyyyyyy: yyyyyyyyyyyyyy/      syyyyyyyyyyyyyyyy\n"
"syyyyyyyy+       yyyo.        `       :yyyyyys   -syyyyyyyysyyyyyyyoyyyyyys`     syyyyyyyyyyyyyyyo\n"
"syyyyyyyy+       yyyyy+`              :yyyyyys     :syyyyyyyyyyyyyy/oyyyyyys.    ::::::::syyyyyyy-\n"
"syyyyyyyy+       yyyyyyy/             :yyyyyys      `/yyyyyyyyyyyyy/`oyyyyyyy+-`      .:syyyyyyy:\n"
"syyyyyyyy+       yyyyyyyys-           :yyyyyys        `+yyyyyyyyyyy/ `+yyyyyyyyysooosyyyyyyyyys-\n"
"syyyyyyyy+       yyyyyyyyyyo.         :yyyyyys          .oyyyyyyyyy/   .oyyyyyyyyyyyyyyyyyyyy/`\n"
"syyyyyyyy+       yyyyyyyyyyyy+`       :yyyyyys            :syyyyyyy/     ./oyyyyyyyyyyyyyy+:\n"
"syyyyyyyo.       .............`       .:::::::             `:::::::.        `-:/++oo++/-.\n"
"syyyyyo.\n"
"syyy+.\n"
"+s+.\n\n";



// #########################################################################
// #                             R A N D O M                               #
// #########################################################################
struct XorShiftRand
{
	unsigned int x, y, z, w;

	XorShiftRand(int seed)
	{
		init(seed);
	}

	void init(int seed)
	{
		x = seed;
		y = 362436069;
		z = 521288629;
		w = 88675123;
	}

	unsigned int next_int(void)
	{
		unsigned int t = x ^ (x << 11);
		x = y;
		y = z;
		z = w;
		return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));// & 0x7FFFFFFF;
	}

	template<class Iter>
	void random_shuffle(Iter beg, Iter end)
	{
		int n = end - beg;
		for (int i = 0; i + 1<n; ++i)
		{
			int j = i + next_int() % (n - i);
			std::swap(*(beg + i), *(beg + j));
		}
	}
};
XorShiftRand xsr(RANDOM_SEED);

#define srand(seed) xsr.init(seed)
#define rand xsr.next_int

double frand()
{
	return (rand() + 0.5) * (1.0 / 4294967296.0);
}


// #########################################################################
// #                              T I M E                                  #
// #########################################################################
class TimeLimit
{
public:

	double t0;
	double dt_limit;


	TimeLimit(double limit)
	{
		t0 = time();
		dt_limit = limit;
	}

	double time() const
	{
#ifndef _MSC_VER
		timeval tv;
		gettimeofday(&tv, 0);
		return tv.tv_sec + 1e-6*tv.tv_usec;
#else
		return double(clock()) / CLOCKS_PER_SEC;
#endif
	}

	double dt() const
	{
		return time() - t0;
	}

	bool go() const
	{
		return dt() < dt_limit;
	}

	double limit() const
	{
		return dt_limit;
	}

	double left() const
	{
		return limit() - dt();
	}
};


// #########################################################################
// #              S I M U L A T E D   A N N E A L I N G                    #
// #########################################################################
struct OptManagerSA
{
	double q0, q, dq;
	int step;
	double scurr;
	double tc;
	TimeLimit tl;


	OptManagerSA(double q1, double q2, double time_left)
		: tl(time_left)
	{
		step = 0;
		q = q0 = q1;
		dq = q2 / q0;
		tc = tl.dt();
	}

	bool accept(double newscore)
	{
		if (newscore <= scurr + 1E-9)
		{
			return true;
		}

		if ((step & 0xFF) == 0)
		{
			tc = tl.dt();
			double t = tc / (tl.limit());
			q = q0 * pow(dq, t);
		}

		double x = (scurr - newscore) / q;
		double y;
		if (-0.1 < x)
		{
			y = 1 + x;
		}
		else
		{
			y = exp(x);
		}

		bool ok = (frand() < y);
		return ok;
	}

	void next_step(double newscore)
	{
		scurr = newscore;
		++step;
	}

	bool go()
	{
		//if((step&0xFF) == 0)
		return tl.go();
		//else
		//	return true;
	}

	double timeLeft()
	{
		return tl.left();
	}
};


// #########################################################################
// #                          H U F F M A N N                              #
// #########################################################################
class Huffmann
{
public:

	struct Node
	{
		char ch;
		int frequency;
		Node *left, *right;

		~Node()
		{
			if (left)
			{
				delete left;
			}

			if (right)
			{
				delete right;
			}
		}
	};

	struct comp
	{
		bool operator()(Node* l, Node* r)
		{
			return l->frequency > r->frequency;
		}
	};


	std::unordered_map<char, std::string> huffmannCodes;
	std::string encodedString;
	Node* root;


	Huffmann(std::string s)
	{
		build(s);
	}


	~Huffmann()
	{
		// todo: delete nodes (prevent leaks)
		delete root;
	}


	Node* getNode(char ch, int freq, Node* left, Node* right)
	{
		Node* node = new Node();

		node->ch = ch;
		node->frequency = freq;
		node->left = left;
		node->right = right;

		return node;
	}

	

	void encode(Node* root, std::string str)
	{
		if (root == nullptr)
		{
			return;
		}

		if (!root->left && !root->right)
		{
			huffmannCodes[root->ch] = str;
		}

		encode(root->left, str + "0");
		encode(root->right, str + "1");
	}

	void decode(Node* root, int& index, std::string str)
	{
		if (root == nullptr)
		{
			return;
		}

		if (!root->left && !root->right)
		{
			std::cout << root->ch;
			return;
		}

		index++;

		if (str[index] == '0')
		{
			decode(root->left, index, str);
		}
		else
		{
			decode(root->right, index, str);
		}
	}

	void build(std::string text)
	{
		std::unordered_map<char, int> freq;
		for (char ch : text)
		{
			freq[ch]++;
		}

		// priority queue to store nodes (we will merge the first 2 every time)
		std::priority_queue < Node*, std::vector<Node*>, comp> queue;

		for (auto pair : freq)
		{
			queue.push(getNode(pair.first, pair.second, nullptr, nullptr));
		}

		
		while (queue.size() > 1)
		{
			// remove first 2 nodes
			Node* left = queue.top();
			queue.pop();

			Node* right = queue.top();
			queue.pop();

			int sum = left->frequency + right->frequency;
			queue.push(getNode('\0', sum, left, right));
		}

		root = queue.top();

		// calculate huffmann code for the built tree
		huffmannCodes.clear();
		encode(root, "");

		//std::cout << "Encoded string:" << std::endl;
		encodedString = "";
		for (char ch : text)
		{
			encodedString += huffmannCodes[ch];
		}
	}
};


// #########################################################################
// #                             B A S E 6 4                               #
// #########################################################################
class Base64
{
public:

	std::string encode(const std::string& str)
	{
		std::string res;

		for (int i = 0; i < str.size() / 6; ++i)
		{
			int n = 0;
			for (int j = 0; j < 6; ++j)
			{
				if (str[i * 6 + j] == '1')
				{
					n |= 1 << (5 - j);
				}
			}
			res += char(35 + n);
		}

		return res;
	}

	std::string decode(std::string str, const std::unordered_map<std::string, std::string>& map)
	{
		std::string res;

		std::string searchStr;
		for (char ch : str)
		{
			unsigned char n = ch - 35;
			for (int j = 0; j < 6; ++j)
			{
				if (n & (1 << (5 - j)))
				{
					searchStr += '1';
				}
				else
				{
					searchStr += '0';
				}

				if (map.count(searchStr))
				{
					res += map.at(searchStr);
					searchStr = "";
				}
			}
		}

		return res;
	}
};


void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	while (str.find(from) != std::string::npos)
	{
		str.replace(str.find(from), from.length(), to);
	}
}


int estimateCodeSize_base64(const std::string& base64string, const std::vector<std::string>& replaces)
{
	int total = 0;

	// #include <bits/stdc++.h>
	total += 25;

	// int l,f,n;
	total += 10;

	// int main(){
	total += 11;

	// std::string s="...",
	total += 17;

	// encoded string
	total += base64string.size();

	// backslashes count twice
	total += std::count(base64string.begin(), base64string.end(), '\\');

	// v[16]={...};
	total += 9;

	// values that got encoded (in order)
	for (const std::string& item : replaces)
	{
		// "...",
		total += 3;
		total += item.size();

		// backslashes count twice
		total += std::count(item.begin(), item.end(), '\\');
	}

	// -1 for extra comma
	total--;

	// decoding base64 string using the lookup table provided above and writing to stdout
	total += 83;

	// } (end of main)
	total += 1;

	return total;
}

std::string getCppCode_base64(const std::string& base64string, const std::vector<std::string>& replaces)
{
	std::string str;

	// #include <bits/stdc++.h>
	str += "#include <bits/stdc++.h>\n";
	str += "int l,f,n;";

	str += "int main(){";

	str += "std::string s=\"";

	std::string tempBase64String = base64string;
	replaceAll(tempBase64String, "\\", "z");
	replaceAll(tempBase64String, "z", "\\\\");
	str += tempBase64String;

	str += "\",v[";
	str += std::to_string(replaces.size());
	str += "]={";

	std::string from;
	from.push_back('\n');
	std::string to = "\\n";

	for (int i = 0; i < replaces.size(); ++i)
	{
		if (i)
		{
			str += ",";
		}

		str += "\"";
		std::string temp = replaces[i];
		replaceAll(temp, from, to);
		str += temp;
		str += "\"";
	}

	str += "};";

	str += "for(char c:s){n=c-35;std::cout<<(f?v[((l&3)<<2)|(n>>4)]+v[n&15]:v[n>>2]);f=~f;l=n;}";

	str += "}";

	return str;
}


int getNumberOfOccurrences(const std::string& str, const std::string& substr)
{
	int count = 0;
	size_t pos = 0;
	while ((pos = str.find(substr, pos)) != std::string::npos)
	{
		++count;
		++pos;
	}
	return count;
}

std::map<std::string, int> collectSubstringsOccurringMultipleTimes(const std::string& str, int minCount, int saveCount = 1)
{
	std::map<std::string, int> map;

	for (int length = 2; length <= 40; ++length)
	{
		for (int i = 0; i < str.size() - length + 1; ++i)
		{
			std::string substr = str.substr(i, length);

			if (!map.count(substr))
			{
				int numOfOccurrences = getNumberOfOccurrences(str, substr);
				map[substr] = numOfOccurrences;
			}
		}
	}

	std::map<std::string, int> filteredMap;
	for (const auto& item : map)
	{
		//if (item.second >= minCount)
		if ((item.second - 1) * (item.first.size() - 1) > saveCount)
		{
			filteredMap[item.first] = item.second;
		}
	}

	return filteredMap;
}


int generateMain(const std::vector<std::string>& replaceVec)
{
	// what to replace
	std::map<char, std::string> replaceMap;
	for (int i = 0; i < replaceVec.size(); ++i)
	{
		replaceMap['a' + i] = replaceVec[i];
	}

	// replace those in copy of original string
	std::string str = mainString;
	for (const auto item : replaceMap)
	{
		std::string s;
		s.push_back(item.first);
		replaceAll(str, item.second, s);
	}

	// collect all letters with ids
	std::map<char, std::string> charMap;
	std::vector<std::string> decodeWordList;
	int id = 0;
	for (char ch : str)
	{
		if (!charMap.count(ch))
		{
			charMap[ch] = std::bitset<4>(id).to_string();
			id++;
			
			// convert char back to original string
			if (replaceMap.count(ch))
			{
				decodeWordList.push_back(replaceMap[ch]);
			}
			// char decoded as itself
			else
			{
				std::string s;
				s.push_back(ch);
				decodeWordList.push_back(s);
			}
		}
	}

	// build string to encode
	std::string strToEncode;
	for (char ch : str)
	{
		strToEncode += charMap[ch];
	}

	// huffmann coding
	// Huffmann huffmann(strToEncode);

	// encode string
	Base64 base64;
	std::string encodedString = base64.encode(strToEncode);	// huffmann.encodedString

	// estimate code size
	int estimatedCodeSize = estimateCodeSize_base64(encodedString, decodeWordList);

	return estimatedCodeSize;
}


std::string generateCppCode(const std::vector<std::string>& replaceVec)
{
	// what to replace
	std::map<char, std::string> replaceMap;
	for (int i = 0; i < replaceVec.size(); ++i)
	{
		replaceMap['a' + i] = replaceVec[i];
	}

	// replace those in copy of original string
	std::string str = mainString;
	for (const auto item : replaceMap)
	{
		std::string s;
		s.push_back(item.first);
		replaceAll(str, item.second, s);
	}

	// collect all letters with ids
	std::map<char, std::string> charMap;
	std::vector<std::string> decodeWordList;
	int id = 0;
	for (char ch : str)
	{
		if (!charMap.count(ch))
		{
			charMap[ch] = std::bitset<4>(id).to_string();
			id++;

			// convert char back to original string
			if (replaceMap.count(ch))
			{
				decodeWordList.push_back(replaceMap[ch]);
			}
			// char decoded as itself
			else
			{
				std::string s;
				s.push_back(ch);
				decodeWordList.push_back(s);
			}
		}
	}

	// build string to encode
	std::string strToEncode;
	for (char ch : str)
	{
		strToEncode += charMap[ch];
	}

	// huffmann coding
	// Huffmann huffmann(strToEncode);

	// encode string
	Base64 base64;
	std::string encodedString = base64.encode(strToEncode);	// huffmann.encodedString

	return getCppCode_base64(encodedString, decodeWordList);
}


void simulatedAnnealing()
{
	// generate all substrings
	std::map<std::string, int> occurrenceMap = collectSubstringsOccurringMultipleTimes(mainString, 4, 30);

	std::vector<std::string> substrings;
	for (const auto& item : occurrenceMap)
	{
		substrings.push_back(item.first);
	}

	// create 5 indices randomly
	int indexes[5] = { 0, 1, 2, 3, 4 };

	// score
	int mainCodeSize = 1e9;

	// SA (jump_begin, jump_end, time_limit)
	OptManagerSA sa(8, 1, RUNNING_TIME);
	int counter = 0;
	while (sa.go())
	{
		sa.next_step(mainCodeSize);

		// choose 1 of the indices randomly
		int randomIndex = rand() % 5;
		int originalIndex = indexes[randomIndex];

		// try out 15 random values for that index
		int bestIndex = indexes[randomIndex];
		int bestCodeSize = 1e9;
		for (int i = 0; i < 50; ++i)
		{
			int candidateIndex = rand() % substrings.size();
			indexes[randomIndex] = candidateIndex;

			// generate those codes, store the best
			int currentCodeSize = generateMain({ substrings[indexes[0]], substrings[indexes[1]], substrings[indexes[2]],
				substrings[indexes[3]], substrings[indexes[4]] });
			if (currentCodeSize < bestCodeSize)
			{
				bestCodeSize = currentCodeSize;
				bestIndex = candidateIndex;
			}

			counter++;
		}

		if (sa.accept(bestCodeSize))
		{
			indexes[randomIndex] = bestIndex;
			mainCodeSize = bestCodeSize;
			std::cout << "Code size: " << mainCodeSize << ", time left: " << sa.tl.left() << std::endl;
		}
		else
		{
			indexes[randomIndex] = originalIndex;
		}
	}

	std::cout << "Total evaluated states: " << counter << std::endl;

	std::cout << generateCppCode({ substrings[indexes[0]], substrings[indexes[1]], substrings[indexes[2]],
		substrings[indexes[3]], substrings[indexes[4]] }) << std::endl;
}


int main()
{
	simulatedAnnealing();

	std::system("PAUSE");
	return 0;
}