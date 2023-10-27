/*lab1: 输入任意命题公式输出他的真值表、主析取范式、主合取范式*/
#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <cmath>
#include <algorithm>
#include <set>
#include <sstream>
#include <stack>
#include <cctype>
#include <map>

using namespace std;

// 函数声明
bool evaluateFormula(const string &, const vector<bool> &, const set<string> &);
string getDNF(const vector<int> &, const vector<vector<bool>> &, const set<string> &);
string getCNF(const vector<int> &, const vector<vector<bool>> &, const set<string> &);
set<string> getPropVar(const string &);
bool isOperator(string);
int getPrecedence(string);
string infixToPostfix(const string &);
int disjunct(int, int);
int condition(int, int);
int dualCondition(int, int);

// PS: 否定!, 合取*, 析取+, 条件->, 双条件<->
// 优先级顺序!, *, +, ->, <->
int main()
{
    // 输入命题公式
    string formula;
    cout << "请输入命题公式: ";
    getline(cin, formula);

    // 获得命题变元
    auto propVar = getPropVar(formula);
    int numVariables = propVar.size();
    cout << "test -- numVariables: " << numVariables << endl;
    cout << "test -- infix to postfix: " << infixToPostfix(formula) << endl;

    // 生成真值表
    vector<vector<bool>> truthValues;
    // n个命题变元对应2*n个取值的可能性
    for (int i = 0; i < pow(2, numVariables); ++i)
    {
        // 将i转变为8位二进制形式, 低位在前
        bitset<8> binary(i);
        vector<bool> row;
        for (int j = numVariables - 1; j >= 0; --j)
        {
            // 从高位存入命题变元对应的真值指派
            row.push_back(binary[j]);
        }
        truthValues.push_back(row);
    }

    // 输出真值表头
    for (auto element : propVar)
        cout << element << "\t";
    cout << formula << endl;
    // 输出真值表
    auto postfix = infixToPostfix(formula); // 获得后缀表达式
    int count = 0;
    vector<int> ent; // 该数组用于记录真值为T对应的truthValues下标
    vector<int> enf; // 该数组用于记录真值为F对应的truthValues下标
    for (const auto &row : truthValues)
    {
        for (bool value : row)
        {
            cout << value << "\t";
        }
        auto res = evaluateFormula(postfix, row, propVar);
        cout << res << endl;
        if (res)
            ent.push_back(count);
        else
            enf.push_back(count);
        ++count;
    }
    cout << "主析取范式: " << getDNF(ent, truthValues, propVar) << endl;
    cout << "主合取范式: " << getCNF(enf, truthValues, propVar) << endl;
    return 0;
}

// 判断字符是否为运算符
bool isOperator(string c)
{
    return (c == "+" || c == "*" || c == "!" || c == "->" || c == "<->");
}

// 获取运算符的优先级
int getPrecedence(string op)
{
    if (op == "!")
        return 5;
    else if (op == "*")
        return 4;
    else if (op == "+")
        return 3;
    else if (op == "->")
        return 2;
    else if (op == "<->")
        return 1;
    return 0;
}

// 将中缀表达式转换为后缀表达式
string infixToPostfix(const string &infix)
{
    string postfix = "";
    stack<string> operatorStack;
    istringstream record(infix);
    string c;
    while (record >> c)
    {
        if (c == "(")
        {
            // 如果是左括号，将其推入栈
            operatorStack.push(c);
        }
        else if (c == ")")
        {
            // 如果是右括号，将栈中的运算符弹出，直到遇到左括号
            while (!operatorStack.empty() && operatorStack.top() != "(")
            {
                postfix += operatorStack.top();
                postfix += " ";
                operatorStack.pop();
            }
            // 弹出左括号
            operatorStack.pop();
        }
        else if (isOperator(c))
        {
            // 如果是运算符
            while (!operatorStack.empty() && getPrecedence(operatorStack.top()) >= getPrecedence(c))
            {
                postfix += operatorStack.top();
                postfix += " ";
                operatorStack.pop();
            }
            // 将当前运算符推入栈
            operatorStack.push(c);
        }
        else
        {
            // 如果是数字或字母，直接添加到后缀表达式
            postfix += c;
            postfix += " ";
        }
    }

    // 将栈中剩余的运算符添加到后缀表达式
    while (!operatorStack.empty())
    {
        postfix += operatorStack.top();
        postfix += " ";
        operatorStack.pop();
    }

    return postfix;
}

// 计算命题公式的真值
bool evaluateFormula(const string &formula, const vector<bool> &truthValues, const set<string> &propVar)
{
    // 建立命题变元对应真值的映射
    map<string, bool> m;
    int i = 0;
    for (auto element : propVar)
    {
        m[element] = truthValues[i++];
    }

    istringstream record(formula);
    string element;
    stack<int> calcStack;
    while (record >> element)
    {
        if (!isOperator(element))
        {
            // 如果不是联结词
            calcStack.push(m[element]);
        }
        else
        {
            if (element == "!") // 否定
            {
                auto val = calcStack.top();
                calcStack.pop();
                // 取反
                val = (val == 0 ? 1 : 0);
                calcStack.push(val);
            }
            else if (element == "*") // 合取
            {
                auto val2 = calcStack.top();
                calcStack.pop();
                auto val1 = calcStack.top();
                calcStack.pop();
                auto val = val1 * val2;
                calcStack.push(val);
            }
            else if (element == "+") // 析取
            {
                auto val2 = calcStack.top();
                calcStack.pop();
                auto val1 = calcStack.top();
                calcStack.pop();
                auto val = disjunct(val1, val2);
                calcStack.push(val);
            }
            else if (element == "->") // 条件
            {
                auto val2 = calcStack.top();
                calcStack.pop();
                auto val1 = calcStack.top();
                calcStack.pop();
                auto val = condition(val1, val2);
                calcStack.push(val);
            }
            else if (element == "<->") // 双条件
            {
                auto val2 = calcStack.top();
                calcStack.pop();
                auto val1 = calcStack.top();
                calcStack.pop();
                auto val = dualCondition(val1, val2);
                calcStack.push(val);
            }
        }
    }
    // 返回计算结果
    auto res = calcStack.top();
    return res;
}

// 计算析取联结词
int disjunct(int val1, int val2)
{
    if (val1 == 0 && val2 == 0)
        return 0;
    return 1;
}

// 计算条件联结词
int condition(int val1, int val2)
{
    if (val1 == 1 && val2 == 0)
        return 0;
    return 1;
}

// 计算双条件联结词
int dualCondition(int val1, int val2)
{
    if (val1 == val2)
        return 1;
    return 0;
}
// 计算主析取范式
string getDNF(const vector<int> &en, const vector<vector<bool>> &truthValues, const set<string> &propVar)
{
    string dnf = "";
    for (auto index : en) // 对于每一个真值为T的指派对应的小项
    {
        auto miniterm = truthValues[index];
        auto iter = propVar.begin();
        dnf += "(";
        for (auto val : miniterm)
        {
            string element = *iter;
            ++iter;
            if (val) // 真值为1的元素取本身, 真值为0的元素取反
            {
                dnf += element;
            }
            else
            {
                dnf += "!";
                dnf += element;
            }
            if (iter != propVar.end())
                dnf += " * ";
        }
        dnf += ")";
        if (index != en.back())
            dnf += " + ";
    }
    return dnf;
}

// 计算主合取范式
string getCNF(const vector<int> &en, const vector<vector<bool>> &truthValues, const set<string> &propVar)
{
    string cnf = "";
    for (auto index : en) // 对于每一个真值为F的指派对应的大项
    {
        auto miniterm = truthValues[index];
        auto iter = propVar.begin();
        cnf += "(";
        for (auto val : miniterm)
        {
            string element = *iter;
            ++iter;
            if (!val) // 真值为0的元素取本身, 真值为1的元素取反
            {
                cnf += element;
            }
            else
            {
                cnf += "!";
                cnf += element;
            }
            if (iter != propVar.end())
                cnf += " + ";
        }
        cnf += ")";
        if (index != en.back())
            cnf += " * ";
    }
    return cnf;
}

// 获取命题变元
set<string> getPropVar(const string &formula)
{
    set<string> s;
    istringstream record(formula);
    string element;
    while (record >> element)
        if (element != "!" && element != "+" && element != "*" && element != "->" && element != "<->" &&
            element != "(" && element != ")")
            s.insert(element);

    return s;
}
