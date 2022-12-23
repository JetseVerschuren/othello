#include <napi.h>
#include <thread>
#include <atomic>
#include "othello.hpp"

class MCTS : public Napi::ObjectWrap<MCTS> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    MCTS(const Napi::CallbackInfo &info);

    static Napi::Value CreateNewItem(const Napi::CallbackInfo &info);

private:
    double _value;

    Napi::Value GetValue(const Napi::CallbackInfo &info);

    Napi::Value SetValue(const Napi::CallbackInfo &info);

    std::atomic<bool> thread_running = {false};

    void ApplyMove(const Napi::CallbackInfo &info);

    Napi::Value DetermineMove(const Napi::CallbackInfo &info);

    Napi::Value GetBoard(const Napi::CallbackInfo &info);
};

Napi::Object MCTS::Init(Napi::Env env, Napi::Object exports) {
    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "MCTS", {
            InstanceMethod<&MCTS::GetValue>("GetValue",
                                            static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
            InstanceMethod<&MCTS::SetValue>("SetValue",
                                            static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
            InstanceMethod<&MCTS::ApplyMove>("applyMove",
                                             static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
            InstanceMethod<&MCTS::DetermineMove>("determineMove", static_cast<napi_property_attributes>(napi_writable |
                                                                                                        napi_configurable)),
            InstanceMethod<&MCTS::GetBoard>("getBoard",
                                            static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
            StaticMethod<&MCTS::CreateNewItem>("CreateNewItem", static_cast<napi_property_attributes>(napi_writable |
                                                                                                      napi_configurable)),
    });

    Napi::FunctionReference *constructor = new Napi::FunctionReference();

    // Create a persistent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    *constructor = Napi::Persistent(func);
    exports.Set("MCTS", func);

    // Store the constructor as the add-on instance data. This will allow this
    // add-on to support multiple instances of itself running on multiple worker
    // threads, as well as multiple instances of itself running in different
    // contexts on the same thread.
    //
    // By default, the value set on the environment here will be destroyed when
    // the add-on is unloaded using the `delete` operator, but it is also
    // possible to supply a custom deleter.
    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

MCTS::MCTS(const Napi::CallbackInfo &info) :
        Napi::ObjectWrap<MCTS>(info) {
    Napi::Env env = info.Env();
    // ...
    Napi::Number value = info[0].As<Napi::Number>();
    this->_value = value.DoubleValue();
}

Napi::Value MCTS::GetValue(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, this->_value);
}

Napi::Value MCTS::SetValue(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    // ...
    Napi::Number value = info[0].As<Napi::Number>();
    this->_value = value.DoubleValue();
    return this->GetValue(info);
}

void MCTS::ApplyMove(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
}

Napi::Value MCTS::DetermineMove(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    std::shared_ptr<Napi::Promise::Deferred> deferred = std::make_shared<Napi::Promise::Deferred>(
            Napi::Promise::Deferred::New(env));
    // There's already a thread running in the background
    if (thread_running) {
        deferred->Reject(Napi::String::New(info.Env(), "Already solving"));
        return deferred->Promise();
    }

    // TODO: What a mess, can't this be done easier?
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function::New(env, [](const Napi::CallbackInfo &info) {}),
            "TSFN", 0, 1,
            [](Napi::Env) {});
    thread_running = true;
    std::thread([tsfn, deferred, this] {
        auto callback = [deferred](Napi::Env env, Napi::Function jsCallback, const int *value) {
            deferred->Resolve({Napi::Number::New(env, *value)});
            delete value;
        };

        // TODO: Actually calculate the move

        std::this_thread::sleep_for(std::chrono::seconds(1));
        int *value = new int(42);
        // TODO: Handle possible error
        tsfn.BlockingCall(value, callback);
        tsfn.Release();
        thread_running = true;
    }).detach();

    return deferred->Promise();
}

Napi::Value MCTS::GetBoard(const Napi::CallbackInfo &info) {
    Othello game;
    std::vector<int8_t> board = game.ToVector();
    Napi::Int8Array out = Napi::Int8Array::New(info.Env(), board.size());
    for (size_t i = 0; i < board.size(); i++) {
        out[i] = board[i];
    }
    return out;
}


// Initialize native add-on
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    MCTS::Init(env, exports);
    return exports;
}

// Create a new item using the constructor stored during Init.
Napi::Value MCTS::CreateNewItem(const Napi::CallbackInfo &info) {
    // Retrieve the instance data we stored during `Init()`. We only stored the
    // constructor there, so we retrieve it here to create a new instance of the
    // JS class the constructor represents.
    Napi::FunctionReference *constructor =
            info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({Napi::Number::New(info.Env(), 42)});
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
