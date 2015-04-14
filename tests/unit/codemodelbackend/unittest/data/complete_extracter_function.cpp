void Function();
template<class T> void TemplateFunction();

#define FunctionMacro(X, Y) X + Y

class base {
    void NotAccessibleFunction();


};
class Class : public base {
    void Method();
    __attribute__((annotate("qt_slot"))) void Slot();
    __attribute__((annotate("qt_signal"))) void Signal();
    __attribute__ ((deprecated)) void DeprecatedFunction();
    void NotAvailableFunction() = delete;
public:
    void function()
    {

    }
};
