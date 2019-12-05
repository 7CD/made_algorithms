#pragma once

#include<vector>
#include<queue>
#include<unordered_map>
#include<cassert>
#include<iostream>
#include <fstream>
#include <string>

using namespace std;

typedef unsigned char byte;

// Входной поток
struct IInputStream {
	// Возвращает false, если поток закончился
	virtual bool Read(byte& value) = 0;
};

// Выходной поток
struct IOutputStream {
	virtual void Write(byte value) = 0;
};

//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BitsWriter {
 public:
  void WriteBit(bool bit) {
    accumulator += static_cast<byte>(bit) << (7 - bitsCount);
    ++bitsCount;
    if (bitsCount == 8) {
      buffer.push_back(accumulator);
      bitsCount = 0;
      accumulator = 0;
    }
  }

  void WriteByte(byte byte_) {
    if (bitsCount == 0) {
      buffer.push_back(byte_);
    } else {
      accumulator += byte_ >> bitsCount;
      buffer.push_back(accumulator);
      accumulator = byte_ << (8 - bitsCount);
    }
  }

  // Если последний байт заполнен неполностью - незаполненые биты будут нулями
  // В конец записывем дополнительный байт, означающий сколько лишних бит было дописано в предыдущий байт
  std::vector<byte> GetResult() {
    if (bitsCount != 0) { // Добавляем в буфер accumulator, только в случае, если в нем что-то есть.
      buffer.push_back(accumulator);
      buffer.push_back(8 - bitsCount);
    } else {              // Иначе добавляем только указание, что лишних бит нет
       buffer.push_back(0);
    }
    return buffer;
  }

 private:
  std::vector<byte> buffer;
  byte accumulator = 0; // формируемый из битов байт
  int bitsCount = 0;    // сколько бит уже записано в accumulator
};

// считывет из IInputStream биты и байты в предположении,
// что в последнем байте указано, сколько лишних бит записано в предыдущий байт
// возвращает false, когда больше считать нельзя
class BitsReader {
 public:
  bool ReadBit(bool& value) {
    if (bitsCount == 0) {
      accumulator = nextByte;
      nextByte = nextNextByte;
      hasNext = input.Read(nextNextByte);
    }
    if (!hasNext && bitsCount > 7 - nextByte) return false; // Возвращаем не все биты предпоследнего байта
    if (!hasNext && nextByte == 0 && bitsCount == 7) { // без этого блока bitsCount обнулится и предыдущий if никогда не сработает, если nextByte = 0
        value = accumulator & (1 << (7 - bitsCount));
        bitsCount = 8; // чтобы выполнялось условие предыдущего "if"
        return true;
    }
    value = accumulator & (1 << (7 - bitsCount));
    bitsCount = (bitsCount + 1) % 8;
    return true;
  }

  bool ReadByte(byte& value) {
    byte char_ = 0;
    for (int i = 0; i < 8; ++i) {
      bool bit;
      if (!ReadBit(bit)) return false;;
      char_ += (static_cast<byte>(bit) << (7 - i));
    }
    value = char_;
    return true;
  }

  BitsReader(IInputStream& input): input(input) {
    input.Read(nextByte);
    hasNext = input.Read(nextNextByte);
    assert(hasNext);
  }

 private:
  IInputStream& input;
  byte accumulator = 0;  // считанный из потока "текущий" байт
  byte nextByte = 0;     // следующий за accumulator
  byte nextNextByte = 0; // следующий за nextByte
  int bitsCount = 0;     // сколько бит из accumulator уже прочитано
  bool hasNext = false;  // конец потока; в accumulator последние биты файла, nextByte = числу лишних бит
};


struct BinaryTreeNode {
  byte char_;         // "символ", байт файла
  unsigned int value; // частота встречаемости
  BinaryTreeNode* left;
  BinaryTreeNode* right;

  BinaryTreeNode(byte char_,
                 unsigned int value,
                 BinaryTreeNode* left,
                 BinaryTreeNode* right)
    : char_(char_)
    , value(value)
    , left(left)
    , right(right) {}

  ~BinaryTreeNode() {
    delete left;
    delete right;
  }
};


BinaryTreeNode* buildHTree(std::vector<byte>& file) {
  assert(!file.empty());
	// считаем частоты
	std::vector<byte> freqs(256, 0);
	for (byte char_ : file) {
    ++freqs[char_];
	}
	// Очередь с приоритетом для получения поддерева с минимальным значением
	auto cmp = [](BinaryTreeNode* a, BinaryTreeNode* b)->bool{ return a->value > b->value; };
	std::priority_queue<BinaryTreeNode*, std::vector<BinaryTreeNode*>, decltype(cmp)> queue(cmp);
  for (int char_ = 0; char_ < 256; ++char_) {
    if (freqs[char_] > 0)
      queue.push(new BinaryTreeNode(char_, freqs[char_], nullptr, nullptr));
  }
  // Строим дерево Хаффмана
  while (queue.size() >= 2) {
    BinaryTreeNode* first = queue.top();
    queue.pop();
    BinaryTreeNode* second = queue.top();
    queue.pop();
    queue.push(new BinaryTreeNode(0, first->value + second->value, first, second));
  }
  return queue.top();
}

// Обходим рекурсивно дерево: сначала в левый лист, затем в правый
// Если пришли в лист печатаем бит "1" и следом байт-символ, хранящийся в листе
// В нелистовом узле печатаем бит "0"
void encodeHTree(BinaryTreeNode* node, BitsWriter& output) {
  if (node == nullptr) return;
  if (node->left == nullptr && node->right == nullptr) {
    output.WriteBit(true);
    output.WriteByte(node->char_);
  } else {
    output.WriteBit(false);
    encodeHTree(node->left, output);
    encodeHTree(node->right, output);
  }
}

// Рекурсивно строим дерево: сначала добавляем левого сына, затем правого
// Читаем бит из потока. Если "1", читаем следом байт-символ и возвращаем соответствующий лист
// Если "0", возвращаем узел с рекурентно добытым левым и правым сыновьями
BinaryTreeNode* decodeHTree(BitsReader& bitsReader) {
  BinaryTreeNode* node;
  bool bit;
  if(!bitsReader.ReadBit(bit)) assert(false);
  if (bit) {
    byte char_;
    bitsReader.ReadByte(char_);
    node = new BinaryTreeNode(char_, 0, nullptr, nullptr);
  } else {
    BinaryTreeNode* left = decodeHTree(bitsReader);
    BinaryTreeNode* right = decodeHTree(bitsReader);
    node = new BinaryTreeNode(0, 0, left, right);
  }
  return node;
}

// Получаем коды символов из дерева Хаффмана
// Налево - 0, направо - 1
void fillTableOfCodes(BinaryTreeNode* node, std::unordered_map<byte, std::vector<bool>>& tableOfCodes, std::vector<bool>& code) {
  if (node == nullptr) {
    code.pop_back();
  } else if (node->left == nullptr && node->right == nullptr) {
    tableOfCodes[node->char_] = code;
    code.pop_back();
  } else {
    code.push_back(false);
    fillTableOfCodes(node->left, tableOfCodes, code);
    code.push_back(true);
    fillTableOfCodes(node->right, tableOfCodes, code);
    code.pop_back();
  }
}


void Encode(IInputStream& original, IOutputStream& compressed) {
  // считываем файл по байтам
  std::vector<byte> originalFile;
  byte oneByte = 0;
  while (original.Read(oneByte)) {
		originalFile.push_back(oneByte);
	}
	// Строим дерево Хаффмана
  BinaryTreeNode* hTree = buildHTree(originalFile);
  // Записываем дерево в начало архива
  BitsWriter bitsWriter;
  encodeHTree(hTree, bitsWriter);
  // Кодируем и записываем последовательность байт исходного файла
  std::unordered_map<byte, std::vector<bool>> tableOfCodes;
  std::vector<bool> tmp_vector;
  fillTableOfCodes(hTree, tableOfCodes, tmp_vector);

  for (byte char_ : originalFile) {
    for (bool bit : tableOfCodes[char_])
      bitsWriter.WriteBit(bit);
  }
  for (byte byte_ : bitsWriter.GetResult()) {
    compressed.Write(byte_);
  }
  delete hTree;
}


void Decode(IInputStream& compressed, IOutputStream& original) {
	BitsReader bitsReader(compressed);
  BinaryTreeNode* hTree = decodeHTree(bitsReader);
  bool bit;
  BinaryTreeNode* node = hTree;
  if (node->left == nullptr && node->right == nullptr) assert(false);
  while (true) {
    if (node->left == nullptr && node->right == nullptr) { // попали в лист - записываем символ из него, переходим к корню
      original.Write(node->char_);
      node = hTree;
    } else {
       if (!bitsReader.ReadBit(bit)) return; // если не в листе считывем бит
       if (bit) node = node->right; // "1" - идем вправо
       else node = node->left; // "0" - идем влево
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Входной поток
class CInputStream : public IInputStream {
public:
	CInputStream(const vector<byte>& _body) : index(0), body(_body) {}

	virtual bool Read(byte& value);

private:
	unsigned int index;
	const vector<unsigned char>& body;
};

bool CInputStream::Read(byte& value)
{
	if (index >= body.size()) {
		return false;
	}

	value = body[index++];
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Выходной поток
class COutputStream : public IOutputStream {
public:
	COutputStream(vector<byte>& _body) : body(_body) { body.clear(); }
	virtual void Write(byte value) { body.push_back(value); }

private:
	vector<byte>& body;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Основное приложение

// Считываем все файлы, что перечислены во входе
void fillInputs(vector< vector<byte> >& inputs)
{
	inputs.clear();

	string currentFileName;
	getline( cin, currentFileName );


		// Есть еще один файл, который следует закодировать
		inputs.push_back(vector<byte>());

		ifstream file;
		file.open(currentFileName.c_str());

		byte value;
		while (file >> value) {
			(*inputs.rbegin()).push_back(value);
		}

}

bool isEqual(const vector<byte>& v1, const vector<byte>& v2)
{
	if (v1.size() != v2.size()) {
		return false;
	}

	for( unsigned int i = 0; i < v1.size(); i++ ) {
		if( v1[i] != v2[i] ) {
			return false;
		}
	}

	return true;
}

int calculateSize(const vector< vector<byte> >& array)
{
	int result = 0;
	for ( unsigned int i  = 0; i < array.size(); i++ ) {
		const vector<byte>& data = array[i];
		result += data.size();
	}

	return result;
}

int main()
{
	// Получаем данные, которые нужно закодировать
	vector< vector<byte> > input;

	fillInputs(input);

	// Сжимаем данные
	vector< vector<byte> > compressed;
	compressed.resize(input.size());
	for (unsigned int i = 0; i < input.size(); i++) {
		CInputStream iStream(input[i]);
		COutputStream oStream(compressed[i]);
		Encode(iStream, oStream);
	}

	// Распаковываем сжатые данные и проверяем, что они совпадают с оригиналом
	for (unsigned int i = 0; i < input.size(); i++) {
		vector<byte> output;
		CInputStream iStream(compressed[i]);
		COutputStream oStream(output);
		Decode(iStream, oStream);
		if (!isEqual(input[i], output)) {
			cout << -1 << endl;
			return 0;
		}
	}

	// Вычисляем степень сжатия
	cout << (100. * calculateSize(compressed) / calculateSize(input));

	return 0;
}
