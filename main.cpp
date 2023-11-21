#include "triangle_app.h"

static void Main() { 
    web_gpu_app::TriangleApp app; 
    app.Run();
}

#ifdef _WIN32
int WinMain() { Main(); }
#else
int main() { Main(); }
#endif
