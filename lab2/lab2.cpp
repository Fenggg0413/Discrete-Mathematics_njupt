#include <iostream>
#include <utility>
#include <set>
#include <sstream>
#include <stack>
#include <vector>
#include <algorithm>
#include <random>
#include <iterator>

#define DEBUG 0

using namespace std;

/*函数声明*/
set<pair<char, char>> process_binary_relation(const string &input);
set<char> getSet(const set<pair<char, char>> &relation);
set<char> getSet(const string &input);
set<pair<char, char>> makeRelation(const set<char> &s);
void printInfo(const set<pair<char, char>> &r);
bool isReflexive(const std::set<std::pair<char, char>> &relation, const std::set<char> &set);
bool isAntiReflexive(const std::set<std::pair<char, char>> &relation, const std::set<char> &set);
bool isSymmetric(const std::set<std::pair<char, char>> &relation);
bool isAntiSymmetric(const std::set<std::pair<char, char>> &relation);
bool isTransitive(const std::set<std::pair<char, char>> &relation);
void judgeRelation(const set<pair<char, char>> &relation, const set<char> &set);
bool isRelation(const string &input);
void warshall(const set<pair<char, char>> &relation, const set<char> &s);

void debug()
{
    set<char> s{'1', '2', '3', '4'};
    auto p = makeRelation(s);
    judgeRelation(p, s);
    warshall(p, s);
}

int main()
{
    if (DEBUG)
    {
        debug();
        return 0;
    }

    cout << "请输入具体的二元关系或者具体的集合: " << endl;
    string input;
    getline(cin, input);
    if(isRelation(input)) // 输入的是关系
    {
        auto relation = process_binary_relation(input);
        auto s = getSet(relation);
        cout << "-----------------------------\n";
        cout << "输入的关系为: " << endl;
        printInfo(relation);
        judgeRelation(relation, s);
    }
    else  //输入的是集合
    {
        auto s = getSet(input);
        auto relation = makeRelation(s);
        cout << "-----------------------------\n";
        cout << "从给定集合中随机生成的关系: " << endl;
        printInfo(relation);
        judgeRelation(relation, s);
    }
    return 0;
}

/*从输入中获取关系集合*/
set<pair<char, char>> process_binary_relation(const string &input)
{
    istringstream record(input);
    char op;
    stack<char> relationStack;
    set<pair<char, char>> r;
    while (record >> op)
    {
        if (op == '<')
        {
            relationStack.push(op);
        }
        else if (op == ',' || op == '{' || op == '}' || op ==' ')
        {
        }
        else if (op == '>')
        {
            vector<char> tmp;
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
    //printInfo(r);
    return r;
}

/*从关系获取集合*/
set<char> getSet(const set<pair<char, char>> &relation)
{
    set<char> s;
    for (auto r : relation)
    {
        s.insert(r.first);
        s.insert(r.second);
    }
    return s;
}

/*从输入获取集合*/
set<char> getSet(const string &input)
{
    set<char> set;
    for(auto ch : input)
        if(ch != ',' && ch != '{' && ch != '}' && ch != ' ')
            set.insert(ch);
    return set;
}

/*从集合中随机生成关系*/
set<pair<char, char>> makeRelation(const set<char> &s)
{
    set<pair<char, char>> relation;
    random_device rd; // 为随机数引擎获取种子
    mt19937 gen(rd());
    auto n = s.size();
    uniform_int_distribution<unsigned> dis(1, n * n);
    uniform_int_distribution<unsigned> ndis(0, n-1);
    auto num = dis(gen); // 生成关系的数量
    auto it = s.begin();

    //随机生成关系
    for (int i = 0; i < num; ++i)
    {
        auto ele1 = next(it, ndis(gen));
        auto ele2 = next(it, ndis(gen));
        if(ele1 != s.end() && ele2 != s.end())
            relation.insert(make_pair(*ele1, *ele2)); 
    }
    //printInfo(relation);
    return relation;
}

/*打印关系集合信息*/
void printInfo(const set<pair<char, char>> &r)
{
    for (auto pair : r)
    {
        cout << "<" << pair.first << "," << pair.second << "> ";
    }
    cout << endl;
}

/*判断是否满足自反性*/
bool isReflexive(const std::set<std::pair<char, char>> &relation, const std::set<char> &set)
{
    for (auto element : set)
    {
        if (relation.find(make_pair(element, element)) == relation.end())
            return false;
    }
    return true;
}

/*判断是否满足反自反性*/
bool isAntiReflexive(const std::set<std::pair<char, char>> &relation, const std::set<char> &set)
{
    for (auto element : set)
    {
        if (relation.find(make_pair(element, element)) != relation.end())
            return false;
    }
    return true;
}

/*判断是否满足对称性*/
bool isSymmetric(const std::set<std::pair<char, char>> &relation)
{
    for (auto r : relation)
    {
        if (relation.find(make_pair(r.second, r.first)) == relation.end())
            return false;
    }
    return true;
}

/*判断是否满足反对称性*/
bool isAntiSymmetric(const std::set<std::pair<char, char>> &relation)
{
    for (auto r : relation)
    {
        if (relation.find(make_pair(r.second, r.first)) != relation.end())
            return false;
    }
    return true;
}

/*判断是否满足传递性*/
bool isTransitive(const std::set<std::pair<char, char>> &relation)
{
    for (auto r1 : relation)
    {
        for (auto r2 : relation)
        {
            /*每当xRy, yRz就要有xRz*/
            if (r1.second == r2.first) // 满足xRy, yRz的情况
            {
                // 没找到xRz
                if (relation.find(make_pair(r1.first, r2.second)) == relation.end())
                    return false;
            }
        }
    }
    return true;
}

/*判断关系的性质*/
void judgeRelation(const set<pair<char, char>> &relation, const set<char> &set)
{
    auto reflexive = isReflexive(relation, set);
    auto antiReflexive = isAntiReflexive(relation, set);
    auto symmetric = isSymmetric(relation);
    auto antiSymmetric = isAntiSymmetric(relation);
    auto transitive = isTransitive(relation);

    cout << "自反性: " << (reflexive ? "满足" : "不满足") << endl;
    cout << "反自反性: " << (antiReflexive ? "满足" : "不满足") << endl;
    cout << "对称性: " << (symmetric ? "满足" : "不满足") << endl;
    cout << "反对称性: " << (antiSymmetric ? "满足" : "不满足") << endl;
    cout << "传递性: " << (transitive ? "满足" : "不满足") << endl;
}

/*判断输入是否为关系*/
bool isRelation(const string &input)
{
    //有<和>说明该输入为关系
    bool flag = find(input.begin(), input.end(), '<') != input.end() && find(input.begin(), input.end(), '>') != input.end();
    return flag;
}

/*Warshall算法*/
void warshall(const set<pair<char, char>> &relation, const set<char> &s)
{
    //TODO:完成Warshall算法
}