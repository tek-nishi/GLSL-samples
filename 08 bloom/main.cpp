//
// シェーダー
//  bloomフィルター
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

// 画像を扱う
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.hpp"
#include "obj.hpp"
#include "texture.hpp"


int main() {
  // GLFW初期化
  if (!glfwInit()) return -1;

  GLFWwindow* window = glfwCreateWindow(800, 600,
                                        "Bloom effect",
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

  // モデルデータ
  Obj model = loadObj("sphere.obj");
  size_t vtx_num = model.vtx.size();

  // 画像
  GLuint texture_id = createTexture("earth_1.png");

  // シェーダーを読み込んで準備
  GLuint program   = Shader::read("sample");
  GLuint fill_prog = Shader::read("fill");

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  float r = 0.0;

  // 「描画バッファ」のフレームバッファ識別子を
  // OpenGLに教えてもらう※後で使う
  GLint current_framebuffer;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, 
                &current_framebuffer);

  // オフスクリーンバッファを用意
  GLuint fbo_texture_id;
  glGenTextures(1, &fbo_texture_id);
  glBindTexture(GL_TEXTURE_2D, fbo_texture_id);

  glTexImage2D(GL_TEXTURE_2D, 
               0, GL_RGB, 1024, 1024, 
               0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  // 拡大縮小時のフィルタを指定
  glTexParameteri(GL_TEXTURE_2D, 
                  GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, 
                  GL_TEXTURE_MAG_FILTER, GL_NEAREST);  

  // オフスクリーンバッファ用の
  // フレームバッファ識別子を
  // １つ作る
  GLuint framebuffer_id;
  glGenFramebuffers(1, &framebuffer_id);  
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

  // フレームバッファにテクスチャを拘束  
  glFramebufferTexture2D(GL_FRAMEBUFFER, 
                         GL_COLOR_ATTACHMENT0, 
                         GL_TEXTURE_2D, 
                         fbo_texture_id, 0);

  // 透視変換表列を用意
  glm::mat4 proj = glm::perspective(glm::radians(35.0),    // 視野角
                                    800.0 / 600.0,         // 縦横比
                                    0.2, 20.0);            // near-z, far-z

  while (!glfwWindowShouldClose(window))
  {
    // オフスクリーン用のフレームバッファを拘束
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

    glEnable(GL_DEPTH);         // 深度バッファON
    glEnable(GL_CULL_FACE);     // 裏面描画OFF
    
    // フレームバッファの内容を消去
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ビューポート設定
    glViewport(0, 0, 1024, 1024);

    // シェーダー発動
    Shader::use(program);

    // シェーダーへ透視変換行列を送る
    {
      GLint id = Shader::uniform(program, "uni_proj_mat");
      glUniformMatrix4fv(id, 1, GL_FALSE, &proj[0][0]);        
    }

    // シェーダーへモデルビュー行列を送る
    {
      glm::mat4 model;

      model = glm::translate(model, glm::vec3(0, 0, -3));
      model = glm::rotate(model,
                          glm::radians(r), glm::vec3(0, 1, 0));
      // model = glm::rotate(model,
      //                     glm::radians(r), glm::vec3(1, 0, 0));

      GLint id = Shader::uniform(program, "uni_model_mat");
      glUniformMatrix4fv(id, 1, GL_FALSE, &model[0][0]);


      // 逆転置行列を生成
      glm::mat3 m = glm::inverseTranspose(glm::mat3(model));
      id = Shader::uniform(program, "uni_normal_mat");
      glUniformMatrix3fv(id, 1, GL_FALSE, &m[0][0]);
    }

    r += 0.25;

    // シェーダーへ値を送る
    {
      GLint id = Shader::uniform(program, "uni_deffuse_dir");
      glm::vec3 v(0, 0, 1);
      glUniform3fv(id, 1, &v[0]);        
    }
    {
      GLint id = Shader::uniform(program, "uni_deffuse");
      glm::vec4 v(1, 1, 1, 1);
      glUniform4fv(id, 1, &v[0]);        
    }
    {
      GLint id = Shader::uniform(program, "uni_ambient");
      glm::vec4 v(0.2, 0.2, 0.2, 1);
      glUniform4fv(id, 1, &v[0]);        
    }
    {
      GLint id = Shader::uniform(program, "uni_specular");
      glm::vec4 v(1, 1, 1, 1);
      glUniform4fv(id, 1, &v[0]);        
    }
    {
      GLint id = Shader::uniform(program, "uni_shine");
      glUniform1f(id, 80.0f);        
    }
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
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, &model.vtx[0]);
    }
    {
      GLint id = Shader::attrib(program, "normal");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 0, &model.normal[0]);
    }
    {
      GLint id = Shader::attrib(program, "uv");
      glEnableVertexAttribArray(id);
      glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, &model.texture[0]);
    }
    
    // 描画
    glDrawArrays(GL_TRIANGLES, 0, vtx_num / 3);

    // 画面表示用のフレームバッファを拘束
    glBindFramebuffer(GL_FRAMEBUFFER, current_framebuffer);

    glDisable(GL_DEPTH);         // 深度バッファOFF
    glDisable(GL_CULL_FACE);     // 裏面描画ON
    
    glViewport(0, 0, 800, 600);

    // シェーダー発動
    Shader::use(fill_prog);
    
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, fbo_texture_id);

      GLint id = Shader::uniform(fill_prog, "uni_texture");
      glUniform1i(id, 0);       // GL_TEXTURE0を使ってはならない
    }
    
    // 頂点配列をシェーダーへ送る
    {
      GLint id = Shader::attrib(fill_prog, "position");
      glEnableVertexAttribArray(id);

      const float vtx[] = {
        -1.0, -1.0,
         1.0, -1.0,
        -1.0,  1.0,

        -1.0,  1.0,
         1.0, -1.0,
         1.0,  1.0,
      };
      glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, vtx);
    }
    {
      GLint id = Shader::attrib(fill_prog, "uv");
      glEnableVertexAttribArray(id);

      const float uv[] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,

        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,
      };
      glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, uv);
    }
    
    // 描画
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  
  return 0;
}
