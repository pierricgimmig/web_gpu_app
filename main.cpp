#include "web_gpu_app.h"

static void Main() { web_gpu_app::App app; }

#ifdef _WIN32
int WinMain() { Main(); }
#else
int main() { Main(); }
#endif
