//
// シェーダー
//   画像対応
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


// 画像を読み込む
GLuint createTexture(std::string path) {
  // テクスチャを１つ生成
  GLuint id;
  glGenTextures(1, &id);

  // テクスチャを拘束
  // NOTICE 以下テクスチャに対する命令は拘束したテクスチャに対して実行される
  glBindTexture(GL_TEXTURE_2D, id);

  // 画像を読み込む
  int width;
  int height;
  int comp;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &comp, 0);

  // アルファの有無でデータ形式が異なる
  GLint type = (comp == 3) ? GL_RGB
                           : GL_RGBA;

  // 画像データをOpenGLへ転送
  glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
  stbi_image_free(data);
  
  // 表示用の細々とした設定
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  return id;
}


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

  GLuint texture_id = createTexture("1554-normal.jpg");

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

  GLfloat uv[] =
  {
    // 前
    //  u,     v,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
      
    // 後
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
      
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
      
    // 左
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
      
    // 右
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
      
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
      
    // 上
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,

    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
      
    // 底
    0.0f, 1.0f,
    1.0f, 0.0f,      
    1.0f, 1.0f,

    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,      
  };

  float tangent[36 * 3];

  // 接線ベクトルを計算
  for (int i = 0; i < 36; ++i) {
    float x = normal[i * 3 + 0];
    float y = normal[i * 3 + 1];
    float z = normal[i * 3 + 2];

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

      model = glm::translate(model, glm::vec3(0, 0, -4));
      model = glm::rotate(model,
                          glm::radians(r), glm::vec3(0, 1, 0));
      model = glm::rotate(model,
                          glm::radians(r), glm::vec3(1, 0, 0));

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
      glBindTexture(GL_TEXTURE_2D, texture_id);

      GLint id = Shader::uniform(program, "uni_texture");
      glUniform1i(id, 0);       // GL_TEXTURE0を使ってはならない
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
    {
      GLint id = Shader::attrib(program, "tangent");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, tangent);
    }
    {
      GLint id = Shader::attrib(program, "uv");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, uv);
    }

    // 描画
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteTextures(1, &texture_id);

  glfwTerminate();
  
  return 0;
}
