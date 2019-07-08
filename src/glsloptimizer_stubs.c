#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <caml/custom.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/mlvalues.h>
#include <caml/fail.h>

#include "glsl/glsl_optimizer.h"

#define Val_none Val_int(0)
#define Some_val(v) Field(v,0)

static enum glslopt_options opts = kGlslOptionSkipPreprocessor;

static glslopt_ctx* gl_ctx = NULL;
static glslopt_ctx* gles2_ctx = NULL;
static glslopt_ctx* gles3_ctx = NULL;
static glslopt_ctx* metal_ctx = NULL;

// The OpenGL ES3 pragma will be replaced manually
static char pragma[] = "#version 300 es";
static int pragma_len = sizeof(pragma) - 1;

static char gl_pragma[] = "#version 330   ";
static char gles2_pragma[] = "#version 200 es";
static char gles3_pragma[] = "#version 300 es";
static char empty_pragma[] = "               ";

/*-- glslopt_target --------------------------------------------------------*/

#if defined(__APPLE__)
  static enum glslopt_target default_target = kGlslTargetMetal;
#elif defined(__EMSCRIPTEN__)
  static enum glslopt_target default_target = kGlslTargetOpenGLES30;
#else
  static enum glslopt_target default_target = kGlslTargetOpenGL;
#endif

/*-- glslopt_shader --------------------------------------------------------*/

void _tg_finalize_glslopt_shader(value v) {
  glslopt_shader* shader = *((glslopt_shader **) Data_custom_val(v));
  glslopt_shader_delete(shader);
}

static struct custom_operations tg_glslopt_shader = {
  .identifier = "glslopt_shader",
  .finalize = _tg_finalize_glslopt_shader,
  .compare = custom_compare_default,
  .hash = custom_hash_default,
  .serialize = custom_serialize_default,
  .deserialize = custom_deserialize_default,
};

static value _tg_copy_glslopt_shader(glslopt_shader* shader) {
  CAMLparam0();
  CAMLlocal1(val);

  // The custom block itself only contains the pointer to glslopt_shader
  val = caml_alloc_custom(&tg_glslopt_shader, sizeof(glslopt_shader*), 0, 1);
  memcpy(Data_custom_val(val), &shader, sizeof(glslopt_shader*));

  CAMLreturn(val);
}

/*-- glslopt_optimize --------------------------------------------------------*/

CAMLprim value tg_convert_shader(value target, value type, value source) {
  CAMLparam3(target, type, source);
  CAMLlocal1(ret);

  enum glslopt_target glsl_target = target != Val_none
    ? Int_val(Some_val(target))
    : default_target;

  // Get the appropriate context and initialize it on demand
  glslopt_ctx* ctx;
  switch (glsl_target) {
    case kGlslTargetOpenGL: // OpenGL
      if (!gl_ctx) gl_ctx = glslopt_initialize(kGlslTargetOpenGL);
      ctx = gl_ctx;
      break;
    case kGlslTargetOpenGLES20: // OpenGL ES2
      if (!gles2_ctx) gles2_ctx = glslopt_initialize(kGlslTargetOpenGLES20);
      ctx = gles2_ctx;
      break;
    case kGlslTargetOpenGLES30: // OpenGL ES3
      if (!gles3_ctx) gles3_ctx = glslopt_initialize(kGlslTargetOpenGLES30);
      ctx = gles3_ctx;
      break;
    case kGlslTargetMetal: // Metal
      if (!metal_ctx) metal_ctx = glslopt_initialize(kGlslTargetMetal);
      ctx = metal_ctx;
      break;
  }

  enum glslopt_shader_type glslopt_type = Int_val(type);
  const char* source_str = String_val(source);

  glslopt_shader* shader = glslopt_optimize(ctx, glslopt_type, source_str, opts);
  if (!glslopt_get_status(shader)) {
    caml_failwith(glslopt_get_log(shader));
  }

  ret = _tg_copy_glslopt_shader(shader);
  CAMLreturn(ret);
}

CAMLprim value tg_get_output(value shader) {
  CAMLparam1(shader);
  CAMLlocal1(str);
  glslopt_shader* shader_val = *((glslopt_shader **) Data_custom_val(shader));
  const char* output = glslopt_get_output(shader_val);

  char* pragma_start = strstr(output, pragma);
  if (pragma_start) {
    char* replacement_pragma;
    switch (glslopt_shader_get_target(shader_val)) {
      case kGlslTargetOpenGL:
        replacement_pragma = gl_pragma;
        break;
      case kGlslTargetOpenGLES20:
        replacement_pragma = gles2_pragma;
        break;
      case kGlslTargetOpenGLES30:
        replacement_pragma = gles3_pragma;
        break;
      default:
        replacement_pragma = empty_pragma;
    }

    strncpy(pragma_start, replacement_pragma, pragma_len);
  }

  str = caml_copy_string(output);
  CAMLreturn(str);
}

CAMLprim value tg_get_input_length(value shaderV) {
  CAMLparam1(shaderV);
  CAMLlocal1(count);
  glslopt_shader* shader = *((glslopt_shader **) Data_custom_val(shaderV));
  count = Val_int(glslopt_shader_get_input_count(shader));
  CAMLreturn(count);
}

CAMLprim value tg_get_uniform_length(value shaderV) {
  CAMLparam1(shaderV);
  CAMLlocal1(count);
  glslopt_shader* shader = *((glslopt_shader **) Data_custom_val(shaderV));
  count = Val_int(glslopt_shader_get_uniform_count(shader));
  CAMLreturn(count);
}

CAMLprim value tg_get_texture_length(value shaderV) {
  CAMLparam1(shaderV);
  CAMLlocal1(count);
  glslopt_shader* shader = *((glslopt_shader **) Data_custom_val(shaderV));
  count = Val_int(glslopt_shader_get_texture_count(shader));
  CAMLreturn(count);
}

CAMLprim value tg_get_input_desc(value shaderV, value index) {
  CAMLparam2(shaderV, index);
  CAMLlocal1(input);

  glslopt_shader* shader = *((glslopt_shader **) Data_custom_val(shaderV));

  const char* name;
  enum glslopt_basic_type type;
  enum glslopt_precision prec;
  int vecSize, matSize, arrSize, location;

  glslopt_shader_get_input_desc(
    shader, Int_val(index),
    &name, &type, &prec, &vecSize, &matSize, &arrSize, &location
  );

  input = caml_alloc(7, 0);
  Store_field(input, 0, caml_copy_string(name));
  Store_field(input, 1, Val_int(type));
  Store_field(input, 2, Val_int(prec));
  Store_field(input, 3, Val_int(vecSize));
  Store_field(input, 4, Val_int(matSize));
  Store_field(input, 5, Val_int(arrSize));
  Store_field(input, 6, Val_int(location));
  CAMLreturn(input);
}

CAMLprim value tg_get_uniform_desc(value shaderV, value index) {
  CAMLparam2(shaderV, index);
  CAMLlocal1(input);

  glslopt_shader* shader = *((glslopt_shader **) Data_custom_val(shaderV));

  const char* name;
  enum glslopt_basic_type type;
  enum glslopt_precision prec;
  int vecSize, matSize, arrSize, location;

  glslopt_shader_get_uniform_desc(
    shader, Int_val(index),
    &name, &type, &prec, &vecSize, &matSize, &arrSize, &location
  );

  input = caml_alloc(7, 0);
  Store_field(input, 0, caml_copy_string(name));
  Store_field(input, 1, Val_int(type));
  Store_field(input, 2, Val_int(prec));
  Store_field(input, 3, Val_int(vecSize));
  Store_field(input, 4, Val_int(matSize));
  Store_field(input, 5, Val_int(arrSize));
  Store_field(input, 6, Val_int(location));
  CAMLreturn(input);
}

CAMLprim value tg_get_texture_desc(value shaderV, value index) {
  CAMLparam2(shaderV, index);
  CAMLlocal1(input);

  glslopt_shader* shader = *((glslopt_shader **) Data_custom_val(shaderV));

  const char* name;
  enum glslopt_basic_type type;
  enum glslopt_precision prec;
  int vecSize, matSize, arrSize, location;

  glslopt_shader_get_texture_desc(
    shader, Int_val(index),
    &name, &type, &prec, &vecSize, &matSize, &arrSize, &location
  );

  input = caml_alloc(7, 0);
  Store_field(input, 0, caml_copy_string(name));
  Store_field(input, 1, Val_int(type));
  Store_field(input, 2, Val_int(prec));
  Store_field(input, 3, Val_int(vecSize));
  Store_field(input, 4, Val_int(matSize));
  Store_field(input, 5, Val_int(arrSize));
  Store_field(input, 6, Val_int(location));
  CAMLreturn(input);
}
