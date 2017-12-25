//
// シェーダー
//   マルチテクスチャ
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
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// 画像を扱う
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.hpp"
#include "texture.hpp"
#include "obj.hpp"


int main() {
  // GLFW初期化
  if (!glfwInit()) return -1;

  GLFWwindow* window = glfwCreateWindow(800, 600,
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

  // モデルデータ読み込み
  Obj model = loadObj("sphere.obj");
  size_t vtx_num = model.vtx.size() / 3;

  // 接線ベクトルを計算
  std::vector<float> tangent(model.vtx.size());

  for (size_t i = 0; i < vtx_num; ++i) {
    float x = model.normal[i * 3 + 0];
    float y = model.normal[i * 3 + 1];
    float z = model.normal[i * 3 + 2];

    // 法線が (0, 1, 0) の場合は外積で正しく計算できない
    glm::vec3 t = glm::cross(glm::vec3(x, y, z), glm::vec3(0, 1, 0));
    // work around
    if (glm::length2(t) == 0.0f) {
      t = glm::vec3(1, 0, 0);
    }

    tangent[i * 3 + 0] = t.x;
    tangent[i * 3 + 1] = t.y;
    tangent[i * 3 + 2] = t.z;
  }

  // 法線マップ
  GLuint normal_map = createTexture("normal-map.jpg");
  GLuint color_map  = createTexture("earth_1.png");

  // シェーダーを読み込んで準備
  GLuint program = Shader::read("sample");

  // シェーダー発動
  Shader::use(program);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  float r = 0.0;

  // 透視変換表列を用意
  glm::mat4 proj = glm::perspective(glm::radians(35.0),  // 視野角
                                    800.0 / 600.0,          // 縦横比
                                    0.2, 20.0);             // near-z, far-z

  glEnable(GL_DEPTH);
  glEnable(GL_CULL_FACE);
  
  while (!glfwWindowShouldClose(window))
  {
    // フレームバッファの内容を消去
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

      model = glm::translate(model, glm::vec3(0, 0, -2));
      model = glm::rotate(model,
                          glm::radians(r), glm::vec3(0, 1, 0));
      // model = glm::rotate(model,
      //                     glm::radians(35.0f), glm::vec3(1, 0, 0));

      GLint id = Shader::uniform(program, "uni_model_mat");
      glUniformMatrix4fv(id, 1, GL_FALSE, &model[0][0]);        

      // 逆転置行列を生成
      glm::mat3 m = glm::inverseTranspose(glm::mat3(model));
      id = Shader::uniform(program, "uni_normal_mat");
      glUniformMatrix3fv(id, 1, GL_FALSE, &m[0][0]);
    }

    r += 0.2;

    // シェーダーへ値を送る
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, normal_map);

      GLint id = Shader::uniform(program, "uni_normal_map");
      glUniform1i(id, 0);       // GL_TEXTURE0を使ってはならない
    }
    {
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, color_map);

      GLint id = Shader::uniform(program, "uni_color_map");
      glUniform1i(id, 1);       // GL_TEXTURE1を使ってはならない
    }


    // 頂点配列をシェーダーへ送る
    {
      GLint id = Shader::attrib(program, "position");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, &model.vtx[0]);
    }
    {
      GLint id = Shader::attrib(program, "normal");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, &model.normal[0]);
    }
    {
      GLint id = Shader::attrib(program, "tangent");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, &tangent[0]);
    }
    {
      GLint id = Shader::attrib(program, "uv");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, &model.texture[0]);
    }

    // 描画
    glDrawArrays(GL_TRIANGLES, 0, vtx_num);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteTextures(1, &normal_map);
  glDeleteTextures(1, &color_map);

  glfwTerminate();
  
  return 0;
}
