#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>

// 随机生成一个n个节点m条边的邻接矩阵
std::vector<std::vector<int>> getAdjMat(int n, int m);
// 打印矩阵
void printMat(const std::vector<std::vector<int>> &mat);
// 判断是否连通
bool isConnected(const std::vector<std::vector<int>> &adjMat);
// 获取奇度数节点
std::vector<int> getOddDegree(const std::vector<std::vector<int>> &adjMat);
// 计算奇度数节点个数
int countOddDegree(const std::vector<std::vector<int>> &adjMat);
// 获取欧拉(回)路, 默认从0下标开始遍历
std::vector<int> getEulerPath(const std::vector<std::vector<int>> &adjMat, int index = 0);
// 获取所有(半)欧拉路
std::vector<std::vector<int>> getAllPath
(const std::vector<std::vector<int>> &adjMat, int edges, int begIndex = 0, int oddNums = 0);
// 打印路径
void printPath(const std::vector<int> &path);

int main()
{
    int n, m;
    std::cin >> n >> m; // 用户输入节点和边的个数
    if (m > (n * (n - 1)) / 2)
        throw("Exceeded the maximum allowed number of edges.");
    auto adjMat = getAdjMat(n, m);
    if (isConnected(adjMat))
    {
        auto oddNums = countOddDegree(adjMat);
        if (oddNums == 0) // 欧拉图
        {
            std::cout << "This graph is Euler graph\n";
            int choice = 0;
            std::cout << "Please select to print \n1. one Euler path   2. all Euler paths.\n";
            std::cin >> choice;
            if (choice == 1)
            {
                auto path = getEulerPath(adjMat);
                printPath(path);
            }
            else if (choice == 2)
            {
                auto allPath = getAllPath(adjMat, m);
                for (const auto &p : allPath)
                    printPath(p);
            }
            else
            {
                std::cout << "No such option!" << std::endl;
            }
        }
        else if (oddNums == 2) // 半欧拉图
        {
            auto oddDegree = getOddDegree(adjMat);
            std::cout << "This graph is Euler graph\n";
            int choice = 0;
            std::cout << "Please select to print \n1. one Euler path   2. all Euler paths.\n";
            std::cin >> choice;
            if (choice == 1)
            {
                auto path = getEulerPath(adjMat, *oddDegree.begin());
                printPath(path);
            }
            else if (choice == 2)
            {
                auto allPath = getAllPath(adjMat, m, *oddDegree.begin(), oddNums);
                for (const auto &p : allPath)
                    printPath(p);
            }
            else
            {
                std::cout << "No such option!" << std::endl;
            }
        }
        else
        {
            std::cout << "This graph is not Euler graph or Semi-Eulerian graph" << std::endl;
        }
    }
    else
    {
        std::cout << "This graph is not connected" << std::endl;
    }
    return 0;
}

// 随机生成一个n个节点m条边的邻接矩阵
std::vector<std::vector<int>> getAdjMat(int n, int m)
{
    // 初始化一个n阶的邻接矩阵
    std::vector<std::vector<int>> mat(n, std::vector<int>(n, 0));
    std::random_device rd; // 为随机数引擎获取种子
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned> dis(0, n - 1); // 指定随机数生成在闭区间[0, n-1]上
    for (int k = 0; k < m; ++k)                            // 随机生成m条边
    {
        int i, j;
        do
        {
            i = dis(gen);
            j = dis(gen);
        } while (i == j || mat[i][j]); // 确保不会生成指向节点本身和重复的边
        mat[i][j] = 1;
        mat[j][i] = 1; // 因为是无向图所以邻接矩阵关于对角线对称
    }
    return std::move(mat);
}

// 打印矩阵
void printMat(const std::vector<std::vector<int>> &mat)
{
    for (const auto &vec : mat)
    {
        for (const auto &val : vec)
            std::cout << val << " ";
        std::cout << "\n";
    }
    std::cout << std::endl;
}

// 判断是否连通
bool isConnected(const std::vector<std::vector<int>> &adjMat)
{
    // 计算可达性矩阵
    auto warShall = [&]() -> std::vector<std::vector<int>>
    {
        auto mat(adjMat);
        int n = mat.size();
        for (int j = 0; j < n; ++j)
            for (int i = 0; i < n; ++i)
                if (mat[i][j] == 1)
                    for (int k = 0; k < n; ++k)
                    {
                        mat[i][k] += mat[j][k];
                        if (mat[i][k] > 1)
                            mat[i][k] = 1;
                    }
        return std::move(mat);
    };
    auto mat = warShall();
    for (const auto &vec : mat)
        for (const auto &val : vec)
            if (!val) // 如果存在不可到达的节点则说明不连通
                return false;
    return true;
}

// 获取奇度数节点
std::vector<int> getOddDegree(const std::vector<std::vector<int>> &adjMat)
{
    std::vector<int> oddDegrees; // 保存奇度数节点下标的数组
    for (int i = 0; i < adjMat.size(); ++i)
    {
        int deg = 0;
        for (int j = 0; j < adjMat.size(); ++j)
            if (adjMat[i][j])
                ++deg;
        if (deg % 2 == 1) // 将奇度数节点下标存入数组
            oddDegrees.push_back(i);
    }
    return std::move(oddDegrees);
};

// 计算奇度数节点个数
int countOddDegree(const std::vector<std::vector<int>> &adjMat)
{
    auto oddDegrees = getOddDegree(adjMat); // 获取奇度数节点数组
    return oddDegrees.size();
}

// 获取欧拉(回)路, 默认从0下标开始遍历
std::vector<int> getEulerPath(const std::vector<std::vector<int>> &adjMat, int index = 0)
{
    std::vector<int> path;
    int nsize = adjMat.size();
    auto mat(adjMat);
    auto dfs = [&](auto self, int u) -> void
    {
        for (int v = 0; v < nsize; ++v)
        {
            if (mat[u][v]) // 如果u到v之间存在边则访问则递归访问v,并且将u到v之间的边删除
            {
                mat[u][v] = mat[v][u] = 0;
                self(self, v);
            }
        }
        path.push_back(u);
    };
    dfs(dfs, index);
    std::reverse(path.begin(), path.end());
    return std::move(path);
}

// 获取所有(半)欧拉路
std::vector<std::vector<int>>
getAllPath(const std::vector<std::vector<int>> &adjMat, int edges, int begIndex = 0, int oddNums = 0)
{
    int nsize = adjMat.size();
    auto mat(adjMat);
    std::vector<std::vector<int>> allPath;
    std::vector<int> path;
    auto dfs = [&](auto self, int u) -> void
    {
        for (int v = 0; v < nsize; ++v)
        {
            if (mat[u][v])
            {
                mat[u][v] = mat[v][u] = 0;
                path.push_back(u);
                self(self, v);
                path.pop_back(); // 将路径回溯，以便寻找其他可能的路径
                mat[u][v] = mat[v][u] = 1;
            }
        }
        if (std::all_of(mat[u].begin(), mat[u].end(), [](int val)
                        { return val == 0; }))
        {
            path.push_back(u);
            if (path.size() == edges + 1) // 如果已经遍历完了每一条边
                allPath.push_back(path);
            path.pop_back();
        }
    };
    if (oddNums == 0) // 欧拉图
    {
        for (int i = 0; i < nsize; ++i)
            dfs(dfs, i);
    }
    else if (oddNums == 2) // 半欧拉图
        dfs(dfs, begIndex);
    return std::move(allPath);
}

// 打印路径
void printPath(const std::vector<int> &path)
{
    int count = 0;
    for (auto node : path)
    {
        ++count;
        std::cout << node;
        if (count < path.size())
            std::cout << " -> ";
    }
    std::cout << std::endl;
}