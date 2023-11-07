/*lab2: 输入可以是具体的二元关系，也可以给定一个具体的集合，随机生成一个二元关系，根据二元关系性质及对应的判定定理, 并求出该二元关系的传递闭包.*/
#include <iostream>
#include <utility>
#include <set>
#include <sstream>
#include <stack>
#include <vector>
#include <algorithm>
#include <random>
#include <iterator>
#include <map>

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
vector<vector<int>> warshall(const set<pair<char, char>> &relation, const set<char> &s);
vector<vector<int>> relationalMatrix(const set<pair<char, char>> &relation, const set<char> &s);
set<pair<char, char>>
transitiveClosure(const set<pair<char, char>> &relation, const set<char> &s);

void debug()
{
    set<char> s{'1', '2', '3', '4'};
    auto r = makeRelation(s);
    printInfo(r);
    judgeRelation(r, s);
    auto mat = relationalMatrix(r, s);
    for (auto row : mat)
    {
        for (auto val : row)
            cout << val << " ";
        cout << '\n';
    }
    cout << endl;
    auto mtr = warshall(r, s);
    for (auto row : mtr)
    {
        for (auto val : row)
            cout << val << " ";
        cout << '\n';
    }
    cout << endl;
    auto tr = transitiveClosure(r, s);
    judgeRelation(tr, s);
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
    if (isRelation(input)) // 输入的是关系
    {
        auto relation = process_binary_relation(input);
        auto s = getSet(relation);
        cout << "-----------------------------\n";
        cout << "输入的关系为: " << endl;
        printInfo(relation);
        judgeRelation(relation, s);
        cout << "-----------------------------\n";
        cout << "该二元关系的传递闭包为: \n";
        auto mtr = warshall(relation, s);
        for (auto row : mtr)
        {
            for (auto val : row)
                cout << val << " ";
            cout << '\n';
        }
        auto tr = transitiveClosure(relation, s);
        printInfo(tr);
    }
    else // 输入的是集合
    {
        auto s = getSet(input);
        auto relation = makeRelation(s);
        cout << "-----------------------------\n";
        cout << "从给定集合中随机生成的关系: " << endl;
        printInfo(relation);
        judgeRelation(relation, s);
        cout << "-----------------------------\n";
        cout << "该二元关系的传递闭包为: \n";
        auto mtr = warshall(relation, s);
        for (auto row : mtr)
        {
            for (auto val : row)
                cout << val << " ";
            cout << '\n';
        }
        auto tr = transitiveClosure(relation, s);
        printInfo(tr);
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
        else if (op == ',' || op == '{' || op == '}' || op == ' ')
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
    // printInfo(r);
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
    for (auto ch : input)
        if (ch != ',' && ch != '{' && ch != '}' && ch != ' ')
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
    uniform_int_distribution<unsigned> ndis(0, n - 1);
    auto num = dis(gen); // 生成关系的数量
    auto it = s.begin();

    // 随机生成关系
    for (int i = 0; i < num; ++i)
    {
        auto ele1 = next(it, ndis(gen));
        auto ele2 = next(it, ndis(gen));
        if (ele1 != s.end() && ele2 != s.end())
            relation.insert(make_pair(*ele1, *ele2));
    }
    // printInfo(relation);
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
    // 有<和>说明该输入为关系
    bool flag = find(input.begin(), input.end(), '<') != input.end() && find(input.begin(), input.end(), '>') != input.end();
    return flag;
}

/*Warshall算法求传递闭包关系矩阵M(t(R))*/
vector<vector<int>> warshall(const set<pair<char, char>> &relation, const set<char> &s)
{
    // 获取关系矩阵
    auto mat = relationalMatrix(relation, s);
    int n = s.size(); // 该矩阵的阶数
    // Warshall算法
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (mat[j][i] == 1)
                for (int k = 0; k < n; ++k)
                {
                    mat[j][k] += mat[i][k];
                    // 关系矩阵不应该出现大于1的值
                    if(mat[j][k] > 1)
                        mat[j][k] = 1;
                }
    // 返回通过Warshall算法求出的传递闭包关系矩阵
    return std::move(mat);
}

/*获取关系矩阵*/
vector<vector<int>> relationalMatrix(const set<pair<char, char>> &relation, const set<char> &s)
{
    vector<vector<int>> mat;
    map<char, int> m;
    int count = 0;
    for (auto ele : s)
    {
        m[ele] = count++; // 建立集合元素对应矩阵下标的映射
        vector<int> row;
        for (auto ele : s)
            row.push_back(0);
        mat.push_back(row);
    }
    // 对于每一对关系将其对应矩阵的值变为1
    for (auto r : relation)
        mat[m[r.first]][m[r.second]] = 1;
    return std::move(mat); // 返回建立的关系矩阵
}

/*求给定二元关系的传递闭包*/
set<pair<char, char>> 
transitiveClosure(const set<pair<char, char>> &relation, const set<char> &s)
{
    map<int, char> m;
    int count = 0;
    set<pair<char, char>> tr; //传递闭包
    for (auto ele : s)
        m[count++] = ele; // 建立矩阵下标对应集合元素的映射
    auto mtr = warshall(relation, s); // 获取传递闭包关系矩阵
    auto n = s.size(); // 矩阵的阶数

    // 遍历矩阵找到值为1的元素, 建立下标对应的关系
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if(mtr[i][j] == 1)
                tr.insert(make_pair(m[i], m[j]));
    return tr;
}