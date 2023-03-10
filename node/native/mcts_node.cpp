#include <napi.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "mcts.hpp"

class MCTS_Node : public Napi::ObjectWrap<MCTS_Node> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    MCTS_Node(const Napi::CallbackInfo &info);

    static Napi::Value CreateNewItem(const Napi::CallbackInfo &info);

private:
    std::atomic<bool> thread_running = {false};
    MCTS mcts;

    void ApplyMove(const Napi::CallbackInfo &info);

    Napi::Value DetermineMove(const Napi::CallbackInfo &info);

    Napi::Value GetBoard(const Napi::CallbackInfo &info);

    Napi::Value OpponentCanMove(const Napi::CallbackInfo &info);
};

Napi::Object MCTS_Node::Init(Napi::Env env, Napi::Object exports) {
    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "MCTS", {
            InstanceMethod<&MCTS_Node::ApplyMove>("applyMove",
                                                  static_cast<napi_property_attributes>(napi_writable |
                                                                                        napi_configurable)),
            InstanceMethod<&MCTS_Node::DetermineMove>("determineMove",
                                                      static_cast<napi_property_attributes>(napi_writable |
                                                                                            napi_configurable)),
            InstanceMethod<&MCTS_Node::GetBoard>("getBoard",
                                                 static_cast<napi_property_attributes>(napi_writable |
                                                                                       napi_configurable)),
            InstanceMethod<&MCTS_Node::OpponentCanMove>("opponentCanMove",
                                                        static_cast<napi_property_attributes>(napi_writable |
                                                                                              napi_configurable)),
            StaticMethod<&MCTS_Node::CreateNewItem>("CreateNewItem",
                                                    static_cast<napi_property_attributes>(napi_writable |
                                                                                          napi_configurable)),
    });

    auto *constructor = new Napi::FunctionReference();

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

MCTS_Node::MCTS_Node(const Napi::CallbackInfo &info) : Napi::ObjectWrap<MCTS_Node>(info) {}

void MCTS_Node::ApplyMove(const Napi::CallbackInfo &info) {
    mcts.ApplyMove(info[0].As<Napi::Number>().Int64Value());
}

Napi::Value MCTS_Node::DetermineMove(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    std::shared_ptr<Napi::Promise::Deferred> deferred = std::make_shared<Napi::Promise::Deferred>(
            Napi::Promise::Deferred::New(env));
    // There's already a thread running in the background
    if (thread_running) {
        deferred->Reject(Napi::String::New(info.Env(), "Already solving"));
        return deferred->Promise();
    }

    double runtime = 2000;
    if (info.Length() > 1 && !info[0].IsUndefined()) runtime = info[0].As<Napi::Number>().DoubleValue();

    // TODO: What a mess, can't this be done easier?
    Napi::ThreadSafeFunction tsfn = Napi::ThreadSafeFunction::New(
            env, Napi::Function::New(env, [](const Napi::CallbackInfo &info) {}),
            "TSFN", 0, 1,
            [](Napi::Env) {});
    thread_running = true;
    std::thread([tsfn, deferred, this, runtime] {
        auto callback = [deferred](Napi::Env env, Napi::Function jsCallback, const int *value) {
            deferred->Resolve({Napi::Number::New(env, *value)});
            delete value;
        };

        uint8_t move = this->mcts.DetermineMove((unsigned int) runtime);

        int *value = new int(move);
        // TODO: Handle possible error
        tsfn.BlockingCall(value, callback);
        tsfn.Release();
        this->thread_running = false;
    }).detach();

    return deferred->Promise();
}

Napi::Value MCTS_Node::GetBoard(const Napi::CallbackInfo &info) {
    std::vector<int8_t> board = mcts.GetBoard();
    Napi::Int8Array out = Napi::Int8Array::New(info.Env(), board.size());
    for (size_t i = 0; i < board.size(); i++) {
        out[i] = board[i];
    }
    return out;
}

Napi::Value MCTS_Node::OpponentCanMove(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), mcts.OpponentCanMove());
}


// Initialize native add-on
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    MCTS_Node::Init(env, exports);
    return exports;
}

// Create a new item using the constructor stored during Init.
Napi::Value MCTS_Node::CreateNewItem(const Napi::CallbackInfo &info) {
    // Retrieve the instance data we stored during `Init()`. We only stored the
    // constructor there, so we retrieve it here to create a new instance of the
    // JS class the constructor represents.
    auto *constructor = info.Env().GetInstanceData<Napi::FunctionReference>();
    return constructor->New({});
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
