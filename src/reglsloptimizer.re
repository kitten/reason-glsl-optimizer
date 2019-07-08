/* glslopt_shader_type */
type shaderType =
  | Vertex
  | Fragment;

/* glslopt_target */
type glslTarget =
  | OpenGL
  | OpenGLES2
  | OpenGLES3
  | Metal;

/* glslopt_basic_type */
type basicType =
  | Float
  | Int
  | Bool
  | Tex2D
  | Tex3D
  | Cube
  | Shadow2D
  | Array2D
  | Other
  | Count;

/* glslopt_precision */
type precision =
  | High
  | Medium
  | Low
  | Count;

/* custom block of glslopt_shader */
type shaderT;

/* record for shader values */
type shaderDescT = {
  name: string,
  basicType: basicType,
  precision: precision,
  vectorSize: int,
  matrixSize: int,
  arraySize: int,
  location: int
};

external convertShader: (
  ~target: glslTarget=?,
  shaderType,
  string
) => shaderT = "tg_convert_shader";

external getOutput: shaderT => string = "tg_get_output";
external describeInputs: shaderT => array(shaderDescT) = "tg_get_inputs";
external describeUniforms: shaderT => array(shaderDescT) = "tg_get_uniforms";
external describeTextures: shaderT => array(shaderDescT) = "tg_get_textures";
