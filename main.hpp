#include <string>
#include <vector>

typedef struct ParsingInfo ParsingInfo;
typedef struct SortingInfo SortingInfo;

ParsingInfo makeParsingInfo(std::string input, std::string output, char delimiter);
SortingInfo makeSortingInfo(std::string substring, int mode, int value1, int value2, bool caseSensitive);

void Exit(std::string str, int code);
void sort_words(std::vector<std::string>* wordList, std::vector<std::string>* passedList, std::string substring, int mode, int val1, int val2, bool caseSensitive);
int contains(std::string string, std::string substring, bool caseSensitive);
void process_args(int argc, char **argv, std::string *input, std::string *output, std::string *substring, char *delimiter, int *mode, int *value1, int *value2);
int main(int argc, char **argv);
void log(std::string msg);