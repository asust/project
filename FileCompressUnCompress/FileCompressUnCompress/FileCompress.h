#pragma once
#include<iostream>
#include<string>
#include<cassert>
#include<time.h>
#include"Huffman.h"
#define _DEBUG_
using namespace std;

//如何输入一个文件夹的名称遍历这下面所有的文件
typedef long long LongType;
struct CharInfo
{
	unsigned char _ch;
	LongType _count;
	string _code;
	CharInfo(unsigned char ch = 0)
		:_ch(ch)
		, _count(0)
	{
	}
	bool operator!=(CharInfo &c)
	{
		return this->_count != c._count;
	}
	CharInfo operator+(CharInfo &c)
	{
		CharInfo tmp;
		tmp._count = this->_count + c._count;
		return tmp;
	}
	bool operator<(CharInfo &c)
	{
		return this->_count < c._count;
	}
	bool operator>(CharInfo &c)
	{
		return this->_count>c._count;
	}
};


class FileCompress
{
public:
	FileCompress()
	{
		for (int i = 0; i < 256; ++i)
		{
			_infos[i]._ch = i;
		}
	}
public:
	bool Compress(const char *filename)
	{
		assert(filename);
		FILE *fIn;
		string path = filename;
		path += ".txt";
#ifdef _DEBUG_
		clock_t t1 = clock();
#endif
		fIn = fopen(path.c_str(), "rb");
		assert(fIn);
		char *ch = new char;
		fread(ch, 1, 1, fIn);
		while (!feof(fIn))
		{
			_infos[(unsigned char)(*ch)]._count++;
			fread(ch, 1, 1, fIn);
		}
		HuffmanTree<CharInfo> ht;
		
		ht.CreateHuffmanTree(_infos, 256, CharInfo());
#ifdef _DEBUG_
		clock_t t2 = clock();
		cout << "建立哈夫曼树的时间" << (t2 - t1) / CLOCKS_PER_SEC << endl;
#endif
		string tmp;
		GetHuffmanCode(ht.ReturnRootNode(), tmp);
		string CompressFilename = filename;
		CompressFilename += ".Huffman";
		FILE *fInCompress;
#ifdef _DEBUG_
		clock_t t3 = clock();
#endif
		fInCompress = fopen(CompressFilename.c_str(), "wb");
		assert(fInCompress);
		rewind(fIn);
		fread(ch, 1, 1, fIn);
		unsigned char InCh = 0;
		int index = 0;
		long long count = 0;
		while (!feof(fIn))//压缩的过程！！！！！！
		{
			string &Code = _infos[(unsigned char)(*ch)]._code;
			for (size_t i = 0; i < Code.size(); ++i)
			{

				InCh = InCh << 1;

				if (Code[i] == '1')
				{
					InCh |= 1;
				}
				if (++index == 8)
				{
					fwrite(&InCh, 1, 1, fInCompress);
					index = 0;
					InCh = 0;
				}
				count++;
			}
			fread(ch, 1, 1, fIn);//读取下一个字符
		}//当index!=0时证明没写完需要进行处理
		if (index != 0)
		{
			InCh = InCh << (8 - index);
			fwrite(&InCh, 1, 1, fInCompress);
		}
#ifdef _DEBUG_
		clock_t t4 = clock();
		cout << "精确压缩的时间" << (t4 - t3) / CLOCKS_PER_SEC << endl;
		clock_t t5 = clock();
#endif
		FILE *fconfig;
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		fconfig = fopen(ConfigFilename.c_str(), "wb");
		assert(fconfig);
		char *ccc = new char;
		fwrite(&count, 8, 1, fconfig);
		LongType Count = 0;
		for (int i = 0; i < 256; ++i)
		{
			if (_infos[i] != CharInfo())
			{
				fwrite(&_infos[i]._ch, 1, 1, fconfig);
				Count = _infos[i]._count;
				fwrite(&Count, 8, 1, fconfig);//必须使用longlong进行接受，不然不够啊
											  /*fputc('\n', fconfig);*/
			}
		}
#ifdef _DEBUG_
		clock_t t6 = clock();
		cout << "书写配置文件花费的时间" << (t6 - t5) / CLOCKS_PER_SEC << endl;
#endif
		fclose(fIn);
		fclose(fInCompress);
		fclose(fconfig);
		return true;
	}
	void UnCompress(const char *huffmanfilename, const char *configfilename)
	{
		FILE *configfile;
		configfile = fopen(configfilename, "rb");
		assert(configfile);
		char *ccc = new char;
		LongType count = 0;//总共计数
		LongType Count = 0;//单个字符计数
		fread(&count, 8, 1, configfile);
		char *ch = new char;
		fread(ch, 1, 1, configfile);//读取配置文件，录入count信息
		while (!feof(configfile))
		{
			unsigned char index = (unsigned char)(*ch);//获得目录
			fread(&Count, 8, 1, configfile);//读取计数信息
			_infos[index]._count = Count;
			fread(ch, 1, 1, configfile);//读取下一个字符信息
										/*ch = fgetc(configfile);*/
		}
		HuffmanTree<CharInfo> ht;
		ht.CreateHuffmanTree(_infos, 256, CharInfo());
		string tmp;
		GetHuffmanCode(ht.ReturnRootNode(), tmp);
		FILE *huffmanfile;
		huffmanfile = fopen(huffmanfilename, "rb");
		assert(huffmanfile);
		FILE *fOut;
		fOut = fopen("outputfile.txt", "wb");
		assert(fOut);
		//跟1与，向右移

		/*char *och = new char;*/
		fread(ch, 1, 1, huffmanfile);
		HuffmanNode<CharInfo> *root = ht.ReturnRootNode();
		if (root->_left == NULL && root->_right == NULL)
		{
			//只有一个节点的情况
			fwrite(ch, 1, 1, fOut);
		}
		int index = 0;

		LongType i = 0;//进行计数

		while (!feof(huffmanfile) && i < count)//解压缩！！！！！！！！！！！！！！！！！
		{
			if (((unsigned char)(*ch) & (unsigned char)128) == 0)
			{
				root = root->_left;
				if (root->_left == NULL && root->_right == NULL)
				{
					fwrite(&root->_value._ch, 1, 1, fOut);
					root = ht.ReturnRootNode();
				}
			}
			else
			{
				root = root->_right;
				if (root->_left == NULL && root->_right == NULL)
				{
					fwrite(&root->_value._ch, 1, 1, fOut);
					root = ht.ReturnRootNode();
				}
			}
			i++;
			*ch = *ch << 1;
			if (++index == 8)
			{
				fread(ch, 1, 1, huffmanfile);
				index = 0;
			}
		}
	}
	void GetHuffmanCode(HuffmanNode<CharInfo> *root, string & str)
	{
		if (root == NULL)
		{
			return;
		}//采用后序遍历的方式
		str += '0';
		GetHuffmanCode(root->_left, str);
		str.pop_back();
		str += '1';
		GetHuffmanCode(root->_right, str);
		str.pop_back();
		if (root->_left == NULL && root->_right == NULL)
		{
			_infos[root->_value._ch]._code = str;
		}
	}
protected:
	CharInfo _infos[256];
};


void TestFileCompress()
{
	FileCompress f;
	FileCompress f1;
#ifdef _DEBUG_
	clock_t start = clock();
#endif
	f.Compress("ttt");
#ifdef _DEBUG_
	clock_t mid = clock();
#endif
	f1.UnCompress("ttt.Huffman", "ttt.config");
#ifdef _DEBUG_
	clock_t finsh = clock();
	cout << "压缩时间" << ((double)(mid - start)) / CLOCKS_PER_SEC << endl;
	cout << "解压缩时间" << ((double)(finsh - mid)) / CLOCKS_PER_SEC << endl;
#endif
}