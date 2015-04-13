void Function();
template<class T> void TemplateFunction();

#define FunctionMacro(X, Y) X + Y

class Class {
    void Method();
    __attribute__((annotate("qt_slot"))) void Slot();
    __attribute__((annotate("qt_signal"))) void Signal();







public:
    void function()
    {

    }
};
