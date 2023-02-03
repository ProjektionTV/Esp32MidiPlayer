#include "src/core/test/stdTestOut.hpp"
#include "src/core/test/tests.hpp"

int main(int argc, const char *argv[]) {
    projektionMidi::test::stdTestOut *out = new projektionMidi::test::stdTestOut();
    bool ok = !test(out);
    delete out;
    return ok ? 0 : 1;
}
