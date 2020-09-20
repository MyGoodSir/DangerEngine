#include "main.h"

//change this to 3 to see stencil buffer stuff and IMGUI implementation
#define TEST_NUMBER 2 

#if TEST_NUMBER == 2

int main() {
	DGR::test2 t;
	t.setup();
	while (!t.shouldClose()) { t.render(); }
	t.cleanup();
	return 0;
}

#elif TEST_NUMBER == 3

int main() {
	DGR::test3 t;
	t.setup();
	while (!t.shouldClose()) { t.render(); }
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	t.cleanup();
	return 0;
}

#endif
