#include "all.h"

#include "Server.h"

using namespace std;
bool is_debug = true;

int main() {

    Server myServer(8080, 4);
    myServer.run();
    return 0;
}
