#include <iostream>
#include <string>
using namespace std;

template <class T>//向量表
class VecList {
private:
    int capacity;
    int length;
    T* arr;
    void doubleListSize() {
        T* oldArr = arr;
        arr = new T[2 * capacity];
        capacity = 2 * capacity;
        for (int i = 0; i < length; i++) {
            arr[i] = oldArr[i];
        }
        delete[] oldArr;
    }
public:
    VecList() {
        length = 0;
        capacity = 100;
        arr = new T[capacity];
    }
    VecList(T* a, int n) {
        length = n;
        capacity = 100 + 2 * n;
        arr = new T[capacity];
        for (int i = 0; i < n; i++) {
            arr[i] = a[i];
        }
    }
    ~VecList() {
        delete[] arr;
    }
    int getLength() {
        return length;
    }
    bool isEmpty() {
        return length == 0;
    }
    void insertEleAtPos(int i, T x) {
        if (length == capacity)
            doubleListSize();
        if (i > length || i < 0)
            throw "Illegal position";
        for (int j = length; j > i; j--)
            arr[j] = arr[j - 1];
        arr[i] = x;
        length++;
    }
    T deleteEleAtPos(int i) {
        if (i >= length || i < 0)
            throw "Illegal position";
        T tmp = arr[i];
        for (int j = i; j < length - 1; j++)
            arr[j] = arr[j + 1];
        length--;
        return tmp;
    }
    void setEleAtPos(int i, T x) {
        if (i >= length || i < 0)
            throw "Illegal position";
        arr[i] = x;
    }
    T getEleAtPos(int i) {
        if (i >= length || i < 0)
            throw "Illegal position";
        return arr[i];
    }
    int locateEle(T x) {
        for (int i = 0; i < length; i++) {
            if (arr[i] == x)
                return i;
        }
        return -1;
    }
    void printList() {
        for (int i = 0; i < length; i++)
            cout << arr[i] << " ";
    }
};

//每格坐标及其属性
//空格为-2，黑格为-1，0-4的黑格分别为0-4，5为灯,点亮为6，禁止放灯为-3
typedef struct
{
    int type;//属性
    int aroundempty;//周围空格数
    int aroundlight;//周围灯数
}grid;

//存储黑格坐标
typedef struct
{
    int x;
    int y;
}black;

//变量声明
int row, column;//图标的行数与列数
grid** result;//存放解
bool alreadyfinish;//是否已经结束
VecList<black> blacklist[5];//存放5种黑格

//函数声明
void copygridBtoA(grid* A, grid* B);//将格子A复制给B
void copychartBtoA(grid** A, grid** B);//复制图表
void input(grid** chart, string s);//输入图表,并将数字黑格存入列表,并求黑格周围白格数
void printgrid(grid** N);//打印图表
bool isblack(grid** c, int x, int y);//检查是否为数字黑格
bool isin(int x, int y);//检查是否在图表范围内
void countwhiteandlight(grid** c);//计算黑格周围白格与灯数
bool isvalid(grid** c);//检查图表是否合理
bool canlight(grid** c, int x, int y);//是否可以放置灯
bool lightup(grid** c, int x, int y);//点亮
bool isfinish(grid** c);//查看是否已经解决
void handle0and4(grid** chart);//在开始前处理0,4
bool notlight(grid** c, int x, int y);//设置不能放灯
void candidate(grid** c, int x, int y, VecList<black>* can, int* n);//搜索可以放灯点亮此格的位置
bool simplehandle(grid** c);//处理一些简单能确定的灯泡，并更新黑格
bool chooseWhiteCell(grid** c, int& wx, int& wy);//选择可能性最大的白格进行回溯
bool hardhandle(grid** c, int depth);//困难求解

//将格子A复制给B
void copygridBtoA(grid* A, grid* B)
{
    A->type = B->type;
    A->aroundempty = B->aroundempty;
    A->aroundlight = B->aroundlight;
}

//复制图表
void copychartBtoA(grid** A, grid** B)
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            copygridBtoA(&A[i][j], &B[i][j]);
        }
    }
}

//输入图表,并将数字黑格存入列表,并求黑格周围白格数
void input(grid** chart, string s)
{
    int c = 0;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (s[c] == '0')
            {
                chart[i][j].type = 0;
                black tmp;
                tmp.x = i;
                tmp.y = j;
                blacklist[0].insertEleAtPos(blacklist[0].getLength(), tmp);
            }
            if (s[c] == '1')
            {
                chart[i][j].type = 1;
                black tmp;
                tmp.x = i;
                tmp.y = j;
                blacklist[1].insertEleAtPos(blacklist[1].getLength(), tmp);
            }
            if (s[c] == '2')
            {
                chart[i][j].type = 2;
                black tmp;
                tmp.x = i;
                tmp.y = j;
                blacklist[2].insertEleAtPos(blacklist[2].getLength(), tmp);
            }
            if (s[c] == '3')
            {
                chart[i][j].type = 3;
                black tmp;
                tmp.x = i;
                tmp.y = j;
                blacklist[3].insertEleAtPos(blacklist[3].getLength(), tmp);
            }
            if (s[c] == '4')
            {
                chart[i][j].type = 4;
                black tmp;
                tmp.x = i;
                tmp.y = j;
                blacklist[4].insertEleAtPos(blacklist[4].getLength(), tmp);
            }
            if (s[c] == 'X')
            {
                chart[i][j].type = -1;
            }
            if (s[c] == '.')
            {
                chart[i][j].type = -2;
            }
            if (s[c] == 'L')chart[i][j].type = 5;
            chart[i][j].aroundempty = -1;
            chart[i][j].aroundlight = -1;
            c++;
            if (c >= s.length())break;
        }
    }
    countwhiteandlight(chart);
}

//打印图表
void printgrid(grid** N)
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (N[i][j].type == -2 || N[i][j].type == 6 || N[i][j].type == -3)cout << ".";
            else if (N[i][j].type == -1)cout << "X";
            else if (N[i][j].type == 0)cout << "0";
            else if (N[i][j].type == 1)cout << "1";
            else if (N[i][j].type == 2)cout << "2";
            else if (N[i][j].type == 3)cout << "3";
            else if (N[i][j].type == 4)cout << "4";
            else if (N[i][j].type == 5)cout << "L";
        }
    }
    cout << endl;
}

//计算黑格周围白格与灯数
void countwhiteandlight(grid** c)
{
    int countw, countl, x, y;
    for (int j = 0; j <= 4; j++)
    {
        for (int i = 0; i < blacklist[j].getLength(); i++)
        {
            countw = 0;
            countl = 0;
            x = blacklist[j].getEleAtPos(i).x;
            y = blacklist[j].getEleAtPos(i).y;
            if (x - 1 >= 0)
            {
                if (c[x - 1][y].type == -2)countw++;
                if (c[x - 1][y].type == 5)countl++;
            }
            if (x + 1 <= row - 1)
            {
                if (c[x + 1][y].type == -2)countw++;
                if (c[x + 1][y].type == 5)countl++;
            }
            if (y - 1 >= 0)
            {
                if (c[x][y - 1].type == -2)countw++;
                if (c[x][y - 1].type == 5)countl++;
            }
            if (y + 1 <= column - 1)
            {
                if (c[x][y + 1].type == -2)countw++;
                if (c[x][y + 1].type == 5)countl++;
            }
            c[x][y].aroundempty = countw;
            c[x][y].aroundlight = countl;
        }
    }
}

//检查是否为数字黑格
bool isblack(grid** c, int x, int y)
{
    if (c[x][y].type >= 0 && c[x][y].type <= 4)return true;
    return false;
}

//检查是否在图表范围内
bool isin(int x, int y)
{
    if (x < 0 || x >= row || y < 0 || y >= column) return false;
    return true;
}

//是否可以放置灯
bool canlight(grid** c, int x, int y)
{
    if (x < 0 || x >= row || y < 0 || y >= column) return false;
    if (c[x][y].type != -2) return false;
    //检查是否可以放置灯泡
    //上
    for (int i = x - 1; i >= 0; i--)
    {
        if (c[i][y].type == 5) return false;
        if (c[i][y].type >= -1 && c[i][y].type <= 4) break;
    }
    //下
    for (int i = x + 1; i < row; i++)
    {
        if (c[i][y].type == 5) return false;
        if (c[i][y].type >= -1 && c[i][y].type <= 4) break;
    }
    //左
    for (int i = y - 1; i >= 0; i--)
    {
        if (c[x][i].type == 5) return false;
        if (c[x][i].type >= -1 && c[x][i].type <= 4) break;
    }
    //右
    for (int i = y + 1; i < column; i++)
    {
        if (c[x][i].type == 5) return false;
        if (c[x][i].type >= -1 && c[x][i].type <= 4) break;
    }

    //检查放灯后黑格数字是否满足
    if (isin(x - 1, y))
    {
        if (isblack(c, x - 1, y))
        {
            if (c[x - 1][y].aroundlight + 1 > c[x - 1][y].type)return false;
        }
    }
    if (isin(x + 1, y))
    {
        if (isblack(c, x + 1, y))
        {
            if (c[x + 1][y].aroundlight + 1 > c[x + 1][y].type)return false;
        }
    }
    if (isin(x, y - 1))
    {
        if (isblack(c, x, y - 1))
        {
            if (c[x][y - 1].aroundlight + 1 > c[x][y - 1].type)return false;
        }
    }
    if (isin(x, y + 1))
    {
        if (isblack(c, x, y + 1))
        {
            if (c[x][y + 1].aroundlight + 1 > c[x][y + 1].type)return false;
        }
    }
    return true;
}

//搜索可以放灯点亮此格的位置
void candidate(grid** c, int x, int y, VecList<black>* can, int* n)
{
    *n = 0; // 通过指针设置n为0

    while (!can->isEmpty()) {
        can->deleteEleAtPos(0);
    }

    // 检查目标位置本身
    if (canlight(c, x, y)) {
        black tmp;
        tmp.x = x;
        tmp.y = y;
        can->insertEleAtPos(can->getLength(), tmp);
        (*n)++;
    }

    // 上方向：从x-1开始直到边界或黑格
    for (int i = x - 1; i >= 0; i--) {
        if (c[i][y].type >= -1 && c[i][y].type <= 4) break; // 遇到黑格停止
        if (canlight(c, i, y)) {
            black tmp;
            tmp.x = i;
            tmp.y = y;
            can->insertEleAtPos(can->getLength(), tmp);
            (*n)++;
        }
    }
    // 下方向
    for (int i = x + 1; i < row; i++) {
        if (c[i][y].type >= -1 && c[i][y].type <= 4) break;
        if (canlight(c, i, y)) {
            black tmp;
            tmp.x = i;
            tmp.y = y;
            can->insertEleAtPos(can->getLength(), tmp);
            (*n)++;
        }
    }
    // 左方向
    for (int i = y - 1; i >= 0; i--) {
        if (c[x][i].type >= -1 && c[x][i].type <= 4) break;
        if (canlight(c, x, i)) {
            black tmp;
            tmp.x = x;
            tmp.y = i;
            can->insertEleAtPos(can->getLength(), tmp);
            (*n)++;
        }
    }
    // 右方向
    for (int i = y + 1; i < column; i++) {
        if (c[x][i].type >= -1 && c[x][i].type <= 4) break;
        if (canlight(c, x, i)) {
            black tmp;
            tmp.x = x;
            tmp.y = i;
            can->insertEleAtPos(can->getLength(), tmp);
            (*n)++;
        }
    }
}

//点亮
bool lightup(grid** c, int x, int y)
{
    if (!canlight(c, x, y))return false;
    //放置灯，并点亮上下左右
    c[x][y].type = 5;
    //上
    for (int i = x - 1; i >= 0 && (c[i][y].type == -2 || c[i][y].type == 6 || c[i][y].type == -3); i--)
    {
        c[i][y].type = 6;
    }
    //下
    for (int i = x + 1; i <= row - 1 && (c[i][y].type == -2 || c[i][y].type == 6 || c[i][y].type == -3); i++)
    {
        c[i][y].type = 6;
    }
    //左
    for (int i = y - 1; i >= 0 && (c[x][i].type == -2 || c[x][i].type == 6 || c[x][i].type == -3); i--)
    {
        c[x][i].type = 6;
    }
    //右
    for (int i = y + 1; i <= column - 1 && (c[x][i].type == -2 || c[x][i].type == 6 || c[x][i].type == -3); i++)
    {
        c[x][i].type = 6;
    }
    countwhiteandlight(c);
    return true;
}

//查看是否已经解决
bool isfinish(grid** c)
{
    //是否有未照亮空格
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (c[i][j].type == -2 || c[i][j].type == -3) return false;
        }
    }

    countwhiteandlight(c);
    //黑格是否满足要求
    int x, y;
    for (int j = 0; j <= 4; j++)
    {
        for (int i = 0; i < blacklist[j].getLength(); i++)
        {
            x = blacklist[j].getEleAtPos(i).x;
            y = blacklist[j].getEleAtPos(i).y;
            if (c[x][y].aroundlight != c[x][y].type) return false;
        }
    }

    return true;
}

//检查图表是否合理
bool isvalid(grid** c)
{
    //检查黑格要求
    int x, y;
    for (int j = 0; j <= 4; j++)
    {
        for (int i = 0; i < blacklist[j].getLength(); i++)
        {
            x = blacklist[j].getEleAtPos(i).x;
            y = blacklist[j].getEleAtPos(i).y;
            if (c[x][y].aroundlight > c[x][y].type)  // 灯数超过需求
                return false;
            if (c[x][y].aroundlight + c[x][y].aroundempty < c[x][y].type)  // 剩余空格不够满足需求
                return false;
        }
    }

    //检查灯泡是否互相照亮
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (c[i][j].type == 5)
            {
                //检查同一行
                for (int k = j + 1; k < column; k++)
                {
                    if (c[i][k].type == 5) return false;  // 发现另一个灯泡
                    if (c[i][k].type >= -1 && c[i][k].type <= 4) break;  // 遇到黑格，停止检查
                }
                //检查同一列
                for (int k = i + 1; k < row; k++)
                {
                    if (c[k][j].type == 5) return false;  // 发现另一个灯泡
                    if (c[k][j].type >= -1 && c[k][j].type <= 4) break;  // 遇到黑格，停止检查
                }
            }
        }
    }

    return true;
}

//在开始前处理0,4
void handle0and4(grid** chart)
{
    int x, y;
    //0
    for (int i = 0; i < blacklist[0].getLength() && i >= 0; i++)
    {
        x = blacklist[0].getEleAtPos(i).x;
        y = blacklist[0].getEleAtPos(i).y;
        notlight(chart, x - 1, y);
        notlight(chart, x + 1, y);
        notlight(chart, x, y - 1);
        notlight(chart, x, y + 1);
    }
    //4
    for (int i = 0; i < blacklist[4].getLength() && i >= 0; i++)
    {
        x = blacklist[4].getEleAtPos(i).x;
        y = blacklist[4].getEleAtPos(i).y;
        lightup(chart, x - 1, y);
        lightup(chart, x + 1, y);
        lightup(chart, x, y - 1);
        lightup(chart, x, y + 1);
    }
    countwhiteandlight(chart);
}

//设置不能放灯
bool notlight(grid** c, int x, int y)
{
    if (x < 0 || x >= row || y < 0 || y >= column)return true;
    if (c[x][y].type == -2)
    {
        c[x][y].type = -3;
        countwhiteandlight(c);
        return true;
    }
    return false;
}

//处理一些简单能确定的灯泡，并更新黑格
bool simplehandle(grid** c)
{
    int x, y;
    int aroundl, aroundw, type;
    bool change = true; 
    countwhiteandlight(c);
    while (change)
    {
        change = false;
        //黑格
        for (int t = 0; t <= 4; t++)
        {
            for (int i = 0; i < blacklist[t].getLength(); i++)
            {
                x = blacklist[t].getEleAtPos(i).x;
                y = blacklist[t].getEleAtPos(i).y;
                aroundl = c[x][y].aroundlight;
                aroundw = c[x][y].aroundempty;
                type = c[x][y].type;

                // 检查是否违反条件
                if (aroundl > type) return false;
                if (aroundl + aroundw < type) return false;

                if (aroundl == type && aroundw > 0)
                {
                    if (x - 1 >= 0 && c[x - 1][y].type == -2)
                    {
                        notlight(c, x - 1, y);
                        change = true;
                    }
                    if (y - 1 >= 0 && c[x][y - 1].type == -2)
                    {
                        notlight(c, x, y - 1);
                        change = true;
                    }
                    if (x + 1 < row && c[x + 1][y].type == -2)
                    {
                        notlight(c, x + 1, y);
                        change = true;
                    }
                    if (y + 1 < column && c[x][y + 1].type == -2)
                    {
                        notlight(c, x, y + 1);
                        change = true;
                    }
                }
                else if (aroundw == (type - aroundl) && aroundw > 0)
                {
                    if (x - 1 >= 0 && c[x - 1][y].type == -2)
                    {
                        if (!lightup(c, x - 1, y)) return false;
                        change = true;
                    }
                    if (y - 1 >= 0 && c[x][y - 1].type == -2)
                    {
                        if (!lightup(c, x, y - 1)) return false;
                        change = true;
                    }
                    if (x + 1 < row && c[x + 1][y].type == -2)
                    {
                        if (!lightup(c, x + 1, y)) return false;
                        change = true;
                    }
                    if (y + 1 < column && c[x][y + 1].type == -2)
                    {
                        if (!lightup(c, x, y + 1)) return false;
                        change = true;
                    }
                }
            }
        }
        if (change)
        {
            countwhiteandlight(c);
            continue; // 重新检查黑格，因为可能新的放置又生成新的唯一确定的格子
        }

        //白格
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                // 只处理未被照亮且不是灯泡的白格
                if (c[i][j].type != -2 && c[i][j].type != -3) continue;
                if (c[i][j].type == 6) continue; // 如果已经被照亮，跳过

                VecList<black> can;
                int cl = 0;
                candidate(c, i, j, &can, &cl);
                if (cl == 0)
                {
                    // 没有候选位置可以照亮这个白格，冲突
                    return false;
                }
                if (cl == 1)
                {
                    int x = can.getEleAtPos(0).x;
                    int y = can.getEleAtPos(0).y;
                    if (lightup(c, x, y))
                    {
                        change = true;
                    }
                }
            }
        }
        if (change) countwhiteandlight(c);
    }
    return isvalid(c);  // 检查最终状态是否有效
}

//选择可能性最大的白格进行回溯
bool chooseWhiteCell(grid** c, int& wx, int& wy)
{
    // 优先选择周围有黑格的白格
    int maxConstraints = 0;
    wx = -1; wy = -1;
    VecList<black> tmp;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (c[i][j].type == -2)
            {
                int constraints = 0;
                // 检查周围的黑格
                if (i > 0 && c[i - 1][j].type >= 0 && c[i - 1][j].type <= 4) constraints++;
                if (i < row - 1 && c[i + 1][j].type >= 0 && c[i + 1][j].type <= 4) constraints++;
                if (j > 0 && c[i][j - 1].type >= 0 && c[i][j - 1].type <= 4) constraints++;
                if (j < column - 1 && c[i][j + 1].type >= 0 && c[i][j + 1].type <= 4) constraints++;

                if (constraints > maxConstraints)
                {
                    maxConstraints = constraints;
                    wx = i;
                    wy = j;
                }
            }
        }
    }
    if (wx != -1) return true;

    // 如果没有找到周围有黑格的白格，则找放灯可能性最大的白格
    int best = 501; 
    int light = 2; 
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            //检查该位置是否可以放灯
            if (c[i][j].type != -2) continue;
            if (!canlight(c, i, j)) continue; 
            VecList<black> tmp;
            int canCount = 0;
            candidate(c, i, j, &tmp, &canCount); // 计算可以照亮这个白格的位置数量
            int illuminated = (c[i][j].type == 6) ? 1 : 0; // 如果已被照亮，则为1，否则为0
            if (wx == -1 ||illuminated < light ||(illuminated == light && canCount < best))
            {
                wx = i;
                wy = j;
                best = canCount;
                light = illuminated;
            }
        }
    }
    return (wx != -1);
}

//困难求解
bool hardhandle(grid** c, int depth)
{
    //开始前先先进行简单求解
    if (!simplehandle(c))return false;
    
    //若已找到解则结束
    if (alreadyfinish) return true;
    if (isfinish(c))
    {
        copychartBtoA(result, c);
        alreadyfinish = true;
        return true;
    }

    // 检查当前状态是否有效
    if (!isvalid(c)) return false;

    //选择可能性最大的白格
    int wx = -1, wy = -1;
    if (!chooseWhiteCell(c, wx, wy)) return false;

    //将当前状态保存下来
    grid** tmpc = new grid * [row];
    for (int i = 0; i < row; i++)
    {
        tmpc[i] = new grid[column];
    }
    copychartBtoA(tmpc, c);


    bool success = false;

    // 尝试放置灯泡
    if (lightup(tmpc, wx, wy))
    {
        if (simplehandle(tmpc) && hardhandle(tmpc, depth + 1))
        {
            success = true;
        }
    }

    if (!success && !alreadyfinish)
    {
        // 恢复原来的图表
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                copygridBtoA(&tmpc[i][j], &c[i][j]);
            }
        }

        // 尝试不放灯泡
        if (notlight(tmpc, wx, wy))
        {
            if (simplehandle(tmpc) && hardhandle(tmpc, depth + 1))
            {
                success = true;
            }
        }
    }

    //删除开辟的空间
    for (int i = 0; i < row; i++)
    {
        delete[] tmpc[i];
    }
    delete[] tmpc;

    return success;
}

int main()
{   //输入题目
    string pre;
    cin >> row;
    cin >> column;
    cin >> pre;
    //存储题目
    grid** question = new grid * [row];
    result = new grid * [row];
    for (int i = 0; i < row; i++)
    {
        question[i] = new grid[column];
        result[i] = new grid[column];
    }
    input(question, pre);

    handle0and4(question); //处理0和4

    if (!simplehandle(question))//先进行简单求解
    {
        cout << "无解" << endl;
        return 0;
    }

    alreadyfinish = false;
    copychartBtoA(result, question);
    if (hardhandle(result, 0))//进行困难求解
    {
        printgrid(result);
    }
    else
    {
        cout << "无解" << endl;
    }

    for (int i = 0; i < row; i++)
    {
        delete[] question[i];
        delete[] result[i];
    }
    delete[] question;
    delete[] result;
    return 0;
}
