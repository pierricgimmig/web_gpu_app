#include "app.h"

static void Main() { 
    web_gpu_app::App app; 
    app.Run();
}

#ifdef _WIN32
int WinMain() { Main(); }
#else
int main() { Main(); }
#endif
