// CalcConsoleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
// by Shengchi Huang (shengchihuang@att.net)
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <assert.h> 
#include <nan.h>
using namespace std;
//-----------------------------------------------------
enum calcTokenType { invalid_calcToken, number_calcToken, operator_calcToken };
class CalcToken {
public:
    CalcToken();
    CalcToken(calcTokenType aType, string aValue);
    ~CalcToken();
    void setType(calcTokenType aType);
    calcTokenType getType() { return type;  }
    void setValue(string aValue);
    string getValue() { return valueStr; }
    int getValueNum() { return valueNum;  }
    bool isOperatorPrecedenceHigh();
    char getTag() { return valueStr[0];  }
private:
    calcTokenType type;
    string valueStr;
    int valueNum;
};
CalcToken::CalcToken() {
    setType(invalid_calcToken);
    setValue("");
}
CalcToken::~CalcToken() {
    //clean up
}
CalcToken::CalcToken(calcTokenType aType, string aValue) {
    setType(aType);
    setValue(aValue);
}
void CalcToken::setType(calcTokenType aType) {
    type = aType;
}
void CalcToken::setValue(string aValue) {
    valueStr = aValue;
    try {
        valueNum = stoi(valueStr);
        type = number_calcToken;
    }
    catch (...) {
        valueNum = -1;
    }
}
bool CalcToken::isOperatorPrecedenceHigh() {
    bool isOperator = (type == operator_calcToken);
    bool isMultiply = (valueStr.empty() == false) && (getTag() == '*');
    return isOperator && isMultiply;
}
//-----------------------------------------------------
class CalcEng {
public:
    CalcEng();
    CalcEng(string userInput);
    ~CalcEng();
    void setInputData(string data);
    string getInputData() { return rawData; }
    int getValue() { return finalValue; }
    bool DoCalculate();
private:
    bool parseRawData();
    string rawData;
    vector<string> strTokenList;
    int finalValue;
};
CalcEng::CalcEng() {
    setInputData("");
}
CalcEng::CalcEng(string userInput) : CalcEng() {
    setInputData(userInput);
}
CalcEng::~CalcEng() {
	//clean up
}
void CalcEng::setInputData(string data) {
    rawData = data;
    finalValue = 0;
}
bool CalcEng::parseRawData() {
    bool bParseSuccess = true;
    string value = "";
    for (unsigned int cur = 0; cur < rawData.length(); cur++) {
        char aChar = rawData[cur];
        //skip whitespaces
        if (isspace(aChar)) {
            continue;
        }
        //see if we have a digit
        if (isdigit(aChar)) {
            value += aChar;
            //peak ahead to see if next character is not a digit
            unsigned int next = cur + 1;
            bool isCurCharLastChar = (cur == rawData.length() - 1);
            bool isNextCharNotDigit = ((next <= rawData.length()-1) && (isdigit(rawData[next]) == false));
            if (isCurCharLastChar || isNextCharNotDigit) {
                //we found a number, save it, reset our temporary string to look for the next value
                strTokenList.push_back(value);
                value = "";
            }
            continue;
        }
        //see if we have an operator
        switch (aChar) {
        case '+':
        case '*':
            value = aChar;
            //we found an operator, save it, reset our temporary string to look for the next value
            strTokenList.push_back(value);
            value = "";
            continue;
        default:
            // operator is doesn't match any case constant
            break;
        }
        //invalid input, stop parsing
        bParseSuccess = false;
        break;
    } //for
    if (bParseSuccess == false) {
        strTokenList.clear();
        vector<string>(strTokenList).swap(strTokenList);
    }
    return bParseSuccess;
}
bool CalcEng::DoCalculate() {
    bool isValid = false;
    isValid = parseRawData();
    if (isValid == false) {
        return isValid;
    }
    isValid = false;
    stack<CalcToken* > myStack;
    CalcToken* pCurrentObj = NULL;
    for (string strToken : strTokenList) {        
        char aChar = strToken[0];
        calcTokenType aType = invalid_calcToken;
        switch (aChar) {
        case '+':
        case '*':
            aType = operator_calcToken;
            break;
        default:  //we found a number
            aType = number_calcToken;
            break;
        } //switch
        pCurrentObj = new CalcToken(aType, strToken);
        bool isFirstItem = myStack.empty();
        bool isCurrentTokenNumber = (pCurrentObj->getType() == number_calcToken);
        bool isCurrentTokenOperator = (pCurrentObj->getType() == operator_calcToken);
        bool isTopNumber = (!isFirstItem) && (myStack.top()->getType() == number_calcToken);
        bool isTopOperator = (!isFirstItem) && (myStack.top()->getType() == operator_calcToken);
        bool isTopOperatorPrecedenceHigh = (!isFirstItem) && (myStack.top()->isOperatorPrecedenceHigh());
        //make sure first item is a number
        if (isFirstItem && !isCurrentTokenNumber) {
            break;
        }
        //if top of stack is a number, make sure next item is an operator
        if (isTopNumber && !isCurrentTokenOperator) {
            break;
        }
        //if top of stack is an operator, make sure current item is a number
        if (isTopOperator && !isCurrentTokenNumber) {
            break;
        }
        //if current operator has a high precedence, then do the calculation
        if (isCurrentTokenNumber && isTopOperatorPrecedenceHigh) {
            CalcToken* pTopOperatorObj = myStack.top();
            myStack.pop();
            CalcToken* pTopNumberObj = myStack.top();
            myStack.pop();
            int operand1 = pTopNumberObj->getValueNum();
            int operand2 = pCurrentObj->getValueNum();
            int calcValue = 0;
            switch (pTopOperatorObj->getTag()) {
            case '*':
                calcValue = operand1 * operand2;
                break;
            case '+':
            default:
                assert(false);							
                calcValue = -1;
            }
            delete pTopOperatorObj;
            delete pTopNumberObj;            
            pCurrentObj->setValue(to_string(calcValue));
        }
        myStack.push(pCurrentObj);
    } 
    //do calculation with lower precedence
    while (myStack.size() > 1)
    {
        CalcToken* pNumObj1 = myStack.top();
        myStack.pop();
        CalcToken* pOperatorObj = myStack.top();
        myStack.pop();
        CalcToken* pNumObj2 = myStack.top();
        myStack.pop();
        int operand1 = pNumObj1->getValueNum();
        int operand2 = pNumObj2->getValueNum();
        int calcValue = 0;
        switch (pOperatorObj->getTag()) {
        case '+':
            calcValue = operand1 + operand2;
            break;
        case '*':
        default:
            assert(false);							
            calcValue = -1;
        }
        delete pNumObj1;
        delete pOperatorObj;
        pNumObj2->setValue(to_string(calcValue));
        myStack.push(pNumObj2);
    }
    CalcToken* pFinalValue = myStack.top();
    finalValue = pFinalValue->getValueNum();
    isValid = true;
    //clean up stack
    while (!myStack.empty())
    {
        delete myStack.top();
        myStack.pop();
    }
    return isValid;
}
//-----------------------------------------------------
int doOneCalculation(const string expression) {
    int value = 0;
    CalcEng calc(expression);
    if (calc.DoCalculate()) value = calc.getValue();
    return value;
}
void MethodJSCalcEng(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	
	v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
  if (info.Length() < 3) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }
  if (!info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }
  int arg0 = info[0]->NumberValue(context).FromJust();
  int arg1 = info[1]->NumberValue(context).FromJust();
	int arg2 = info[2]->NumberValue(context).FromJust();	
	string testInput = "";
	int calculatedValue = 0;
	string outputToUser = "";
	//--------------------------------------
	// do calculation: arg0 + arg1 * arg2
	testInput = to_string(arg0);
	testInput += "+";
	testInput += to_string(arg1);
	testInput += "*";
	testInput += to_string(arg2);	
	calculatedValue = doOneCalculation(testInput);
	outputToUser += "\t" + testInput + "=" + to_string(calculatedValue);
	//--------------------------------------
	/*
	outputToUser += "\n\tBuilt-in internal tests...";
	//--------------------------------------	
	testInput = "1+2*3";
	calculatedValue = doOneCalculation(testInput);	
	assert(calculatedValue == 7);	
	outputToUser += "\n\t" + testInput + "=" + to_string(calculatedValue);
	//--------------------------------------
	testInput = "1+2";
	calculatedValue = doOneCalculation(testInput);	
	assert(calculatedValue == 3);	
	outputToUser += "\n\t" + testInput + "=" + to_string(calculatedValue);
	//--------------------------------------
	testInput = "1*2+3";
	calculatedValue = doOneCalculation(testInput);	
	assert(calculatedValue == 5);	
	outputToUser += "\n\t" + testInput + "=" + to_string(calculatedValue);
	//--------------------------------------
	testInput = "2+3*4";
	calculatedValue = doOneCalculation(testInput);	
	assert(calculatedValue == 14);	
	outputToUser += "\n\t" + testInput + "=" + to_string(calculatedValue);	
	//--------------------------------------
	testInput = "2*3+4";
	calculatedValue = doOneCalculation(testInput);	
	assert(calculatedValue == 10);	
	outputToUser += "\n\t" + testInput + "=" + to_string(calculatedValue);
	//--------------------------------------
	testInput = "12+13*14+15*16";
	calculatedValue = doOneCalculation(testInput);	
	assert(calculatedValue == 434);		
	outputToUser += "\n\t" + testInput + "=" + to_string(calculatedValue);
	*/
	//--------------------------------------
	outputToUser = "(*version 20201019_1457*): " + outputToUser;
	info.GetReturnValue().Set(Nan::New(outputToUser).ToLocalChecked());
}
void Init(v8::Local<v8::Object> exports) {
  v8::Local<v8::Context> context = exports->CreationContext();
  exports->Set(context,
               Nan::New("JSCalcEng").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(MethodJSCalcEng)
                   ->GetFunction(context)
                   .ToLocalChecked());
}
NODE_MODULE(JSCalcEng, Init)