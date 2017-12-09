#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

template<class T>
class Heap {
private:
	int len;
	T *root;
	void heapfy(T *ch)
	{
		if (ch - root == len)
			return;
		T *less;
		less = left(ch);
		if (right(ch)<root + len && *right(ch)<*less)
			less = right(ch);
		if (*less<*ch)
		{
			std::swap(*less, *ch);
			if (less - root<len / 2 - 1)
				heapfy(less);
		}
	}

	void build(T ch[])
	{
		for (int i = len / 2 - 1; i >= 0; i--)
		{
			heapfy(ch + i);
		}
	}
public:

	Heap(T* r, int n) :
		len(n), root(r)
	{
		build(r);
	}


	T *right(T * ch)
	{
		return (ch - root + 1) * 2 + root;
	}
	T *left(T *ch)
	{
		return (ch - root + 1) * 2 - 1 + root;
	}
	T *parent(T *ch)
	{
		return (ch - root - 1) / 2 + root;
	}

	T extractMin()
	{
		T t = root[0];
		root[0] = root[len - 1];
		len--;
		heapfy(root);
		return t;
	}
	void insert(const T &ch)
	{
		len++;
		root[len - 1] = ch;

		T *child = root + len - 1;
		T *p = parent(child);
		while (child != root && *child<*p)
		{
			T t;
			t = *p;
			*p = *child;
			*child = t;
			child = p;
			p = parent(p);
		}
	}

	bool empty() const
	{
		return len<1;
	}
};





class Ch//字符类
{
public:
	char c;
	unsigned int freq;
	Ch()
	{

	}

	Ch(char ch, int f) :
		c(ch), freq(f)
	{

	}

	bool operator<(const Ch &ch)
	{

		return this->freq<ch.freq;
	}
	bool operator>(const Ch &ch)
	{
		return this->freq>ch.freq;
	}
	bool operator<=(const Ch &ch)
	{
		return this->freq <= ch.freq;
	}
	bool operator>=(const Ch &ch)
	{
		return this->freq >= ch.freq;
	}
	bool operator==(const Ch &ch)
	{
		return this->freq == ch.freq;
	}
};

class huffTree :
	public Ch
{
public:
	huffTree * left;
	huffTree *right;
	huffTree() :left(NULL), right(NULL)
	{

	}

	huffTree(char c, int f) :
		Ch(c, f), left(NULL), right(NULL)
	{

	}
	huffTree(const Ch &c) :Ch(c.c, c.freq), left(NULL), right(NULL)
	{

	}
};

struct huffCode
{
	int len;//Huffman编码的位长
	unsigned char * code;//指向Huffman编码存储的内存区域
};

//将一个指针包装为一个类，避免大块数据的移动
template<class T>
class HeapNode
{
public:
	T * pt;
	HeapNode() :
		pt(NULL)
	{

	}
	HeapNode(T * const p) :
		pt(p)
	{

	}

	bool operator<(const HeapNode &node)
	{
		return *(this->pt)<*(node.pt);
	}
};

huffTree *huffmanTree(Ch ch[], int n)
{
	huffTree * tree = new huffTree[n];
	HeapNode<huffTree> *nodes=new HeapNode<huffTree>[n];
	for (int i = 0; i<n; i++)
	{
		tree[i] = huffTree(ch[i]);
		nodes[i].pt = tree + i;//nodes[i]=HeapNode<huffTree>(tree+i);
	}
	Heap <HeapNode<huffTree>> Q(nodes, n);//Q为优先级队列

	huffTree *z;
	for (int i = 0; i<n - 1; i++)
	{
		z = new huffTree;
		z->right = Q.extractMin().pt;
		z->left = Q.extractMin().pt;
		z->freq = z->left->freq + z->right->freq;
		Q.insert(HeapNode<huffTree>(z));
	}
	z = Q.extractMin().pt;
	delete[] nodes;
	return z;

}
void _printHuffCode(huffTree *z, bool path[], int n)//递归版输出Huffman编码
{
	if (z->left == NULL)
	{
		for (int i = 0; i<n; i++)
			cout << path[i];
		cout << z->c << endl;
		return;
	}
	path[n] = 0;
	_printHuffCode(z->left, path, n+1 );
	path[n] = 1;
	_printHuffCode(z->right, path, n+1 );
}
void printHuffCode(huffTree *z, int n)
{
	bool *path = new bool[n];
	_printHuffCode(z, path, 0);
	delete[]path;
}
//通过文件流生成字符的频率表
void genChTb(ifstream &ifs,Ch *chTb,int n)
{
	for (int i = 0; i < n; i++)
	{
		chTb[i].c = i;
		chTb[i].freq = 0;
	}
	unsigned char c;
	while (ifs)
	{
		c=ifs.get();
		chTb[c].freq += 1;
	}
}

//建立ASCII码对Huffman编码的映射
void _genHuffMap(huffTree *z,huffCode huffMap[], bool path[], int n)//递归版输出Huffman编码
{
	if (z->left == NULL)
	{
		unsigned char c = z->c;
		huffMap[c].len = n;
		int bytes = n / 8 + (n % 8 == 0 ? 0 : 1);
		huffMap[c].code = new unsigned char[bytes];
		for (int i = 0; i < bytes; i++)
		{
			huffMap[c].code[i] = 0;
			for (int j = 0; j < 8; j++)
			{
				huffMap[c].code[i] = (huffMap[c].code[i] << 1) + path[i*8 +j];
			}
		}
		
		return;
	}
	path[n] = 0;
	_genHuffMap(z->left,huffMap, path, n+1 );
	path[n] = 1;
	_genHuffMap(z->right,huffMap, path, n+1 );
}
void genHuffMap(huffTree * hufftree, huffCode huffMap[],int n)
{
	bool *path = new bool[n];
	_genHuffMap(hufftree, huffMap, path, 0);
}


void huffCompress(huffCode huffMap[],ifstream &ifs, ofstream &ofs)
{
	unsigned char c;
	const int buffSize = 32;
	unsigned char buffer[buffSize];//存储尚未输出的编码
	int pos=0;//buffer的位指针
	while (!ifs.eof())
	{
		c = ifs.get();
	
		for (int i = 0; i < huffMap[c].len; i++)
		{
			buffer[pos / 8] &= ~(0x1 << i % 8);
			buffer[pos / 8] |= huffMap[c].code[i / 8] & (0x1 << i % 8);
			pos++;
			if (pos == buffSize * 8)
			{
				pos = 0;
				ofs.write((char *)buffer, buffSize);
			}
		}

	}
	//输出buffer中最后几个值
	while (pos%8!=0)
	{
		buffer[pos / 8] = buffer[pos / 8] & (~(0x1 << pos % 8));
		pos++;
	}
	ofs.write((char *)buffer, pos/8);

}

int main(int argc,char *argv[])
{
	if (argc != 2)
	{
		cerr << "Wrong parameter" << endl;
		return 1;
	}
	
	ifstream ifs;
	ifs.open(argv[1]);
	ofstream ofs("compressed.huf", ios_base::binary);

	if (ifs.is_open() == false)
	{
		cout << "Opening file failed!";
		return 1;
	}
	Ch chTab[256];
	genChTb(ifs, chTab,256);
	huffTree *huff = huffmanTree(chTab, 256);
	huffCode * huffMap = new huffCode[256];
	
	genHuffMap(huff,huffMap, 256);
	//ifs.close();
	//ifs.open("pride and prejudice.txt");
	ifs.clear();
	ifs.seekg(0, ios::beg);
	huffCompress(huffMap, ifs, ofs);
	
	ifs.close();
	ofs.close();
	system("pause");
	return 0;
}
