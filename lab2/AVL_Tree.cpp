#include <cstdio>
#include <cstdint>
#include <cerrno>
#include <cstring>
#include <new>

const int MAX_WORD_LENGTH = 256;
const int KEY_BUFFER_SIZE = MAX_WORD_LENGTH + 1;
const int LINE_BUFFER_SIZE = 1024;
const int FILE_MAGIC_SIZE = 8;

const char STRING_END = '\0';
const char NEWLINE_CHAR = '\n';
const char CARRIAGE_RETURN_CHAR = '\r';
const char SPACE_CHAR = ' ';
const char TAB_CHAR = '\t';
const char UPPER_A_CHAR = 'A';
const char UPPER_Z_CHAR = 'Z';
const char LOWER_A_CHAR = 'a';
const char DIGIT_ZERO_CHAR = '0';
const char DIGIT_NINE_CHAR = '9';

const char FILE_MAGIC[FILE_MAGIC_SIZE] = {
    'A', 'V', 'L', 'D', 'I', 'C', 'T', '1'
};

struct TNode {
    char Key[KEY_BUFFER_SIZE];
    uint64_t Value;
    int Height;
    TNode* Left;
    TNode* Right;
};

int StrLen(const char* s);
void StrCopy(char* dst, const char* src);
int StrCompare(const char* a, const char* b);
char ToLowerChar(char c);
void ToLowerWord(char* s);
void RemoveNewline(char* s);
void SkipSpaces(const char* s, int& pos);
void ReadWord(const char* s, int& pos, char* word);
uint64_t ReadUInt64(const char* s, int& pos);
int StartsWith(const char* s, const char* pref);
int MaxInt(int a, int b);

int GetHeight(TNode* node);
void FixHeight(TNode* node);
int BalanceFactor(TNode* node);
TNode* RotateRight(TNode* node);
TNode* RotateLeft(TNode* node);
TNode* Balance(TNode* node);
TNode* CreateNode(const char* key, uint64_t value);
TNode* FindNode(TNode* node, const char* key);
TNode* InsertNode(TNode* node, const char* key, uint64_t value, bool& isInserted);
void ClearTree(TNode* node);
TNode* FindMin(TNode* node);
TNode* RemoveMin(TNode* node);
TNode* EraseNode(TNode* node, const char* key, bool& isErased);
uint64_t CountNodes(TNode* node);

bool WriteExact(const void* data, size_t size, size_t count, FILE* file);
bool ReadExact(void* data, size_t size, size_t count, FILE* file);
bool SaveNodes(TNode* node, FILE* file);
bool LoadNodesFromFile(FILE* file, uint64_t nodeCount, TNode*& root);
bool IsFileEmpty(FILE* file);
void PrintSystemError();
void PrintInvalidFormatError();

class TAvlDictionary {
public:
    TNode* Root;

    TAvlDictionary() {
        Root = 0;
    }

    ~TAvlDictionary() {
        ClearTree(Root);
    }

    bool Insert(const char* key, uint64_t value) {
        bool isInserted = false;
        Root = InsertNode(Root, key, value, isInserted);
        return isInserted;
    }

    bool Find(const char* key, uint64_t& value) const {
        TNode* foundNode = FindNode(Root, key);
        if (foundNode == 0) {
            return false;
        }
        value = foundNode->Value;
        return true;
    }

    bool Erase(const char* key) {
        bool isErased = false;
        Root = EraseNode(Root, key, isErased);
        return isErased;
    }

    void Clear() {
        ClearTree(Root);
        Root = 0;
    }

    bool Save(const char* path) const {
        FILE* file = std::fopen(path, "wb");
        if (file == 0) {
            return false;
        }

        bool isOk = true;
        uint64_t nodeCount = CountNodes(Root);

        if (!WriteExact(FILE_MAGIC, sizeof(char), FILE_MAGIC_SIZE, file)) {
            isOk = false;
        }

        if (isOk && !WriteExact(&nodeCount, sizeof(uint64_t), 1, file)) {
            isOk = false;
        }

        if (isOk && !SaveNodes(Root, file)) {
            isOk = false;
        }

        if (std::fclose(file) != 0) {
            isOk = false;
        }

        return isOk;
    }

    bool Load(const char* path) {
        errno = 0;
        FILE* file = std::fopen(path, "rb");

        if (file == 0) {
            if (errno == ENOENT) {
                Clear();
                return true;
            }
            return false;
        }

        if (IsFileEmpty(file)) {
            std::fclose(file);
            Clear();
            return true;
        }

        char magic[FILE_MAGIC_SIZE];
        uint64_t nodeCount = 0;
        TNode* newRoot = 0;
        bool isOk = true;

        if (!ReadExact(magic, sizeof(char), FILE_MAGIC_SIZE, file)) {
            isOk = false;
        }

        if (isOk) {
            int i = 0;
            while (i < FILE_MAGIC_SIZE) {
                if (magic[i] != FILE_MAGIC[i]) {
                    isOk = false;
                    break;
                }
                ++i;
            }
        }

        if (isOk && !ReadExact(&nodeCount, sizeof(uint64_t), 1, file)) {
            isOk = false;
        }

        if (isOk && !LoadNodesFromFile(file, nodeCount, newRoot)) {
            isOk = false;
        }

        if (isOk) {
            char extraByte = 0;
            size_t readCount = std::fread(&extraByte, sizeof(char), 1, file);
            if (readCount != 0) {
                isOk = false;
            }
        }

        std::fclose(file);

        if (!isOk) {
            ClearTree(newRoot);
            errno = 0;
            return false;
        }

        Clear();
        Root = newRoot;
        return true;
    }
};

int StrLen(const char* s) {
    int len = 0;
    while (s[len] != STRING_END) {
        ++len;
    }
    return len;
}

void StrCopy(char* dst, const char* src) {
    int i = 0;
    while (src[i] != STRING_END) {
        dst[i] = src[i];
        ++i;
    }
    dst[i] = STRING_END;
}

int StrCompare(const char* a, const char* b) {
    int i = 0;
    while (a[i] != STRING_END && b[i] != STRING_END) {
        if (a[i] < b[i]) {
            return -1;
        }
        if (a[i] > b[i]) {
            return 1;
        }
        ++i;
    }

    if (a[i] == STRING_END && b[i] == STRING_END) {
        return 0;
    }

    if (a[i] == STRING_END) {
        return -1;
    }

    return 1;
}

char ToLowerChar(char c) {
    if (c >= UPPER_A_CHAR && c <= UPPER_Z_CHAR) {
        return static_cast<char>(c - UPPER_A_CHAR + LOWER_A_CHAR);
    }
    return c;
}

void ToLowerWord(char* s) {
    int i = 0;
    while (s[i] != STRING_END) {
        s[i] = ToLowerChar(s[i]);
        ++i;
    }
}

void RemoveNewline(char* s) {
    int i = 0;
    while (s[i] != STRING_END) {
        if (s[i] == NEWLINE_CHAR || s[i] == CARRIAGE_RETURN_CHAR) {
            s[i] = STRING_END;
            return;
        }
        ++i;
    }
}

void SkipSpaces(const char* s, int& pos) {
    while (s[pos] == SPACE_CHAR || s[pos] == TAB_CHAR) {
        ++pos;
    }
}

void ReadWord(const char* s, int& pos, char* word) {
    int i = 0;
    while (s[pos] != STRING_END &&
           s[pos] != SPACE_CHAR &&
           s[pos] != TAB_CHAR &&
           s[pos] != NEWLINE_CHAR &&
           s[pos] != CARRIAGE_RETURN_CHAR) {
        if (i < MAX_WORD_LENGTH) {
            word[i] = s[pos];
            ++i;
        }
        ++pos;
    }
    word[i] = STRING_END;
}

uint64_t ReadUInt64(const char* s, int& pos) {
    uint64_t value = 0;
    while (s[pos] >= DIGIT_ZERO_CHAR && s[pos] <= DIGIT_NINE_CHAR) {
        value = value * 10 + static_cast<uint64_t>(s[pos] - DIGIT_ZERO_CHAR);
        ++pos;
    }
    return value;
}

int StartsWith(const char* s, const char* pref) {
    int i = 0;
    while (pref[i] != STRING_END) {
        if (s[i] != pref[i]) {
            return 0;
        }
        ++i;
    }
    return 1;
}

int MaxInt(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int GetHeight(TNode* node) {
    if (node == 0) {
        return 0;
    }
    return node->Height;
}

void FixHeight(TNode* node) {
    int leftHeight = GetHeight(node->Left);
    int rightHeight = GetHeight(node->Right);
    node->Height = MaxInt(leftHeight, rightHeight) + 1;
}

int BalanceFactor(TNode* node) {
    return GetHeight(node->Left) - GetHeight(node->Right);
}

TNode* RotateRight(TNode* node) {
    TNode* leftNode = node->Left;
    node->Left = leftNode->Right;
    leftNode->Right = node;

    FixHeight(node);
    FixHeight(leftNode);

    return leftNode;
}

TNode* RotateLeft(TNode* node) {
    TNode* rightNode = node->Right;
    node->Right = rightNode->Left;
    rightNode->Left = node;

    FixHeight(node);
    FixHeight(rightNode);

    return rightNode;
}

TNode* Balance(TNode* node) {
    FixHeight(node);

    if (BalanceFactor(node) == 2) {
        if (BalanceFactor(node->Left) < 0) {
            node->Left = RotateLeft(node->Left);
        }
        return RotateRight(node);
    }

    if (BalanceFactor(node) == -2) {
        if (BalanceFactor(node->Right) > 0) {
            node->Right = RotateRight(node->Right);
        }
        return RotateLeft(node);
    }

    return node;
}

TNode* CreateNode(const char* key, uint64_t value) {
    TNode* node = new TNode;
    StrCopy(node->Key, key);
    node->Value = value;
    node->Height = 1;
    node->Left = 0;
    node->Right = 0;
    return node;
}

TNode* FindNode(TNode* node, const char* key) {
    while (node != 0) {
        int compareResult = StrCompare(key, node->Key);

        if (compareResult == 0) {
            return node;
        }

        if (compareResult < 0) {
            node = node->Left;
        } else {
            node = node->Right;
        }
    }

    return 0;
}

TNode* InsertNode(TNode* node, const char* key, uint64_t value, bool& isInserted) {
    if (node == 0) {
        isInserted = true;
        return CreateNode(key, value);
    }

    int compareResult = StrCompare(key, node->Key);

    if (compareResult == 0) {
        isInserted = false;
        return node;
    }

    if (compareResult < 0) {
        node->Left = InsertNode(node->Left, key, value, isInserted);
    } else {
        node->Right = InsertNode(node->Right, key, value, isInserted);
    }

    return Balance(node);
}

void ClearTree(TNode* node) {
    if (node == 0) {
        return;
    }

    ClearTree(node->Left);
    ClearTree(node->Right);
    delete node;
}

TNode* FindMin(TNode* node) {
    if (node->Left == 0) {
        return node;
    }
    return FindMin(node->Left);
}

TNode* RemoveMin(TNode* node) {
    if (node->Left == 0) {
        return node->Right;
    }

    node->Left = RemoveMin(node->Left);
    return Balance(node);
}

TNode* EraseNode(TNode* node, const char* key, bool& isErased) {
    if (node == 0) {
        isErased = false;
        return 0;
    }

    int compareResult = StrCompare(key, node->Key);

    if (compareResult < 0) {
        node->Left = EraseNode(node->Left, key, isErased);
        if (node == 0) {
            return 0;
        }
        return Balance(node);
    }

    if (compareResult > 0) {
        node->Right = EraseNode(node->Right, key, isErased);
        if (node == 0) {
            return 0;
        }
        return Balance(node);
    }

    isErased = true;

    TNode* leftSubtree = node->Left;
    TNode* rightSubtree = node->Right;
    delete node;

    if (rightSubtree == 0) {
        return leftSubtree;
    }

    TNode* minNode = FindMin(rightSubtree);
    minNode->Right = RemoveMin(rightSubtree);
    minNode->Left = leftSubtree;

    return Balance(minNode);
}

uint64_t CountNodes(TNode* node) {
    if (node == 0) {
        return 0;
    }

    return 1 + CountNodes(node->Left) + CountNodes(node->Right);
}

bool WriteExact(const void* data, size_t size, size_t count, FILE* file) {
    return std::fwrite(data, size, count, file) == count;
}

bool ReadExact(void* data, size_t size, size_t count, FILE* file) {
    return std::fread(data, size, count, file) == count;
}

bool SaveNodes(TNode* node, FILE* file) {
    if (node == 0) {
        return true;
    }

    if (!SaveNodes(node->Left, file)) {
        return false;
    }

    uint16_t keyLength = static_cast<uint16_t>(StrLen(node->Key));

    if (!WriteExact(&keyLength, sizeof(uint16_t), 1, file)) {
        return false;
    }

    if (keyLength > 0) {
        if (!WriteExact(node->Key, sizeof(char), keyLength, file)) {
            return false;
        }
    }

    if (!WriteExact(&node->Value, sizeof(uint64_t), 1, file)) {
        return false;
    }

    if (!SaveNodes(node->Right, file)) {
        return false;
    }

    return true;
}

bool LoadNodesFromFile(FILE* file, uint64_t nodeCount, TNode*& root) {
    root = 0;
    uint64_t i = 0;

    while (i < nodeCount) {
        uint16_t keyLength = 0;
        char key[KEY_BUFFER_SIZE];
        uint64_t value = 0;
        bool isInserted = false;

        int j = 0;
        while (j < KEY_BUFFER_SIZE) {
            key[j] = STRING_END;
            ++j;
        }

        if (!ReadExact(&keyLength, sizeof(uint16_t), 1, file)) {
            ClearTree(root);
            root = 0;
            return false;
        }

        if (keyLength > MAX_WORD_LENGTH) {
            ClearTree(root);
            root = 0;
            return false;
        }

        if (keyLength > 0) {
            if (!ReadExact(key, sizeof(char), keyLength, file)) {
                ClearTree(root);
                root = 0;
                return false;
            }
        }

        key[keyLength] = STRING_END;

        if (!ReadExact(&value, sizeof(uint64_t), 1, file)) {
            ClearTree(root);
            root = 0;
            return false;
        }

        root = InsertNode(root, key, value, isInserted);
        if (!isInserted) {
            ClearTree(root);
            root = 0;
            return false;
        }

        ++i;
    }

    return true;
}

bool IsFileEmpty(FILE* file) {
    if (std::fseek(file, 0, SEEK_END) != 0) {
        return false;
    }

    long size = std::ftell(file);
    if (size < 0) {
        std::rewind(file);
        return false;
    }

    std::rewind(file);
    return size == 0;
}

void PrintSystemError() {
    const char* errorText = std::strerror(errno);
    if (errorText == 0) {
        std::printf("ERROR: unknown system error\n");
        return;
    }
    std::printf("ERROR: %s\n", errorText);
}

void PrintInvalidFormatError() {
    std::printf("ERROR: invalid file format\n");
}

int main() {
    TAvlDictionary dictionary;
    char line[LINE_BUFFER_SIZE];

    while (std::fgets(line, LINE_BUFFER_SIZE, stdin) != 0) {
        RemoveNewline(line);

        try {
            if (line[0] == '+') {
                int pos = 1;
                char word[KEY_BUFFER_SIZE];
                uint64_t value = 0;

                SkipSpaces(line, pos);
                ReadWord(line, pos, word);
                ToLowerWord(word);

                SkipSpaces(line, pos);
                value = ReadUInt64(line, pos);

                if (dictionary.Insert(word, value)) {
                    std::printf("OK\n");
                } else {
                    std::printf("Exist\n");
                }
            } else if (line[0] == '-') {
                int pos = 1;
                char word[KEY_BUFFER_SIZE];

                SkipSpaces(line, pos);
                ReadWord(line, pos, word);
                ToLowerWord(word);

                if (dictionary.Erase(word)) {
                    std::printf("OK\n");
                } else {
                    std::printf("NoSuchWord\n");
                }
            } else if (line[0] == '!') {
                if (StartsWith(line, "! Save")) {
                    int pos = 6;
                    char path[LINE_BUFFER_SIZE];

                    SkipSpaces(line, pos);
                    ReadWord(line, pos, path);

                    errno = 0;
                    if (dictionary.Save(path)) {
                        std::printf("OK\n");
                    } else {
                        if (errno != 0) {
                            PrintSystemError();
                        } else {
                            std::printf("ERROR: save failed\n");
                        }
                    }
                } else if (StartsWith(line, "! Load")) {
                    int pos = 6;
                    char path[LINE_BUFFER_SIZE];

                    SkipSpaces(line, pos);
                    ReadWord(line, pos, path);

                    errno = 0;
                    if (dictionary.Load(path)) {
                        std::printf("OK\n");
                    } else {
                        if (errno != 0) {
                            PrintSystemError();
                        } else {
                            PrintInvalidFormatError();
                        }
                    }
                }
            } else if (line[0] != STRING_END) {
                int pos = 0;
                char word[KEY_BUFFER_SIZE];
                uint64_t value = 0;

                ReadWord(line, pos, word);
                ToLowerWord(word);

                if (dictionary.Find(word, value)) {
                    std::printf("OK: %llu\n", static_cast<unsigned long long>(value));
                } else {
                    std::printf("NoSuchWord\n");
                }
            }
        } catch (const std::bad_alloc&) {
            std::printf("ERROR: not enough memory\n");
        }
    }

    return 0;
}