#include "Converter.h"
#include <node.h>
#include <v8.h>
#include <iostream>

using namespace std;
using namespace v8;

// TODO: check fallback value, and throw in some cases to prevent undefined behaviors

/**
 * Vector Int Started
 */
int ToCPPInt(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key)
{
    Local<String> prop = String::NewFromUtf8(isolate, key).ToLocalChecked();
    if (!obj->Has(context, prop).FromMaybe(false))
        return 0;
    return obj->Get(context, prop).ToLocalChecked()->Int32Value(context).FromMaybe(0);
}

vector<int> ToCPPVectorInt(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<int>();

    Local<Array> arr = value.As<Array>();
    vector<int> vec = vector<int>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec[i] = arr->Get(context, i).ToLocalChecked()->Int32Value(context).FromMaybe(0);
    }
    return vec;
}

vector<vector<int>> ToCPPVectorInt2D(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<int>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<int>> vec2d = vector<vector<int>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec2d[i] = ToCPPVectorInt(context, arr->Get(context, i).ToLocalChecked());
    }
    return vec2d;
}

vector<vector<vector<int>>> ToCPPVectorInt3D(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<vector<int>>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<vector<int>>> vec3d = vector<vector<vector<int>>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec3d[i] = ToCPPVectorInt2D(context, arr->Get(context, i).ToLocalChecked());
    }
    return vec3d;
}

vector<vector<vector<vector<int>>>> ToCPPVectorInt4D(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<vector<vector<int>>>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<vector<vector<int>>>> vec4d = vector<vector<vector<vector<int>>>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec4d[i] = ToCPPVectorInt3D(context, arr->Get(context, i).ToLocalChecked());
    }
    return vec4d;
}
/**
 * Vector Int Ended
 */

/**
 * Vector Double Started
 */
double ToCPPDouble(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key)
{
    Local<String> prop = String::NewFromUtf8(isolate, key).ToLocalChecked();
    if (!obj->Has(context, prop).FromMaybe(false))
        return 0;
    return obj->Get(context, prop).ToLocalChecked()->NumberValue(context).FromMaybe(0);
}

vector<double> ToCPPVectorDouble(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<double>();

    Local<Array> arr = value.As<Array>();
    vector<double> vec = vector<double>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec[i] = arr->Get(context, i).ToLocalChecked()->NumberValue(context).FromMaybe(0);
    }
    return vec;
}

vector<vector<double>> ToCPPVectorDouble2D(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<double>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<double>> vec2d = vector<vector<double>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec2d[i] = ToCPPVectorDouble(context, arr->Get(context, i).ToLocalChecked());
    }
    return vec2d;
}

vector<vector<vector<double>>> ToCPPVectorDouble3D(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<vector<double>>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<vector<double>>> vec3d = vector<vector<vector<double>>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec3d[i] = ToCPPVectorDouble2D(context, arr->Get(context, i).ToLocalChecked());
    }
    return vec3d;
}

vector<vector<vector<vector<double>>>> ToCPPVectorDouble4D(Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<vector<vector<double>>>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<vector<vector<double>>>> vec4d = vector<vector<vector<vector<double>>>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec4d[i] = ToCPPVectorDouble3D(context, arr->Get(context, i).ToLocalChecked());
    }
    return vec4d;
}
/**
 * Vector Double Ended
 */

/**
 * Vector Bool Started
 */
bool ToCPPBool(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key)
{
    Local<String> prop = String::NewFromUtf8(isolate, key).ToLocalChecked();
    if (!obj->Has(context, prop).FromMaybe(false))
        return false;
    return obj->Get(context, prop).ToLocalChecked()->BooleanValue(isolate);
}

vector<bool> ToCPPVectorBool(Isolate *isolate, Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<bool>();

    Local<Array> arr = value.As<Array>();
    vector<bool> vec = vector<bool>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec[i] = arr->Get(context, i).ToLocalChecked()->BooleanValue(isolate);
    }
    return vec;
}

vector<vector<bool>> ToCPPVectorBool2D(Isolate *isolate, Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<bool>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<bool>> vec2d = vector<vector<bool>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec2d[i] = ToCPPVectorBool(isolate, context, arr->Get(context, i).ToLocalChecked());
    }
    return vec2d;
}

vector<vector<vector<bool>>> ToCPPVectorBool3D(Isolate *isolate, Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<vector<bool>>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<vector<bool>>> vec3d = vector<vector<vector<bool>>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec3d[i] = ToCPPVectorBool2D(isolate, context, arr->Get(context, i).ToLocalChecked());
    }
    return vec3d;
}

vector<vector<vector<vector<bool>>>> ToCPPVectorBool4D(Isolate *isolate, Local<Context> context, Local<Value> value)
{
    if (!value->IsArray())
        return vector<vector<vector<vector<bool>>>>();

    Local<Array> arr = value.As<Array>();
    vector<vector<vector<vector<bool>>>> vec4d = vector<vector<vector<vector<bool>>>>(arr->Length());

    for (uint32_t i = 0; i < arr->Length(); ++i)
    {
        vec4d[i] = ToCPPVectorBool3D(isolate, context, arr->Get(context, i).ToLocalChecked());
    }
    return vec4d;
}
/**
 * Vector Bool Ended
 */

/**
 * Vector String Started
 */
string ToCPPString(Isolate *isolate, Local<Context> context, Local<Object> obj, const char *key)
{
    Local<String> prop = String::NewFromUtf8(isolate, key).ToLocalChecked();
    if (!obj->Has(context, prop).FromMaybe(false))
        return "";
    Local<Value> val = obj->Get(context, prop).ToLocalChecked();
    String::Utf8Value utf8(isolate, val);
    return *utf8 ? *utf8 : "";
}
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
    const char *propertyName)
{
    Local<String> key = String::NewFromUtf8(isolate, propertyName).ToLocalChecked();

    if (!source->HasOwnProperty(context, key).FromMaybe(false))
    {
        return {}; // Empty Object
    }

    Local<Value> value = source->Get(context, key).ToLocalChecked();
    // Check if value is undefined, null, empty, or not an object
    // If so, return an empty object
    if (value.IsEmpty() || value->IsUndefined() || value->IsNull() || !value->IsObject())
    {
        return {}; // Empty Object
    }
    return value->ToObject(context).ToLocalChecked();
}

/**
 * ============================ TO V8 Convert utilities ============================
 * =================================================================================
 * =================================================================================
 * =================================================================================
 * =================================================================================
 * =================================================================================
 * =================================================================================
 * =================================================================================
 */
Local<Integer> ToV8Int(Isolate *isolate, const int &value)
{
    return Integer::New(isolate, value);
}

Local<Array> ToV8ArrayInt(Isolate *isolate, Local<Context> context, const vector<int> &vec)
{
    Local<Array> arr = Array::New(isolate, vec.size());
    for (uint32_t i = 0; i < vec.size(); ++i)
    {
        arr->Set(context, i, Integer::New(isolate, vec[i])).Check();
    }
    return arr;
}

Local<Array> ToV8ArrayInt2D(Isolate *isolate, Local<Context> context, const vector<vector<int>> &vec)
{
    Local<Array> outer = Array::New(isolate, vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        Local<Array> inner = ToV8ArrayInt(isolate, context, vec[i]);
        outer->Set(context, i, inner).Check();
    }

    return outer;
}

Local<Array> ToV8ArrayInt3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<int>>> &vec)
{
    Local<Array> result = Array::New(isolate, vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        Local<Array> subArray = ToV8ArrayInt2D(isolate, context, vec[i]);
        result->Set(context, i, subArray).Check();
    }

    return result;
}

Local<Array> ToV8ArrayInt4D(Isolate *isolate, Local<Context> context, const vector<vector<vector<vector<int>>>> &vec)
{
    Local<Array> result = Array::New(isolate, vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        Local<Array> subArray = ToV8ArrayInt3D(isolate, context, vec[i]);
        result->Set(context, i, subArray).Check();
    }

    return result;
}
/**
 * Vector Int Ended
 */

/**
 * Vector Number Started
 */
Local<Number> ToV8Number(Isolate *isolate, const double &value)
{
    return Number::New(isolate, value);
}

Local<Array> ToV8ArrayNumber(Isolate *isolate, Local<Context> context, const vector<double> &vec)
{
    Local<Array> arr = Array::New(isolate, vec.size());
    for (uint32_t i = 0; i < vec.size(); ++i)
    {
        arr->Set(context, i, Number::New(isolate, vec[i])).Check();
    }
    return arr;
}

Local<Array> ToV8ArrayNumber2D(Isolate *isolate, Local<Context> context, const vector<vector<double>> &vec)
{
    Local<Array> outer = Array::New(isolate, vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        Local<Array> inner = ToV8ArrayNumber(isolate, context, vec[i]);
        outer->Set(context, i, inner).Check();
    }

    return outer;
}

Local<Array> ToV8ArrayNumber3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<double>>> &vec)
{
    Local<Array> result = Array::New(isolate, vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        Local<Array> subArray = ToV8ArrayNumber2D(isolate, context, vec[i]);
        result->Set(context, i, subArray).Check();
    }

    return result;
}
/**
 * Vector Number Ended
 */

/**
 * Vector Bool Started
 */
Local<Boolean> ToV8Bool(Isolate *isolate, const bool &value)
{
    return Boolean::New(isolate, value);
}

Local<Array> ToV8ArrayBoolean(Isolate *isolate, Local<Context> context, const vector<bool> &vec)
{
    Local<Array> arr = Array::New(isolate, vec.size());
    for (uint32_t i = 0; i < vec.size(); ++i)
    {
        arr->Set(context, i, Boolean::New(isolate, vec[i])).Check();
    }
    return arr;
}

Local<Array> ToV8ArrayBoolean2D(Isolate *isolate, Local<Context> context, const vector<vector<bool>> &vec)
{
    Local<Array> outer = Array::New(isolate, vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        Local<Array> inner = ToV8ArrayBoolean(isolate, context, vec[i]);
        outer->Set(context, i, inner).Check();
    }

    return outer;
}

Local<Array> ToV8ArrayBoolean3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<bool>>> &vec)
{
    Local<Array> result = Array::New(isolate, vec.size());

    for (size_t i = 0; i < vec.size(); ++i)
    {
        Local<Array> subArray = ToV8ArrayBoolean2D(isolate, context, vec[i]);
        result->Set(context, i, subArray).Check();
    }

    return result;
}
/**
 * Vector Bool Ended
 */

/**
 * Vector Winlines
 */
Local<Object> ToV8Winline(Isolate *isolate, Local<Context> context, const CommonLibrary::Winline &winline)
{
    Local<Object> obj = Object::New(isolate);

    obj->Set(context, String::NewFromUtf8(isolate, "winPosition").ToLocalChecked(), ToV8ArrayInt(isolate, context, winline.winPosition)).Check();
    obj->Set(context, String::NewFromUtf8(isolate, "lineId").ToLocalChecked(), Integer::New(isolate, winline.lineId)).Check();
    obj->Set(context, String::NewFromUtf8(isolate, "symbolId").ToLocalChecked(), Integer::New(isolate, winline.symbolId)).Check();
    obj->Set(context, String::NewFromUtf8(isolate, "winAmount").ToLocalChecked(), Number::New(isolate, winline.winAmount)).Check();
    obj->Set(context, String::NewFromUtf8(isolate, "multiplier").ToLocalChecked(), Integer::New(isolate, winline.multiplier)).Check();
    obj->Set(context, String::NewFromUtf8(isolate, "winAmountWithoutMultiplier").ToLocalChecked(), Number::New(isolate, winline.winAmountWithoutMultiplier)).Check();
    return obj;
}

Local<Array> ToV8ArrayWinlines(Isolate *isolate, Local<Context> context, const vector<CommonLibrary::Winline> &winLines)
{
    Local<Array> arr = Array::New(isolate, winLines.size());
    for (size_t i = 0; i < winLines.size(); ++i)
    {
        arr->Set(context, i, ToV8Winline(isolate, context, winLines[i])).Check();
    }
    return arr;
}

Local<Array> ToV8ArrayWinlines2D(Isolate *isolate, Local<Context> context, const vector<vector<CommonLibrary::Winline>> &winLines)
{
    Local<Array> outer = Array::New(isolate, winLines.size());

    for (size_t i = 0; i < winLines.size(); ++i)
    {
        Local<Array> inner = ToV8ArrayWinlines(isolate, context, winLines[i]);
        outer->Set(context, i, inner).Check();
    }

    return outer;
}

Local<Array> ToV8ArrayWinlines3D(Isolate *isolate, Local<Context> context, const vector<vector<vector<CommonLibrary::Winline>>> &winLines)
{
    Local<Array> outer = Array::New(isolate, winLines.size());

    for (size_t i = 0; i < winLines.size(); ++i)
    {
        Local<Array> inner = ToV8ArrayWinlines2D(isolate, context, winLines[i]);
        outer->Set(context, i, inner).Check();
    }

    return outer;
}
/**
 * Vector Winlines Ended
 */