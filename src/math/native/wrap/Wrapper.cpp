#include <iostream>
#include <node.h>
#include <v8.h>
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <string>
#include "../../libs/Converter.h"
#include "../math/Math.cpp"
#include "GameConfig.h"
#include "../../libs/json.hpp"

using namespace std;
using namespace chrono;
using namespace GameMath;
using namespace v8;
using std::vector;
using json = nlohmann::ordered_json;

#define SET(context, obj, key, val) obj->Set(context, v8::String::NewFromUtf8(isolate, key).ToLocalChecked(), val).Check()

// CPP → V8
v8::Local<v8::Value> CppToV8(v8::Isolate *isolate, const json &j)
{
    Local<Context> context = isolate->GetCurrentContext();
    if (j.is_null())
        return v8::Null(isolate);
    if (j.is_boolean())
        return v8::Boolean::New(isolate, j.get<bool>());
    if (j.is_number_integer())
        return v8::Integer::New(isolate, j.get<int>());
    if (j.is_number_unsigned())
        return v8::Integer::NewFromUnsigned(isolate, j.get<unsigned int>());
    if (j.is_number_float())
    {
        double rounded = std::round(j.get<double>() * 1e6) / 1e6;
        return v8::Number::New(isolate, rounded);
    }
    if (j.is_string())
        return v8::String::NewFromUtf8(isolate, j.get<std::string>().c_str()).ToLocalChecked();

    if (j.is_array())
    {
        v8::Local<v8::Array> arr = v8::Array::New(isolate, j.size());
        for (size_t i = 0; i < j.size(); ++i)
        {
            arr->Set(context, static_cast<uint32_t>(i), CppToV8(isolate, j[i])).Check();
        }
        return arr;
    }

    if (j.is_object())
    {
        v8::Local<v8::Object> obj = v8::Object::New(isolate);
        for (auto &el : j.items())
        {
            obj->Set(
                   context,
                   v8::String::NewFromUtf8(isolate, el.key().c_str()).ToLocalChecked(),
                   CppToV8(isolate, el.value()))
                .Check();
        }
        return obj;
    }

    return v8::Undefined(isolate);
}

// V8 → CPP
json V8ToCpp(v8::Isolate *isolate, v8::Local<v8::Value> value)
{
    Local<Context> context = isolate->GetCurrentContext();
    if (value->IsNull() || value->IsUndefined())
        return nullptr;
    if (value->IsBoolean())
        return value->BooleanValue(isolate);
    if (value->IsInt32())
        return value->Int32Value(context).ToChecked();
    if (value->IsUint32())
        return value->Uint32Value(context).ToChecked();
    if (value->IsNumber())
        return value->NumberValue(context).ToChecked();
    if (value->IsString())
    {
        v8::String::Utf8Value str(isolate, value);
        return std::string(*str);
    }
    if (value->IsArray())
    {
        json arr = json::array();
        v8::Local<v8::Array> v8Arr = value.As<v8::Array>();
        for (uint32_t i = 0; i < v8Arr->Length(); ++i)
        {
            v8::Local<v8::Value> element;
            if (v8Arr->Get(context, i).ToLocal(&element))
            {
                arr.push_back(V8ToCpp(isolate, element));
            }
        }
        return arr;
    }
    if (value->IsObject())
    {
        json obj = json::object();
        v8::Local<v8::Object> v8Obj = value.As<v8::Object>();
        v8::Local<v8::Array> props;
        if (v8Obj->GetOwnPropertyNames(context).ToLocal(&props))
        {
            for (uint32_t i = 0; i < props->Length(); ++i)
            {
                v8::Local<v8::Value> key;
                if (!props->Get(context, i).ToLocal(&key))
                    continue;
                v8::String::Utf8Value keyStr(isolate, key);
                v8::Local<v8::Value> val;
                if (v8Obj->Get(context, key).ToLocal(&val))
                {
                    obj[*keyStr] = V8ToCpp(isolate, val);
                }
            }
        }
        return obj;
    }

    return nullptr;
}

// Convert Cpp value to V8 value
template <typename T>
v8::Local<v8::Value> ConvertCppToV8(v8::Isolate *isolate, const T &obj)
{
    try
    {
        json j = obj;
        return CppToV8(isolate, j);
    }
    catch (const std::exception &e)
    {
        v8::Local<v8::String> error = v8::String::NewFromUtf8(
                                          isolate, ("ConvertCppToV8 error: " + std::string(e.what())).c_str())
                                          .ToLocalChecked();
        isolate->ThrowException(v8::Exception::Error(error));
        return v8::Undefined(isolate);
    }
}

// Convert V8 value to Cpp value
template <typename T>
T ConvertV8ToCpp(v8::Isolate *isolate, v8::Local<v8::Value> val)
{
    try
    {
        json j = V8ToCpp(isolate, val);
        return j.get<T>();
    }
    catch (const std::exception &e)
    {
        v8::Local<v8::String> error = v8::String::NewFromUtf8(
                                          isolate, ("ConvertV8ToCpp error: " + std::string(e.what())).c_str())
                                          .ToLocalChecked();
        isolate->ThrowException(v8::Exception::Error(error));
        return T{};
    }
}

namespace Wrapper
{
    void DoSpin(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        try
        {
            GameStruct::SpinRequest req = ConvertV8ToCpp<GameStruct::SpinRequest>(isolate, args[0]);
            GameMath::GameLogic gameLogic(req.profile, nullptr); // Always use default config on spin
            GameStruct::SpinResult result = gameLogic.Spin(req);
            args.GetReturnValue().Set(ConvertCppToV8(isolate, result));
        }
        catch (const std::exception &e)
        {
            v8::Local<v8::String> error = v8::String::NewFromUtf8(
                                              isolate, ("DoSpin error: " + std::string(e.what())).c_str())
                                              .ToLocalChecked();
            isolate->ThrowException(v8::Exception::Error(error));
        }
    }

    void DoCheat(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        Isolate *isolate = args.GetIsolate();
        try
        {
            GameStruct::SpinRequest req = ConvertV8ToCpp<GameStruct::SpinRequest>(isolate, args[0]);
            GameMath::CheatLogic gameLogic(req.profile, &req.gameConfig);
            GameStruct::SpinResult result = gameLogic.Spin(req);
            args.GetReturnValue().Set(ConvertCppToV8(isolate, result));
        }
        catch (const std::exception &e)
        {
            v8::Local<v8::String> error = v8::String::NewFromUtf8(
                                              isolate, ("DoCheat error: " + std::string(e.what())).c_str())
                                              .ToLocalChecked();
            isolate->ThrowException(v8::Exception::Error(error));
        }
    }

    void GetConfigs(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        Isolate *isolate = args.GetIsolate();

        try
        {
            json j = V8ToCpp(isolate, args[0]);
            int profile = j.value("profile", 0);

            GameConfig result = GameMath::GameLogic(profile, nullptr).GetConfigs();
            args.GetReturnValue().Set(ConvertCppToV8(isolate, result));
        }
        catch (const std::exception &e)
        {
            v8::Local<v8::String> error = v8::String::NewFromUtf8(
                                              isolate, ("GetConfigs error: " + std::string(e.what())).c_str())
                                              .ToLocalChecked();
            isolate->ThrowException(v8::Exception::Error(error));
        }
    }
}

// Module initialization
void Initialize(v8::Local<v8::Object> exports)
{
    NODE_SET_METHOD(exports, "DoSpin", Wrapper::DoSpin);
    NODE_SET_METHOD(exports, "GetConfigs", Wrapper::GetConfigs);
    NODE_SET_METHOD(exports, "DoCheat", Wrapper::DoCheat);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);