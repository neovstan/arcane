#include <iostream>
// let the standard be included first

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>
#include <windows.h>

#include "main_window.h"

static void glfw_error_callback(int error, const char* description);
static GLFWwindow* create_window(int width, int height);
static void setup_imgui_context();
static void setup_imgui_backends_for_window(GLFWwindow* const window);
static void prepare_for_new_imgui_frame();
static void move_window(GLFWwindow* const window, int& window_pos_x, int& window_pos_y,
                        double& cursor_pos_x, double& cursor_pos_y, bool& allow_to_move);
static void render_window(GLFWwindow* const window);
static void shutdown_imgui();
static void shutdown_window(GLFWwindow* const window);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, PSTR cmdline, int cmdshow) {
  modification::loader::main_window main_window{};
  main_window.load();

  const auto window = create_window(main_window.width, main_window.height);

  setup_imgui_context();
  setup_imgui_backends_for_window(window);

  int window_pos_x{}, window_pos_y{};
  double cursor_pos_x{}, cursor_pos_y{};
  bool allow_to_move{};

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    prepare_for_new_imgui_frame();

    move_window(window, window_pos_x, window_pos_y, cursor_pos_x, cursor_pos_y, allow_to_move);

    ImGui::SetNextWindowPos({});
    ImGui::SetNextWindowSize(
        {static_cast<float>(main_window.width), static_cast<float>(main_window.height)});

    main_window.update();
    if (!main_window.state) {
      break;
    }

    render_window(window);
  }

  main_window.save();

  shutdown_imgui();
  shutdown_window(window);
}

static void glfw_error_callback(int error, const char* description) {
}

static GLFWwindow* create_window(int width, int height) {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) return nullptr;

  glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

  const auto window = glfwCreateWindow(width, height, "", nullptr, nullptr);
  if (!window) return nullptr;

  RECT resolution{};
  GetWindowRect(GetDesktopWindow(), &resolution);

  glfwSetWindowPos(window, (resolution.right - width) / 2, (resolution.bottom - height) / 2);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  return window;
}

static void setup_imgui_context() {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = nullptr;

  ImGui::StyleColorsDark();
}

static void setup_imgui_backends_for_window(GLFWwindow* const window) {
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL2_Init();
}

static void prepare_for_new_imgui_frame() {
  ImGui_ImplOpenGL2_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

static void move_window(GLFWwindow* const window, int& window_pos_x, int& window_pos_y,
                        double& cursor_pos_x, double& cursor_pos_y, bool& allow_to_move) {
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    glfwGetCursorPos(window, &cursor_pos_x, &cursor_pos_y);
    glfwGetWindowPos(window, &window_pos_x, &window_pos_y);

    allow_to_move = !ImGui::IsAnyItemHovered();
  } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && allow_to_move) {
    int new_window_pos_x{}, new_window_pos_y{};
    glfwGetWindowPos(window, &new_window_pos_x, &new_window_pos_y);

    int new_window_pos_rel_to_old_x{new_window_pos_x - window_pos_x},
        new_window_pos_rel_to_old_y{new_window_pos_y - window_pos_y};

    double new_cursor_pos_x{}, new_cursor_pos_y{};
    glfwGetCursorPos(window, &new_cursor_pos_x, &new_cursor_pos_y);
    new_cursor_pos_x += new_window_pos_rel_to_old_x;
    new_cursor_pos_y += new_window_pos_rel_to_old_y;

    const double move_x{new_cursor_pos_x - cursor_pos_x}, move_y{new_cursor_pos_y - cursor_pos_y};

    glfwSetWindowPos(window, window_pos_x + move_x, window_pos_y + move_y);
  } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
    allow_to_move = true;
  }
}

static void render_window(GLFWwindow* const window) {
  ImGui::Render();

  int display_w{}, display_h{};
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

  glfwMakeContextCurrent(window);
  glfwSwapBuffers(window);
}

static void shutdown_imgui() {
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

static void shutdown_window(GLFWwindow* const window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}
