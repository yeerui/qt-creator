include(codemodelbackend.pri)

requires(!isEmpty(LLVM_LIBS))

DESTDIR = $$IDE_BIN_PATH
target.path = /bin


