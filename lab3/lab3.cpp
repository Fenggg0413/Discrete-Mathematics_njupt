#include <iostream>
#include <utility>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <stack>
#include <algorithm>

#define DEBUG 0

auto getRelation(const std::string &input) -> std::set<std::pair<char, char>>;
auto getSet(const std::set<std::pair<char, char>> &relationSet) -> std::set<char>;
auto getCOV(const std::set<std::pair<char, char>> &relationSet, const std::set<char> &set_)
    -> std::set<std::pair<char, char>>;
auto lower(char c1, char c2, const std::set<std::pair<char, char>> &relation, const std::set<char> &s) -> char;
auto upper(char c1, char c2, const std::set<std::pair<char, char>> &relation, const std::set<char> &s) -> char;
auto lowerBound(const std::set<char> &s, const std::set<std::pair<char, char>> &relation)
    -> char;
auto uppperBound(const std::set<char> &s, const std::set<std::pair<char, char>> &relation)
    -> char;
auto printInfo(const std::set<std::pair<char, char>> &relation) -> void;
auto isBoundedLattice(const std::set<char> &s, const std::set<std::pair<char, char>> &relation) -> bool;
auto isComplementedLatticeconst(std::set<char> &s, const std::set<std::pair<char, char>> &relation) -> bool;

void debug()
{
    std::cout << "debug\n"
              << std::endl;
    std::set<std::pair<char, char>> relation_set{
        {'a', 'a'},
        {'a', 'b'},
        {'a', 'c'},
        {'a', 'd'},
        {'a', 'e'},
        {'b', 'b'},
        {'b', 'c'},
        {'b', 'e'},
        {'c', 'c'},
        {'c', 'e'},
        {'d', 'd'},
        {'d', 'd'},
        {'d', 'e'},
        {'e', 'e'}};
    printInfo(relation_set);
    auto s = getSet(relation_set);
    for (auto x : s)
        std::cout << x << " ";
    std::cout << std::endl;
    auto cov = getCOV(relation_set, s);
    printInfo(cov);
    std::cout << uppperBound(s, relation_set) << std::endl;
    std::cout << lowerBound(s, relation_set) << std::endl;
    std::cout << "b union d = " << upper('b', 'd', relation_set, s) << std::endl;
    std::cout << "a union b = " << upper('a', 'b', relation_set, s) << std::endl;
    std::cout << "a union e = " << upper('a', 'e', relation_set, s) << std::endl;
    std::cout << "b intersect d = " << lower('b', 'd', relation_set, s) << std::endl;
    std::cout << "c intersect e = " << lower('c', 'e', relation_set, s) << std::endl;
    std::cout << "c intersect d = " << lower('c', 'd', relation_set, s) << std::endl;
    if(isComplementedLatticeconst(s,relation_set))
        std::cout << "此格为有补格\n";
}

int main()
{
    if (DEBUG)
    {
        debug();
        return 0;
    }
    std::cout << "请输入偏序关系: \n";
    std::string input;
    std::getline(std::cin, input);
    auto relation_set = getRelation(input);
    auto s = getSet(relation_set);
    auto cov = getCOV(relation_set, s);
    std::cout << "盖住关系: ";
    printInfo(cov);
    if(isComplementedLatticeconst(s, relation_set))
        std::cout << "此格是有补格" << std::endl;
    else
        std::cout << "此格不是有补格" << std::endl;
    return 0;
}

/*从输入中获取关系集合*/
auto getRelation(const std::string &input) -> std::set<std::pair<char, char>>
{
    std::istringstream record(input);
    char op;
    std::stack<char> relationStack;
    std::set<std::pair<char, char>> r;
    while (record >> op)
    {
        if (op == '<')
        {
            relationStack.push(op);
        }
        else if (op == ',' || op == '{' || op == '}' || op == ' ')
        {
        }
        else if (op == '>')
        {
            std::vector<char> tmp;
            while (relationStack.top() != '<')
            {
                tmp.push_back(relationStack.top());
                relationStack.pop();
            }
            relationStack.pop();
            r.insert({tmp.back(), tmp.front()});
        }
        else
        {
            /*op是元素的情况*/
            relationStack.push(op);
        }
    }
    // printInfo(r);
    return r;
}

/* 获取元素集合 */
auto getSet(const std::set<std::pair<char, char>> &relationSet) -> std::set<char>
{
    std::set<char> set_;
    for (auto [x, y] : relationSet)
    {
        set_.insert(x);
        set_.insert(y);
    }
    return set_;
}

/*获取盖住关系*/
auto getCOV(const std::set<std::pair<char, char>> &relationSet, const std::set<char> &set_)
    -> std::set<std::pair<char, char>>
{
    std::set<std::pair<char, char>> cov;
    for (auto [x, y] : relationSet)
    {
        if (x != y)
        {
            bool flag = true; // 判断<x, y>是否为盖住关系的标志
            for (auto z : set_)
            {
                if (relationSet.find(std::make_pair(x, z)) != relationSet.end() && relationSet.find(std::make_pair(z, y)) != relationSet.end() && z != x && z != y)
                {
                    flag = false; // 如果存在元素z满足偏序关系<x,z>,<z,y>则<x,y>不是盖住关系}
                    break;
                }
            }
            if (flag)
                cov.insert({x, y});
        }
    }
    return cov;
}

/*交运算*/
auto lower(char c1, char c2, const std::set<std::pair<char, char>> &relation, const std::set<char> &s) -> char
{
    if (relation.find({c1, c2}) != relation.end())
        return c1; // c1是下界
    // 找到一个ret元素使得同时满足<ret, c1>, <ret, c2>,将结果存于一个最下界集,该集合应为原集合的子集
    std::set<char> lowerBounds;
    for (auto ret : s)
    {
        if (relation.find({ret, c1}) != relation.end() && relation.find({ret, c2}) != relation.end())
            lowerBounds.insert(ret);
    }
    if (lowerBounds.empty())
        return '\0';
    return uppperBound(lowerBounds, relation);
}

/*并运算*/
auto upper(char c1, char c2, const std::set<std::pair<char, char>> &relation, const std::set<char> &s) -> char
{
    if (relation.find({c1, c2}) != relation.end())
        return c2; // c2是上界

    // 找到一个ret元素使得同时满足<c1, ret>, <c2, ret>,将结果存于一个最上界集,该集合应为原集合的子集
    std::set<char> upperBounds;
    for (auto ret : s)
    {
        if (relation.find({c1, ret}) != relation.end() && relation.find({c2, ret}) != relation.end())
            upperBounds.insert(ret);
    }
    if (upperBounds.empty())
        return '\0'; // 该并运算不存在结果
    return lowerBound(upperBounds, relation);
}

/*找到一个集合的最下界*/
auto lowerBound(const std::set<char> &s, const std::set<std::pair<char, char>> &relation)
    -> char
{
    char min = *s.begin();
    for (auto ch : s)
    {
        if (relation.find({ch, min}) != relation.end())
            min = ch;
    }
    return min;
}

/*找到一个集合的最上界*/
auto uppperBound(const std::set<char> &s, const std::set<std::pair<char, char>> &relation)
    -> char
{
    char max = *s.begin();
    for (auto ch : s)
    {
        if (relation.find({max, ch}) != relation.end())
            max = ch;
    }
    return max;
}

auto printInfo(const std::set<std::pair<char, char>> &relation) -> void
{
    for (auto [x, y] : relation)
    {
        std::cout << "<" << x << "," << y << "> ";
    }
    std::cout << std::endl;
}

/*判断是否是有界格*/
auto isBoundedLattice(const std::set<char> &s, const std::set<std::pair<char, char>> &relation) -> bool
{
    auto max = uppperBound(s, relation);
    auto min = lowerBound(s, relation);
    if (max != '\0' && min != '\0')
        return true;
    return false;
}

/*判断是否为有补格*/
auto isComplementedLatticeconst(std::set<char> &s, const std::set<std::pair<char, char>> &relation) -> bool
{
    if (!isBoundedLattice(s, relation))
        return false;
    auto _1 = uppperBound(s, relation);
    auto _0 = lowerBound(s, relation);
    std::map<char, bool> complementeds; // 对每个元素是否有补元建立映射
    for (auto a : s)
    {
        for (auto b : s)
            if (upper(a, b, relation, s) == _1 && lower(a, b, relation, s) == _0)
                complementeds[a] = true;
    }
    for(auto [ch, boolean] : complementeds)
        if(!boolean)
            return false;
    return true;
}