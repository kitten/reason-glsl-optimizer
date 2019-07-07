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

let vsShader = convertShader(Vertex, vs);
let fsShader = convertShader(Vertex, fs);

print_endline("vs:");
print_endline(getOutput(vsShader));
print_endline("fs:");
print_endline(getOutput(fsShader));
