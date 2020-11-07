// TestHelloWorld.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <istream>
#include <cmath>
//#include <bits/stdc++.h>
#include <windows.h>
using namespace std;

struct Node //字符结点
{
	char char_number = 0; //对应字符
	int counter = 0; //对应出现次数
	bool flag = 0;  //判断是否已经在树中
	vector<char> code;  //对应叶节点字符编码结果
	int parent = -1;  //父节点下标
	int leftchild = 0;  //左孩子下标
	int rightchild = 0; //右孩子下标
};

//哈夫曼编码
void TextIn(vector<char>& character_list, vector<Node>& leafnode); //读入文件，转为二进制字符串
void CharacterCount(vector<char> character_list, vector<Node>& leafnode); //计算字符频率
void sort(vector<Node>& leafnode, int length);
void GetHuffmanTree(Node root[], vector<Node> leafnode); //建立哈夫曼树
void GetHuffmanCode(Node root[], vector<Node>& leafnode); //求各节点的char *code
void ZipTextOut(vector<Node> leafnode, vector<char> character_list, int& balance); //输出压缩文件
void UnzipText(Node root[], vector<char>& character_list, int& balance);  //解压文件输出
void TextOut(vector<char>& character_list); //用于对比解压后文件

void menu(); //最开始输出一个页面

//费诺编码
//算数编码
int main()
{
	vector<char> character_list; //字符序列,扫描完文件后就不再改变
	vector<Node> leafnode; //叶向量
	Node* root;
	int balance = 0;
	double start_time; //用于计时
	double end_time;

	TextIn(character_list, leafnode);
	root = new Node[2 * leafnode.size() - 1]; //哈夫曼数组root,n个叶节点，有2*n-1个结点
	for (int i = 0; i < 2 * leafnode.size() - 1; i++)
	{
		Node tmp;
		root[i] = tmp;
	}

	(*root).parent = NULL;
	(*root).counter = leafnode.size();

	while (true)
	{
		system("cls"); //每选择完一次功能后清屏
		menu();
		int choice;
		cin >> choice; //选择功能
		bool quite_flag = 0;

		switch (choice)
		{
		case 1:  //哈夫曼编码
		{
			start_time = clock();

			GetHuffmanTree(root, leafnode);
			GetHuffmanCode(root, leafnode);

			ZipTextOut(leafnode, character_list, balance);

			end_time = clock();
			cout << "文件 诺贝尔化学奖.txt 已编码压缩" << "   " << "用时" << double(end_time - start_time) << endl;
			Sleep(500);  //延迟半秒
			system("Pause");
			break;
		}
		case 2: //哈夫曼解码
		{
			start_time = clock();
			UnzipText(root, character_list, balance);
			end_time = clock();
			TextOut(character_list);
			cout << "文件已译码输出" << "   " << "用时" << double(end_time - start_time) << endl;
			Sleep(500);
			system("Pause");
			break;
		}
		case 3:
		{
			Sleep(500);
			return 0;
		}
		}
	}
	return 0;
}

void TextIn(vector<char>& character_list, vector<Node>& leafnode)
{
	char ch;
	//以二进制流的方式打开文件  //原本为ascii文件，但无法输出， 改为二进制流 ios::binary
	ifstream file;
	file.open("F:/VisualStudio/SourceCode/HuffmanCode/HuffmanCode/诺贝尔化学奖.txt", ios::in | ios::binary); //脑机接口新突破.docx 诺贝尔化学奖.txt
	//扫描文件，获得字符
	while (file.peek() != EOF)
	{
		file.get(ch);
		character_list.push_back(ch);
	}
	//关闭文件
	file.close();
	//计算字符出现频率，生成叶节点信息
	CharacterCount(character_list, leafnode);
}
void CharacterCount(vector<char> character_list, vector<Node>& leafnode)
{
	int counter[256]; //由于字符遍历的时间太长（n方的时间复杂度），之前跑了7分多钟都没有结果，所以改成下面这种方法
	for (int i = 0; i < 256; i++)  //counter数组初始化为0
	{
		counter[i] = 0;
	}
	cout << "字符数" << character_list.size() << endl;
	for (int i = 0; i < character_list.size(); i++) //遍历一遍对应的位置+1
	{
		char tmp = character_list[i];  //这里char可以是负数！
		counter[tmp + 128]++;
	}
	for (int i = 0; i < 256; i++)//建立叶节点向量
	{
		Node newnode;
		newnode.char_number = char(i - 128);  //原本想直接转为字符，但因为后续的一些打印验证过程中不能全部打印（第128字符的原因），所以这一步放在最后
		newnode.counter = counter[i];
		newnode.flag = 0;
		newnode.leftchild = -1;
		newnode.rightchild = -1;
		leafnode.push_back(newnode);
	}
	sort(leafnode, leafnode.size()); //排序
}
void sort(vector<Node>& leafnode, int length)
{
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < length - 1; j++)
		{
			if (leafnode[j].counter < leafnode[j + 1].counter)
			{
				Node tmp = leafnode[j];
				leafnode[j] = leafnode[j + 1];
				leafnode[j + 1] = tmp;
			}
		}
	}
}

void GetHuffmanTree(Node root[], vector<Node> leafnode)
{
	int length = leafnode.size();  //叶节点数目
	for (int i = 0; i < length; i++) //将叶节点加入哈夫曼树
	{
		root[i] = leafnode[i];
	}
	for (int i = 0; i < length - 1; i++) //将非叶节点加入哈夫曼树
	{
		int x1, x2, y1, y2;
		x1 = x2 = 900000000;//最小和次小的counter
		y1 = y2 = 0;  //最小和次小的下标
		//找权值最小的两个子树
		for (int j = 0; j < length + i; j++) //counter最小的子树下标
		{
			if (root[j].counter < x1 && root[j].flag == 0) //下标j的结点counter小于当前结点，且flag为0
			{
				y1 = j;
				x1 = root[y1].counter;
			}
		}
		root[y1].flag = 1;
		for (int j = 0; j < length + i; j++)  //权值次小
		{
			if (root[j].counter < x2 && root[j].flag == 0)
			{
				y2 = j;
				x2 = root[y2].counter;
			}
		}
		root[y2].flag = 1;

		root[y1].parent = length + i;
		root[y2].parent = length + i;
		root[length + i].char_number = -1;
		root[length + i].counter = root[y1].counter + root[y2].counter;
		root[length + i].leftchild = y1;
		root[length + i].rightchild = y2;
	}
	root[2 * length - 2].parent = -1; //根节点的父节点为-1
}

void GetHuffmanCode(Node root[], vector<Node>& leafnode) //自底向上建立code
{
	for (int i = 0; i < leafnode.size(); i++)
	{
		vector<char> code;
		int parent = 0;
		int route = i;   //route作为向上走的下标参量
		while (parent != -1)
		{
			parent = root[route].parent;
			char left_or_right = '0';
			if (root[parent].leftchild == route) //是父节点的左子节点
			{
				left_or_right = '0';
				code.insert(code.begin(), left_or_right); //加入code的头部
			}
			else if (root[parent].rightchild == route)//是父节点的右子节点
			{
				left_or_right = '1';
				code.insert(code.begin(), left_or_right);
			}
			route = parent;
		}
		leafnode[i].code = code;
		root[i].code = code;
		//打印一下code
		/*cout << i << "         ";
		for(int j = 0;j < code.size();j++)
			cout << leafnode[i].code[j];
		cout << endl;*/
	}
}

void ZipTextOut(vector<Node> leafnode, vector<char> character_list, int& balance)
{
	ofstream newfile("F:/VisualStudio/SourceCode/HuffmanCode/HuffmanCode/zip.txt", ios::out | ios::binary);
	vector<char> binary_array;
	vector<char> code;

	for (int i = 0; i < character_list.size(); i++)
	{
		int tmp = character_list[i];
		vector<char> code;
		int j;
		for (j = 0; j < leafnode.size(); j++) //找对应字符的code
		{
			if (tmp == leafnode[j].char_number)
			{
				code = leafnode[j].code;
				break;
			}
		}
		for (int k = 0; k < code.size(); k++) //得到编码的01序列
		{
			binary_array.push_back(code[k]);
		}
	}

	int multiple = binary_array.size() / 8 * 8;  //输出0和1是字符，需要再转一次字符  8个01一组，多的直接输出
	balance = binary_array.size() - binary_array.size() / 8 * 8; //此时为5
	for (int i = 1; i < binary_array.size() / 8 * 8; i++)
	{
		i--;
		char ch;
		int x = 0;
		for (int j = 7; j >= 0; j--, i++)
		{
			int tmp1 = 0;
			if (binary_array[i] == '1')
				tmp1 = pow(2, j);
			x += tmp1;
		}
		code.push_back(x - 128);
	}
	for (int i = 0; i < code.size(); i++)
	{
		newfile << code[i];
	}
	for (int i = binary_array.size() / 8 * 8; i < binary_array.size(); i++)
	{
		newfile << binary_array[i];
	}
	newfile.close();
}

void UnzipText(Node root[], vector<char>& character_list, int& balance)
{
	vector<char> array;
	vector<char> binary_array;
	ifstream zip("F:/VisualStudio/SourceCode/HuffmanCode/HuffmanCode/zip.txt", ios::in | ios::binary); //打开文件

	ofstream newfile("F:/VisualStudio/SourceCode/HuffmanCode/HuffmanCode/unzip.txt", ios::out | ios::binary);


	while (zip.peek() != EOF) //获得文件内容
	{
		char ch;
		zip.get(ch); //不能是zip >>，有些字符为回车
		array.push_back(ch);
	}
	zip.close();


	int length_of_code = array.size() - balance;
	for (int i = 0; i < length_of_code; i++)
	{
		int tmp = (int)(array[i] + 128);
		vector<int> binary;  //将字符转为8位2进制
		for (int k = 7; k >= 0; k--)
			binary.push_back(((tmp >> k) & 1));//与1做位操作，前面位数均为0
		for (int k = 0; k < 8; k++)
		{
			if (binary[k] == 0)
				binary_array.push_back('0');
			else
				binary_array.push_back('1');
		}
	}
	for (int i = length_of_code; i < array.size(); i++) //压入多余的01
		binary_array.push_back(array[i]);

	for (int i = 1; i < binary_array.size(); i++)
	{
		i--;  //注意这里的i--！！！！
		int child = 0;
		int index = 510;
		while (root[index].leftchild != -1 && root[index].rightchild != -1) //若为叶节点，则子节点为-1
		{
			if (binary_array[i] == '0') //找左右子节点
			{
				index = root[index].leftchild;
			}
			else if (binary_array[i] == '1')
			{
				index = root[index].rightchild;
			}
			i++;
		}
		newfile << (char)root[index].char_number;
	}
	newfile.close();
}

void TextOut(vector<char>& character_list)
{
	ofstream newfile("对比文件.docx", ios::out | ios::binary);
	for (int i = 0; i < character_list.size(); i++)
		newfile << character_list[i];
}

void menu()
{
	cout << "################################################################" << endl;
	cout << "                                                                " << endl;
	cout << "                          输入命令                              " << endl;
	cout << "                                                                " << endl;
	cout << "                        1. 压缩文件                             " << endl;
	cout << "                        2. 解压文件                             " << endl;
	cout << "                        3. 退出程序                             " << endl;
	cout << "                                                                " << endl;
	cout << "################################################################" << endl;
}



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
