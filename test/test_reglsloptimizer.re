open Reglsloptimizer;

Printexc.record_backtrace(true);

let vs = {|
  #version 300 es

  in vec3 position;
  in vec3 color;

  uniform mat4 mat;

  out vec4 out_color;

  void main() {
    gl_Position = mat * vec4(position.xyz, 1.0);
    out_color = vec4(color.xyz, 1.0);
  }
|};

let fs = {|
  #version 300 es

  in vec4 out_color;
  out vec4 frag_color;

  void main() {
    frag_color = out_color;
  }
|};

let vsMetal = convertShader(~target=Metal, Vertex, vs);
let fsMetal = convertShader(~target=Metal, Fragment, fs);

let vsGL = convertShader(~target=OpenGL, Vertex, vs);
let fsGL = convertShader(~target=OpenGL, Fragment, fs);

print_endline("vs metal:");
print_endline(getOutput(vsMetal));
print_endline("fs metal:");
print_endline(getOutput(fsMetal));

print_endline("vs gl:");
print_endline(getOutput(vsGL));
print_endline("fs gl:");
print_endline(getOutput(fsGL));
