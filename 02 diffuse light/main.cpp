//
// シェダー
//


#define _USE_MATH_DEFINES    // TIPS:M_PIとかを使えるようにする
#define GLFW_INCLUDE_GLEXT   // GLEXT(OpenGL拡張)を利用
#define GLFW_INCLUDE_GLU     // gluを利用

#if defined(_MSC_VER)
// Windows:GLEWをスタティックライブラリ形式で利用
#define GLEW_STATIC
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#if defined(_MSC_VER)
// Windows:外部ライブラリのリンク指定
#if defined (_DEBUG)
#pragma comment(lib, "glfw3d.lib")
#pragma comment(lib, "glew32sd.lib")
#else
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#endif
#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "GLU32.lib")
#endif

#define GLM_FORCE_SWIZZLE     // イカした機能を利用する
#include <glm/glm.hpp>        // glmの基本機能
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "shader.hpp"



int main() {
  // GLFW初期化
  if (!glfwInit()) return -1;

  GLFWwindow* window = glfwCreateWindow(640, 480,
                                        "Shader",
                                        nullptr, nullptr);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

#if defined (_MSC_VER)
  // GLEWを初期化(Windows)
  if (glewInit() != GLEW_OK)
  {
    // パソコンがOpenGL拡張に対応していなければ終了
    glfwTerminate();
    return -1;
  }
#endif

  // シェーダーを読み込んで準備
  GLuint program = Shader::read("sample");

  // シェーダー発動
  Shader::use(program);

  // 頂点配列を用意
  //  立方体の頂点配列
  GLfloat position[] =
  {
    // 前
    //  x,     y,    z,
    -0.5f,  0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f,  0.5f, 0.5f,

    -0.5f,  0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
      
    // 後
    -0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
      
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
      
    // 左
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
      
    // 右
    0.5f,  0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
      
    0.5f,  0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,
      
    // 上
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f,  0.5f,
    0.5f, 0.5f, -0.5f,

    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f,  0.5f,
    0.5f, 0.5f,  0.5f,
      
    // 底
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,      
    0.5f, -0.5f,  0.5f,

    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,      
  };

  // 立方体の面法線
  GLfloat normal[] =
  {
    // 前
    // x,    y,    z,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    
    // 後
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,

    // 左
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

    // 右
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    // 上
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    // 下
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
  };

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  float r = 0.0;

  // 透視変換表列を用意
  glm::mat4 proj = glm::perspective(glm::radians(35.0),  // 視野角
                                    640.0 / 480.0,       // 縦横比
                                    0.2, 20.0);          // near-z, far-z

  glEnable(GL_DEPTH);         // 深度バッファON
  glEnable(GL_CULL_FACE);     // 裏面描画OFF

  while (!glfwWindowShouldClose(window))
  {
    // フレームバッファの内容を消去1
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ビューポート設定
    // TIPS GLFW初期化時に設定されている
    // glViewport(0, 0, 640, 480);

    // シェーダーへ透視変換行列を送る
    {
      GLint id = Shader::uniform(program, "uni_proj_mat");
      glUniformMatrix4fv(id, 1, GL_FALSE, &proj[0][0]);        
    }

    // シェーダーへモデルビュー行列を送る
    {
      glm::mat4 model;

      model = glm::translate(model, glm::vec3(0, 0, -5));
      model = glm::rotate(model,
                          glm::radians(r), glm::vec3(0, 1, 0));
      model = glm::rotate(model,
                          glm::radians(r), glm::vec3(1, 0, 0));

      GLint id = Shader::uniform(program, "uni_model_mat");
      glUniformMatrix4fv(id, 1, GL_FALSE, &model[0][0]);

      // 逆転置行列
      glm::mat3 m = glm::inverseTranspose(glm::mat3(model));
      id = Shader::uniform(program, "uni_normal_mat");
      glUniformMatrix3fv(id, 1, GL_FALSE, &m[0][0]);
    }

    r += 1.0;

    // シェーダーへ値を送る
    {
      GLint id = Shader::uniform(program, "color");
      glm::vec4 v(1, 1, 1, 1);
      glUniform4fv(id, 1, &v[0]);        
    }

    // 頂点配列をシェーダーへ送る
    {
      GLint id = Shader::attrib(program, "position");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, position);
    }
    {
      GLint id = Shader::attrib(program, "normal");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, normal);
    }
    // 描画
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  
  return 0;
}
