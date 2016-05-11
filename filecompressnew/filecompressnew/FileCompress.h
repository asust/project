#pragma once
#include<iostream>
#include<string>
#include<cassert>
#include"Huffman.h"
using namespace std;

//�������һ���ļ��е����Ʊ������������е��ļ�
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
		fIn = fopen(path.c_str(), "rb");
		assert(fIn);
		char ch;
		ch = fgetc(fIn);
		while (!feof(fIn))
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fIn);
		}
		HuffmanTree<CharInfo> ht;
		ht.CreateHuffmanTree(_infos, 256, CharInfo());
		string tmp;
		GetHuffmanCode(ht.ReturnRootNode(), tmp);
		string CompressFilename = filename;
		CompressFilename += ".Huffman";
		FILE *fInCompress;
		fInCompress = fopen(CompressFilename.c_str(), "wb");
		assert(fInCompress);
		fseek(fIn,0,SEEK_SET);
		ch = fgetc(fIn);
		unsigned char InCh = 0;
		int index = 0;
		long long count = 0;
		while (!feof(fIn))//ѹ���Ĺ��̣�����������
		{
			string &Code = _infos[(unsigned char)ch]._code;
			for (size_t i = 0; i < Code.size(); ++i)
			{

				InCh = InCh << 1;

				if (Code[i] == '1')
				{
					InCh |= 1;
				}
				if (++index == 8)
				{
					fputc(InCh, fInCompress);
					index = 0;
					InCh = 0;
				}
				count++;
			}
			ch = fgetc(fIn);//��ȡ��һ���ַ�
		}//��index!=0ʱ֤��ûд����Ҫ���д���
		if (index != 0)
		{
			InCh = InCh << (8 - index);
			fputc(InCh, fInCompress);
		}
		FILE *fconfig;
		string ConfigFilename = filename;
		ConfigFilename += ".config";
		fconfig = fopen(ConfigFilename.c_str(), "wb");
		assert(fconfig);
		char *ccc = new char;
		//fwrite(&count, 8, 1, fconfig);///д�����ļ��Ĺ���

		//��ʱʹ��fwrite�����ܼ�������д������ʹ��longlong������д������Ҫ�ָ�4���ֽں͵�4���ֽڷֱ���д
		fputc('\n', fconfig);
		LongType Count = 0;
		for (int i = 0; i < 256; ++i)
		{
			string line;
			line.clear();
			if (_infos[i] != CharInfo())
			{
				//if (_infos[i]._ch == '\0')
				//{

				//}
				char  countmp[20];
				line += _infos[i]._ch;
				line = +',';
				line += itoa(_infos[i]._count, countmp, 10);
				line += '\n';
				fputs(line.c_str(),fconfig);
				//fwrite(&_infos[i]._ch, 1, 1, fconfig);
				//Count = _infos[i]._count;
				//fwrite(&Count, 8, 1, fconfig);//����ʹ��longlong���н��ܣ���Ȼ������
				//							  /*fputc('\n', fconfig);*/

			}
		}
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
		LongType count = 0;//�ܹ�����
		LongType Count = 0;//�����ַ�����
		fread(&count, 8, 1, configfile);
		fgetc(configfile);
		string ttmp;
		fgets((char *)ttmp.c_str(), 100,configfile);
		char tttch = ttmp[0];
		char *ch = new char;
		fread(ch, 1, 1, configfile);
		while (!feof(configfile))
		{
			unsigned char index = (unsigned char)(*ch);//���Ŀ¼
			fread(&Count, 8, 1, configfile);//��ȡ������Ϣ
			_infos[index]._count = Count;
			fread(ch, 1, 1, configfile);//��ȡ��һ���ַ���Ϣ
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
		fOut = fopen("outputfile.jpg", "wb");
		assert(fOut);
		//��1�룬������

		/*char *och = new char;*/
		fread(ch, 1, 1, huffmanfile);
		HuffmanNode<CharInfo> *root = ht.ReturnRootNode();
		if (root->_left == NULL && root->_right == NULL)
		{
			//ֻ��һ���ڵ�����
			fwrite(ch, 1, 1, fOut);
		}
		int index = 0;

		LongType i = 0;//���м���

		while (!feof(huffmanfile) && i < count)//��ѹ������������������������������������
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
		}//���ú�������ķ�ʽ
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

FileCompress f;
FileCompress f1;
#include<time.h>
void TestFileCompress()
{
	
	clock_t start = clock();
	f.Compress("ttt");

//	f1.UnCompress("ttt.Huffman", "ttt.config");
	clock_t finsh = clock();
	cout << ((double)(finsh - start)) / CLOCKS_PER_SEC << endl;
}