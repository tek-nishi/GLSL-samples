#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>


//
// シェーダーのセットアップ
//

namespace Shader {

GLuint compile(GLuint type, const std::string& text)
{
  // シェーダー識別子を１つ生成
  GLuint shader = glCreateShader(type);

  // TIPS:glShaderSource は char** を要求する
  const char* text_ptr = text.c_str();
  glShaderSource(shader, 1, &text_ptr, 0);
  glCompileShader(shader);

#ifdef DEBUG
  // コンパイルが成功したか調べる
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE)
  {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

    // エラー内容を出力
    std::string log(length + 1, ' ');
    glGetShaderInfoLog(shader, length, &length, &log[0]);
    std::cout << "compile error:" << log << std::endl;

    return GL_FALSE;
  }
#endif
  return shader;
}

void setup(const GLuint program, const std::string& v_source, const std::string& f_source)
{
  GLuint vertex_shader   = compile(GL_VERTEX_SHADER, v_source);
  GLuint fragment_shader = compile(GL_FRAGMENT_SHADER, f_source);

  // コンパイルしたシェーダープログラムをリンクする
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

#ifdef DEBUG
  // リンクが成功したか調べる
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE)
  {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

    // エラー内容を出力
    std::string log(length + 1, ' ');
    glGetProgramInfoLog(program, length, &length, &log[0]);
    std::cout << "link error:" << log << std::endl;
  }
#endif

  // FIXME:ここでないとglGetAttribLocationとかglGetUniformLocationとか
  //       できないのか??
  
  // リンクしたらもう要らない
  if (vertex_shader)
  {
    glDetachShader(program, vertex_shader);
    glDeleteShader(vertex_shader);
  }

  if (fragment_shader)
  {
    glDetachShader(program, fragment_shader);
    glDeleteShader(fragment_shader);
  }
}


// 読み込み
//   file 拡張子を除いたファイルパス
GLuint read(const std::string& file)
{
  // .vshを読み込む
  std::string vsh_path = file + ".vsh";
  std::ifstream vsh_fs(vsh_path);
  assert(vsh_fs);
  // テキストファイル→std::string
  std::string v_source((std::istreambuf_iterator<char>(vsh_fs)), std::istreambuf_iterator<char>());

  // .fshを読み込む
  std::string fsh_path = file + ".fsh";
  std::ifstream fsh_fs(fsh_path);
  assert(fsh_fs);
  std::string f_source((std::istreambuf_iterator<char>(fsh_fs)), std::istreambuf_iterator<char>());

  // プログラム識別子を生成
  GLuint program = glCreateProgram();
  // バーテックスシェーダーとフラグメントシェーダーを生成
  setup(program, v_source, f_source);

  return program;
}

// シェーダー内アトリビュート変数の識別子を取得
GLint attrib(const GLuint program, const std::string& name)
{
  return glGetAttribLocation(program, name.c_str());
}

// シェーダー内ユニフォーム変数の識別子を取得
GLint uniform(const GLuint program, const std::string& name)
{
  return glGetUniformLocation(program, name.c_str());
}

// シェーダープログラムの使用開始
void use(const GLuint program)
{
  glUseProgram(program);
}

}
