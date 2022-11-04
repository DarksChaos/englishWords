#include <iostream>
#include <fstream>
#include "main.hpp"
#include <string.h>

// ./prog -in words.txt -out found.txt -sub ab -mode 5 -value1 2 -value2 5

#define HELP_MESSAGE "\n\
Special commands:\n\
'--help:':    help message\n\
'--newsort':  create a new sorting info that will be added'\n\
INFO: Before writing to the output file, you will be prompted to confirm writing.\n\
Format: './prog [input] [output] [args ...]'\n\n\
Args:\n\
'-sub [substring]': set substring\n\
'-mode [mode]': set mode\n\
'-value1 [value]': set value 1 (optional depending on mode)\n\
'-value2 [value]': set value 2 (also optional)\n\
'-case [true/false]': case sensitivity : true will consider 'E' and 'e' as different letters.\n\
'-delim [delimiter]' set delimiter (optional: default delimiter is ',')\n\n\
MODES: modes define how words will be chosen (note: limit values are included)\n\
0: word must not contain the substring\n\
1: word must contain the substring\n\
2: word must contain the substring exactly [value1] times\n\
3: word must contain the substring at least [value1] times\n\
4: word must contain the substring at most [value1] times\n\
5: word must contain the substring not [value1] times\n\
6: word must contain the substring between [value1] and [value2] times\n\
7: word must contain the substring a number of times not included between [value1] and [value2] times\n"

#define CREATE_INFO                                                                             \
    if (input.size() == 0)                                                                      \
        Exit("Input name not given", 1);                                                        \
    if (output.size() == 0)                                                                     \
        Exit("Output name not given", 1);                                                       \
    if (substring.size() == 0)                                                                  \
        Exit("Substring not given", 1);                                                         \
    switch (mode)                                                                               \
    {                                                                                           \
    case 0:                                                                                     \
    case 1:                                                                                     \
        break;                                                                                  \
    case 6:                                                                                     \
    case 7:                                                                                     \
        if (value2 < 0)                                                                         \
            Exit("Value 2 required, not provided", 1);                                          \
    case 2:                                                                                     \
    case 3:                                                                                     \
    case 4:                                                                                     \
    case 5:                                                                                     \
        if (value1 < 0)                                                                         \
            Exit("Value1 required, not provided", 1);                                           \
        break;                                                                                  \
    default:                                                                                    \
        Exit("Invalid mode " + std::to_string(mode) + " please provide a valid mode (0-7)", 1); \
    }                                                                                           \
    currentInfo = makeSortingInfo(substring, mode, value1, value2, caseSensitive);              \
    (*sortingInfo).push_back(currentInfo);

struct ParsingInfo
{
    std::fstream input;
    std::fstream output;
    char delimiter;
};

struct SortingInfo
{
    std::string substring;
    int mode;
    int value1;
    int value2;
    bool caseSensitive;
};

ParsingInfo makeParsingInfo(std::string input, std::string output, char delimiter)
{
    ParsingInfo info;
    info.delimiter = delimiter;
    info.input.open(input, std::ios::in);
    info.output.open(output, std::ios::out);
    if (!info.input.is_open())
        Exit("Input file " + input + " cannot be open", 1);
    if (!info.output.is_open())
        Exit("Output file " + output + " cannot be open", 1);
    return info;
}

SortingInfo makeSortingInfo(std::string substring, int mode, int value1, int value2, bool caseSensitive)
{
    SortingInfo info;
    info.substring = substring;
    info.mode = mode;
    info.value1 = value1;
    info.value2 = value2;
    info.caseSensitive = caseSensitive;
    return info;
}

void printSortingInfo(SortingInfo info)
{
    log("Substring: " + info.substring + "\nMode: " + std::to_string(info.mode) + "\nValue 1: " + std::to_string(info.value1) +
        "\nValue 2: " + std::to_string(info.value2) + "\nCase: " + std::to_string(info.caseSensitive) + "\n");
}

void Exit(std::string msg, int code)
{
    std::cout << (code == 0 ? "EXITING: " : "ERROR: ") << msg << " (code " << code << ") (type --help)";
    exit(code);
}

void sort_words(std::vector<std::string> *wordList, std::vector<std::string> *passedList, std::string substring, int mode, int val1, int val2, bool caseSensitive)
{
    for (int i = 0; i < (*wordList).size(); i++)
    {
        bool passed;
        int timesFound = contains((*wordList).at(i), substring, caseSensitive);
        switch (mode)
        {
        case 0: // not contained
            passed = (timesFound == 0);
            break;
        case 1: // contained
            passed = (timesFound > 0);
            break;
        case 2: // contained x times
            passed = (timesFound == val1);
            break;
        case 3: // contained x or more times
            passed = (timesFound >= val1);
            break;
        case 4: // contained x or less times
            passed = (timesFound <= val1);
            break;
        case 5: // contained not x times
            passed = (timesFound != val1);
            break;
        case 6: // contained between x and y times
            passed = (val1 <= timesFound <= val2);
            break;
        case 7: // contained under x or over y
            passed = (timesFound < val1 || timesFound > val2);
            break;
        default:
            passed = false;
        }
        if (passed)
        {
            passedList->push_back(wordList->at(i));
        }
    }
}

int contains(std::string string, std::string substring, bool caseSensitive)
{
    int size1 = string.size(), size2 = substring.size();
    if (size1 < size2)
        return false;
    int found = 0;
    bool broken = false;
    for (int i = 0; i < size1; i++) // check if substring starts at string[i]
    {
        broken = false;
        for (int j = i; j < i + size2; j++) // check for match char by char
        {
            char str1 = string[j], str2 = substring[j - i];
            if ((caseSensitive && str1 != str2) || std::tolower(str1) != std::tolower(str2))
            {
                broken = true;
            }
            if (broken)
                break;
        }
        if (!broken)
            found++;
    }
    return found;
}

void process_args(int argc, char **argv, ParsingInfo *parsingInfo, std::vector<SortingInfo> *sortingInfo)
{
    SortingInfo currentInfo;
    bool correct = true, correctArg = true, sortingArgGiven = false, caseSensitive = true;
    std::string input, output, substring;
    int mode = 1, value1 = -1, value2 = -1; // mode set by default to "word must contain substring"
    char delimiter = ',';

    int i = 1;
    if (argc >= 3)
    {
        input = argv[1], output = argv[2];
        i = 3;
    }

    // arguments
    for (; i < argc; i++)
    {
        correctArg = true;
        // funcs that require no params
        std::string argument(argv[i]), param;
        if (argument == "--help")
        {
            std::cout << HELP_MESSAGE;
            exit(0);
        }
        if (argument == "--newsort")
        {
            // check if sorting info has been given (prevent error if --newsort is called before any other argument)
            if (sortingArgGiven)
            {
                CREATE_INFO;
            }
        }
        // funcs that require next arg
        else if (i != argc - 1)
        {
            param = argv[i + 1];
            if (argument == "-sub")
            {
                substring = param;
                sortingArgGiven = true;
            }

            else if (argument == "-case")
            {
                if (param == "true")
                    caseSensitive = true;
                else if (param == "false")
                    caseSensitive = false;
                else
                    Exit("Incorrect boolean value after -case", 1);
                sortingArgGiven = true;
            }

            else if (argument == "-mode")
            {
                mode = std::stoi(param);
                sortingArgGiven = true;
            }

            else if (argument == "-value1" || argument == "-v1")
            {
                value1 = std::stoi(param);
                sortingArgGiven = true;
            }

            else if (argument == "-value2" || argument == "-v2")
            {
                value2 = std::stoi(param);
                sortingArgGiven = true;
            }

            else if (argument == "-delim")
            {
                delimiter = param[0];
            }
            else
                correctArg = correct = false;
            i++;
        }
        else
            correctArg = correct = false;

        // invalid funcs
        if (!correctArg)
            Exit("Incorrect command " + argument + (param.size() == 0 ? "" : " " + param), 1);
        (*parsingInfo) = makeParsingInfo(input, output, delimiter);
    }
    CREATE_INFO;
}

void log(std::string msg)
{
    if (msg == "DONE")
        std::cout << "DONE\n";
    else
        std::cout << "INFO: " << msg << ": ";
}

int main(int argc, char **argv)
{
    // INPUT --------------------------------------------------------------
    log("Step 1, parsing arguments");
    ParsingInfo parsingInfo;
    std::vector<SortingInfo> sortingInfo;
    process_args(argc, argv, &parsingInfo, &sortingInfo);

    // PARSE FILE ---------------------------------------------------------
    std::vector<std::string> wordList;
    std::string word;
    while (parsingInfo.input)
    {
        char c = parsingInfo.input.get();
        if (c == parsingInfo.delimiter)
        {
            wordList.push_back(word);
            word = "";
        }
        else
            word += c;
    }
    log("DONE");
    // SORT WORDS ---------------------------------------------------------
    log("Step 2, Applying sorts");
    std::vector<std::string> remainingWords = wordList, buffer;
    for (int i = 0; i < sortingInfo.size(); i++)
    {
        SortingInfo info = sortingInfo.at(i);
        sort_words(&remainingWords, &buffer, info.substring, info.mode, info.value1, info.value2, info.caseSensitive);
        remainingWords = buffer;
        buffer.clear();
    }
    log("DONE");
    // WRITE TO FILE ------------------------------------------------------
    log("Step 3, Writing to file " + std::to_string(remainingWords.size()) + " words");
    std::string confirm;
    int passed = -1;

    std::cout << "Confirm [Y/N]: ";
    std::cin >> confirm;
    switch (confirm[0])
    {
    case 'y':
    case 'Y':
        passed = 1;
        break;
    default:
        passed = 0;
        break;
    }

    if (passed == 0)
        Exit("Cancelled!", 0);

    std::string outputStr;
    int size = remainingWords.size();
    for (int i = 0; i < size; i++)
    {
        outputStr += remainingWords.at(i);
        if (i != size - 1)
            outputStr += ",";
    }

    parsingInfo.output << outputStr;
    parsingInfo.output.close();
    log("DONE");

    return EXIT_SUCCESS;
}