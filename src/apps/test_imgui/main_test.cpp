#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

int main()
{
    // Initialize GLFW
    if (!glfwInit())
        return -1;

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello ImGui Docking", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::GetStyle().ScaleAllSizes(1.5); // Scale up all sizes
    ImGui::GetIO().FontGlobalScale = 1.5; // Scale up font size

    // Setup ImGui GLFW and OpenGL bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Enable docking
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    bool closeable = true;
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create a dock space
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


        // Example window
        ImGui::Begin("Hello, world!", &closeable);
        ImGui::Text("This is some example text.");
        ImGui::End();

        ImGui::Begin("tab 2");
        ImGui::Text("This is some example text.");
        ImGui::End();

        ImGui::Begin("tab 123");
        ImGui::Text("This is some example text.");
        ImGui::End();

        // Rendering
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy GLFW window
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
