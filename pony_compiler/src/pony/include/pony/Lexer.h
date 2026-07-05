//===- Lexer.h - Lexer for the Pony language
//-------------------------------===//

#ifndef PONY_LEXER_H
#define PONY_LEXER_H

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

namespace pony {

/// Structure definition a location in a file.
struct Location {
  std::shared_ptr<std::string> file;  ///< filename.
  int line;                           ///< line number.
  int col;                            ///< column number.
};

// List of Token returned by the lexer.
enum Token : int {
  tok_semicolon = ';',
  tok_parenthese_open = '(',
  tok_parenthese_close = ')',
  tok_bracket_open = '{',
  tok_bracket_close = '}',
  tok_sbracket_open = '[',
  tok_sbracket_close = ']',

  tok_eof = -1,

  tok_return = -2,
  tok_var = -3,
  tok_def = -4,

  tok_identifier = -5,
  tok_number = -6,
};

/// The Lexer is an abstract base class providing all the facilities that the
/// Parser expects. It goes through the stream one token at a time and keeps
/// track of the location in the file for debugging purpose.
/// It relies on a subclass to provide a `readNextLine()` method. The subclass
/// can proceed by reading the next line from the standard input or from a
/// memory mapped file.
class Lexer {
 public:
  /// Create a lexer for the given filename. The filename is kept only for
  /// debugging purpose (attaching a location to a Token).
  Lexer(std::string filename)
      : lastLocation(
            {std::make_shared<std::string>(std::move(filename)), 0, 0}) {}
  virtual ~Lexer() = default;

  /// Look at the current token in the stream.
  Token getCurToken() { return curTok; }

  /// Move to the next token in the stream and return it.
  Token getNextToken() { return curTok = getTok(); }

  /// Move to the next token in the stream, asserting on the current token
  /// matching the expectation.
  void consume(Token tok) {
    assert(tok == curTok && "consume Token mismatch expectation");
    getNextToken();
  }

  /// Return the current identifier (prereq: getCurToken() == tok_identifier)
  llvm::StringRef getId() {
    assert(curTok == tok_identifier);
    return identifierStr;
  }

  double getValue() {
    assert(curTok == tok_number);
    return numVal;
  }

  /// Return the location for the beginning of the current token.
  Location getLastLocation() { return lastLocation; }

  // Return the current line in the file.
  int getLine() { return curLineNum; }

  // Return the current column in the file.
  int getCol() { return curCol; }

 private:
  /// Delegate to a derived class fetching the next line. Returns an empty
  /// string to signal end of file (EOF). Lines are expected to always finish
  /// with "\n"
  virtual llvm::StringRef readNextLine() = 0;

  // TODO: Implement function getNextChar().

  // Function description:
  // 该函数从curLineBuffer中获取当前行的下一个char，如果已经处理到当前行最后一个char，则通过读取下一行
  //                       来更新curLineBuffer以确保curLineBuffer非空。

  // Hints: 1. 函数实现过程中可能会用到lexer的部分成员变量（如curLineBuffer）；
  //        2.
  //        注意读到文档结尾，读到某一行结尾等特殊情况的处理。一般来说，读到文档结尾应返回EOF，某一行结尾最后一个char为'\n'；
  //        3. 注意行列位置信息的同步更新；
  //        4. 关于llvm::StringRef的部分函数：llvm::StringRef example;
  //        example.front(); example.drop_front(); example.empty()。
  //为什么返回的是int类型而不是char类型？因为需要返回EOF，EOF是一个特殊的值，通常定义为-1，而char类型无法表示负数，所以使用int类型来返回char和EOF。
  int getNextChar() {
    //处理结尾和行尾的情况
    if (curLineBuffer.empty())
    {
      curLineBuffer = readNextLine();
      //更新行列位置信息
      if (curLineBuffer.empty())
      {
        return EOF;
      }
      curLineNum++;
      curCol = 0;
    }
    char nextChar = curLineBuffer.front();
    curLineBuffer = curLineBuffer.drop_front(); //去掉开头字符
    curCol++;
    return nextChar;
  }

  ///  Return the next token from standard input.
  Token getTok() {
    // Skip any whitespace.
    
    while (isspace(lastChar)) lastChar = Token(getNextChar());

    // Save the current location before reading the token characters.
    //保存第一个token的位置信息
    lastLocation.line = curLineNum;
    lastLocation.col = curCol;

    // TODO: 补充成员函数getTok()。
    //       1. 能够识别“return”、“def”和“var”三个关键字，需要注意其各类大小写不规范书写形式；
    //       2. 能够识别标识符（函数名，变量名等）：
    //          • 标识符由字母、数字或下划线组成；
    //          • 标识符以字母或下划线开头；
    //          • 标识符中有数字时，数字不可连续出现
    //          例如：有效的标识符可以是 ab3c, _b, placeholder 等。
    //       3.
    //       在识别每种Token的同时，将其存放在某种数据结构中，以便最终在终端输出
    //
    // Hints: 1. 在实现第1，2点时，可参考getTok()函数中现有的识别数字的方法。
    //        2. 一些有用的函数:  isalpha(); isalnum();
    /*
     *
     *  Write your code here.
     *
     */

    // TODO: 3.
    // 改进识别数字的方法，使编译器可以识别并在终端报告非法数字，非法表示包括：9.9.9，9..9，.999，..9，9..，9e01等。
    //isdigit()函数用于检查一个字符是否为数字字符（0-9）。在识别数字时，我们需要确保数字的格式合法，例如小数点只能出现一次，不能连续出现，科学计数法中的e后面必须跟一个数字等。我们可以通过在识别数字的过程中添加一些状态变量来跟踪小数点和科学计数法的使用情况，从而判断数字是否合法。

    if (isdigit(lastChar) || lastChar == '.') {
  std::string numStr;
  int dotCount = 0;

  //读取基础数字和小数点部分
  do {
    if (lastChar == '.') {
      dotCount++;
    }
    numStr += lastChar;
    lastChar = Token(getNextChar());
  } while (isdigit(lastChar) || lastChar == '.');

  //定义一个 lambda 表达式
  auto eatGarbage = [&]() {
    while (isalnum(lastChar) || lastChar == '.' || lastChar == '_') {
      lastChar = Token(getNextChar());
    }
  };

  //检查小数点格式
  if (numStr.front() == '.' || numStr.back() == '.') {
    std::string errorStr = std::to_string(lastLocation.line) + ":" + std::to_string(lastLocation.col) + ": ERROR: Invalid number format: decimal point cannot be at the beginning or end. ";
    tokenStrings.push_back(errorStr);
    eatGarbage();
    return getNextToken(); 
  }
  if (dotCount > 1) {
    std::string errorStr = std::to_string(lastLocation.line) + ":" + std::to_string(lastLocation.col) + ": ERROR: Invalid number format: multiple decimal points are not allowed. ";
    tokenStrings.push_back(errorStr);
    eatGarbage();
    return getNextToken(); 
  }

  //处理科学计数法 e/E
  if (lastChar == 'e' || lastChar == 'E') {
    numStr += lastChar;
    lastChar = Token(getNextChar());

    
    if (lastChar >= '1' && lastChar <= '9') {
      //第一位合法，继续读取后面的有效数字
      while (isdigit(lastChar)) {
        numStr += lastChar;
        lastChar = Token(getNextChar());
      }
    } else {
      //报错：e 后面跟了 0 (如 9e0, 9e01), 或者非数字, 符号等
      //不直接输出报错，而是把报错信息存入 tokenStrings 中以便最终在 ponyc.cpp 遍历输出
      std::string errorStr = std::to_string(lastLocation.line) + ":" + std::to_string(lastLocation.col) + ": ERROR: Invalid number format: scientific notation 'e' must be followed by 1-9.\n";
      tokenStrings.push_back(errorStr);

      eatGarbage();
      return getNextToken();
    }
  }

  //检查是否有非法的后缀尾巴 (比如输入是 9a, 9e10abc 等)
  if (isalpha(lastChar) || lastChar == '_') {
    std::string errorStr = std::to_string(lastLocation.line) + ":" + std::to_string(lastLocation.col) + ": ERROR: Invalid number format: unexpected character '" + (char)lastChar + "' after number.\n";
    tokenStrings.push_back(errorStr);
    eatGarbage();
    return getNextToken();
  }

  //成功识别
  numVal = strtod(numStr.c_str(), nullptr);
  //存入 tokenStrings 中以便最终在 ponyc.cpp 遍历输出
  tokenStrings.push_back(numStr);
  return tok_number;
}

    if (lastChar == '#') {
      //识别注释，注释以#开头，直到行尾结束
      // Comment until end of line.
      do {
        lastChar = Token(getNextChar());
      } while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

      if (lastChar != EOF) return getTok();
    }
    //识别关键字和标识符
    if (isalpha(lastChar) || lastChar == '_') 
      {
        std::string identifierStr;
        bool flag = true;
        char frontNum = ' ';
        do {
          if (isdigit(lastChar))
          {
            if (isdigit(frontNum))
            {
              flag = false;
            }
          }
          frontNum = lastChar;
          identifierStr += lastChar;
          lastChar = Token(getNextChar());
        } while (isalnum(lastChar) || lastChar == '_' || isalpha(lastChar));
        //识别到一个完整的标识符，判断它是否是关键字
        if (!flag)
        {
          std::string errorStr = std::to_string(lastLocation.line) + ":" + std::to_string(lastLocation.col) + ": ERROR: Invalid identifier format: consecutive digits are not allowed in identifiers. ";
          tokenStrings.push_back(errorStr);
          return getNextToken();
        }
        std::string lowerIdentifierStr;
        for (char c : identifierStr)        {
          lowerIdentifierStr += tolower(c);
        }
        if (lowerIdentifierStr == "return") { tokenStrings.push_back(lowerIdentifierStr); return tok_return; }
        if (lowerIdentifierStr == "def") { tokenStrings.push_back(lowerIdentifierStr); return tok_def; }
        if (lowerIdentifierStr == "var") { tokenStrings.push_back(lowerIdentifierStr); return tok_var; }
        //如果不是关键字，那么就是一个普通的标识符
        this->identifierStr = identifierStr; //保存标识符的字符串值
        tokenStrings.push_back(identifierStr); //存入 tokenStrings 中以便最终在 ponyc.cpp 遍历输出
        return tok_identifier;
        }
      
    // Check for end of file.  Don't eat the EOF.
    if (lastChar == EOF) return tok_eof;

    // Otherwise, just return the character as its ascii value.
    Token thisChar = Token(lastChar);
    lastChar = Token(getNextChar());
    tokenStrings.push_back(std::string(1, thisChar));
    return thisChar;
  }

  /// The last token read from the input.
  Token curTok = tok_eof; //这个是当前的token

  /// Location for `curTok`.
  Location lastLocation; ///这个是当前token所在的位置

  /// If the current Token is an identifier, this string contains the value.
  std::string identifierStr; //这个是当前token的字符串值，比如如果当前token是一个标识符，那么identifierStr就存储了这个标识符的名字

  /// If the current Token is a number, this contains the value.
  double numVal = 0; //这个是当前token的数值，如果当前token是一个数字，那么numVal就存储了这个数字的值

  /// The last value returned by getNextChar(). We need to keep it around as we
  /// always need to read ahead one character to decide when to end a token and
  /// we can't put it back in the stream after reading from it.
  Token lastChar = Token(' '); //这个是上一个读取的字符，初始值为一个空格

  /// Keep track of the current line number in the input stream
  int curLineNum = 0; ///这个是当前行号

  /// Keep track of the current column number in the input stream
  int curCol = 0; ///这个是当前列号
  //lastLocation记录了当前token所在的位置，curLineNum和curCol记录了当前行号和列号，这些信息可以用来在编译过程中进行错误报告等操作。
  //lastLocation和
  /// Buffer supplied by the derived class on calls to `readNextLine()`
  llvm::StringRef curLineBuffer = "\n"; //这个是当前行的内容，初始值为一个换行符
public:
  std::vector<std::string> tokenStrings;  //新加的
  //每次识别到一个合法的token，就放进去
};

/// A lexer implementation operating on a buffer in memory.
class LexerBuffer final : public Lexer {
 public:
  LexerBuffer(const char *begin, const char *end, std::string filename)
      : Lexer(std::move(filename)), current(begin), end(end) {}

 private:
  /// Provide one line at a time to the Lexer, return an empty string when
  /// reaching the end of the buffer.
  llvm::StringRef readNextLine() override {
    auto *begin = current;
    while (current <= end && *current && *current != '\n') ++current;
    if (current <= end && *current) ++current;
    llvm::StringRef result{begin, static_cast<size_t>(current - begin)};
    return result;
  }
  const char *current, *end;
};
  // namespace pony
}
#endif  // PONY_LEXER_H
