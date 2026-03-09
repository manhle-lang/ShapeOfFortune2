// Converter.h
#ifndef RG_CONVERTER_H
#define RG_CONVERTER_H

#include <v8.h>
#include "./CommonLibrary.h"

using namespace std;
using namespace v8;

int ToCPPInt(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key);
vector<int> ToCPPVectorInt(Local<Context> context, Local<Value> value);
vector<vector<int>> ToCPPVectorInt2D(Local<Context> context, Local<Value> value);
vector<vector<vector<int>>> ToCPPVectorInt3D(Local<Context> context, Local<Value> value);
vector<vector<vector<vector<int>>>> ToCPPVectorInt4D(Local<Context> context, Local<Value> value);
/**
 * Vector Int Ended
 */

/**
 * Vector Double Started
 */
double ToCPPDouble(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key);
vector<double> ToCPPVectorDouble(Local<Context> context, Local<Value> value);
vector<vector<double>> ToCPPVectorDouble2D(Local<Context> context, Local<Value> value);
vector<vector<vector<double>>> ToCPPVectorDouble3D(Local<Context> context, Local<Value> value);
vector<vector<vector<vector<double>>>> ToCPPVectorDouble4D(Local<Context> context, Local<Value> value);
/**
 * Vector Double Ended
 */

/**
 * Vector Bool Started
 */
bool ToCPPBool(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key);
vector<bool> ToCPPVectorBool(Isolate *isolate, Local<Context> context, Local<Value> value);
vector<vector<bool>> ToCPPVectorBool2D(Isolate *isolate, Local<Context> context, Local<Value> value);
vector<vector<vector<bool>>> ToCPPVectorBool3D(Isolate *isolate, Local<Context> context, Local<Value> value);
vector<vector<vector<vector<bool>>>> ToCPPVectorBool4D(Isolate *isolate, Local<Context> context, Local<Value> value);
/**
 * Vector Bool Ended
 */

/**
 * Vector String Started
 */
string ToCPPString(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key);
/**
 * Vector String Ended
 */

/**
 * Object
 */
Local<Object> CPPGetSafeObjectProperty(
    Isolate *isolate,
    Local<Context> context,
    Local<Object> source,
    const char *propertyName);

/**
 * Vector Int Started
 */
Local<Integer> ToV8Int(Isolate *isolate, const int &value);
Local<Array> ToV8ArrayInt(Isolate *isolate, Local<Context> context, const vector<int> &vec);
Local<Array> ToV8ArrayInt2D(Isolate *isolate, Local<Context> context, const vector<vector<int>> &vec);
Local<Array> ToV8ArrayInt3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<int>>> &vec);
Local<Array> ToV8ArrayInt4D(Isolate *isolate, Local<Context> context, const vector<vector<vector<vector<int>>>> &vec);
/**
 * Vector Int Ended
 */

/**
 * Vector Number Started
 */
Local<Number> ToV8Number(Isolate *isolate, const double &value);
Local<Array> ToV8ArrayNumber(Isolate *isolate, Local<Context> context, const vector<double> &vec);
Local<Array> ToV8ArrayNumber2D(Isolate *isolate, Local<Context> context, const vector<vector<double>> &vec);
Local<Array> ToV8ArrayNumber3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<double>>> &vec);
/**
 * Vector Number Ended
 */

/**
 * Vector Bool Started
 */
Local<Boolean> ToV8Bool(Isolate *isolate, const bool &value);
Local<Array> ToV8ArrayBoolean(Isolate *isolate, Local<Context> context, const vector<bool> &vec);
Local<Array> ToV8ArrayBoolean2D(Isolate *isolate, Local<Context> context, const vector<vector<bool>> &vec);
Local<Array> ToV8ArrayBoolean3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<bool>>> &vec);
/**
 * Vector Bool Ended
 */

/**
 * Vector Winlines
 */
Local<Object> ToV8Winline(Isolate *isolate, Local<Context> context, const CommonLibrary::Winline &winline);
Local<Array> ToV8ArrayWinlines(Isolate *isolate, Local<Context> context, const vector<CommonLibrary::Winline> &winLines);
Local<Array> ToV8ArrayWinlines2D(Isolate *isolate, Local<Context> context, const vector<vector<CommonLibrary::Winline>> &winLines);
Local<Array> ToV8ArrayWinlines3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<CommonLibrary::Winline>>> &winLines);
/**
 * Vector Winlines Ended
 */

#endif